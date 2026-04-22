/**
 * Simple tests for Order class
 * Validates: Requirements 15.2, 15.3
 *
 * Property 24: Order Creation on Checkout
 */
#include "../include/Order.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <cassert>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "Order Class Simple Tests\n";
    std::cout << "========================\n\n";

    int passed = 0;
    int failed = 0;

    // Helper lambda
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
    // Test 1: Order created with correct total amount
    // -------------------------------------------------------
    std::cout << "\nTest 1: Order total amount\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(1, "Laptop", 50000.0, 5, "Dell", 2));
        Cart cart(&inv);
        cart.addProduct(1, 1);

        Order order(cart, 50150.0, "UPI");
        check(order.getTotalAmount() == 50150.0, "Total amount stored correctly");
    }

    // -------------------------------------------------------
    // Test 2: Order captures all products and quantities
    // -------------------------------------------------------
    std::cout << "\nTest 2: Products and quantities captured\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(2, "Smartphone", 20000.0, 10, "Samsung", 1));
        inv.addProduct(std::make_unique<Clothing>(3, "T-Shirt", 500.0, 20, "M", "Cotton"));
        Cart cart(&inv);
        cart.addProduct(2, 2);
        cart.addProduct(3, 3);

        Order order(cart, 99.99, "Card");
        const auto& prods = order.getProducts();

        check(prods.size() == 2, "Two products captured");
        check(prods[0].first == "Smartphone" && prods[0].second == 2,
              "First product name and quantity correct");
        check(prods[1].first == "T-Shirt" && prods[1].second == 3,
              "Second product name and quantity correct");
    }

    // -------------------------------------------------------
    // Test 3: Order gets a unique incrementing ID
    // -------------------------------------------------------
    std::cout << "\nTest 3: Unique incrementing order IDs\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Book>(4, "Clean Code", 400.0, 10, "Martin", "Tech"));
        Cart cart(&inv);
        cart.addProduct(4, 1);

        Order o1(cart, 100.0, "Cash");
        int id1 = o1.getOrderId();

        // Reset cart for second order
        Inventory inv2;
        inv2.addProduct(std::make_unique<Book>(5, "Pragmatic Programmer", 450.0, 10, "Hunt", "Tech"));
        Cart cart2(&inv2);
        cart2.addProduct(5, 1);

        Order o2(cart2, 200.0, "UPI");
        int id2 = o2.getOrderId();

        check(id2 == id1 + 1, "Second order ID is one more than first");
        check(id1 != id2, "Order IDs are unique");
    }

    // -------------------------------------------------------
    // Test 4: Order date/time strings are non-empty
    // -------------------------------------------------------
    std::cout << "\nTest 4: Date and time are non-empty\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(6, "Headphones", 3000.0, 5, "Sony", 1));
        Cart cart(&inv);
        cart.addProduct(6, 1);

        Order order(cart, 3540.0, "Card");
        check(!order.getOrderDate().empty(), "Order date is non-empty");
        check(!order.getOrderTime().empty(), "Order time is non-empty");
        // Date format: DD/MM/YYYY (10 chars)
        check(order.getOrderDate().size() == 10, "Order date has correct length (DD/MM/YYYY)");
        // Time format: HH:MM:SS (8 chars)
        check(order.getOrderTime().size() == 8, "Order time has correct length (HH:MM:SS)");
    }

    // -------------------------------------------------------
    // Test 5: serialize/deserialize round-trip
    // -------------------------------------------------------
    std::cout << "\nTest 5: Serialize/deserialize round-trip\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(7, "Laptop", 45000.0, 3, "HP", 2));
        inv.addProduct(std::make_unique<Clothing>(8, "Jeans", 1500.0, 10, "L", "Denim"));
        Cart cart(&inv);
        cart.addProduct(7, 1);
        cart.addProduct(8, 2);

        Order original(cart, 55000.0, "UPI");
        std::string serialized = original.serialize();

        Order restored = Order::deserialize(serialized);

        check(restored.getOrderId() == original.getOrderId(), "Round-trip: orderId preserved");
        check(restored.getTotalAmount() == original.getTotalAmount(), "Round-trip: totalAmount preserved");
        check(restored.getPaymentMethod() == original.getPaymentMethod(), "Round-trip: paymentMethod preserved");
        check(restored.getOrderDate() == original.getOrderDate(), "Round-trip: orderDate preserved");
        check(restored.getOrderTime() == original.getOrderTime(), "Round-trip: orderTime preserved");
        check(restored.getProducts().size() == original.getProducts().size(), "Round-trip: product count preserved");

        if (restored.getProducts().size() == original.getProducts().size()) {
            bool productsMatch = true;
            for (std::size_t i = 0; i < original.getProducts().size(); ++i) {
                if (restored.getProducts()[i].first != original.getProducts()[i].first ||
                    restored.getProducts()[i].second != original.getProducts()[i].second) {
                    productsMatch = false;
                    break;
                }
            }
            check(productsMatch, "Round-trip: product names and quantities preserved");
        }
    }

    // -------------------------------------------------------
    // Test 6: Multiple orders get different IDs
    // -------------------------------------------------------
    std::cout << "\nTest 6: Multiple orders have different IDs\n";
    {
        std::vector<int> ids;
        for (int i = 0; i < 5; ++i) {
            Inventory inv;
            inv.addProduct(std::make_unique<Book>(100 + i, "Book" + std::to_string(i), 300.0, 5, "Author", "Genre"));
            Cart cart(&inv);
            cart.addProduct(100 + i, 1);
            Order o(cart, 354.0, "Cash");
            ids.push_back(o.getOrderId());
        }

        bool allUnique = true;
        for (std::size_t i = 0; i < ids.size(); ++i) {
            for (std::size_t j = i + 1; j < ids.size(); ++j) {
                if (ids[i] == ids[j]) { allUnique = false; break; }
            }
        }
        check(allUnique, "All 5 orders have unique IDs");

        bool allIncrementing = true;
        for (std::size_t i = 1; i < ids.size(); ++i) {
            if (ids[i] != ids[i-1] + 1) { allIncrementing = false; break; }
        }
        check(allIncrementing, "Order IDs are strictly incrementing");
    }

    // -------------------------------------------------------
    // Test 7: display() runs without crashing
    // -------------------------------------------------------
    std::cout << "\nTest 7: display() runs without error\n";
    {
        Inventory inv;
        inv.addProduct(std::make_unique<Electronics>(200, "Monitor", 15000.0, 2, "LG", 1));
        Cart cart(&inv);
        cart.addProduct(200, 1);
        Order order(cart, 17700.0, "Card");
        order.display();
        check(true, "display() completed without crash");
    }

    // -------------------------------------------------------
    // Summary
    // -------------------------------------------------------
    std::cout << "\n========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
