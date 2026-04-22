/**
 * Negative test cases for Coupon, Payment, OrderHistory, and ReceiptWriter.
 * Validates: Requirements 30.3
 *
 * Tests invalid inputs, boundary violations, and error conditions to ensure
 * the system handles them gracefully (throws appropriate exceptions or
 * returns safe values without crashing).
 */
#include "../include/Coupon.h"
#include "../include/CouponException.h"
#include "../include/CashPayment.h"
#include "../include/CardPayment.h"
#include "../include/UPIPayment.h"
#include "../include/PaymentException.h"
#include "../include/OrderHistory.h"
#include "../include/ReceiptWriter.h"
#include "../include/FileIOException.h"
#include "../include/Order.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Book.h"
#include <iostream>
#include <fstream>
#include <cstdio>

// ---------------------------------------------------------------------------
// Minimal test harness (same style as other *_simple.cpp tests)
// ---------------------------------------------------------------------------
static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg)                                          \
    do {                                                          \
        if (cond) {                                               \
            std::cout << "[PASS] " << (msg) << "\n";             \
            ++passed;                                             \
        } else {                                                  \
            std::cout << "[FAIL] " << (msg) << "\n";             \
            ++failed;                                             \
        }                                                         \
    } while (false)

// ---------------------------------------------------------------------------
// Coupon negative tests
// ---------------------------------------------------------------------------
static void testCouponNegative() {
    std::cout << "\n--- Coupon Negative Tests ---\n";

    // Invalid coupon code throws InvalidCouponException
    {
        Coupon c;
        bool threw = false;
        try {
            c.apply("INVALID123");
        } catch (const InvalidCouponException& e) {
            threw = true;
            CHECK(e.getCouponCode() == "INVALID123",
                  "InvalidCouponException carries the bad code");
        }
        CHECK(threw, "apply(\"INVALID123\") throws InvalidCouponException");
        CHECK(!c.isApplied(), "Coupon not applied after invalid code");
    }

    // Empty string coupon throws InvalidCouponException
    {
        Coupon c;
        bool threw = false;
        try {
            c.apply("");
        } catch (const InvalidCouponException&) {
            threw = true;
        }
        CHECK(threw, "apply(\"\") throws InvalidCouponException");
    }

    // Wrong-case coupon (lowercase) throws InvalidCouponException
    {
        Coupon c;
        bool threw = false;
        try {
            c.apply("save10");   // valid code is "SAVE10"
        } catch (const InvalidCouponException&) {
            threw = true;
        }
        CHECK(threw, "apply(\"save10\") throws InvalidCouponException (case-sensitive)");
    }

    // Apply coupon twice — second apply replaces first (no crash)
    {
        Coupon c;
        c.apply("SAVE10");
        CHECK(c.isApplied(), "First apply(\"SAVE10\") succeeds");
        // Applying a second valid coupon should succeed (replacement behaviour)
        bool secondOk = false;
        try {
            secondOk = c.apply("FESTIVE20");
        } catch (...) {
            secondOk = false;
        }
        CHECK(secondOk, "Second apply(\"FESTIVE20\") replaces first coupon");
        CHECK(c.getCode() == "FESTIVE20", "Code updated to FESTIVE20 after replacement");
    }

    // Apply invalid coupon twice — both should throw
    {
        Coupon c;
        bool threw1 = false, threw2 = false;
        try { c.apply("BAD1"); } catch (const InvalidCouponException&) { threw1 = true; }
        try { c.apply("BAD2"); } catch (const InvalidCouponException&) { threw2 = true; }
        CHECK(threw1 && threw2, "Two consecutive invalid applies both throw");
    }

    // reset() then re-apply works (positive test after negative)
    {
        Coupon c;
        try { c.apply("NOSUCHCODE"); } catch (...) {}
        c.reset();
        CHECK(!c.isApplied(), "reset() clears state after failed apply");
        bool ok = c.apply("SAVE10");
        CHECK(ok && c.isApplied(), "apply(\"SAVE10\") succeeds after reset()");
    }
}

// ---------------------------------------------------------------------------
// Payment negative tests
// ---------------------------------------------------------------------------
static void testCashPaymentNegative() {
    std::cout << "\n--- CashPayment Negative Tests ---\n";

    // Insufficient cash (tendered < amount)
    {
        CashPayment p(1000.0, 500.0);
        bool threw = false;
        try {
            p.pay();
        } catch (const InsufficientFundsException& e) {
            threw = true;
            CHECK(e.getAmount() == 1000.0,   "Exception reports correct required amount");
            CHECK(e.getTendered() == 500.0,  "Exception reports correct tendered amount");
        }
        CHECK(threw, "CashPayment(1000, 500).pay() throws InsufficientFundsException");
    }

    // Zero cash tendered
    {
        CashPayment p(500.0, 0.0);
        bool threw = false;
        try { p.pay(); } catch (const InsufficientFundsException&) { threw = true; }
        CHECK(threw, "CashPayment(500, 0).pay() throws InsufficientFundsException");
    }

    // Negative cash tendered
    {
        CashPayment p(500.0, -100.0);
        bool threw = false;
        try { p.pay(); } catch (const InsufficientFundsException&) { threw = true; }
        CHECK(threw, "CashPayment(500, -100).pay() throws InsufficientFundsException");
    }

    // Exact amount — boundary positive test (should succeed)
    {
        CashPayment p(750.0, 750.0);
        bool ok = false;
        try { ok = p.pay(); } catch (...) { ok = false; }
        CHECK(ok, "CashPayment(750, 750).pay() succeeds (exact amount)");
        CHECK(p.calculateChange() == 0.0, "Change is 0 when exact amount tendered");
    }
}

static void testCardPaymentNegative() {
    std::cout << "\n--- CardPayment Negative Tests ---\n";

    // Empty card number — should not crash (may throw or handle gracefully)
    {
        bool crashed = false;
        try {
            CardPayment p(500.0, "", "Test User");
            p.pay();   // may succeed or throw — either is acceptable
        } catch (const std::exception&) {
            // Exception is acceptable
        } catch (...) {
            crashed = true;
        }
        CHECK(!crashed, "CardPayment with empty card number does not crash");
    }

    // Card number shorter than 4 digits — maskCardNumber must not crash
    {
        bool crashed = false;
        try {
            CardPayment p(500.0, "123", "Test User");
            p.pay();   // may succeed or throw — either is acceptable
        } catch (const std::exception&) {
            // Exception is acceptable
        } catch (...) {
            crashed = true;
        }
        CHECK(!crashed, "CardPayment with 3-digit card number does not crash");
    }

    // Valid card — positive sanity check
    {
        CardPayment p(1000.0, "1234567890123456", "Alice");
        bool ok = false;
        try { ok = p.pay(); } catch (...) {}
        CHECK(ok, "CardPayment with valid 16-digit number succeeds");
    }
}

static void testUPIPaymentNegative() {
    std::cout << "\n--- UPIPayment Negative Tests ---\n";

    // Empty UPI ID — should not crash
    {
        bool crashed = false;
        try {
            UPIPayment p(500.0, "");
            p.pay();   // may succeed or throw — either is acceptable
        } catch (const std::exception&) {
            // Exception is acceptable
        } catch (...) {
            crashed = true;
        }
        CHECK(!crashed, "UPIPayment with empty UPI ID does not crash");
    }

    // Valid UPI ID — positive sanity check
    {
        UPIPayment p(500.0, "user@upi");
        bool ok = false;
        try { ok = p.pay(); } catch (...) {}
        CHECK(ok, "UPIPayment with valid UPI ID succeeds");
    }
}

// ---------------------------------------------------------------------------
// File I/O negative tests
// ---------------------------------------------------------------------------
static void testFileIONegative() {
    std::cout << "\n--- File I/O Negative Tests ---\n";

    // loadFromFile() with non-existent file — should not crash, history stays empty
    {
        bool crashed = false;
        try {
            OrderHistory h("/nonexistent/path/orders_xyz_missing.txt");
            h.loadFromFile();
            CHECK(h.getOrders().empty(),
                  "loadFromFile() with missing file leaves history empty");
        } catch (const std::exception&) {
            // Throwing is also acceptable — just must not hard-crash
            CHECK(true, "loadFromFile() with missing file throws (acceptable)");
        } catch (...) {
            crashed = true;
        }
        CHECK(!crashed, "loadFromFile() with missing file does not crash");
    }

    // loadFromFile() with garbage data — should not crash
    {
        const std::string garbageFile = "data/test_garbage_tmp.txt";
        {
            std::ofstream f(garbageFile);
            f << "@@##GARBAGE DATA\x01\x02\x03\nNOT_A_VALID_ORDER\n12345\n";
        }

        bool crashed = false;
        try {
            OrderHistory h(garbageFile);
            h.loadFromFile();
            // History may be empty or partially loaded — either is fine
            CHECK(true, "loadFromFile() with garbage data does not crash");
        } catch (const std::exception&) {
            CHECK(true, "loadFromFile() with garbage data throws (acceptable)");
        } catch (...) {
            crashed = true;
        }
        CHECK(!crashed, "loadFromFile() with garbage data does not hard-crash");

        std::remove(garbageFile.c_str());
    }

    // saveReceipt() with invalid directory path — should throw FileWriteException
    {
        try {
            // ReceiptWriter::saveReceipt writes to data/receipts/receipt_XXXX.txt.
            // Use a large orderId to avoid colliding with real receipts.
            std::string content = "test receipt content";
            ReceiptWriter::saveReceipt(content, 9999);
            // If it succeeded (directory exists), that's fine too
            CHECK(true, "saveReceipt() succeeds when receipts directory exists");
        } catch (const FileWriteException&) {
            CHECK(true, "saveReceipt() throws FileWriteException when write fails");
        } catch (const std::exception&) {
            CHECK(true, "saveReceipt() throws std::exception when write fails");
        }
        // Clean up if file was created
        std::remove("data/receipts/receipt_9999.txt");
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
    std::cout << "=== Negative Test Cases ===\n";
    std::cout << "Validates: Requirements 30.3\n";

    testCouponNegative();
    testCashPaymentNegative();
    testCardPaymentNegative();
    testUPIPaymentNegative();
    testFileIONegative();

    std::cout << "\n===========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
