/**
 * Simple tests for OrderHistory class
 * Validates: Requirements 15.4, 15.5, 15.6, 19.2, 19.3, 19.4, 19.5
 *
 * Property 23: Order History Persistence Round Trip
 * Property 25: Order History Display Completeness
 * Property 29: Most Purchased Product Calculation
 * Property 30: Total Revenue Calculation
 * Property 31: Total GST Calculation
 * Property 32: Sales Analytics Display Completeness
 */
#include "../include/OrderHistory.h"
#include "../include/Order.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdio>

static const std::string TMP_FILE = "data/test_orders_tmp.txt";

// Helper: build a simple order with one product
static Order makeOrder(const std::string& productName, int qty, double total,
                       const std::string& payment) {
    Inventory inv;
    inv.addProduct(std::make_unique<Book>(1, productName, total / qty, qty + 5,
                                         "Author", "Genre"));
    Cart cart(&inv);
    cart.addProduct(1, qty);
    return Order(cart, total, payment);
}

int main() {
    std::cout << "OrderHistory Simple Tests\n";
    std::cout << "=========================\n\n";

    int passed = 0;
    int failed = 0;

    auto check = [&](bool condition, const std::string& name) {
        if (condition) {
            std::cout << "[PASS] " << name << "\n";
            ++passed;
        } else {
            std::cout << "[FAIL] " << name << "\n";
            ++failed;
        }
    };

    // -------------------------------------------------------
    // Property 23: Order History Persistence Round Trip
    // -------------------------------------------------------
    std::cout << "\nProperty 23: Persistence Round Trip\n";
    {
        // Remove any leftover temp file
        std::remove(TMP_FILE.c_str());

        OrderHistory history1(TMP_FILE);

        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(1, "Laptop", 50000.0, 5, "Dell", 2));
        inv.addProduct(std::make_unique<Clothing>(2, "T-Shirt", 500.0, 10, "M", "Cotton"));
        inv.addProduct(std::make_unique<Book>(3, "Clean Code", 400.0, 8, "Martin", "Tech"));
        Cart cart(&inv);
        cart.addProduct(1, 1);
        cart.addProduct(2, 2);
        cart.addProduct(3, 1);

        Order o1(cart, 1000.0, "UPI");
        Order o2 = makeOrder("Phone", 1, 2000.0, "Card");
        Order o3 = makeOrder("Headphones", 2, 3000.0, "Cash");

        history1.addOrder(o1);
        history1.addOrder(o2);
        history1.addOrder(o3);
        history1.saveToFile();

        OrderHistory history2(TMP_FILE);
        history2.loadFromFile();

        check(history2.getOrders().size() == 3,
              "Loaded same number of orders (3)");
        check(std::abs(history2.getOrders()[0].getTotalAmount() - 1000.0) < 0.01,
              "First order total matches");
        check(std::abs(history2.getOrders()[1].getTotalAmount() - 2000.0) < 0.01,
              "Second order total matches");
        check(std::abs(history2.getOrders()[2].getTotalAmount() - 3000.0) < 0.01,
              "Third order total matches");
        check(history2.getOrders()[0].getPaymentMethod() == "UPI",
              "First order payment method matches");
        check(history2.getOrders()[1].getPaymentMethod() == "Card",
              "Second order payment method matches");

        std::remove(TMP_FILE.c_str());
    }

    // -------------------------------------------------------
    // Property 25: Order History Display Completeness
    // -------------------------------------------------------
    std::cout << "\nProperty 25: Display Completeness\n";
    {
        OrderHistory history;
        history.addOrder(makeOrder("Laptop", 1, 50000.0, "UPI"));
        history.addOrder(makeOrder("Phone", 2, 30000.0, "Card"));

        // Should not crash
        history.displayHistory();
        check(true, "displayHistory() with orders does not crash");

        OrderHistory emptyHistory;
        emptyHistory.displayHistory();
        check(true, "displayHistory() with no orders does not crash");
    }

    // -------------------------------------------------------
    // Property 29: Most Purchased Product Calculation
    // -------------------------------------------------------
    std::cout << "\nProperty 29: Most Purchased Product\n";
    {
        // Laptop: qty 3 total, Phone: qty 1 total
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(10, "Laptop", 50000.0, 10, "Dell", 2));
        inv.addProduct(std::make_unique<Electronics>(11, "Phone", 20000.0, 10, "Samsung", 1));
        Cart cart1(&inv);
        cart1.addProduct(10, 2);  // Laptop x2
        Order o1(cart1, 100000.0, "UPI");

        Inventory inv2;
        inv2.addProduct(std::make_unique<Electronics>(12, "Laptop", 50000.0, 10, "Dell", 2));
        inv2.addProduct(std::make_unique<Electronics>(13, "Phone", 20000.0, 10, "Samsung", 1));
        Cart cart2(&inv2);
        cart2.addProduct(12, 1);  // Laptop x1
        cart2.addProduct(13, 1);  // Phone x1
        Order o2(cart2, 70000.0, "Card");

        OrderHistory history;
        history.addOrder(o1);
        history.addOrder(o2);

        // Laptop total: 3, Phone total: 1
        check(history.getMostPurchasedProduct() == "Laptop",
              "Most purchased product is Laptop (qty 3 vs Phone qty 1)");
    }

    // -------------------------------------------------------
    // Property 29 edge: empty history returns "N/A"
    // -------------------------------------------------------
    {
        OrderHistory empty;
        check(empty.getMostPurchasedProduct() == "N/A",
              "getMostPurchasedProduct() returns N/A for empty history");
    }

    // -------------------------------------------------------
    // Property 30: Total Revenue Calculation
    // -------------------------------------------------------
    std::cout << "\nProperty 30: Total Revenue\n";
    {
        OrderHistory history;
        history.addOrder(makeOrder("Item1", 1, 1000.0, "UPI"));
        history.addOrder(makeOrder("Item2", 1, 2000.0, "Card"));

        check(std::abs(history.getTotalRevenue() - 3000.0) < 0.01,
              "Total revenue is 1000 + 2000 = 3000");
    }

    {
        OrderHistory empty;
        check(std::abs(empty.getTotalRevenue() - 0.0) < 0.01,
              "Total revenue is 0 for empty history");
    }

    // -------------------------------------------------------
    // Property 31: Total GST Calculation
    // -------------------------------------------------------
    std::cout << "\nProperty 31: Total GST\n";
    {
        OrderHistory history;
        history.addOrder(makeOrder("Item1", 1, 1000.0, "UPI"));
        history.addOrder(makeOrder("Item2", 1, 2000.0, "Card"));

        // GST = 3000 * 0.18 / 1.18 ≈ 457.627...
        double expectedGST = 3000.0 * 0.18 / 1.18;
        check(std::abs(history.getTotalGST() - expectedGST) < 0.01,
              "Total GST ≈ 457.63 for revenue of 3000");
    }

    // -------------------------------------------------------
    // Property 32: Sales Analytics Display Completeness
    // -------------------------------------------------------
    std::cout << "\nProperty 32: Sales Analytics Display\n";
    {
        OrderHistory history;
        history.addOrder(makeOrder("Laptop", 1, 50000.0, "UPI"));
        history.addOrder(makeOrder("Phone", 2, 30000.0, "Card"));

        history.displaySalesAnalytics();
        check(true, "displaySalesAnalytics() with orders does not crash");

        OrderHistory empty;
        empty.displaySalesAnalytics();
        check(true, "displaySalesAnalytics() with no orders does not crash");
    }

    // -------------------------------------------------------
    // Summary
    // -------------------------------------------------------
    std::cout << "\n=========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
