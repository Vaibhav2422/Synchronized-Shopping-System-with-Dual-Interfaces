/**
 * test_exceptions.cpp
 * Comprehensive unit tests for all exception classes in the Shopping Management System.
 * Tests: ShoppingException, InvalidInputException hierarchy, StockException hierarchy,
 *        PaymentException hierarchy, CouponException hierarchy, FileIOException hierarchy.
 * Requirements: 30.1, 30.2, 30.3, 30.4
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "../include/ShoppingException.h"
#include "../include/InvalidInputException.h"
#include "../include/StockException.h"
#include "../include/PaymentException.h"
#include "../include/CouponException.h"
#include "../include/FileIOException.h"

// ─── Test result tracking ────────────────────────────────────────────────────

static int passed = 0;
static int failed = 0;

#define PASS(msg) do { std::cout << "  [PASS] " << msg << std::endl; ++passed; } while(0)
#define FAIL(msg) do { std::cout << "  [FAIL] " << msg << std::endl; ++failed; } while(0)
#define CHECK(cond, msg) do { if (cond) { PASS(msg); } else { FAIL(msg); } } while(0)

// ─── 1. ShoppingException base class ─────────────────────────────────────────

void testShoppingException() {
    std::cout << "\n[1] ShoppingException base class\n";

    // Default error code
    {
        ShoppingException ex("Something went wrong");
        CHECK(std::string(ex.what()) == "Something went wrong", "what() returns correct message");
        CHECK(ex.getErrorCode() == "SHOP_ERR", "Default error code is SHOP_ERR");
        CHECK(!ex.getTimestamp().empty(), "Timestamp is non-empty");
        CHECK(ex.getDetailedMessage() == "[SHOP_ERR] Something went wrong",
              "getDetailedMessage() formats correctly");
    }

    // Custom error code
    {
        ShoppingException ex("Custom error", "CUSTOM_CODE");
        CHECK(ex.getErrorCode() == "CUSTOM_CODE", "Custom error code stored correctly");
        CHECK(ex.getDetailedMessage() == "[CUSTOM_CODE] Custom error",
              "getDetailedMessage() uses custom code");
    }

    // Empty message
    {
        ShoppingException ex("", "EMPTY_MSG");
        CHECK(std::string(ex.what()) == "", "Empty message stored correctly");
        CHECK(ex.getDetailedMessage() == "[EMPTY_MSG] ", "getDetailedMessage() handles empty message");
    }

    // Catchable as std::runtime_error
    {
        bool caught = false;
        try {
            throw ShoppingException("runtime error test");
        } catch (const std::runtime_error&) {
            caught = true;
        }
        CHECK(caught, "ShoppingException is catchable as std::runtime_error");
    }

    // Catchable as std::exception
    {
        bool caught = false;
        try {
            throw ShoppingException("exception test");
        } catch (const std::exception&) {
            caught = true;
        }
        CHECK(caught, "ShoppingException is catchable as std::exception");
    }
}

// ─── 2. InvalidInputException hierarchy ──────────────────────────────────────

void testInvalidInputException() {
    std::cout << "\n[2] InvalidInputException hierarchy\n";

    // InvalidInputException base
    {
        InvalidInputException ex("bad input");
        CHECK(std::string(ex.what()) == "bad input", "InvalidInputException message correct");
        CHECK(ex.getErrorCode() == "INPUT_ERR", "Error code is INPUT_ERR");
        bool caught = false;
        try { throw InvalidInputException("test"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "InvalidInputException catchable as ShoppingException");
    }

    // InvalidProductIdException
    {
        InvalidProductIdException ex(42);
        CHECK(std::string(ex.what()) == "Invalid product ID: 42", "Message contains product ID");
        CHECK(ex.getProductId() == 42, "getProductId() returns correct value");
        CHECK(ex.getErrorCode() == "INPUT_ERR", "Error code is INPUT_ERR");

        // Negative ID
        InvalidProductIdException negEx(-1);
        CHECK(negEx.getProductId() == -1, "Negative product ID stored correctly");
        CHECK(std::string(negEx.what()) == "Invalid product ID: -1", "Negative ID in message");

        // Zero ID
        InvalidProductIdException zeroEx(0);
        CHECK(zeroEx.getProductId() == 0, "Zero product ID stored correctly");

        // Large ID
        InvalidProductIdException largeEx(999999);
        CHECK(largeEx.getProductId() == 999999, "Large product ID stored correctly");

        // Catchable as InvalidInputException
        bool caught = false;
        try { throw InvalidProductIdException(1); } catch (const InvalidInputException&) { caught = true; }
        CHECK(caught, "InvalidProductIdException catchable as InvalidInputException");

        // Catchable as ShoppingException
        caught = false;
        try { throw InvalidProductIdException(1); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "InvalidProductIdException catchable as ShoppingException");
    }

    // InvalidQuantityException
    {
        InvalidQuantityException ex(5);
        CHECK(std::string(ex.what()) == "Invalid quantity: 5", "Message contains quantity");
        CHECK(ex.getRequestedQuantity() == 5, "getRequestedQuantity() returns correct value");
        CHECK(ex.getErrorCode() == "INPUT_ERR", "Error code is INPUT_ERR");

        // Zero quantity
        InvalidQuantityException zeroEx(0);
        CHECK(zeroEx.getRequestedQuantity() == 0, "Zero quantity stored correctly");

        // Negative quantity
        InvalidQuantityException negEx(-3);
        CHECK(negEx.getRequestedQuantity() == -3, "Negative quantity stored correctly");
        CHECK(std::string(negEx.what()) == "Invalid quantity: -3", "Negative quantity in message");

        // Catchable as InvalidInputException
        bool caught = false;
        try { throw InvalidQuantityException(1); } catch (const InvalidInputException&) { caught = true; }
        CHECK(caught, "InvalidQuantityException catchable as InvalidInputException");
    }

    // InvalidMenuChoiceException
    {
        InvalidMenuChoiceException ex(7);
        CHECK(std::string(ex.what()) == "Invalid menu choice: 7", "Message contains choice");
        CHECK(ex.getChoice() == 7, "getChoice() returns correct value");
        CHECK(ex.getErrorCode() == "INPUT_ERR", "Error code is INPUT_ERR");

        // Negative choice
        InvalidMenuChoiceException negEx(-1);
        CHECK(negEx.getChoice() == -1, "Negative choice stored correctly");

        // Zero choice
        InvalidMenuChoiceException zeroEx(0);
        CHECK(zeroEx.getChoice() == 0, "Zero choice stored correctly");

        // Catchable as InvalidInputException
        bool caught = false;
        try { throw InvalidMenuChoiceException(99); } catch (const InvalidInputException&) { caught = true; }
        CHECK(caught, "InvalidMenuChoiceException catchable as InvalidInputException");
    }
}

// ─── 3. StockException hierarchy ─────────────────────────────────────────────

void testStockException() {
    std::cout << "\n[3] StockException hierarchy\n";

    // OutOfStockException base
    {
        OutOfStockException ex("Widget", 3);
        CHECK(std::string(ex.what()) == "Product out of stock: Widget", "Message contains product name");
        CHECK(ex.getProductName() == "Widget", "getProductName() correct");
        CHECK(ex.getAvailableQuantity() == 3, "getAvailableQuantity() correct");
        CHECK(ex.getErrorCode() == "STOCK_ERR", "Error code is STOCK_ERR");

        // Catchable as ShoppingException
        bool caught = false;
        try { throw OutOfStockException("X", 0); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "OutOfStockException catchable as ShoppingException");

        // Zero available
        OutOfStockException zeroEx("ZeroItem", 0);
        CHECK(zeroEx.getAvailableQuantity() == 0, "Zero available quantity stored correctly");

        // Empty product name
        OutOfStockException emptyEx("", 5);
        CHECK(emptyEx.getProductName() == "", "Empty product name stored correctly");
    }

    // ProductUnavailableException
    {
        ProductUnavailableException ex("Laptop");
        CHECK(std::string(ex.what()) == "Product out of stock: Laptop", "Message correct");
        CHECK(ex.getProductName() == "Laptop", "getProductName() correct");
        CHECK(ex.getAvailableQuantity() == 0, "Available quantity is 0 for unavailable product");
        CHECK(ex.getErrorCode() == "STOCK_ERR", "Error code is STOCK_ERR");

        // Catchable as OutOfStockException
        bool caught = false;
        try { throw ProductUnavailableException("X"); } catch (const OutOfStockException&) { caught = true; }
        CHECK(caught, "ProductUnavailableException catchable as OutOfStockException");

        // Catchable as ShoppingException
        caught = false;
        try { throw ProductUnavailableException("X"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "ProductUnavailableException catchable as ShoppingException");
    }

    // InsufficientStockException
    {
        InsufficientStockException ex("Phone", 10, 3);
        CHECK(ex.getProductName() == "Phone", "getProductName() correct");
        CHECK(ex.getRequestedQuantity() == 10, "getRequestedQuantity() correct");
        CHECK(ex.getAvailableQuantity() == 3, "getAvailableQuantity() correct");
        CHECK(ex.getErrorCode() == "STOCK_ERR", "Error code is STOCK_ERR");

        std::string detailed = ex.getDetailedMessage();
        CHECK(detailed.find("Phone") != std::string::npos, "Detailed message contains product name");
        CHECK(detailed.find("10") != std::string::npos, "Detailed message contains requested qty");
        CHECK(detailed.find("3") != std::string::npos, "Detailed message contains available qty");
        CHECK(detailed.find("STOCK_ERR") != std::string::npos, "Detailed message contains error code");

        // Catchable as OutOfStockException
        bool caught = false;
        try { throw InsufficientStockException("X", 5, 2); } catch (const OutOfStockException&) { caught = true; }
        CHECK(caught, "InsufficientStockException catchable as OutOfStockException");

        // Catchable as ShoppingException
        caught = false;
        try { throw InsufficientStockException("X", 5, 2); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "InsufficientStockException catchable as ShoppingException");

        // Boundary: requested == available (edge case)
        InsufficientStockException edgeEx("Edge", 5, 5);
        CHECK(edgeEx.getRequestedQuantity() == 5, "Requested == available stored correctly");
        CHECK(edgeEx.getAvailableQuantity() == 5, "Available == requested stored correctly");

        // Zero available
        InsufficientStockException zeroEx("ZeroAvail", 1, 0);
        CHECK(zeroEx.getAvailableQuantity() == 0, "Zero available stored correctly");
    }
}

// ─── 4. PaymentException hierarchy ───────────────────────────────────────────

void testPaymentException() {
    std::cout << "\n[4] PaymentException hierarchy\n";

    // PaymentFailedException base
    {
        PaymentFailedException ex("Network error", "UPI", 1500.0);
        CHECK(std::string(ex.what()) == "Payment failed: Network error", "Message correct");
        CHECK(ex.getPaymentMethod() == "UPI", "getPaymentMethod() correct");
        CHECK(ex.getAmount() == 1500.0, "getAmount() correct");
        CHECK(ex.getErrorCode() == "PAY_ERR", "Error code is PAY_ERR");

        // Catchable as ShoppingException
        bool caught = false;
        try { throw PaymentFailedException("err", "Cash", 100.0); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "PaymentFailedException catchable as ShoppingException");

        // Zero amount
        PaymentFailedException zeroEx("err", "Card", 0.0);
        CHECK(zeroEx.getAmount() == 0.0, "Zero amount stored correctly");
    }

    // InsufficientFundsException
    {
        InsufficientFundsException ex(1000.0, 500.0);
        CHECK(ex.getPaymentMethod() == "Cash", "Payment method is Cash");
        CHECK(ex.getAmount() == 1000.0, "Required amount stored correctly");
        CHECK(ex.getTendered() == 500.0, "Tendered amount stored correctly");
        CHECK(ex.getErrorCode() == "PAY_ERR", "Error code is PAY_ERR");

        std::string detailed = ex.getDetailedMessage();
        CHECK(detailed.find("PAY_ERR") != std::string::npos, "Detailed message has error code");
        CHECK(detailed.find("1000") != std::string::npos, "Detailed message has required amount");
        CHECK(detailed.find("500") != std::string::npos, "Detailed message has tendered amount");

        // Catchable as PaymentFailedException
        bool caught = false;
        try { throw InsufficientFundsException(100.0, 50.0); } catch (const PaymentFailedException&) { caught = true; }
        CHECK(caught, "InsufficientFundsException catchable as PaymentFailedException");

        // Catchable as ShoppingException
        caught = false;
        try { throw InsufficientFundsException(100.0, 50.0); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "InsufficientFundsException catchable as ShoppingException");

        // Boundary: tendered == required (exact change)
        InsufficientFundsException exactEx(500.0, 500.0);
        CHECK(exactEx.getAmount() == 500.0, "Exact amount stored correctly");
        CHECK(exactEx.getTendered() == 500.0, "Exact tendered stored correctly");

        // Zero tendered
        InsufficientFundsException zeroEx(100.0, 0.0);
        CHECK(zeroEx.getTendered() == 0.0, "Zero tendered stored correctly");
    }

    // InvalidCardException
    {
        InvalidCardException ex("1234567890123456", 2500.0);
        CHECK(ex.getCardNumber() == "1234567890123456", "Card number stored correctly");
        CHECK(ex.getAmount() == 2500.0, "Amount stored correctly");
        CHECK(ex.getPaymentMethod() == "Card", "Payment method is Card");
        CHECK(ex.getErrorCode() == "PAY_ERR", "Error code is PAY_ERR");

        // Catchable as PaymentFailedException
        bool caught = false;
        try { throw InvalidCardException("0000", 100.0); } catch (const PaymentFailedException&) { caught = true; }
        CHECK(caught, "InvalidCardException catchable as PaymentFailedException");

        // Empty card number
        InvalidCardException emptyEx("", 100.0);
        CHECK(emptyEx.getCardNumber() == "", "Empty card number stored correctly");
    }

    // PaymentTimeoutException
    {
        PaymentTimeoutException ex("UPI", 750.0);
        CHECK(ex.getPaymentMethod() == "UPI", "Payment method stored correctly");
        CHECK(ex.getAmount() == 750.0, "Amount stored correctly");
        CHECK(ex.getErrorCode() == "PAY_ERR", "Error code is PAY_ERR");
        CHECK(std::string(ex.what()) == "Payment failed: Payment timeout", "Message correct");

        // Catchable as PaymentFailedException
        bool caught = false;
        try { throw PaymentTimeoutException("Card", 200.0); } catch (const PaymentFailedException&) { caught = true; }
        CHECK(caught, "PaymentTimeoutException catchable as PaymentFailedException");

        // Catchable as ShoppingException
        caught = false;
        try { throw PaymentTimeoutException("Card", 200.0); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "PaymentTimeoutException catchable as ShoppingException");
    }
}

// ─── 5. CouponException hierarchy ────────────────────────────────────────────

void testCouponException() {
    std::cout << "\n[5] CouponException hierarchy\n";

    // InvalidCouponException base
    {
        InvalidCouponException ex("BADCODE");
        CHECK(std::string(ex.what()) == "Invalid coupon code: BADCODE", "Message correct");
        CHECK(ex.getCouponCode() == "BADCODE", "getCouponCode() correct");
        CHECK(ex.getErrorCode() == "COUPON_ERR", "Error code is COUPON_ERR");

        // Catchable as ShoppingException
        bool caught = false;
        try { throw InvalidCouponException("X"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "InvalidCouponException catchable as ShoppingException");

        // Empty coupon code
        InvalidCouponException emptyEx("");
        CHECK(emptyEx.getCouponCode() == "", "Empty coupon code stored correctly");
        CHECK(std::string(emptyEx.what()) == "Invalid coupon code: ", "Empty code in message");
    }

    // ExpiredCouponException
    {
        ExpiredCouponException ex("SAVE10", "2023-12-31");
        CHECK(ex.getCouponCode() == "SAVE10", "getCouponCode() correct");
        CHECK(ex.getExpiryDate() == "2023-12-31", "getExpiryDate() correct");
        CHECK(ex.getErrorCode() == "COUPON_ERR", "Error code is COUPON_ERR");

        std::string detailed = ex.getDetailedMessage();
        CHECK(detailed.find("SAVE10") != std::string::npos, "Detailed message has coupon code");
        CHECK(detailed.find("2023-12-31") != std::string::npos, "Detailed message has expiry date");
        CHECK(detailed.find("COUPON_ERR") != std::string::npos, "Detailed message has error code");

        // Catchable as InvalidCouponException
        bool caught = false;
        try { throw ExpiredCouponException("X", "2020-01-01"); } catch (const InvalidCouponException&) { caught = true; }
        CHECK(caught, "ExpiredCouponException catchable as InvalidCouponException");

        // Catchable as ShoppingException
        caught = false;
        try { throw ExpiredCouponException("X", "2020-01-01"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "ExpiredCouponException catchable as ShoppingException");

        // Empty expiry date
        ExpiredCouponException emptyDateEx("CODE", "");
        CHECK(emptyDateEx.getExpiryDate() == "", "Empty expiry date stored correctly");
    }

    // CouponNotApplicableException
    {
        CouponNotApplicableException ex("FESTIVE20", "Minimum order not met");
        CHECK(ex.getCouponCode() == "FESTIVE20", "getCouponCode() correct");
        CHECK(ex.getReason() == "Minimum order not met", "getReason() correct");
        CHECK(ex.getErrorCode() == "COUPON_ERR", "Error code is COUPON_ERR");

        std::string detailed = ex.getDetailedMessage();
        CHECK(detailed.find("FESTIVE20") != std::string::npos, "Detailed message has coupon code");
        CHECK(detailed.find("Minimum order not met") != std::string::npos, "Detailed message has reason");
        CHECK(detailed.find("COUPON_ERR") != std::string::npos, "Detailed message has error code");

        // Catchable as InvalidCouponException
        bool caught = false;
        try { throw CouponNotApplicableException("X", "reason"); } catch (const InvalidCouponException&) { caught = true; }
        CHECK(caught, "CouponNotApplicableException catchable as InvalidCouponException");

        // Catchable as ShoppingException
        caught = false;
        try { throw CouponNotApplicableException("X", "reason"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "CouponNotApplicableException catchable as ShoppingException");

        // Empty reason
        CouponNotApplicableException emptyReasonEx("CODE", "");
        CHECK(emptyReasonEx.getReason() == "", "Empty reason stored correctly");
    }
}

// ─── 6. FileIOException hierarchy ────────────────────────────────────────────

void testFileIOException() {
    std::cout << "\n[6] FileIOException hierarchy\n";

    // FileIOException base
    {
        FileIOException ex("data/test.txt", "read");
        CHECK(std::string(ex.what()) == "File read error: data/test.txt", "Message correct");
        CHECK(ex.getFilename() == "data/test.txt", "getFilename() correct");
        CHECK(ex.getErrorCode() == "FILE_ERR", "Error code is FILE_ERR");

        // Catchable as ShoppingException
        bool caught = false;
        try { throw FileIOException("f.txt", "op"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "FileIOException catchable as ShoppingException");

        // Empty filename
        FileIOException emptyEx("", "write");
        CHECK(emptyEx.getFilename() == "", "Empty filename stored correctly");
    }

    // FileNotFoundException
    {
        FileNotFoundException ex("data/orders.txt");
        CHECK(std::string(ex.what()) == "File not found error: data/orders.txt", "Message correct");
        CHECK(ex.getFilename() == "data/orders.txt", "getFilename() correct");
        CHECK(ex.getErrorCode() == "FILE_ERR", "Error code is FILE_ERR");

        // Catchable as FileIOException
        bool caught = false;
        try { throw FileNotFoundException("f.txt"); } catch (const FileIOException&) { caught = true; }
        CHECK(caught, "FileNotFoundException catchable as FileIOException");

        // Catchable as ShoppingException
        caught = false;
        try { throw FileNotFoundException("f.txt"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "FileNotFoundException catchable as ShoppingException");

        // Empty filename
        FileNotFoundException emptyEx("");
        CHECK(emptyEx.getFilename() == "", "Empty filename in FileNotFoundException");
    }

    // FileWriteException
    {
        FileWriteException ex("data/receipts/receipt_001.txt");
        CHECK(std::string(ex.what()) == "File write error: data/receipts/receipt_001.txt", "Message correct");
        CHECK(ex.getFilename() == "data/receipts/receipt_001.txt", "getFilename() correct");
        CHECK(ex.getErrorCode() == "FILE_ERR", "Error code is FILE_ERR");

        // Catchable as FileIOException
        bool caught = false;
        try { throw FileWriteException("f.txt"); } catch (const FileIOException&) { caught = true; }
        CHECK(caught, "FileWriteException catchable as FileIOException");

        // Catchable as ShoppingException
        caught = false;
        try { throw FileWriteException("f.txt"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "FileWriteException catchable as ShoppingException");
    }

    // FileReadException
    {
        FileReadException ex("data/config.json");
        CHECK(std::string(ex.what()) == "File read error: data/config.json", "Message correct");
        CHECK(ex.getFilename() == "data/config.json", "getFilename() correct");
        CHECK(ex.getErrorCode() == "FILE_ERR", "Error code is FILE_ERR");

        // Catchable as FileIOException
        bool caught = false;
        try { throw FileReadException("f.txt"); } catch (const FileIOException&) { caught = true; }
        CHECK(caught, "FileReadException catchable as FileIOException");

        // Catchable as ShoppingException
        caught = false;
        try { throw FileReadException("f.txt"); } catch (const ShoppingException&) { caught = true; }
        CHECK(caught, "FileReadException catchable as ShoppingException");
    }
}

// ─── 7. Exception hierarchy polymorphism ─────────────────────────────────────

void testExceptionHierarchyPolymorphism() {
    std::cout << "\n[7] Exception hierarchy polymorphism\n";

    // Catch all derived exceptions as ShoppingException
    {
        std::vector<std::string> caught;

        auto tryCatch = [&](auto throwFn, const std::string& label) {
            try {
                throwFn();
            } catch (const ShoppingException& e) {
                caught.push_back(label + ": " + e.getErrorCode());
            }
        };

        tryCatch([]{ throw InvalidProductIdException(1); },       "InvalidProductId");
        tryCatch([]{ throw InvalidQuantityException(-1); },       "InvalidQuantity");
        tryCatch([]{ throw InvalidMenuChoiceException(99); },     "InvalidMenuChoice");
        tryCatch([]{ throw ProductUnavailableException("X"); },   "ProductUnavailable");
        tryCatch([]{ throw InsufficientStockException("X",5,2);}, "InsufficientStock");
        tryCatch([]{ throw InsufficientFundsException(100,50); }, "InsufficientFunds");
        tryCatch([]{ throw InvalidCardException("0000",100); },   "InvalidCard");
        tryCatch([]{ throw PaymentTimeoutException("UPI",100); }, "PaymentTimeout");
        tryCatch([]{ throw InvalidCouponException("BAD"); },      "InvalidCoupon");
        tryCatch([]{ throw ExpiredCouponException("X","2020"); }, "ExpiredCoupon");
        tryCatch([]{ throw CouponNotApplicableException("X","r");},"CouponNotApplicable");
        tryCatch([]{ throw FileNotFoundException("f.txt"); },     "FileNotFound");
        tryCatch([]{ throw FileWriteException("f.txt"); },        "FileWrite");
        tryCatch([]{ throw FileReadException("f.txt"); },         "FileRead");

        CHECK(caught.size() == 14, "All 14 derived exceptions caught as ShoppingException");
    }

    // Verify error codes are correct for each hierarchy
    {
        try { throw InvalidProductIdException(1); }
        catch (const ShoppingException& e) {
            CHECK(e.getErrorCode() == "INPUT_ERR", "InvalidProductIdException has INPUT_ERR code");
        }

        try { throw OutOfStockException("X", 0); }
        catch (const ShoppingException& e) {
            CHECK(e.getErrorCode() == "STOCK_ERR", "OutOfStockException has STOCK_ERR code");
        }

        try { throw PaymentFailedException("err", "UPI", 100.0); }
        catch (const ShoppingException& e) {
            CHECK(e.getErrorCode() == "PAY_ERR", "PaymentFailedException has PAY_ERR code");
        }

        try { throw InvalidCouponException("X"); }
        catch (const ShoppingException& e) {
            CHECK(e.getErrorCode() == "COUPON_ERR", "InvalidCouponException has COUPON_ERR code");
        }

        try { throw FileNotFoundException("f.txt"); }
        catch (const ShoppingException& e) {
            CHECK(e.getErrorCode() == "FILE_ERR", "FileNotFoundException has FILE_ERR code");
        }
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "============================================\n";
    std::cout << "  Exception Classes – Unit Tests           \n";
    std::cout << "============================================\n";

    testShoppingException();
    testInvalidInputException();
    testStockException();
    testPaymentException();
    testCouponException();
    testFileIOException();
    testExceptionHierarchyPolymorphism();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed == 0) ? 0 : 1;
}
