#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>

/**
 * InputValidator is a static utility class providing input sanitization
 * and validation functions to protect against malicious or malformed input.
 *
 * SECURITY NOTE: All user-supplied strings should be passed through
 * sanitizeString() before being stored, logged, or displayed.
 * Numeric inputs should be validated with validateNumericInput().
 * Product IDs should be validated with validateProductId().
 *
 * Requirements: 27.2
 */
class InputValidator {
public:
    // Prevent instantiation — all methods are static
    InputValidator() = delete;

    /**
     * Sanitizes a string by removing characters that are not alphanumeric,
     * spaces, hyphens, underscores, dots, or at-signs (@).
     * This prevents injection of shell metacharacters, control codes, etc.
     *
     * @param input  The raw user-supplied string
     * @return       A sanitized copy with disallowed characters removed
     */
    static std::string sanitizeString(const std::string& input);

    /**
     * Validates that a string represents a valid numeric value (integer or
     * floating-point).  Leading/trailing whitespace is tolerated.
     *
     * @param input  The string to validate
     * @return       true if the string is a valid number, false otherwise
     */
    static bool validateNumericInput(const std::string& input);

    /**
     * Validates a product ID: must be a positive integer within the
     * acceptable range [1, 99999].
     *
     * @param id  The candidate product ID
     * @return    true if the ID is within the valid range, false otherwise
     */
    static bool validateProductId(int id);

    /**
     * Validates a file path to prevent directory traversal attacks.
     * Rejects paths containing ".." sequences and ensures the path
     * starts with the allowed "data/" prefix.
     *
     * SECURITY: This prevents attackers from escaping the data/ directory
     * by supplying paths like "../../etc/passwd" (Requirement 27.3).
     *
     * @param path  The file path to validate
     * @return      true if the path is safe, false otherwise
     */
    static bool validateFilePath(const std::string& path);
};

#endif // INPUT_VALIDATOR_H
