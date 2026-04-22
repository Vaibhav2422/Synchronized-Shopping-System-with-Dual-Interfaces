# Shopping Management System

[![CI](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/YOUR_USERNAME/YOUR_REPO/branch/main/graph/badge.svg)](https://codecov.io/gh/YOUR_USERNAME/YOUR_REPO)

A C++17 console-based shopping management system demonstrating OOP principles including inheritance, polymorphism, encapsulation, design patterns (Factory, Strategy, Singleton, Prototype), and RAII memory management.

---

## System Requirements

- **Compiler:** g++ with C++17 support (GCC 7+ recommended; MinGW on Windows)
- **Build tool:** `make` or `mingw32-make` (MinGW on Windows)
- **OS:** Windows (MinGW) or Linux/macOS

---

## Directory Structure

```
.
├── src/            # Implementation files (.cpp)
├── include/        # Header files (.h)
├── tests/          # Unit and property-based test files
├── obj/            # Compiled object files (auto-created)
├── bin/            # Output binaries (auto-created)
├── data/           # Runtime data files (auto-created)
│   └── receipts/   # Generated receipt files
├── rapidcheck/     # RapidCheck PBT library source
└── Makefile
```

---

## Building the Application

### Build the main application

```bash
mingw32-make app       # Windows (MinGW)
make app               # Linux / macOS
```

The binary is output to `bin/shopping_system.exe` (Windows) or `bin/shopping_system` (Linux/macOS).

### Run the application

```bash
bin/shopping_system    # Linux / macOS
bin\shopping_system    # Windows
```

Or use the Makefile shortcut:

```bash
mingw32-make run
```

### Clean build artifacts

```bash
mingw32-make clean
```

---

## Running Tests

### Run all simple unit tests

```bash
mingw32-make test_all
```

### Run individual test suites

| Command                        | Tests                        |
|-------------------------------|------------------------------|
| `mingw32-make test`           | Product tests                |
| `mingw32-make test_order`     | Order tests                  |
| `mingw32-make test_order_history` | Order history & analytics |
| `mingw32-make test_receipt`   | Receipt generation           |
| `mingw32-make test_payment`   | Payment processing           |
| `mingw32-make test_bill`      | Bill calculation             |
| `mingw32-make test_coupon`    | Coupon validation            |
| `mingw32-make test_cart`      | Cart operations              |
| `mingw32-make test_inventory` | Inventory operations         |
| `mingw32-make test_admin`     | Admin operations             |
| `mingw32-make test_ui`        | UI / exception handling      |
| `mingw32-make test_integration` | End-to-end checkout flow   |

### Run property-based tests (requires RapidCheck)

```bash
mingw32-make test_pbt
```

---

## Sample Credentials

| Role  | Username | Password  |
|-------|----------|-----------|
| Admin | `admin`  | `admin123` |

Access the admin panel from the main menu to add/update/remove products and view inventory.

---

## Available Coupon Codes

| Code        | Discount |
|-------------|----------|
| `SAVE10`    | 10% off  |
| `FESTIVE20` | 20% off  |

Coupons are applied after product and cart discounts, before GST.

---

## Features

- Browse products by category (Electronics, Clothing, Books)
- Search products by name (case-insensitive partial match)
- Filter products by category
- Add/remove items from cart with real-time inventory sync
- Apply coupon codes for additional discounts
- Automatic 5% cart discount when subtotal exceeds ₹5000
- 18% GST applied on post-discount amount
- Multiple payment methods: UPI, Card, Cash
- Automatic receipt generation saved to `data/receipts/`
- Order history with persistence across sessions (`data/orders.txt`)
- Sales analytics: most purchased product, total revenue, total GST
- Admin panel for inventory management
- System logging to `data/system.log`

---

## Discount Calculation Order

1. Product discounts (Electronics 10%, Clothing 20%, Books 5%)
2. Cart discount (5% if subtotal > ₹5000)
3. Coupon discount (applied to amount after above discounts)
4. GST (18% on final discounted amount)

---

## Test Coverage

### Coverage Targets

| Metric          | Target | How to Check                              |
|-----------------|--------|-------------------------------------------|
| Line coverage   | ≥ 90%  | `make -f Makefile.linux coverage`         |
| Branch coverage | ≥ 85%  | `make -f Makefile.linux coverage`         |

### Generating Coverage Reports Locally

Requires GCC, `lcov`, and `genhtml` (Linux only):

```bash
# Install lcov (Ubuntu/Debian)
sudo apt-get install lcov

# Generate coverage report
make -f Makefile.linux coverage

# Open HTML report
xdg-open coverage_html/index.html
```

The report is generated in `coverage_html/index.html`. Coverage data is also uploaded to [Codecov](https://codecov.io) on every CI run.

### Coverage by Module

| Module / Class         | Test File(s)                                              | Coverage Notes                                      |
|------------------------|-----------------------------------------------------------|-----------------------------------------------------|
| `Product` hierarchy    | `test_product_simple`, `test_product` (PBT)               | All virtual methods exercised; clone() tested       |
| `Electronics`          | `test_product_simple`, `test_product` (PBT)               | display(), calculateDiscount(), getCategory()       |
| `Clothing`             | `test_product_simple`, `test_product` (PBT)               | display(), calculateDiscount(), getCategory()       |
| `Book`                 | `test_product_simple`, `test_product` (PBT)               | display(), calculateDiscount(), getCategory()       |
| `ProductFactory`       | `test_product_factory`                                    | All three factory methods tested                    |
| `Inventory`            | `test_inventory_operations_simple`, `test_inventory_operations` (PBT), `test_inventory_display`, `test_sample_data` | addProduct, findProduct, search, filter, display |
| `Cart`                 | `test_cart_operations_simple`, `test_cart_operations` (PBT), `test_edge_cases`, `test_boundary_cases` | add/remove, calculations, edge cases |
| `Bill`                 | `test_bill_simple`, `test_bill` (PBT), `test_boundary_cases` | All discount paths, GST, coupon integration      |
| `Coupon`               | `test_coupon_simple`, `test_coupon` (PBT), `test_negative_cases` | Valid/invalid codes, reset, calculateDiscount  |
| `Payment` hierarchy    | `test_payment_simple`, `test_payment` (PBT), `test_negative_cases` | UPI, Card, Cash; insufficient funds path      |
| `Order`                | `test_order_simple`                                       | Construction, serialization, deserialization        |
| `OrderHistory`         | `test_order_history_simple`, `test_negative_cases`        | addOrder, persistence, analytics, missing file      |
| `ReceiptWriter`        | `test_receipt_simple`, `test_negative_cases`              | generateReceipt, saveReceipt, filename generation   |
| `Logger`               | `test_utility`                                            | Singleton, all log levels, timestamps               |
| `Admin`                | `test_admin_simple`                                       | Authentication, CRUD operations                     |
| `UIManager`            | `test_ui_simple`, `test_integration`                      | Exception handling, menu flow                       |
| `ConfigManager`        | `test_utility`                                            | Singleton, defaults, config loading                 |
| `InputValidator`       | `test_utility`                                            | sanitizeString, validateNumericInput, validateProductId, validateFilePath |
| `ANSIColors`           | `test_utility`                                            | All constants, utility functions                    |
| `Transaction`          | `test_transaction_simple`                                 | commit/rollback, checkpoint                         |
| Exception hierarchy    | `test_exceptions`                                         | All 14 exception classes, hierarchy polymorphism    |
| Edge cases             | `test_edge_cases`                                         | Empty cart, zero qty, large qty, empty inventory    |
| Boundary cases         | `test_boundary_cases`                                     | Price/qty boundaries, cart threshold, GST precision |
| Negative cases         | `test_negative_cases`                                     | Invalid coupons, insufficient cash, file I/O errors |
| Unit cases             | `test_unit_cases`                                         | Coupon codes, sample data, payment availability     |
| Integration            | `test_integration`                                        | End-to-end checkout, admin flow, persistence        |

### Untested Code and Justification

| Code Path                                      | Justification                                                                 |
|------------------------------------------------|-------------------------------------------------------------------------------|
| `UIManager` interactive menu loops             | Requires stdin simulation; covered indirectly via `test_ui_simple`            |
| `UIManager::displayLoadingAnimation()`         | Time-dependent animation; tested for no-crash in `test_ui_simple`             |
| `main.cpp` top-level exception handler         | Requires process-level testing; covered by integration tests                  |
| `Logger` file write failures                   | Requires filesystem permission manipulation; not feasible in unit tests        |
| `Admin::showAdminMenu()` interactive loop      | Requires stdin simulation; authentication and CRUD tested separately           |
| ANSI terminal rendering                        | Visual output; correctness of escape codes verified, rendering is terminal-dependent |

### Test Suite Summary

| Test Category          | Files                                                    | Test Count (approx.) |
|------------------------|----------------------------------------------------------|----------------------|
| Unit tests (simple)    | 17 `*_simple.cpp` files + 8 additional unit test files   | ~300 tests           |
| Property-based tests   | 6 PBT files using RapidCheck                             | ~52 properties       |
| Integration tests      | `test_integration.cpp`                                   | ~15 scenarios        |
| Edge case tests        | `test_edge_cases.cpp`                                    | ~30 tests            |
| Boundary tests         | `test_boundary_cases.cpp`                                | ~48 tests            |
| Negative tests         | `test_negative_cases.cpp`                                | ~25 tests            |
| Exception tests        | `test_exceptions.cpp`                                    | ~80 tests            |
| Utility tests          | `test_utility.cpp`                                       | ~60 tests            |
| **Total**              |                                                          | **~610 tests**       |
