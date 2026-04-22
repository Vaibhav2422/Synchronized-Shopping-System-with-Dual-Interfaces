#include "InputValidator.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

// SECURITY: Allowed character whitelist for sanitized strings.
// Only alphanumeric characters plus a small set of safe punctuation are kept.
static bool isAllowedChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c))
        || c == ' '
        || c == '-'
        || c == '_'
        || c == '.'
        || c == '@';
}

std::string InputValidator::sanitizeString(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (char c : input) {
        if (isAllowedChar(c)) {
            result += c;
        }
        // Disallowed characters are silently dropped (not replaced),
        // so the caller receives a safe subset of the original input.
    }
    return result;
}

bool InputValidator::validateNumericInput(const std::string& input) {
    if (input.empty()) return false;

    std::size_t start = 0;
    // Allow optional leading sign
    if (input[0] == '+' || input[0] == '-') {
        start = 1;
    }
    if (start >= input.size()) return false;

    bool hasDot = false;
    bool hasDigit = false;
    for (std::size_t i = start; i < input.size(); ++i) {
        char c = input[i];
        if (c == '.') {
            if (hasDot) return false;  // More than one decimal point
            hasDot = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDigit;
}

bool InputValidator::validateProductId(int id) {
    // SECURITY: Reject IDs outside the valid range to prevent
    // out-of-bounds access or integer overflow in downstream code.
    return id >= 1 && id <= 99999;
}

bool InputValidator::validateFilePath(const std::string& path) {
    // SECURITY: Reject any path containing ".." to prevent directory
    // traversal attacks (e.g., "../../etc/passwd") (Requirement 27.3).
    if (path.find("..") != std::string::npos) {
        return false;
    }

    // SECURITY: Restrict all file operations to the data/ directory.
    // Paths must start with "data/" to stay within the allowed area.
    if (path.substr(0, 5) != "data/") {
        return false;
    }

    return true;
}
