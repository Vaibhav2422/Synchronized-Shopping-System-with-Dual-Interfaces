/**
 * Simple tests for ReceiptWriter class
 * Validates: Requirements 9.1, 9.2, 9.3, 9.4, 9.5, 9.6, 9.7
 *
 * Property 21: Receipt File Generation
 * Property 22: Receipt Content Completeness
 * Property 50: Receipt Filename Display
 */
#include "../include/ReceiptWriter.h"
#include "../include/Order.h"
#include "../include/Bill.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Electronics.h"
#include "../include/Book.h"
#include "../include/Coupon.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>

int main() {
    std::cout << "ReceiptWriter Simple Tests\n";
    std::cout << "==========================\n\n";

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
    // Property 50: Receipt Filename Display
    // -------------------------------------------------------
    std::cout << "\nProperty 50: Receipt Filename Display\n";
    {
        std::string filename = ReceiptWriter::getReceiptFilename(42);
        check(filename == "data/receipts/receipt_0042.txt",
              "getReceiptFilename(42) == \"data/receipts/receipt_0042.txt\"");

        check(ReceiptWriter::getReceiptFilename(1) == "data/receipts/receipt_0001.txt",
              "getReceiptFilename(1) zero-pads to 4 digits");

        check(ReceiptWriter::getReceiptFilename(1000) == "data/receipts/receipt_1000.txt",
              "getReceiptFilename(1000) uses 4 digits exactly");
    }

    // Build a shared order and bill for the remaining tests
    Inventory inv;
    inv.addProduct(std::make_unique<Electronics>(1, "Laptop", 50000.0, 5, "Dell", 2));
    inv.addProduct(std::make_unique<Book>(2, "Clean Code", 400.0, 8, "Martin", "Tech"));
    Cart cart(&inv);
    cart.addProduct(1, 1);
    cart.addProduct(2, 2);

    Order order(cart, 0.0, "UPI");

    Bill bill;
    Coupon coupon;
    bill.calculate(cart, &coupon);

    // -------------------------------------------------------
    // Property 22: Receipt Content Completeness
    // -------------------------------------------------------
    std::cout << "\nProperty 22: Receipt Content Completeness\n";
    {
        std::string receipt = ReceiptWriter::generateReceipt(order, bill);

        // Must contain order ID
        check(receipt.find(std::to_string(order.getOrderId())) != std::string::npos,
              "Receipt contains order ID");

        // Must contain date
        check(receipt.find(order.getOrderDate()) != std::string::npos,
              "Receipt contains order date");

        // Must contain payment method
        check(receipt.find(order.getPaymentMethod()) != std::string::npos,
              "Receipt contains payment method");

        // Must contain at least one product name
        bool hasProduct = false;
        for (const auto& item : order.getProducts()) {
            if (receipt.find(item.first) != std::string::npos) {
                hasProduct = true;
                break;
            }
        }
        check(hasProduct, "Receipt contains at least one product name");

        // Must contain "Thank you"
        check(receipt.find("Thank you") != std::string::npos,
              "Receipt contains \"Thank you\"");

        // Must contain the title
        check(receipt.find("SHOPPING MANAGEMENT SYSTEM - RECEIPT") != std::string::npos,
              "Receipt contains title");
    }

    // -------------------------------------------------------
    // Property 21: Receipt File Generation
    // -------------------------------------------------------
    std::cout << "\nProperty 21: Receipt File Generation\n";
    {
        std::string receipt = ReceiptWriter::generateReceipt(order, bill);
        int orderId = order.getOrderId();
        std::string expectedPath = ReceiptWriter::getReceiptFilename(orderId);

        // Remove any pre-existing file
        std::remove(expectedPath.c_str());

        bool saved = ReceiptWriter::saveReceipt(receipt, orderId);
        check(saved, "saveReceipt() returns true on success");

        // Verify file exists
        std::ifstream f(expectedPath);
        check(f.good(), "Receipt file exists at expected path");

        // Verify file is non-empty
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        check(!content.empty(), "Receipt file is non-empty");

        // Verify content matches what was written
        check(content == receipt, "Receipt file content matches generated receipt");

        // Cleanup
        std::remove(expectedPath.c_str());
    }

    // -------------------------------------------------------
    // Summary
    // -------------------------------------------------------
    std::cout << "\n==========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
