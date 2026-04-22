/**
 * Integration tests for the complete checkout flow, admin flow, and persistence flow.
 * Tests end-to-end: add to cart → apply coupon → checkout → receipt → order history.
 * Admin flow: authenticate → add product → update price → update stock → remove product.
 * Persistence flow: create orders → save → reload → verify.
 *
 * Validates: Requirements 3.2, 3.3, 4.1, 5.1, 5.2, 6.1, 7.1, 7.5, 8.1, 9.1, 9.6,
 *            15.2, 15.3, 15.5, 15.6, 17.2, 17.5, 20.1, 20.2, 20.3, 20.4
 */

#include "../include/Inventory.h"
#include "../include/Cart.h"
#include "../include/Coupon.h"
#include "../include/Bill.h"
#include "../include/Order.h"
#include "../include/OrderHistory.h"
#include "../include/ReceiptWriter.h"
#include "../include/ProductFactory.h"
#include "../include/Admin.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

// ─── helpers ────────────────────────────────────────────────────────────────

static const std::string TMP_ORDERS = "data/test_integration_orders.txt";

static void initInventory(Inventory& inv) {
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop",    45000.0, 5,  "Dell",    2));
    inv.addProduct(ProductFactory::createElectronics(102, "Smartphone",25000.0, 10, "Samsung", 1));
    inv.addProduct(ProductFactory::createClothing   (201, "T-Shirt",     500.0, 20, "M",       "Cotton"));
    inv.addProduct(ProductFactory::createClothing   (202, "Jeans",      1500.0, 15, "32",      "Denim"));
    inv.addProduct(ProductFactory::createBook       (301, "Clean Code",  800.0, 10, "Martin",  "Technical"));
    inv.addProduct(ProductFactory::createBook       (302, "C++ Primer",  600.0,  8, "Lippman", "Technical"));
}

static int passed = 0;
static int failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "[PASS] " << name << "\n";
        ++passed;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        ++failed;
    }
}

// ─── Test 1: Add to cart → inventory decremented ────────────────────────────

static void testInventoryDecrementOnAdd() {
    std::cout << "\n--- Test 1: Inventory decremented on add ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    int before = inv.findProduct(101)->getQuantityAvailable();
    cart.addProduct(101, 2);
    int after = inv.findProduct(101)->getQuantityAvailable();

    check(after == before - 2, "Inventory decremented by quantity added");
    check(cart.getItemCount() == 2, "Cart item count matches quantity");
}

// ─── Test 2: Duplicate add increments quantity ───────────────────────────────

static void testDuplicateAddIncrementsQuantity() {
    std::cout << "\n--- Test 2: Duplicate add increments quantity ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    cart.addProduct(201, 2);
    size_t itemsBefore = cart.getItems().size();
    cart.addProduct(201, 3);
    size_t itemsAfter = cart.getItems().size();

    check(itemsBefore == itemsAfter, "No duplicate cart entry created");

    bool found = false;
    for (const auto& item : cart.getItems()) {
        if (item.product->getProductId() == 201) {
            check(item.quantity == 5, "Quantity incremented to 5 (2+3)");
            found = true;
            break;
        }
    }
    check(found, "Product 201 found in cart");
}

// ─── Test 3: Apply coupon SAVE10 ─────────────────────────────────────────────

static void testApplyCouponSAVE10() {
    std::cout << "\n--- Test 3: Apply coupon SAVE10 ---\n";

    Coupon coupon;
    bool ok = coupon.apply("SAVE10");

    check(ok, "SAVE10 coupon applied successfully");
    check(coupon.isApplied(), "Coupon reports as applied");
    check(std::fabs(coupon.getDiscountPercentage() - 0.10) < 1e-9,
          "SAVE10 discount is 10%");
    check(std::fabs(coupon.calculateDiscount(1000.0) - 100.0) < 0.01,
          "SAVE10 discount on 1000 = 100");
}

// ─── Test 4: Apply coupon FESTIVE20 ──────────────────────────────────────────

static void testApplyCouponFESTIVE20() {
    std::cout << "\n--- Test 4: Apply coupon FESTIVE20 ---\n";

    Coupon coupon;
    coupon.apply("FESTIVE20");

    check(coupon.isApplied(), "FESTIVE20 coupon applied");
    check(std::fabs(coupon.getDiscountPercentage() - 0.20) < 1e-9,
          "FESTIVE20 discount is 20%");
}

// ─── Test 5: Bill calculation with coupon ────────────────────────────────────

static void testBillCalculationWithCoupon() {
    std::cout << "\n--- Test 5: Bill calculation with coupon ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    // Laptop 45000 (Electronics 10% discount = 4500)
    cart.addProduct(101, 1);

    Coupon coupon;
    coupon.apply("SAVE10");

    Bill bill;
    bill.calculate(cart, &coupon);

    // subtotal = 45000
    // product discount = 4500  → after = 40500
    // cart discount (>5000) = 40500 * 0.05 = 2025 → after = 38475
    // coupon discount (10%) = 38475 * 0.10 = 3847.5 → after = 34627.5
    // GST (18%) = 34627.5 * 0.18 = 6232.95 → final = 40860.45

    check(std::fabs(bill.getSubtotal() - 45000.0) < 0.01,
          "Subtotal = 45000");
    check(bill.getFinalTotal() > 0.0, "Final total is positive");
    check(bill.getFinalTotal() < bill.getSubtotal(),
          "Final total is less than subtotal after discounts");
    check(bill.getGSTAmount() > 0.0, "GST amount is positive");
}

// ─── Test 6: Bill GST is 18% of post-discount amount ─────────────────────────

static void testBillGSTRate() {
    std::cout << "\n--- Test 6: GST is 18% of post-discount amount ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    // T-Shirt 500 (Clothing 20% discount = 100) → after = 400
    // No cart discount (subtotal < 5000), no coupon
    cart.addProduct(201, 1);

    Coupon coupon;
    Bill bill;
    bill.calculate(cart, &coupon);

    // subtotal = 500, product discount = 100, after = 400
    // cart discount = 0 (< 5000), coupon = 0
    // GST = 400 * 0.18 = 72, final = 472
    double expectedGST = 400.0 * 0.18;
    double expectedFinal = 400.0 + expectedGST;

    check(std::fabs(bill.getGSTAmount() - expectedGST) < 0.01,
          "GST = 18% of 400 = 72");
    check(std::fabs(bill.getFinalTotal() - expectedFinal) < 0.01,
          "Final total = 400 + 72 = 472");
}

// ─── Test 7: Order created on checkout ───────────────────────────────────────

static void testOrderCreatedOnCheckout() {
    std::cout << "\n--- Test 7: Order created on checkout ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    cart.addProduct(301, 2);  // Clean Code x2

    Coupon coupon;
    Bill bill;
    bill.calculate(cart, &coupon);

    Order order(cart, bill.getFinalTotal(), "UPI");

    check(order.getOrderId() > 0, "Order has positive ID");
    check(order.getPaymentMethod() == "UPI", "Payment method stored");
    check(std::fabs(order.getTotalAmount() - bill.getFinalTotal()) < 0.01,
          "Order total matches bill final total");
    check(!order.getProducts().empty(), "Order contains products");
    check(!order.getOrderDate().empty(), "Order has a date");
}

// ─── Test 8: Order saved to history ──────────────────────────────────────────

static void testOrderSavedToHistory() {
    std::cout << "\n--- Test 8: Order saved to history ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);
    cart.addProduct(102, 1);

    Coupon coupon;
    Bill bill;
    bill.calculate(cart, &coupon);

    Order order(cart, bill.getFinalTotal(), "Card");

    OrderHistory history(TMP_ORDERS);
    history.addOrder(order);

    check(history.getOrders().size() == 1, "History contains 1 order");
    check(std::fabs(history.getOrders()[0].getTotalAmount() - order.getTotalAmount()) < 0.01,
          "Stored order total matches");
}

// ─── Test 9: Order history persistence round-trip ────────────────────────────

static void testOrderHistoryPersistence() {
    std::cout << "\n--- Test 9: Order history persistence round-trip ---\n";

    std::remove(TMP_ORDERS.c_str());

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);
    cart.addProduct(101, 1);

    Coupon coupon;
    Bill bill;
    bill.calculate(cart, &coupon);

    Order order(cart, bill.getFinalTotal(), "Cash");

    {
        OrderHistory history(TMP_ORDERS);
        history.addOrder(order);
        history.saveToFile();
    }

    {
        OrderHistory loaded(TMP_ORDERS);
        loaded.loadFromFile();

        check(loaded.getOrders().size() == 1, "Loaded 1 order from file");
        check(std::fabs(loaded.getOrders()[0].getTotalAmount() - order.getTotalAmount()) < 0.01,
              "Loaded order total matches original");
        check(loaded.getOrders()[0].getPaymentMethod() == "Cash",
              "Loaded order payment method matches");
    }

    std::remove(TMP_ORDERS.c_str());
}

// ─── Test 10: Receipt file created ───────────────────────────────────────────

static void testReceiptFileCreated() {
    std::cout << "\n--- Test 10: Receipt file created ---\n";

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);
    cart.addProduct(302, 1);  // C++ Primer

    Coupon coupon;
    Bill bill;
    bill.calculate(cart, &coupon);

    Order order(cart, bill.getFinalTotal(), "UPI");

    std::string receipt = ReceiptWriter::generateReceipt(order, bill);
    std::string path = ReceiptWriter::getReceiptFilename(order.getOrderId());

    // Clean up any pre-existing file
    std::remove(path.c_str());

    bool saved = ReceiptWriter::saveReceipt(receipt, order.getOrderId());
    check(saved, "saveReceipt() returns true");

    std::ifstream f(path);
    check(f.good(), "Receipt file exists at expected path");

    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    check(!content.empty(), "Receipt file is non-empty");
    check(content.find("C++ Primer") != std::string::npos,
          "Receipt contains product name");

    std::remove(path.c_str());
}

// ─── Test 11: Full end-to-end checkout flow ───────────────────────────────────

static void testFullCheckoutFlow() {
    std::cout << "\n--- Test 11: Full end-to-end checkout flow ---\n";

    std::remove(TMP_ORDERS.c_str());

    Inventory inv;
    initInventory(inv);
    Cart cart(&inv);

    // Step 1: Add items
    int laptopStockBefore = inv.findProduct(101)->getQuantityAvailable();
    int shirtStockBefore  = inv.findProduct(201)->getQuantityAvailable();

    cart.addProduct(101, 1);
    cart.addProduct(201, 2);

    check(inv.findProduct(101)->getQuantityAvailable() == laptopStockBefore - 1,
          "Laptop stock decremented by 1");
    check(inv.findProduct(201)->getQuantityAvailable() == shirtStockBefore - 2,
          "T-Shirt stock decremented by 2");

    // Step 2: Apply coupon
    Coupon coupon;
    coupon.apply("FESTIVE20");
    check(coupon.isApplied(), "Coupon FESTIVE20 applied");

    // Step 3: Calculate bill
    Bill bill;
    bill.calculate(cart, &coupon);
    check(bill.getFinalTotal() > 0.0, "Bill final total is positive");

    // Step 4: Create order
    Order order(cart, bill.getFinalTotal(), "UPI");
    check(order.getOrderId() > 0, "Order created with valid ID");

    // Step 5: Save to history
    OrderHistory history(TMP_ORDERS);
    history.addOrder(order);
    history.saveToFile();

    // Step 6: Generate receipt
    std::string receipt = ReceiptWriter::generateReceipt(order, bill);
    std::string receiptPath = ReceiptWriter::getReceiptFilename(order.getOrderId());
    std::remove(receiptPath.c_str());
    bool saved = ReceiptWriter::saveReceipt(receipt, order.getOrderId());
    check(saved, "Receipt saved successfully");

    std::ifstream f(receiptPath);
    check(f.good(), "Receipt file exists");
    f.close();

    // Step 7: Verify persistence
    OrderHistory loaded(TMP_ORDERS);
    loaded.loadFromFile();
    check(loaded.getOrders().size() == 1, "Order persisted to file");

    // Step 8: Clear cart
    cart.clear();
    check(cart.isEmpty(), "Cart cleared after checkout");

    // Cleanup
    std::remove(receiptPath.c_str());
    std::remove(TMP_ORDERS.c_str());
}

// ─── Test 12: Admin authentication ───────────────────────────────────────────

static void testAdminAuthentication() {
    std::cout << "\n--- Test 12: Admin authentication ---\n";

    Inventory inv;
    Admin admin(&inv);

    check(!admin.isAuthenticated(), "Admin not authenticated initially");

    bool badLogin = admin.authenticate("admin", "wrongpass");
    check(!badLogin, "Wrong password rejected");
    check(!admin.isAuthenticated(), "Still not authenticated after bad login");

    bool goodLogin = admin.authenticate("admin", "admin123");
    check(goodLogin, "Valid credentials accepted");
    check(admin.isAuthenticated(), "Admin is authenticated after valid login");
}

// ─── Test 13: Admin flow – add, update price, update stock, remove ────────────

static void testAdminInventoryFlow() {
    std::cout << "\n--- Test 13: Admin inventory management flow ---\n";

    Inventory inv;
    initInventory(inv);
    Admin admin(&inv);

    // Authenticate
    bool ok = admin.authenticate("admin", "admin123");
    check(ok, "Admin authenticated");

    // Add a new product via Inventory (simulates admin addNewProduct action)
    inv.addProduct(ProductFactory::createBook(999, "Design Patterns", 1200.0, 5, "GoF", "Technical"));
    check(inv.findProduct(999) != nullptr, "New product added to inventory");
    check(inv.findProduct(999)->getName() == "Design Patterns", "New product name correct");
    check(std::fabs(inv.findProduct(999)->getPrice() - 1200.0) < 0.01, "New product price correct");
    check(inv.findProduct(999)->getQuantityAvailable() == 5, "New product stock correct");

    // Update price (simulates admin updateProductPrice action)
    bool priceUpdated = inv.updatePrice(999, 950.0);
    check(priceUpdated, "Price update returns true");
    check(std::fabs(inv.findProduct(999)->getPrice() - 950.0) < 0.01, "Price updated to 950");

    // Update stock (simulates admin updateProductStock action)
    bool stockUpdated = inv.updateStock(999, 20);
    check(stockUpdated, "Stock update returns true");
    check(inv.findProduct(999)->getQuantityAvailable() == 20, "Stock updated to 20");

    // Update non-existent product returns false
    bool badPrice = inv.updatePrice(9999, 100.0);
    check(!badPrice, "Price update for non-existent product returns false");
    bool badStock = inv.updateStock(9999, 10);
    check(!badStock, "Stock update for non-existent product returns false");

    // Remove product (simulates admin removeProduct action)
    bool removed = inv.removeProduct(999);
    check(removed, "Product removed successfully");
    check(inv.findProduct(999) == nullptr, "Product no longer in inventory after removal");

    // Remove non-existent product returns false
    bool badRemove = inv.removeProduct(999);
    check(!badRemove, "Removing non-existent product returns false");
}

// ─── Test 14: Persistence flow – multiple orders, save, reload ───────────────

static void testPersistenceMultipleOrders() {
    std::cout << "\n--- Test 14: Persistence flow – multiple orders ---\n";

    static const std::string TMP_MULTI = "data/test_integration_multi.txt";
    std::remove(TMP_MULTI.c_str());

    Inventory inv;
    initInventory(inv);

    // Order 1: Laptop + SAVE10 coupon, Cash
    Cart cart1(&inv);
    cart1.addProduct(101, 1);
    Coupon coupon1;
    coupon1.apply("SAVE10");
    Bill bill1;
    bill1.calculate(cart1, &coupon1);
    Order order1(cart1, bill1.getFinalTotal(), "Cash");

    // Order 2: T-Shirt + Jeans, no coupon, Card
    Cart cart2(&inv);
    cart2.addProduct(201, 2);
    cart2.addProduct(202, 1);
    Coupon coupon2;
    Bill bill2;
    bill2.calculate(cart2, &coupon2);
    Order order2(cart2, bill2.getFinalTotal(), "Card");

    // Save both orders
    {
        OrderHistory history(TMP_MULTI);
        history.addOrder(order1);
        history.addOrder(order2);
        history.saveToFile();
    }

    // Reload and verify
    {
        OrderHistory loaded(TMP_MULTI);
        loaded.loadFromFile();

        check(loaded.getOrders().size() == 2, "Loaded 2 orders from file");
        check(std::fabs(loaded.getOrders()[0].getTotalAmount() - order1.getTotalAmount()) < 0.01,
              "First order total matches after reload");
        check(loaded.getOrders()[0].getPaymentMethod() == "Cash",
              "First order payment method matches after reload");
        check(std::fabs(loaded.getOrders()[1].getTotalAmount() - order2.getTotalAmount()) < 0.01,
              "Second order total matches after reload");
        check(loaded.getOrders()[1].getPaymentMethod() == "Card",
              "Second order payment method matches after reload");

        // Verify analytics work on loaded data
        double revenue = loaded.getTotalRevenue();
        check(revenue > 0.0, "Total revenue is positive");
        check(std::fabs(revenue - (order1.getTotalAmount() + order2.getTotalAmount())) < 0.01,
              "Total revenue equals sum of order totals");
    }

    std::remove(TMP_MULTI.c_str());
}

// ─── main ────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== Integration Tests: Checkout, Admin, and Persistence Flows ===\n";

    testInventoryDecrementOnAdd();
    testDuplicateAddIncrementsQuantity();
    testApplyCouponSAVE10();
    testApplyCouponFESTIVE20();
    testBillCalculationWithCoupon();
    testBillGSTRate();
    testOrderCreatedOnCheckout();
    testOrderSavedToHistory();
    testOrderHistoryPersistence();
    testReceiptFileCreated();
    testFullCheckoutFlow();
    testAdminAuthentication();
    testAdminInventoryFlow();
    testPersistenceMultipleOrders();

    std::cout << "\n=================================================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
