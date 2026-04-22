# Security Guide — Shopping Management System

This document describes the security measures implemented in the Shopping Management
System and provides guidance for developers and administrators.

---

## 1. Sensitive Data Handling

### 1.1 Card Numbers

**Requirement 27.2 / 8.4**

- Full card numbers are **never** stored to disk, logged, or printed to the terminal.
- `CardPayment::maskCardNumber()` replaces all but the last four digits with `****-****-****-`.
- Only the masked form (`****-****-****-XXXX`) is used in any display or log entry.

```
// SECURITY: Never expose full card number in display or logs.
// Only the last 4 digits are shown; all preceding digits are masked.
std::string CardPayment::maskCardNumber() const { ... }
```

### 1.2 Passwords / Admin Credentials

**Requirement 27.1**

- The default credentials (`admin` / `admin123`) are **hardcoded for demonstration only**.
- In a production deployment, credentials **must** be stored as salted hashes (e.g., bcrypt)
  in a secure credential store, never in plain text.
- The `ConfigManager` loads credentials from `data/config.json`; this file should be
  protected with restrictive file permissions (see §4) and excluded from version control.

### 1.3 Payment Information

- No payment details (card numbers, UPI IDs, cash amounts) are written to any file.
- Log entries for payment events record only the transaction amount and method, not
  any account identifiers.

---

## 2. Input Validation and Sanitization

**Requirement 27.2**

All user-supplied strings are processed through `InputValidator` (see
`include/InputValidator.h` / `src/InputValidator.cpp`) before being stored, logged,
or passed to business logic.

### 2.1 `InputValidator::sanitizeString()`

Removes any character that is not alphanumeric, a space, `-`, `_`, `.`, or `@`.
This prevents injection of shell metacharacters, ANSI escape sequences, and
control codes into log files or displayed output.

Applied to:
- Product search terms (`UIManager::handleSearchProducts`)
- Category filter input (`UIManager::handleFilterProducts`)
- Coupon codes (`UIManager::handleApplyCoupon`)
- UPI IDs and card holder names (`UIManager::handleCheckout`)

### 2.2 `InputValidator::validateNumericInput()`

Validates that a string represents a well-formed integer or floating-point number
before conversion, preventing `std::stoi`/`std::stod` exceptions from malformed input.

### 2.3 `InputValidator::validateProductId()`

Ensures product IDs are positive integers in the range `[1, 99999]`, rejecting
negative values, zero, and excessively large values that could cause out-of-bounds
access.

Applied to:
- `UIManager::handleAddToCart`
- `UIManager::handleRemoveFromCart`

---

## 3. File Path Validation and Directory Traversal Prevention

**Requirement 27.3**

`InputValidator::validateFilePath()` enforces two rules before any file is opened:

1. **No `..` sequences** — rejects paths such as `../../etc/passwd`.
2. **`data/` prefix required** — all file operations are restricted to the `data/`
   subdirectory; absolute paths and paths outside `data/` are rejected.

Applied to:
- `ReceiptWriter::saveReceipt` — receipt files in `data/receipts/`
- `OrderHistory::saveToFile` / `loadFromFile` — order history in `data/orders.txt`
- `Logger` — log file path is hardcoded to `data/system.log` (no user input)

---

## 4. File Permissions

**Requirement 27.5**

Restrictive permissions are set on sensitive files immediately after creation to
prevent unauthorized read access.

| File                          | Permission | Rationale                              |
|-------------------------------|------------|----------------------------------------|
| `data/system.log`             | `600`      | Contains internal system events        |
| `data/orders.txt`             | `600`      | Contains order history and amounts     |
| `data/receipts/receipt_*.txt` | `644`      | Readable by owner and group; no write  |

### Platform Notes

- **POSIX (Linux / macOS / WSL):** `chmod()` from `<sys/stat.h>` is called after
  each file is written.  The helper `setFilePermissions(path, mode)` is implemented
  in `Logger`, `OrderHistory`, and `ReceiptWriter` using `#ifdef` guards.
- **Windows (native):** NTFS ACLs control access.  The application does not call
  `_chmod` on Windows because the NTFS permission model differs from POSIX.
  Administrators should configure appropriate NTFS ACLs on the `data/` directory
  (e.g., deny read access to other users via Windows Explorer → Properties →
  Security).

---

## 5. Logging Security

- Log entries are written only to `data/system.log` (hardcoded path, no user
  influence).
- Log messages are sanitized: no raw user input is written verbatim; only
  structured fields (component name, event type, numeric IDs) appear in log lines.
- The log file is opened in append mode; the Logger singleton ensures a single
  writer, preventing race conditions in single-threaded use.

---

## 6. Known Limitations and Production Recommendations

| Limitation                          | Recommendation                                      |
|-------------------------------------|-----------------------------------------------------|
| Hardcoded admin credentials         | Use a hashed credential store (bcrypt/argon2)       |
| No TLS / network encryption         | Add TLS if the system is extended to a network API  |
| No session management               | Implement session tokens for multi-user scenarios   |
| `config.json` stored in plain text  | Encrypt sensitive fields or use a secrets manager   |
| No rate limiting on login attempts  | Add exponential back-off for failed auth attempts   |

---

## 7. Reporting Security Issues

If you discover a security vulnerability, please report it privately to the project
maintainers before public disclosure.  Do **not** open a public GitHub issue for
security-sensitive findings.
