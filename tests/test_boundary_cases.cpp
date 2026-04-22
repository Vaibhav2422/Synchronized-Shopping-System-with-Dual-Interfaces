/**
 * Boundary Case Tests for Shopping Management System
 * **Validates: Requirements 30.4**
 *
 * Covers:
 *  1. Price boundary tests (min ~0.01, very large 1e12, exactly 0)
 *  2. Quantity boundary tests (qty=1, qty exceeds stock, qty=0 rejected, qty=-1 rejected)
 *  3. Cart discount threshold boundary (4999.99, 5000.00, 5000.01)
 *  4. GST calculation precision (gst == afterDiscounts * 0.18 within 0.01)
 *  5. Coupon discount precision (SAVE10 and FESTIVE20 on various amounts)
 *  6. Bill final total precision (finalTotal == afterDiscounts * 1.18 within 0.01)
 */

#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Bill.h"
#include "../include/Coupon.h"
#include "../include/ProductFactory.h"
#include "../include/StockException.h"
#include "../include/InvalidInputException.h"

#include <iostream>
#include <cmath>
#include <climits>

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) \
    do { \
        if (cond) { ++passed; std::cout << "  [PASS] " << msg << "\n"; } \
        else      { ++failed; std::cout << "  [FAIL] " << msg << "\n"; } \
    } while (false)

void testPriceBoundaries() {
    std::cout << "\n--- 1. Price Boundary Tests ---\n";
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(1, "CheapBook", 0.01, 100, "Author", "Genre"));
        cart.addProduct(1, 1);
        double subtotal = cart.calculateSubtotal();
        CHECK(std::fabs(subtotal - 0.01) < 1e-9, "Subtotal == 0.01 for min-price product");
        Bill bill; bill.calculate(cart, nullptr);
        CHECK(std::isfinite(bill.getFinalTotal()), "Bill finalTotal is finite for price=0.01");
        CHECK(bill.getFinalTotal() > 0.0, "Bill finalTotal > 0 for price=0.01");
    }
    {
        Inventory inv; Cart cart(&inv);
        const double bigPrice = 1e12;
        inv.addProduct(ProductFactory::createElectronics(2, "SuperItem", bigPrice, 5, "Brand", 1));
        cart.addProduct(2, 1);
        double subtotal = cart.calculateSubtotal();
        CHECK(std::isfinite(subtotal), "Subtotal is finite for price=1e12");
        CHECK(std::fabs(subtotal - bigPrice) < 1.0, "Subtotal == 1e12 for single item");
        Bill bill; bool threw = false;
        try { bill.calculate(cart, nullptr); } catch (...) { threw = true; }
        CHECK(!threw, "Bill::calculate does not throw for price=1e12");
        CHECK(std::isfinite(bill.getFinalTotal()), "Bill finalTotal is finite for price=1e12");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createClothing(3, "FreeItem", 0.0, 10, "M", "Cotton"));
        bool added = false;
        try { added = cart.addProduct(3, 1); } catch (...) {}
        if (added) {
            CHECK(std::fabs(cart.calculateSubtotal()) < 1e-9, "Subtotal == 0 for zero-price product");
            Bill bill; bill.calculate(cart, nullptr);
            CHECK(std::fabs(bill.getFinalTotal()) < 1e-9, "Bill finalTotal == 0 for zero-price product");
        } else {
            CHECK(true, "Zero-price product rejected (acceptable behaviour)");
        }
    }
}

void testQuantityBoundaries() {
    std::cout << "\n--- 2. Quantity Boundary Tests ---\n";
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(10, "Laptop", 45000.0, 5, "Dell", 2));
        bool added = false;
        try { added = cart.addProduct(10, 1); } catch (...) {}
        CHECK(added, "addProduct(id, 1) succeeds");
        CHECK(cart.getItemCount() == 1, "getItemCount() == 1 after adding qty=1");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(11, "Laptop", 45000.0, 5, "Dell", 2));
        bool threw = false;
        try { cart.addProduct(11, 0); }
        catch (const InvalidQuantityException&) { threw = true; }
        catch (...) { threw = true; }
        CHECK(threw || cart.isEmpty(), "addProduct(id, 0) throws or leaves cart empty");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(12, "Laptop", 45000.0, 5, "Dell", 2));
        bool threw = false;
        try { cart.addProduct(12, -1); }
        catch (const InvalidQuantityException&) { threw = true; }
        catch (...) { threw = true; }
        CHECK(threw || cart.isEmpty(), "addProduct(id, -1) throws or leaves cart empty");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(13, "Laptop", 45000.0, 3, "Dell", 2));
        bool threw = false;
        try { cart.addProduct(13, 10); }
        catch (const InsufficientStockException&) { threw = true; }
        catch (const OutOfStockException&) { threw = true; }
        catch (...) { threw = true; }
        CHECK(threw || cart.isEmpty(), "addProduct(id, 10) when stock==3 throws or leaves cart empty");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(14, "Laptop", 45000.0, 5, "Dell", 2));
        bool threw = false;
        try { cart.addProduct(14, INT_MAX); }
        catch (const InsufficientStockException&) { threw = true; }
        catch (const OutOfStockException&) { threw = true; }
        catch (...) { threw = true; }
        CHECK(threw || cart.isEmpty(), "addProduct(id, INT_MAX) when stock==5 throws or leaves cart empty");
    }
}

void testCartDiscountThreshold() {
    std::cout << "\n--- 3. Cart Discount Threshold Boundary ---\n";
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(20, "Book4999", 4999.99, 10, "Author", "Genre"));
        cart.addProduct(20, 1);
        CHECK(std::fabs(cart.calculateSubtotal() - 4999.99) < 0.001, "Subtotal == 4999.99");
        CHECK(std::fabs(cart.calculateCartDiscount()) < 1e-9, "Cart discount == 0 when subtotal == 4999.99");
        Bill bill; bill.calculate(cart, nullptr);
        double expectedAfter = 4999.99 * 0.95;
        CHECK(std::fabs(bill.getGSTAmount() - expectedAfter * 0.18) < 0.01, "GST correct for subtotal=4999.99");
        CHECK(std::fabs(bill.getFinalTotal() - expectedAfter * 1.18) < 0.01, "FinalTotal correct for subtotal=4999.99");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(21, "Book5000", 5000.00, 10, "Author", "Genre"));
        cart.addProduct(21, 1);
        CHECK(std::fabs(cart.calculateSubtotal() - 5000.00) < 0.001, "Subtotal == 5000.00");
        CHECK(std::fabs(cart.calculateCartDiscount()) < 1e-9, "Cart discount == 0 when subtotal == 5000.00 (not strictly greater)");
        Bill bill; bill.calculate(cart, nullptr);
        double expectedAfter = 5000.00 * 0.95;
        CHECK(std::fabs(bill.getFinalTotal() - expectedAfter * 1.18) < 0.01, "FinalTotal correct for subtotal=5000.00");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(22, "Book5001", 5000.01, 10, "Author", "Genre"));
        cart.addProduct(22, 1);
        CHECK(std::fabs(cart.calculateSubtotal() - 5000.01) < 0.001, "Subtotal == 5000.01");
        double cartDisc = cart.calculateCartDiscount();
        CHECK(std::fabs(cartDisc - 5000.01 * 0.05) < 0.001, "Cart discount == 5% of 5000.01");
        Bill bill; bill.calculate(cart, nullptr);
        double expectedAfter = 5000.01 - (5000.01 * 0.05) - (5000.01 * 0.05);
        CHECK(std::fabs(bill.getFinalTotal() - expectedAfter * 1.18) < 0.01, "FinalTotal correct for subtotal=5000.01");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(23, "Gadget5000", 5000.00, 5, "Brand", 1));
        cart.addProduct(23, 1);
        CHECK(std::fabs(cart.calculateCartDiscount()) < 1e-9, "Cart discount == 0 for Electronics at subtotal=5000.00");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(24, "Gadget5001", 5000.01, 5, "Brand", 1));
        cart.addProduct(24, 1);
        CHECK(std::fabs(cart.calculateCartDiscount() - 5000.01 * 0.05) < 0.001, "Cart discount == 5% of 5000.01 for Electronics");
    }
}

void testGSTPrecision() {
    std::cout << "\n--- 4. GST Calculation Precision ---\n";
    const double GST_RATE = 0.18;
    const double TOL = 0.01;
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(30, "Phone", 1000.0, 5, "Brand", 1));
        cart.addProduct(30, 1);
        Bill bill; bill.calculate(cart, nullptr);
        CHECK(std::fabs(bill.getGSTAmount() - 900.0 * GST_RATE) < TOL, "GST == 162.00 for Electronics 1000 (afterDiscounts=900)");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createClothing(31, "Shirt", 500.0, 10, "M", "Cotton"));
        cart.addProduct(31, 1);
        Bill bill; bill.calculate(cart, nullptr);
        CHECK(std::fabs(bill.getGSTAmount() - 400.0 * GST_RATE) < TOL, "GST == 72.00 for Clothing 500 (afterDiscounts=400)");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(32, "Novel", 200.0, 10, "Author", "Fiction"));
        cart.addProduct(32, 1);
        Bill bill; bill.calculate(cart, nullptr);
        CHECK(std::fabs(bill.getGSTAmount() - 190.0 * GST_RATE) < TOL, "GST == 34.20 for Book 200 (afterDiscounts=190)");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(33, "FractBook", 333.33, 10, "Author", "Genre"));
        cart.addProduct(33, 1);
        Bill bill; bill.calculate(cart, nullptr);
        double expectedGST = 333.33 * 0.95 * GST_RATE;
        CHECK(std::fabs(bill.getGSTAmount() - expectedGST) < TOL, "GST precision correct for fractional price 333.33");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(34, "Laptop", 6000.0, 5, "Brand", 1));
        cart.addProduct(34, 1);
        Bill bill; bill.calculate(cart, nullptr);
        // afterDiscounts = 6000 - 600 (10%) - 300 (5% cart) = 5100
        CHECK(std::fabs(bill.getGSTAmount() - 5100.0 * GST_RATE) < TOL, "GST == 918.00 for Electronics 6000 with cart discount");
    }
}

void testCouponDiscountPrecision() {
    std::cout << "\n--- 5. Coupon Discount Precision ---\n";
    const double TOL = 0.01;
    { Coupon c; c.apply("SAVE10"); CHECK(std::fabs(c.calculateDiscount(1000.0) - 100.0) < TOL, "SAVE10: 10% of 1000 == 100.00"); }
    { Coupon c; c.apply("SAVE10"); CHECK(std::fabs(c.calculateDiscount(333.33) - 33.333) < TOL, "SAVE10: 10% of 333.33 == 33.33"); }
    { Coupon c; c.apply("SAVE10"); CHECK(std::fabs(c.calculateDiscount(0.01) - 0.001) < 1e-6, "SAVE10: 10% of 0.01 == 0.001"); }
    { Coupon c; c.apply("FESTIVE20"); CHECK(std::fabs(c.calculateDiscount(500.0) - 100.0) < TOL, "FESTIVE20: 20% of 500 == 100.00"); }
    { Coupon c; c.apply("FESTIVE20"); CHECK(std::fabs(c.calculateDiscount(750.50) - 150.10) < TOL, "FESTIVE20: 20% of 750.50 == 150.10"); }
    { Coupon c; c.apply("FESTIVE20"); CHECK(std::fabs(c.calculateDiscount(50000.0) - 10000.0) < TOL, "FESTIVE20: 20% of 50000 == 10000.00"); }
    { Coupon c; CHECK(std::fabs(c.calculateDiscount(1000.0)) < TOL, "Unapplied coupon: calculateDiscount returns 0"); }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(40, "Book", 1000.0, 10, "Author", "Genre"));
        cart.addProduct(40, 1);
        Coupon coupon; coupon.apply("SAVE10");
        Bill bill; bill.calculate(cart, &coupon);
        // afterProductDisc=950, couponDisc=95, taxable=855, finalTotal=855*1.18
        CHECK(std::fabs(bill.getFinalTotal() - 855.0 * 1.18) < TOL, "Bill finalTotal correct with SAVE10 coupon on Book 1000");
    }
}

void testBillFinalTotalPrecision() {
    std::cout << "\n--- 6. Bill Final Total Precision ---\n";
    const double TOL = 0.01;
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(50, "Phone", 2000.0, 5, "Brand", 1));
        cart.addProduct(50, 1);
        Bill bill; bill.calculate(cart, nullptr);
        // afterDiscounts = 2000 - 200 (10%) = 1800
        CHECK(std::fabs(bill.getFinalTotal() - 1800.0 * 1.18) < TOL, "FinalTotal == afterDiscounts * 1.18 for Electronics 2000");
        CHECK(std::fabs(bill.getFinalTotal() - (bill.getGSTAmount() + 1800.0)) < TOL, "FinalTotal == afterDiscounts + GST");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createClothing(51, "Jeans", 1500.0, 5, "32", "Denim"));
        cart.addProduct(51, 1);
        Bill bill; bill.calculate(cart, nullptr);
        // afterDiscounts = 1500 - 300 (20%) = 1200
        CHECK(std::fabs(bill.getFinalTotal() - 1200.0 * 1.18) < TOL, "FinalTotal == afterDiscounts * 1.18 for Clothing 1500");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createElectronics(52, "Laptop", 10000.0, 5, "Brand", 1));
        cart.addProduct(52, 1);
        Coupon coupon; coupon.apply("FESTIVE20");
        Bill bill; bill.calculate(cart, &coupon);
        // subtotal=10000, prodDisc=1000, cartDisc=500, afterProdCart=8500, couponDisc=1700, taxable=6800
        CHECK(std::fabs(bill.getFinalTotal() - 6800.0 * 1.18) < TOL, "FinalTotal correct for Electronics 10000 with FESTIVE20 and cart discount");
    }
    {
        Inventory inv; Cart cart(&inv);
        Bill bill; bill.calculate(cart, nullptr);
        CHECK(std::fabs(bill.getFinalTotal()) < TOL, "FinalTotal == 0 for empty cart");
        CHECK(std::fabs(bill.getGSTAmount()) < TOL, "GST == 0 for empty cart");
    }
    {
        Inventory inv; Cart cart(&inv);
        inv.addProduct(ProductFactory::createBook(53, "BigBook", 8000.0, 5, "Author", "Genre"));
        cart.addProduct(53, 1);
        Coupon coupon; coupon.apply("SAVE10");
        Bill bill; bill.calculate(cart, &coupon);
        // subtotal=8000, prodDisc=400, cartDisc=400, afterProdCart=7200, couponDisc=720, taxable=6480
        CHECK(std::fabs(bill.getFinalTotal() - 6480.0 * 1.18) < TOL, "FinalTotal correct for Book 8000 with SAVE10 and cart discount");
        double taxable = bill.getFinalTotal() / 1.18;
        CHECK(std::fabs(bill.getGSTAmount() - taxable * 0.18) < TOL, "GST == taxable * 0.18 (accounting identity)");
    }
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  Boundary Case Tests\n";
    std::cout << "  Validates: Requirements 30.4\n";
    std::cout << "========================================\n";

    testPriceBoundaries();
    testQuantityBoundaries();
    testCartDiscountThreshold();
    testGSTPrecision();
    testCouponDiscountPrecision();
    testBillFinalTotalPrecision();

    std::cout << "\n========================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "========================================\n";

    return (failed == 0) ? 0 : 1;
}
