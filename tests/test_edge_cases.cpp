/**
 * Edge Case Test Matrix
 * **Validates: Requirements 30.2**
 *
 * Covers:
 *  - Empty cart operations
 *  - Empty cart checkout (Bill::calculate on empty cart)
 *  - Zero quantity add to cart
 *  - Negative quantity add to cart
 *  - Invalid product ID
 *  - Extremely large quantity (exceeds stock)
 *  - Extremely large price (no overflow)
 *  - Empty inventory operations
 *  - Duplicate product IDs in inventory
 *  - Cart with single item
 *  - Cart with maximum realistic items (10 products)
 */

#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Bill.h"
#include "../include/Coupon.h"
#include "../include/ProductFactory.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include "../include/StockException.h"
#include "../include/InvalidInputException.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <climits>
#include <sstream>

// ─── Simple test runner ───────────────────────────────────────────────────────
static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) \
    do { \
        if (cond) { \
            ++passed; \
            std::cout << "  [PASS] " << msg << "\n"; \
        } else { \
            ++failed; \
            std::cout << "  [FAIL] " << msg << "\n"; \
        } \
    } while (false)

// ─── Helper ──────────────────────────────────────────────────────────────────
static void initInventory(Inventory& inv) {
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop",      45000.0, 5,  "Dell",    2));
    inv.addProduct(ProductFactory::createElectronics(102, "Smartphone",  25000.0, 10, "Samsung", 1));
    inv.addProduct(ProductFactory::createElectronics(103, "Headphones",   3000.0, 15, "Sony",    1));
    inv.addProduct(ProductFactory::createClothing   (201, "T-Shirt",       500.0, 20, "M",       "Cotton"));
    inv.addProduct(ProductFactory::createClothing   (202, "Jeans",        1500.0, 15, "32",      "Denim"));
    inv.addProduct(ProductFactory::createClothing   (203, "Jacket",       3000.0, 8,  "L",       "Leather"));
    inv.addProduct(ProductFactory::createBook       (301, "C++ Programming", 600.0, 12, "Bjarne Stroustrup", "Technical"));
    inv.addProduct(ProductFactory::createBook       (302, "Clean Code",      800.0, 10, "Robert Martin",     "Technical"));
    inv.addProduct(ProductFactory::createBook       (303, "Pragmatic Programmer", 700.0, 8, "Hunt & Thomas", "Technical"));
}

// ─── 1. Empty cart ────────────────────────────────────────────────────────────
void testEmptyCart() {
    std::cout << "\n--- 1. Empty Cart ---\n";
    Inventory inv;
    Cart cart(&inv);

    CHECK(cart.isEmpty(),                          "isEmpty() == true on fresh cart");
    CHECK(cart.getItemCount() == 0,                "getItemCount() == 0 on fresh cart");
    CHECK(std::fabs(cart.calculateSubtotal())        < 1e-9, "calculateSubtotal() == 0 on empty cart");
    CHECK(std::fabs(cart.calculateProductDiscounts()) < 1e-9, "calculateProductDiscounts() == 0 on empty cart");
    CHECK(std::fabs(cart.calculateCartDiscount())     < 1e-9, "calculateCartDiscount() == 0 on empty cart");
}

// ─── 2. Empty cart checkout (Bill::calculate must not crash) ──────────────────
void testEmptyCartCheckout() {
    std::cout << "\n--- 2. Empty Cart Checkout ---\n";
    Inventory inv;
    Cart cart(&inv);

    bool threw = false;
    Bill bill;
    try {
        bill.calculate(cart, nullptr);
    } catch (...) {
        threw = true;
    }

    CHECK(!threw,                                  "Bill::calculate on empty cart does not throw");
    CHECK(std::fabs(bill.getSubtotal())   < 1e-9,  "Bill subtotal == 0 for empty cart");
    CHECK(std::fabs(bill.getFinalTotal()) < 1e-9,  "Bill finalTotal == 0 for empty cart");
}

// ─── 3. Zero quantity add to cart ────────────────────────────────────────────
void testZeroQuantityAdd() {
    std::cout << "\n--- 3. Zero Quantity Add ---\n";
    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    bool threw = false;
    try {
        cart.addProduct(101, 0);
    } catch (const InvalidQuantityException&) {
        threw = true;
    } catch (...) {
        threw = true;   // any exception counts as rejection
    }

    // Either throws OR returns false (cart stays empty)
    bool cartStillEmpty = cart.isEmpty();
    CHECK(threw || cartStillEmpty,
          "addProduct(id, 0) throws or leaves cart empty");
}

// ─── 4. Negative quantity ─────────────────────────────────────────────────────
void testNegativeQuantity() {
    std::cout << "\n--- 4. Negative Quantity ---\n";
    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    bool threw = false;
    try {
        cart.addProduct(101, -1);
    } catch (const InvalidQuantityException&) {
        threw = true;
    } catch (...) {
        threw = true;
    }

    bool cartStillEmpty = cart.isEmpty();
    CHECK(threw || cartStillEmpty,
          "addProduct(id, -1) throws or leaves cart empty");
}

// ─── 5. Invalid product ID ────────────────────────────────────────────────────
void testInvalidProductId() {
    std::cout << "\n--- 5. Invalid Product ID ---\n";
    Inventory inv;   // empty inventory
    Cart cart(&inv);

    bool threw = false;
    try {
        cart.addProduct(999999, 1);
    } catch (const InvalidProductIdException&) {
        threw = true;
    } catch (...) {
        threw = true;
    }

    bool cartStillEmpty = cart.isEmpty();
    CHECK(threw || cartStillEmpty,
          "addProduct(999999, 1) on empty inventory throws or leaves cart empty");
}

// ─── 6. Extremely large quantity (exceeds stock) ──────────────────────────────
void testExtremelyLargeQuantity() {
    std::cout << "\n--- 6. Extremely Large Quantity ---\n";
    Inventory inv;
    // Product 101 has stock = 5
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 5, "Dell", 2));
    Cart cart(&inv);

    bool threw = false;
    try {
        cart.addProduct(101, INT_MAX);
    } catch (const InsufficientStockException&) {
        threw = true;
    } catch (const OutOfStockException&) {
        threw = true;
    } catch (...) {
        threw = true;
    }

    bool cartStillEmpty = cart.isEmpty();
    CHECK(threw || cartStillEmpty,
          "addProduct(id, INT_MAX) when stock==5 throws or leaves cart empty");
}

// ─── 7. Extremely large price ─────────────────────────────────────────────────
void testExtremelyLargePrice() {
    std::cout << "\n--- 7. Extremely Large Price ---\n";
    Inventory inv;
    const double bigPrice = 1e15;
    inv.addProduct(ProductFactory::createElectronics(501, "SuperComputer", bigPrice, 10, "IBM", 5));
    Cart cart(&inv);

    cart.addProduct(501, 1);

    double subtotal = cart.calculateSubtotal();
    CHECK(std::isfinite(subtotal),                 "calculateSubtotal() is finite for price=1e15");
    CHECK(std::fabs(subtotal - bigPrice) < 1.0,    "calculateSubtotal() == 1e15 for single item");

    // Bill should also not overflow
    Bill bill;
    bool threw = false;
    try {
        bill.calculate(cart, nullptr);
    } catch (...) {
        threw = true;
    }
    CHECK(!threw,                                  "Bill::calculate does not throw for large price");
    CHECK(std::isfinite(bill.getFinalTotal()),      "Bill finalTotal is finite for large price");
}

// ─── 8. Empty inventory operations ───────────────────────────────────────────
void testEmptyInventory() {
    std::cout << "\n--- 8. Empty Inventory ---\n";
    Inventory inv;   // no products added

    Product* p = inv.findProduct(42);
    CHECK(p == nullptr,                            "findProduct() returns nullptr on empty inventory");

    auto results = inv.searchByName("anything");
    CHECK(results.empty(),                         "searchByName() returns empty on empty inventory");

    auto filtered = inv.filterByCategory("Electronics");
    CHECK(filtered.empty(),                        "filterByCategory() returns empty on empty inventory");

    // displayAllProducts must not crash
    bool threw = false;
    {
        std::stringstream buf;
        std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
        try {
            inv.displayAllProducts();
        } catch (...) {
            threw = true;
        }
        std::cout.rdbuf(old);
    }
    CHECK(!threw,                                  "displayAllProducts() does not crash on empty inventory");
}

// ─── 9. Duplicate product IDs in inventory ────────────────────────────────────
void testDuplicateProductIds() {
    std::cout << "\n--- 9. Duplicate Product IDs ---\n";
    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop",  45000.0, 5, "Dell",   2));
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop2", 50000.0, 3, "Lenovo", 1));

    // The inventory should either reject the duplicate or replace it.
    // Either way, findProduct(101) must return exactly one product (no crash).
    bool threw = false;
    Product* found = nullptr;
    try {
        found = inv.findProduct(101);
    } catch (...) {
        threw = true;
    }

    CHECK(!threw,                                  "findProduct after duplicate add does not throw");
    // At least one product with id 101 must be findable (or none if rejected)
    // The key invariant: no crash and the result is deterministic.
    CHECK(found == nullptr || found->getProductId() == 101,
          "findProduct(101) returns nullptr or a product with id==101");
}

// ─── 10. Cart with single item ────────────────────────────────────────────────
void testSingleItemCart() {
    std::cout << "\n--- 10. Single Item Cart ---\n";
    Inventory inv;
    // Book: 5% discount, price=600, qty=1
    inv.addProduct(ProductFactory::createBook(301, "C++ Programming", 600.0, 12, "Bjarne", "Technical"));
    Cart cart(&inv);

    cart.addProduct(301, 1);

    CHECK(!cart.isEmpty(),                         "Cart is not empty after adding one item");
    CHECK(cart.getItemCount() == 1,                "getItemCount() == 1 for single item");

    double subtotal = cart.calculateSubtotal();
    CHECK(std::fabs(subtotal - 600.0) < 0.01,      "Subtotal == 600 for single book");

    double discount = cart.calculateProductDiscounts();
    CHECK(std::fabs(discount - 30.0) < 0.01,       "Product discount == 30 (5% of 600)");

    // Subtotal 600 < 5000 → no cart discount
    double cartDisc = cart.calculateCartDiscount();
    CHECK(std::fabs(cartDisc) < 0.01,              "Cart discount == 0 when subtotal < 5000");
}

// ─── 11. Cart with 10 different products ─────────────────────────────────────
void testMaxRealisticCart() {
    std::cout << "\n--- 11. Cart with 10 Different Products ---\n";
    Inventory inv;
    initInventory(inv);
    // Add one more product to reach 10
    inv.addProduct(ProductFactory::createBook(304, "Design Patterns", 750.0, 10, "GoF", "Technical"));
    Cart cart(&inv);

    // Add all 10 products (1 unit each)
    int ids[] = {101, 102, 103, 201, 202, 203, 301, 302, 303, 304};
    for (int id : ids) {
        cart.addProduct(id, 1);
    }

    CHECK(cart.getItemCount() == 10,               "getItemCount() == 10 after adding 10 products");
    CHECK(!cart.isEmpty(),                         "Cart is not empty with 10 items");

    double subtotal = cart.calculateSubtotal();
    // Expected: 45000+25000+3000+500+1500+3000+600+800+700+750 = 80850
    CHECK(std::fabs(subtotal - 80850.0) < 0.01,    "Subtotal == 80850 for 10 products");

    double productDisc = cart.calculateProductDiscounts();
    // Electronics 10%: (45000+25000+3000)*0.10 = 7300
    // Clothing 20%:    (500+1500+3000)*0.20    = 1000
    // Books 5%:        (600+800+700+750)*0.05  = 142.5
    double expectedPD = 7300.0 + 1000.0 + 142.5;
    CHECK(std::fabs(productDisc - expectedPD) < 0.01,
          "Product discounts == 8442.50 for 10 products");

    // Subtotal > 5000 → 5% cart discount
    double cartDisc = cart.calculateCartDiscount();
    CHECK(std::fabs(cartDisc - subtotal * 0.05) < 0.01,
          "Cart discount == 5% of subtotal when subtotal > 5000");

    // Bill should compute without error
    Bill bill;
    bool threw = false;
    try {
        bill.calculate(cart, nullptr);
    } catch (...) {
        threw = true;
    }
    CHECK(!threw,                                  "Bill::calculate does not throw for 10-item cart");
    CHECK(std::isfinite(bill.getFinalTotal()),      "Bill finalTotal is finite for 10-item cart");
    CHECK(bill.getFinalTotal() > 0.0,              "Bill finalTotal > 0 for 10-item cart");
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "========================================\n";
    std::cout << "  Edge Case Test Matrix\n";
    std::cout << "  Validates: Requirements 30.2\n";
    std::cout << "========================================\n";

    testEmptyCart();
    testEmptyCartCheckout();
    testZeroQuantityAdd();
    testNegativeQuantity();
    testInvalidProductId();
    testExtremelyLargeQuantity();
    testExtremelyLargePrice();
    testEmptyInventory();
    testDuplicateProductIds();
    testSingleItemCart();
    testMaxRealisticCart();

    std::cout << "\n========================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "========================================\n";

    return (failed == 0) ? 0 : 1;
}
