# PROJECT INFO — Shopping Management System

---

## Title

**Shopping Management System with Dual-Interface Customer Portal**

---

## Description

A full-stack, production-grade shopping management platform built in **C++17** (backend) and **React + Vite** (frontend). The system runs two synchronized C++ REST API servers and two React frontends simultaneously — one for the shopkeeper and one for customers — sharing data through files on disk.

The project started as a console-based OOP demonstration and evolved into a complete dual-interface web application with customer identity management, loyalty tracking, real-time analytics, and property-based testing.

---

## Objectives

- Demonstrate advanced C++ OOP principles: inheritance, polymorphism, encapsulation, abstraction, RAII
- Implement industry-standard design patterns: Factory, Strategy, Singleton, Prototype
- Build a real REST API server in pure C++ using Winsock2 (no frameworks)
- Provide a modern React frontend with a premium dark-gold aesthetic
- Introduce a customer identity layer with secure authentication and loyalty tracking
- Achieve ≥90% test coverage with ~610 unit tests and 10 property-based tests using RapidCheck
- Follow SOLID principles throughout the entire codebase

---

## Tech Stack

| Layer | Technology |
|---|---|
| Backend language | C++17 (g++ / MinGW) |
| HTTP server | Raw Winsock2 sockets (Windows) |
| Frontend | React 18 + Vite |
| Styling | Custom CSS (dark gold theme, no UI library) |
| HTTP client | Axios |
| Routing | React Router v6 |
| Notifications | react-hot-toast |
| Icons | lucide-react |
| Property-based testing | RapidCheck |
| Unit testing | Custom test runner (C++) |
| Build system | GNU Make (Makefile) |
| Data storage | Flat files (orders.txt, customers.json, receipts/) |

---

## Architecture Overview

```
┌─────────────────────────┐     ┌─────────────────────────┐
│  Shopkeeper Interface   │     │   Customer Interface     │
│  React + Vite :5173     │     │   React + Vite :5174     │
└────────────┬────────────┘     └────────────┬────────────┘
             │ HTTP                           │ HTTP
             ▼                               ▼
┌─────────────────────────┐     ┌─────────────────────────┐
│  Shopkeeper Server      │     │   Customer Server        │
│  C++ Winsock2 :8081     │     │   C++ Winsock2 :8082     │
│  (billing, inventory,   │     │   (auth, profile,        │
│   analytics, customers) │     │    stats, loyalty)       │
└────────────┬────────────┘     └────────────┬────────────┘
             │                               │
             └──────────────┬────────────────┘
                            │ shared files
                    ┌───────▼────────┐
                    │  data/         │
                    │  orders.txt    │
                    │  customers.json│
                    │  receipts/     │
                    │  system.log    │
                    │  config.json   │
                    └────────────────┘
```

---

## Project Structure

```
.
├── src/                    # C++ implementation files
├── include/                # C++ header files
├── tests/                  # Unit + property-based test files (~610 tests)
├── frontend/               # Shopkeeper React app (port 5173)
│   └── src/
│       ├── pages/          # Catalog, Cart, Checkout, Orders, Analytics, Admin
│       ├── components/     # Navbar
│       └── api.js          # Axios API client
├── obj/                    # Compiled object files
├── bin/                    # Output binaries
├── data/                   # Runtime data (auto-created)
│   └── receipts/           # Per-order receipt .txt files
├── rapidcheck/             # RapidCheck PBT library (submodule)
├── .kiro/specs/            # Kiro spec files (requirements, design, tasks)
└── Makefile
```

---

## Core Features

### Shopkeeper Side
- Browse products by category (Electronics, Clothing, Books)
- Case-insensitive partial name search and category filter
- Add/remove items from cart with real-time inventory sync
- Apply coupon codes (SAVE10 — 10%, FESTIVE20 — 20%)
- Automatic 5% cart discount when subtotal exceeds ₹5000
- 18% GST (CGST 9% + SGST 9%) on post-discount amount
- Three payment methods: UPI, Card, Cash (with change calculation)
- Customer selection at checkout: Guest / Select Existing / Add New
- Receipt auto-generated and saved to `data/receipts/`
- Order history with full persistence across sessions
- Sales analytics: revenue, GST, top products, payment breakdown
- Customer rankings table with click-to-view order history modal
- Admin panel: add/update/delete products, manage inventory

### Customer Side
- Secure login with Customer_ID + password (SHA-256 hashed)
- Personal dashboard: total spent, total saved, top category, loyalty points
- Full billing history with expandable per-order item breakdown
- Loyalty points balance and redemption info
- Session token authentication (UUID, in-memory)

### Customer Management (Shopkeeper Server)
- Register new customers during checkout (name, phone, password)
- Live search by name, phone, or Customer_ID (case-insensitive partial match)
- Auto-generated sequential IDs: CUST0001, CUST0002, …
- Loyalty points: 1 point per ₹100 spent, tracked internally
- Ranked customer list sorted by total spending
- Customer data persisted to `data/customers.json`

---

## Design Patterns

| Pattern | Where Used |
|---|---|
| Factory | `ProductFactory` — creates Electronics, Clothing, Book instances |
| Strategy | `Payment` hierarchy — UPI, Card, Cash are interchangeable strategies |
| Singleton | `Logger`, `ConfigManager`, `CustomerManager` — single global instances |
| Prototype | `Product::clone()` — deep-copies inventory items into cart |

---

## SOLID Principles

| Principle | How Applied |
|---|---|
| Single Responsibility | Each class has one job: Cart manages items, Bill calculates, ReceiptWriter writes files |
| Open/Closed | New product types or payment methods extend base classes without modifying existing code |
| Liskov Substitution | Any `Product*` or `Payment*` can be substituted with a derived class transparently |
| Interface Segregation | Minimal interfaces — Payment only exposes `pay()` and `getPaymentMethod()` |
| Dependency Inversion | UIManager receives Cart, Inventory, OrderHistory via constructor injection |

---

## Discount Calculation Order

```
1. Product discounts    (Electronics 10%, Clothing 20%, Books 5%)
2. Cart discount        (5% if subtotal > ₹5000)
3. Coupon discount      (SAVE10 = 10%, FESTIVE20 = 20%)
4. GST                  (18% on final discounted amount = CGST 9% + SGST 9%)
```

---

## API Endpoints

### Shopkeeper Server (port 8081)

| Method | Endpoint | Description |
|---|---|---|
| GET | `/api/products` | All products |
| GET | `/api/products/search?q=` | Search by name |
| GET | `/api/products/filter?category=` | Filter by category |
| GET | `/api/cart` | Current cart |
| POST | `/api/cart/add` | Add to cart |
| POST | `/api/cart/remove` | Remove from cart |
| POST | `/api/cart/clear` | Clear cart |
| GET | `/api/bill?redeem=` | Bill preview |
| POST | `/api/checkout` | Complete purchase |
| GET | `/api/orders` | Order history |
| GET | `/api/analytics` | Sales analytics |
| GET | `/api/coupons` | Available coupons |
| POST | `/api/coupon/apply` | Apply coupon |
| POST | `/api/coupon/remove` | Remove coupon |
| GET | `/api/loyalty` | Loyalty points |
| POST | `/api/admin/login` | Admin auth |
| POST | `/api/admin/product` | Add product |
| PUT | `/api/admin/product/:id` | Update product |
| DELETE | `/api/admin/product/:id` | Delete product |
| GET | `/api/customers/search?q=` | Search customers |
| POST | `/api/customers/register` | Register customer |
| GET | `/api/customers/ranked` | Ranked by spending |
| POST | `/api/customers/:id/link-order` | Link order to customer |

### Customer Server (port 8082)

| Method | Endpoint | Description |
|---|---|---|
| POST | `/api/auth/login` | Customer login → session token |
| GET | `/api/me` | Profile (requires X-Token) |
| GET | `/api/me/stats` | Total spent, saved, products, top category |
| GET | `/api/me/orders` | Full purchase history |
| GET | `/api/me/loyalty` | Loyalty points balance |
| POST | `/api/me/redeem` | Redeem loyalty points |

---

## Data Models

### Customer Record (`data/customers.json`)
```json
{
  "customers": [
    {
      "customerId": "CUST0001",
      "name": "Rahul Sharma",
      "phone": "9876543210",
      "passwordHash": "a665a45920422f9d417e4867efdc4fb8...",
      "loyaltyPoints": 150,
      "orderIds": [1, 4, 7],
      "createdAt": "19/04/2026"
    }
  ],
  "nextIndex": 2
}
```

### Order Record (`data/orders.txt`)
Pipe-delimited format: `orderId|date|paymentMethod|totalAmount|item1:qty:price|item2:qty:price|...`

---

## Correctness Properties (Property-Based Tests)

All 10 properties are verified using **RapidCheck** with ≥100 iterations each.

| # | Property | Validates |
|---|---|---|
| 1 | Customer ID Uniqueness — all generated IDs are distinct | Req 1.3 |
| 2 | Customer Persistence Round-Trip — serialize → deserialize produces identical records | Req 8.1, 8.2 |
| 3 | Search Completeness — any registered customer is always findable by phone/ID/name substring | Req 2.2, 2.6 |
| 4 | Authentication Correctness — correct password always succeeds, wrong password always fails | Req 3.2 |
| 5 | Data Isolation — session A never returns data belonging to customer B | Req 3.4, 3.5 |
| 6 | Password Never Stored in Plaintext — raw password never appears in customers.json | Req 3.6 |
| 7 | Bill Contains No Loyalty Data — receipt never contains "loyalty", "points", or "pts" | Req 4.3, 9.4 |
| 8 | Loyalty Points Calculation — for any amount A, earned points == floor(A / 100) | Req 9.1 |
| 9 | Customer Stats Accuracy — totalSpent == sum of order totals, totalProducts == sum of quantities | Req 5.2, 5.4 |
| 10 | Ranked Customer List Order — ranked list is strictly non-increasing by total spending | Req 6.1 |

---

## Test Coverage

| Category | Files | Approx. Count |
|---|---|---|
| Unit tests (simple) | 17 `*_simple.cpp` + 8 additional | ~300 |
| Property-based tests | 10 PBT files (RapidCheck) | ~52 properties |
| Integration tests | `test_integration.cpp` | ~15 |
| Edge case tests | `test_edge_cases.cpp` | ~30 |
| Boundary tests | `test_boundary_cases.cpp` | ~48 |
| Negative tests | `test_negative_cases.cpp` | ~25 |
| Exception tests | `test_exceptions.cpp` | ~80 |
| Utility tests | `test_utility.cpp` | ~60 |
| **Total** | | **~610** |

Coverage targets: **≥90% line coverage**, **≥85% branch coverage**

---

## Build & Run

### Prerequisites
- g++ with C++17 support (GCC 7+ / MinGW on Windows)
- `mingw32-make` (Windows) or `make` (Linux/macOS)
- Node.js 18+ and npm (for frontend)

### Build C++ backend
```bash
mingw32-make app          # builds bin/shopping_system.exe
mingw32-make server       # builds bin/shopping_server.exe (port 8081)
mingw32-make customer_server  # builds bin/customer_server.exe (port 8082)
```

### Run servers
```bash
bin\shopping_server.exe   # start shopkeeper API
bin\customer_server.exe   # start customer API
```

### Run frontend
```bash
cd frontend && npm install && npm run dev   # shopkeeper UI at :5173
```

### Run all tests
```bash
mingw32-make test_all     # all unit tests
mingw32-make test_pbt     # property-based tests
```

### Clean
```bash
mingw32-make clean
```

---

## Default Credentials

| Role | Username / ID | Password |
|---|---|---|
| Admin | `admin` | `admin123` |
| Customer | Generated (e.g. `CUST0001`) | Set at registration |

---

## Available Coupons

| Code | Discount |
|---|---|
| `SAVE10` | 10% off |
| `FESTIVE20` | 20% off |

---

## Security Notes

- Passwords are hashed with SHA-256 (pure C++, no OpenSSL dependency)
- Card numbers are masked — only last 4 digits shown
- No sensitive payment data is written to disk
- All user inputs are sanitized before processing
- Admin credentials should be changed before production use
- Session tokens are UUID-based, stored in memory only (not persisted)
- See `SECURITY.md` for full security considerations

---

## Performance Characteristics

| Operation | Complexity | Notes |
|---|---|---|
| Product lookup | O(n) | Linear scan — use hash map for >10k products |
| Cart add/remove | O(n) | Scans inventory + cart |
| Bill calculation | O(n) | Iterates cart items |
| Coupon lookup | O(1) | Fixed map |
| Order save | O(n×m) | Full file rewrite |
| Customer search | O(n) | Linear scan of customers array |

Recommended maximums: 10,000 products, 100,000 orders, unlimited customers (file-bound).

---

## Known Limitations

- Order history uses full file rewrite on every save (append-only log would be faster at scale)
- Product search is O(n) — no index
- Customer server session tokens are in-memory only (lost on restart)
- No HTTPS — intended for local/LAN use
- Windows-only server build (Winsock2); Linux port requires replacing with POSIX sockets

---

## Version

See `VERSION` file for current semantic version. Version metadata is defined as compile-time constants in `include/Version.h`.

---

## Authors & Context

Built as an academic Object-Oriented Programming project (Semester 4, B.Tech AIDS) demonstrating:
- C++ OOP mastery (inheritance, polymorphism, templates, smart pointers)
- Design patterns in a real application context
- Full-stack development (C++ backend + React frontend)
- Formal software correctness via property-based testing
- Professional software engineering practices (SOLID, RAII, CI/CD)

---

## Related Documents

| File | Contents |
|---|---|
| `README.md` | Quick start, build instructions, test commands |
| `Design.md` | Full OOP design, class diagrams, pattern explanations |
| `Requirements.md` | 30 formal requirements with EARS-pattern acceptance criteria |
| `PERFORMANCE.md` | Algorithmic complexity, memory estimates, scalability limits |
| `SECURITY.md` | Security considerations and best practices |
| `DEPLOYMENT.md` | Deployment and environment setup guide |
| `DEMO.md` | Demo walkthrough and sample inputs |
| `.kiro/specs/dual-interface-customer-system/` | Spec for the customer portal feature |
