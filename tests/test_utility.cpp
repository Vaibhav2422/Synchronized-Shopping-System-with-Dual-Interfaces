/**
 * test_utility.cpp
 * Unit tests for utility classes: ANSIColors, InputValidator, Logger, ConfigManager.
 * Requirements: 30.1, 30.2, 30.3, 30.4
 */

#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>

#include "../include/ANSIColors.h"
#include "../include/InputValidator.h"
#include "../include/Logger.h"
#include "../include/ConfigManager.h"

// ─── Test result tracking ────────────────────────────────────────────────────

static int passed = 0;
static int failed = 0;

#define PASS(msg) do { std::cout << "  [PASS] " << msg << std::endl; ++passed; } while(0)
#define FAIL(msg) do { std::cout << "  [FAIL] " << msg << std::endl; ++failed; } while(0)
#define CHECK(cond, msg) do { if (cond) { PASS(msg); } else { FAIL(msg); } } while(0)

// Helper: capture stdout during a function call
static std::string captureStdout(std::function<void()> fn) {
    std::ostringstream buf;
    std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
    fn();
    std::cout.rdbuf(old);
    return buf.str();
}

// ─── 1. ANSIColors constants ──────────────────────────────────────────────────

void testANSIColorConstants() {
    std::cout << "\n[1] ANSIColors constants\n";

    // All constants must be non-empty strings
    CHECK(std::string(ANSIColors::BG_BLACK).size() > 0,   "BG_BLACK is non-empty");
    CHECK(std::string(ANSIColors::BG_RESET).size() > 0,   "BG_RESET is non-empty");
    CHECK(std::string(ANSIColors::CYAN).size() > 0,       "CYAN is non-empty");
    CHECK(std::string(ANSIColors::YELLOW).size() > 0,     "YELLOW is non-empty");
    CHECK(std::string(ANSIColors::GREEN).size() > 0,       "GREEN is non-empty");
    CHECK(std::string(ANSIColors::RED).size() > 0,         "RED is non-empty");
    CHECK(std::string(ANSIColors::WHITE).size() > 0,       "WHITE is non-empty");
    CHECK(std::string(ANSIColors::RESET).size() > 0,       "RESET is non-empty");
    CHECK(std::string(ANSIColors::BOLD).size() > 0,        "BOLD is non-empty");
    CHECK(std::string(ANSIColors::UNDERLINE).size() > 0,   "UNDERLINE is non-empty");

    // All constants must start with ESC character (\033)
    CHECK(std::string(ANSIColors::BG_BLACK)[0] == '\033',  "BG_BLACK starts with ESC");
    CHECK(std::string(ANSIColors::CYAN)[0] == '\033',      "CYAN starts with ESC");
    CHECK(std::string(ANSIColors::YELLOW)[0] == '\033',    "YELLOW starts with ESC");
    CHECK(std::string(ANSIColors::GREEN)[0] == '\033',     "GREEN starts with ESC");
    CHECK(std::string(ANSIColors::RED)[0] == '\033',       "RED starts with ESC");
    CHECK(std::string(ANSIColors::RESET)[0] == '\033',     "RESET starts with ESC");
    CHECK(std::string(ANSIColors::BOLD)[0] == '\033',      "BOLD starts with ESC");

    // Constants must be distinct from each other
    CHECK(std::string(ANSIColors::CYAN) != std::string(ANSIColors::YELLOW),
          "CYAN and YELLOW are distinct");
    CHECK(std::string(ANSIColors::GREEN) != std::string(ANSIColors::RED),
          "GREEN and RED are distinct");
    CHECK(std::string(ANSIColors::BOLD) != std::string(ANSIColors::UNDERLINE),
          "BOLD and UNDERLINE are distinct");
    CHECK(std::string(ANSIColors::BG_BLACK) != std::string(ANSIColors::BG_RESET),
          "BG_BLACK and BG_RESET are distinct");
}

// ─── 2. ANSIColors utility functions ─────────────────────────────────────────

void testANSIColorFunctions() {
    std::cout << "\n[2] ANSIColors utility functions\n";

    // clearScreen() emits ANSI escape sequences to stdout
    {
        std::string out = captureStdout([]{ ANSIColors::clearScreen(); });
        CHECK(!out.empty(), "clearScreen() produces output");
        CHECK(out.find('\033') != std::string::npos, "clearScreen() output contains ESC");
    }

    // setCursorPosition() emits ANSI escape with row/col
    {
        std::string out = captureStdout([]{ ANSIColors::setCursorPosition(5, 10); });
        CHECK(!out.empty(), "setCursorPosition() produces output");
        CHECK(out.find('\033') != std::string::npos, "setCursorPosition() output contains ESC");
        CHECK(out.find('5') != std::string::npos, "setCursorPosition() output contains row");
        CHECK(out.find("10") != std::string::npos, "setCursorPosition() output contains col");
    }

    // hideCursor() emits ANSI escape
    {
        std::string out = captureStdout([]{ ANSIColors::hideCursor(); });
        CHECK(!out.empty(), "hideCursor() produces output");
        CHECK(out.find('\033') != std::string::npos, "hideCursor() output contains ESC");
    }

    // showCursor() emits ANSI escape
    {
        std::string out = captureStdout([]{ ANSIColors::showCursor(); });
        CHECK(!out.empty(), "showCursor() produces output");
        CHECK(out.find('\033') != std::string::npos, "showCursor() output contains ESC");
    }

    // hideCursor and showCursor produce different sequences
    {
        std::string hide = captureStdout([]{ ANSIColors::hideCursor(); });
        std::string show = captureStdout([]{ ANSIColors::showCursor(); });
        CHECK(hide != show, "hideCursor() and showCursor() produce different sequences");
    }

    // setCursorPosition with boundary values
    {
        bool noThrow = true;
        try {
            captureStdout([]{ ANSIColors::setCursorPosition(1, 1); });
            captureStdout([]{ ANSIColors::setCursorPosition(0, 0); });
            captureStdout([]{ ANSIColors::setCursorPosition(999, 999); });
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "setCursorPosition() does not throw for any int values");
    }
}

// ─── 3. InputValidator::sanitizeString ───────────────────────────────────────

void testInputValidatorSanitize() {
    std::cout << "\n[3] InputValidator::sanitizeString\n";

    // Normal alphanumeric input passes through unchanged
    CHECK(InputValidator::sanitizeString("Hello123") == "Hello123",
          "Alphanumeric input unchanged");

    // Spaces are allowed
    CHECK(InputValidator::sanitizeString("Hello World") == "Hello World",
          "Spaces are allowed");

    // Allowed special chars: - _ . @
    CHECK(InputValidator::sanitizeString("user@example.com") == "user@example.com",
          "Email-like string passes through");
    CHECK(InputValidator::sanitizeString("file-name_v1.0") == "file-name_v1.0",
          "Hyphens, underscores, dots allowed");

    // Disallowed characters are stripped
    std::string result = InputValidator::sanitizeString("Hello<World>");
    CHECK(result.find('<') == std::string::npos, "< is stripped");
    CHECK(result.find('>') == std::string::npos, "> is stripped");
    CHECK(result.find("Hello") != std::string::npos, "Allowed chars preserved after stripping");

    // Shell metacharacters stripped
    result = InputValidator::sanitizeString("rm -rf /; echo pwned");
    CHECK(result.find(';') == std::string::npos, "Semicolon stripped");
    CHECK(result.find('/') == std::string::npos, "Slash stripped");

    // SQL injection attempt stripped
    result = InputValidator::sanitizeString("' OR '1'='1");
    CHECK(result.find('\'') == std::string::npos, "Single quote stripped");

    // Empty string
    CHECK(InputValidator::sanitizeString("") == "", "Empty string returns empty string");

    // String with only disallowed chars
    result = InputValidator::sanitizeString("<>{}[]|\\");
    CHECK(result.empty(), "String of only disallowed chars returns empty");

    // Newline and tab stripped
    result = InputValidator::sanitizeString("line1\nline2\ttab");
    CHECK(result.find('\n') == std::string::npos, "Newline stripped");
    CHECK(result.find('\t') == std::string::npos, "Tab stripped");

    // Long string
    std::string longStr(1000, 'a');
    CHECK(InputValidator::sanitizeString(longStr).size() == 1000,
          "Long alphanumeric string preserved");
}

// ─── 4. InputValidator::validateNumericInput ──────────────────────────────────

void testInputValidatorNumeric() {
    std::cout << "\n[4] InputValidator::validateNumericInput\n";

    // Valid integers
    CHECK(InputValidator::validateNumericInput("0"),     "\"0\" is valid");
    CHECK(InputValidator::validateNumericInput("42"),    "\"42\" is valid");
    CHECK(InputValidator::validateNumericInput("1000"),  "\"1000\" is valid");
    CHECK(InputValidator::validateNumericInput("+5"),    "\"+5\" is valid");
    CHECK(InputValidator::validateNumericInput("-3"),    "\"-3\" is valid");

    // Valid floats
    CHECK(InputValidator::validateNumericInput("3.14"),  "\"3.14\" is valid");
    CHECK(InputValidator::validateNumericInput("0.0"),   "\"0.0\" is valid");
    CHECK(InputValidator::validateNumericInput("-1.5"),  "\"-1.5\" is valid");
    CHECK(InputValidator::validateNumericInput("+2.7"),  "\"+2.7\" is valid");

    // Invalid inputs
    CHECK(!InputValidator::validateNumericInput(""),       "Empty string is invalid");
    CHECK(!InputValidator::validateNumericInput("abc"),    "Letters are invalid");
    CHECK(!InputValidator::validateNumericInput("1.2.3"),  "Double dot is invalid");
    CHECK(!InputValidator::validateNumericInput("1a2"),    "Mixed alphanumeric is invalid");
    CHECK(!InputValidator::validateNumericInput("+-5"),    "Double sign is invalid");
    CHECK(!InputValidator::validateNumericInput("+"),      "Sign only is invalid");
    CHECK(!InputValidator::validateNumericInput("-"),      "Minus only is invalid");
    CHECK(!InputValidator::validateNumericInput(" 5"),     "Leading space is invalid");
    CHECK(!InputValidator::validateNumericInput("5 "),     "Trailing space is invalid");
    CHECK(!InputValidator::validateNumericInput("1,000"),  "Comma is invalid");

    // Boundary: single digit
    CHECK(InputValidator::validateNumericInput("9"), "Single digit \"9\" is valid");
    CHECK(InputValidator::validateNumericInput("0"), "Single digit \"0\" is valid");

    // Boundary: just a decimal point
    CHECK(!InputValidator::validateNumericInput("."), "Lone dot is invalid");
    CHECK(InputValidator::validateNumericInput("0."), "\"0.\" is valid (trailing dot)");
    CHECK(InputValidator::validateNumericInput(".5"), "Leading dot \".5\" is valid");
}

// ─── 5. InputValidator::validateProductId ────────────────────────────────────

void testInputValidatorProductId() {
    std::cout << "\n[5] InputValidator::validateProductId\n";

    // Valid range [1, 99999]
    CHECK(InputValidator::validateProductId(1),     "ID 1 is valid (lower boundary)");
    CHECK(InputValidator::validateProductId(99999), "ID 99999 is valid (upper boundary)");
    CHECK(InputValidator::validateProductId(100),   "ID 100 is valid");
    CHECK(InputValidator::validateProductId(50000), "ID 50000 is valid (midpoint)");

    // Invalid: zero and negative
    CHECK(!InputValidator::validateProductId(0),    "ID 0 is invalid");
    CHECK(!InputValidator::validateProductId(-1),   "ID -1 is invalid");
    CHECK(!InputValidator::validateProductId(-100), "ID -100 is invalid");

    // Invalid: above upper boundary
    CHECK(!InputValidator::validateProductId(100000), "ID 100000 is invalid (above upper boundary)");
    CHECK(!InputValidator::validateProductId(999999), "ID 999999 is invalid");

    // Boundary neighbors
    CHECK(!InputValidator::validateProductId(0),     "ID 0 is invalid (just below lower boundary)");
    CHECK(InputValidator::validateProductId(1),      "ID 1 is valid (lower boundary)");
    CHECK(InputValidator::validateProductId(99999),  "ID 99999 is valid (upper boundary)");
    CHECK(!InputValidator::validateProductId(100000),"ID 100000 is invalid (just above upper boundary)");
}

// ─── 6. InputValidator::validateFilePath ─────────────────────────────────────

void testInputValidatorFilePath() {
    std::cout << "\n[6] InputValidator::validateFilePath\n";

    // Valid paths (must start with "data/")
    CHECK(InputValidator::validateFilePath("data/orders.txt"),
          "data/orders.txt is valid");
    CHECK(InputValidator::validateFilePath("data/config.json"),
          "data/config.json is valid");
    CHECK(InputValidator::validateFilePath("data/receipts/receipt_001.txt"),
          "data/receipts/receipt_001.txt is valid");
    CHECK(InputValidator::validateFilePath("data/system.log"),
          "data/system.log is valid");

    // Invalid: directory traversal
    CHECK(!InputValidator::validateFilePath("../etc/passwd"),
          "../etc/passwd is invalid (traversal)");
    CHECK(!InputValidator::validateFilePath("data/../etc/passwd"),
          "data/../etc/passwd is invalid (traversal)");
    CHECK(!InputValidator::validateFilePath("../../secret"),
          "../../secret is invalid (traversal)");

    // Invalid: does not start with "data/"
    CHECK(!InputValidator::validateFilePath("etc/passwd"),
          "etc/passwd is invalid (wrong prefix)");
    CHECK(!InputValidator::validateFilePath("/data/file.txt"),
          "/data/file.txt is invalid (absolute path)");
    CHECK(!InputValidator::validateFilePath(""),
          "Empty path is invalid");
    CHECK(!InputValidator::validateFilePath("file.txt"),
          "file.txt is invalid (no data/ prefix)");

    // Edge: exactly "data/" prefix
    CHECK(InputValidator::validateFilePath("data/x"),
          "data/x is valid (minimal valid path)");
}

// ─── 7. Logger singleton and basic logging ────────────────────────────────────

void testLogger() {
    std::cout << "\n[7] Logger singleton and logging\n";

    // Singleton: same instance returned each time
    {
        Logger& l1 = Logger::getInstance();
        Logger& l2 = Logger::getInstance();
        CHECK(&l1 == &l2, "Logger::getInstance() returns same instance");
    }

    // Timestamp is non-empty and has expected format
    {
        Logger& logger = Logger::getInstance();
        std::string ts = logger.getCurrentTimestamp();
        CHECK(!ts.empty(), "getCurrentTimestamp() returns non-empty string");
        // Should contain date separator '-' and time separator ':'
        CHECK(ts.find('-') != std::string::npos, "Timestamp contains '-' (date separator)");
        CHECK(ts.find(':') != std::string::npos, "Timestamp contains ':' (time separator)");
        // Should contain millisecond separator '.'
        CHECK(ts.find('.') != std::string::npos, "Timestamp contains '.' (millisecond separator)");
    }

    // Static logging methods do not throw
    {
        bool noThrow = true;
        try {
            Logger::logInfo("TestComponent", "Info message");
            Logger::logWarning("TestComponent", "Warning message");
            Logger::logError("TestComponent", "Error message");
            Logger::logDebug("TestComponent", "Debug message");
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "All static log methods execute without throwing");
    }

    // logException does not throw
    {
        bool noThrow = true;
        try {
            std::runtime_error ex("test exception");
            Logger::logException("TestComponent", ex);
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "logException() does not throw");
    }

    // clearLog does not throw
    {
        bool noThrow = true;
        try {
            Logger::clearLog();
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "clearLog() does not throw");
    }

    // Logging with empty component and message does not throw
    {
        bool noThrow = true;
        try {
            Logger::logInfo("", "");
            Logger::logError("", "");
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "Logging with empty strings does not throw");
    }

    // Logging with long strings does not throw
    {
        bool noThrow = true;
        try {
            std::string longMsg(1000, 'x');
            Logger::logInfo("Component", longMsg);
        } catch (...) {
            noThrow = false;
        }
        CHECK(noThrow, "Logging with long message does not throw");
    }

    // Two consecutive timestamps are non-decreasing
    {
        Logger& logger = Logger::getInstance();
        std::string ts1 = logger.getCurrentTimestamp();
        std::string ts2 = logger.getCurrentTimestamp();
        // Lexicographic comparison works for ISO-format timestamps
        CHECK(ts1 <= ts2, "Consecutive timestamps are non-decreasing");
    }
}

// ─── 8. ConfigManager defaults ───────────────────────────────────────────────

void testConfigManagerDefaults() {
    std::cout << "\n[8] ConfigManager defaults\n";

    ConfigManager& cfg = ConfigManager::getInstance();

    // GST rate: default 0.18, must be in [0, 1]
    double gst = cfg.getGSTRate();
    CHECK(gst >= 0.0 && gst <= 1.0, "GST rate is in valid range [0, 1]");
    CHECK(std::fabs(gst - 0.18) < 0.001 || gst > 0.0,
          "GST rate is positive (default 0.18 or loaded from config)");

    // Cart discount threshold: must be positive
    double threshold = cfg.getCartDiscountThreshold();
    CHECK(threshold > 0.0, "Cart discount threshold is positive");

    // Cart discount rate: must be positive
    double rate = cfg.getCartDiscountRate();
    CHECK(rate > 0.0, "Cart discount rate is positive");
    CHECK(rate <= 1.0, "Cart discount rate is <= 1.0");

    // Admin credentials: non-empty
    std::string username = cfg.getAdminUsername();
    std::string password = cfg.getAdminPassword();
    CHECK(!username.empty(), "Admin username is non-empty");
    CHECK(!password.empty(), "Admin password is non-empty");

    // Coupons map: non-empty, all values in (0, 1]
    const auto& coupons = cfg.getCoupons();
    CHECK(!coupons.empty(), "Coupons map is non-empty");
    bool allValid = true;
    for (const auto& kv : coupons) {
        if (kv.second <= 0.0 || kv.second > 1.0) allValid = false;
    }
    CHECK(allValid, "All coupon discount values are in (0, 1]");

    // Singleton: same instance
    ConfigManager& cfg2 = ConfigManager::getInstance();
    CHECK(&cfg == &cfg2, "ConfigManager::getInstance() returns same instance");
}

// ─── 9. ConfigManager with known default values ───────────────────────────────

void testConfigManagerKnownDefaults() {
    std::cout << "\n[9] ConfigManager known default values\n";

    ConfigManager& cfg = ConfigManager::getInstance();

    // If config file is absent, defaults should be used.
    // We test that the values are within expected ranges regardless.
    double gst = cfg.getGSTRate();
    CHECK(gst > 0.0, "GST rate is positive");

    double threshold = cfg.getCartDiscountThreshold();
    CHECK(threshold >= 100.0, "Cart discount threshold is at least 100");

    // Coupons should include at least one entry
    const auto& coupons = cfg.getCoupons();
    CHECK(coupons.size() >= 1, "At least one coupon is configured");

    // Check that SAVE10 or FESTIVE20 exists (default coupons)
    bool hasSave10    = coupons.count("SAVE10") > 0;
    bool hasFestive20 = coupons.count("FESTIVE20") > 0;
    CHECK(hasSave10 || hasFestive20,
          "Default coupons (SAVE10 or FESTIVE20) are present");

    if (hasSave10) {
        CHECK(std::fabs(coupons.at("SAVE10") - 0.10) < 0.001,
              "SAVE10 discount is 10%");
    }
    if (hasFestive20) {
        CHECK(std::fabs(coupons.at("FESTIVE20") - 0.20) < 0.001,
              "FESTIVE20 discount is 20%");
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "============================================\n";
    std::cout << "  Utility Classes – Unit Tests             \n";
    std::cout << "============================================\n";

    testANSIColorConstants();
    testANSIColorFunctions();
    testInputValidatorSanitize();
    testInputValidatorNumeric();
    testInputValidatorProductId();
    testInputValidatorFilePath();
    testLogger();
    testConfigManagerDefaults();
    testConfigManagerKnownDefaults();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed == 0) ? 0 : 1;
}
