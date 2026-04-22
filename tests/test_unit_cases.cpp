/**
 * test_unit_cases.cpp
 * Comprehensive unit tests for Shopping Management System
 * Tests: coupon codes, sample data, payment methods, empty cart, zero stock, no search results
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>

#include "../include/Coupon.h"
#include "../include/CouponException.h"
#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include "../include/Cart.h"
#include "../include/Payment.h"
#include "../include/PaymentException.h"
#include "../include/UPIPayment.h"
#include "../include/CardPayment.h"
#include "../include/CashPayment.h"
#include "../include/StockException.h"
#include "../include/InvalidInputException.h"

// ─── Test result tracking ────────────────────────────────────────────────────

static int passed = 0;
static int failed = 0;

#define PASS(msg) do { std::cout << "  [PASS] " << msg << std::endl; ++passed; } while(0)
#define FAIL(msg) do { std::cout << "  [FAIL] " << msg << std::endl; ++failed; } while(0)

#define CHECK(cond, msg) do { if (cond) { PASS(msg); } else { FAIL(msg); } } while(0)

// ─── 1. Valid Coupon Codes ────────────────────────────────────────────────────

void testValidCouponCodes() {
    std::cout << "\n[1] Valid Coupon Codes\n";

    // SAVE10 applies 10% discount
    {
        Coupon c;
        bool applied = false;
        try { applied = c.apply("SAVE10"); } catch (...) {}
        CHECK(applied, "SAVE10 is accepted");
        CHECK(c.isApplied(), "SAVE10 sets applied flag");
        CHECK(c.getCode() == "SAVE10", "SAVE10 code stored correctly");
        CHECK(std::fabs(c.getDiscountPercentage() - 0.10) < 0.001, "SAVE10 gives 10% discount");
        CHECK(std::fabs(c.calculateDiscount(1000.0) - 100.0) < 0.01, "SAVE10 calculates 100 off 1000");
    }

    // FESTIVE20 applies 20% discount
    {
        Coupon c;
        bool applied = false;
        try { applied = c.apply("FESTIVE20"); } catch (...) {}
        CHECK(applied, "FESTIVE20 is accepted");
        CHECK(c.isApplied(), "FESTIVE20 sets applied flag");
        CHECK(c.getCode() == "FESTIVE20", "FESTIVE20 code stored correctly");
        CHECK(std::fabs(c.getDiscountPercentage() - 0.20) < 0.001, "FESTIVE20 gives 20% discount");
        CHECK(std::fabs(c.calculateDiscount(500.0) - 100.0) < 0.01, "FESTIVE20 calculates 100 off 500");
    }

    // Invalid coupon throws exception
    {
        Coupon c;
        bool threw = false;
        try { c.apply("BADCODE"); } catch (const InvalidCouponException&) { threw = true; }
        CHECK(threw, "Invalid coupon code throws InvalidCouponException");
        CHECK(!c.isApplied(), "Invalid coupon does not set applied flag");
    }

    // reset() clears applied coupon
    {
        Coupon c;
        c.apply("SAVE10");
        c.reset();
        CHECK(!c.isApplied(), "reset() clears applied coupon");
        CHECK(c.getCode().empty(), "reset() clears coupon code");
    }
}

// ─── 2. Sample Data Initialization ───────────────────────────────────────────

void testSampleDataInitialization() {
    std::cout << "\n[2] Sample Data Initialization\n";

    Inventory inv;
    inv.initializeSampleData();

    const auto& products = inv.getProducts();

    // Total count
    CHECK(products.size() >= 9, "At least 9 products initialized");

    // Electronics (IDs 101-103)
    CHECK(inv.findProduct(101) != nullptr, "Electronics product 101 (Laptop) exists");
    CHECK(inv.findProduct(102) != nullptr, "Electronics product 102 (Smartphone) exists");
    CHECK(inv.findProduct(103) != nullptr, "Electronics product 103 (Headphones) exists");

    // Clothing (IDs 201-203)
    CHECK(inv.findProduct(201) != nullptr, "Clothing product 201 (T-Shirt) exists");
    CHECK(inv.findProduct(202) != nullptr, "Clothing product 202 (Jeans) exists");
    CHECK(inv.findProduct(203) != nullptr, "Clothing product 203 (Jacket) exists");

    // Books (IDs 301-303)
    CHECK(inv.findProduct(301) != nullptr, "Book product 301 (C++ Programming) exists");
    CHECK(inv.findProduct(302) != nullptr, "Book product 302 (Clean Code) exists");
    CHECK(inv.findProduct(303) != nullptr, "Book product 303 (Pragmatic Programmer) exists");

    // Category counts
    auto electronics = inv.filterByCategory("Electronics");
    auto clothing    = inv.filterByCategory("Clothing");
    auto books       = inv.filterByCategory("Books");
    CHECK(electronics.size() >= 3, "At least 3 Electronics products");
    CHECK(clothing.size()    >= 3, "At least 3 Clothing products");
    CHECK(books.size()       >= 3, "At least 3 Books products");

    // All prices and quantities are positive
    bool allPricesPositive = true, allQtyPositive = true;
    for (const auto& p : products) {
        if (p->getPrice() <= 0)            allPricesPositive = false;
        if (p->getQuantityAvailable() <= 0) allQtyPositive    = false;
    }
    CHECK(allPricesPositive, "All sample products have positive prices");
    CHECK(allQtyPositive,    "All sample products have positive quantities");
}

// ─── 3. Payment Method Availability ──────────────────────────────────────────

void testPaymentMethodAvailability() {
    std::cout << "\n[3] Payment Method Availability\n";

    // UPI
    {
        UPIPayment upi(1000.0, "user@upi");
        CHECK(upi.getPaymentMethod() == "UPI", "UPI payment method name is 'UPI'");
        CHECK(std::fabs(upi.getAmount() - 1000.0) < 0.01, "UPI amount stored correctly");
        bool ok = false;
        try { ok = upi.pay(); } catch (...) {}
        CHECK(ok, "UPI pay() returns true");
    }

    // Card
    {
        CardPayment card(2500.0, "1234567890123456", "Alice");
        CHECK(card.getPaymentMethod() == "Card", "Card payment method name is 'Card'");
        CHECK(std::fabs(card.getAmount() - 2500.0) < 0.01, "Card amount stored correctly");
        bool ok = false;
        try { ok = card.pay(); } catch (...) {}
        CHECK(ok, "Card pay() returns true");
    }

    // Cash – sufficient funds
    {
        CashPayment cash(750.0, 1000.0);
        CHECK(cash.getPaymentMethod() == "Cash", "Cash payment method name is 'Cash'");
        CHECK(std::fabs(cash.calculateChange() - 250.0) < 0.01, "Cash change calculated correctly");
        bool ok = false;
        try { ok = cash.pay(); } catch (...) {}
        CHECK(ok, "Cash pay() returns true when funds sufficient");
    }

    // Cash – insufficient funds throws
    {
        CashPayment cash(1000.0, 500.0);
        bool threw = false;
        try { cash.pay(); } catch (const InsufficientFundsException&) { threw = true; }
        CHECK(threw, "Cash pay() throws InsufficientFundsException when funds insufficient");
    }

    // Polymorphic dispatch through base pointer
    {
        std::unique_ptr<Payment> p1 = std::make_unique<UPIPayment>(100.0, "a@b");
        std::unique_ptr<Payment> p2 = std::make_unique<CardPayment>(200.0, "0000111122223333", "Bob");
        std::unique_ptr<Payment> p3 = std::make_unique<CashPayment>(300.0, 500.0);
        bool ok1 = false, ok2 = false, ok3 = false;
        try { ok1 = p1->pay(); } catch (...) {}
        try { ok2 = p2->pay(); } catch (...) {}
        try { ok3 = p3->pay(); } catch (...) {}
        CHECK(ok1 && ok2 && ok3, "All three payment types succeed via base pointer");
    }
}

// ─── 4. Empty Cart Edge Case ──────────────────────────────────────────────────

void testEmptyCartEdgeCase() {
    std::cout << "\n[4] Empty Cart Edge Case\n";

    Inventory inv;
    inv.initializeSampleData();
    Cart cart(&inv);

    // isEmpty() and getItemCount()
    CHECK(cart.isEmpty(), "New cart is empty");
    CHECK(cart.getItemCount() == 0, "New cart has 0 items");

    // All calculations return 0 on empty cart
    CHECK(std::fabs(cart.calculateSubtotal() - 0.0) < 0.01,
          "Empty cart subtotal is 0");
    CHECK(std::fabs(cart.calculateProductDiscounts() - 0.0) < 0.01,
          "Empty cart product discounts is 0");
    CHECK(std::fabs(cart.calculateCartDiscount() - 0.0) < 0.01,
          "Empty cart cart discount is 0");

    // After adding and clearing, cart is empty again
    cart.addProduct(101, 1);
    CHECK(!cart.isEmpty(), "Cart not empty after adding product");
    cart.clear();
    CHECK(cart.isEmpty(), "Cart empty after clear()");
    CHECK(cart.getItemCount() == 0, "Item count 0 after clear()");
    CHECK(std::fabs(cart.calculateSubtotal() - 0.0) < 0.01,
          "Subtotal 0 after clear()");
}

// ─── 5. Zero Stock Edge Case ──────────────────────────────────────────────────

void testZeroStockEdgeCase() {
    std::cout << "\n[5] Zero Stock Edge Case\n";

    // Build inventory with a product that has 0 stock
    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(501, "OutOfStockItem", 999.0, 0, "Brand", 1));
    inv.addProduct(ProductFactory::createElectronics(502, "InStockItem",    500.0, 5, "Brand", 1));

    Cart cart(&inv);

    // Cannot add zero-stock product
    bool threw = false;
    try {
        cart.addProduct(501, 1);
    } catch (const ProductUnavailableException&) {
        threw = true;
    } catch (const OutOfStockException&) {
        threw = true;
    }
    CHECK(threw, "Adding zero-stock product throws stock exception");
    CHECK(cart.isEmpty(), "Cart remains empty after failed add");

    // Can still add in-stock product
    bool added = false;
    try { added = cart.addProduct(502, 1); } catch (...) {}
    CHECK(added, "In-stock product can be added normally");
    CHECK(!cart.isEmpty(), "Cart has item after successful add");

    // Depleting stock to zero then trying to add
    Inventory inv2;
    inv2.addProduct(ProductFactory::createClothing(601, "LimitedItem", 200.0, 2, "S", "Cotton"));
    Cart cart2(&inv2);

    // Add all available stock
    cart2.addProduct(601, 2);
    CHECK(cart2.getItemCount() == 2, "Added all available stock");

    // Now inventory has 0 left – trying to add more should throw
    bool threw2 = false;
    try {
        cart2.addProduct(601, 1);
    } catch (const ProductUnavailableException&) {
        threw2 = true;
    } catch (const InsufficientStockException&) {
        threw2 = true;
    } catch (const OutOfStockException&) {
        threw2 = true;
    }
    CHECK(threw2, "Adding when stock depleted throws stock exception");
}

// ─── 6. No Search Results Edge Case ──────────────────────────────────────────

void testNoSearchResultsEdgeCase() {
    std::cout << "\n[6] No Search Results Edge Case\n";

    Inventory inv;
    inv.initializeSampleData();

    // Search for a term that matches nothing
    auto results = inv.searchByName("xyznonexistent999");
    CHECK(results.empty(), "Search for non-existent term returns empty vector");
    CHECK(results.size() == 0, "Empty result has size 0");

    // Filter by a category that doesn't exist
    auto catResults = inv.filterByCategory("Furniture");
    CHECK(catResults.empty(), "Filter by non-existent category returns empty vector");

    // Empty search string – implementation-defined, but must not crash
    bool noThrow = true;
    std::vector<const Product*> emptySearch;
    try { emptySearch = inv.searchByName(""); } catch (...) { noThrow = false; }
    CHECK(noThrow, "Empty search string does not throw");

    // Verify that a valid search still works after empty/no-result searches
    auto validResults = inv.searchByName("Laptop");
    CHECK(!validResults.empty(), "Valid search still works after no-result searches");
    CHECK(validResults[0]->getName() == "Laptop", "Valid search returns correct product");
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "============================================\n";
    std::cout << "  Shopping Management System – Unit Tests  \n";
    std::cout << "============================================\n";

    testValidCouponCodes();
    testSampleDataInitialization();
    testPaymentMethodAvailability();
    testEmptyCartEdgeCase();
    testZeroStockEdgeCase();
    testNoSearchResultsEdgeCase();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed == 0) ? 0 : 1;
}
