# Performance Reference

This document covers algorithmic complexity, memory footprint, benchmark estimates, and scalability limits for the Shopping Management System.

---

## Algorithmic Complexity

### Inventory Operations

| Operation | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| `addProduct()` | O(1) amortized | O(1) | `vector::push_back` |
| `removeProduct(id)` | O(n) | O(1) | Linear scan by ID |
| `findProduct(id)` | O(n) | O(1) | Linear scan by ID |
| `updatePrice(id)` | O(n) | O(1) | Delegates to `findProduct` |
| `updateStock(id)` | O(n) | O(1) | Delegates to `findProduct` |
| `searchByName(term)` | O(n) | O(k) | k = number of matches |
| `filterByCategory(cat)` | O(n) | O(k) | k = number of matches |
| `displayAllProducts()` | O(n) | O(1) | Iterates all products |

> **Current bottleneck:** All lookup and search operations are O(n) due to linear scanning of `std::vector<unique_ptr<Product>>`. For catalogs up to ~10,000 products this is acceptable, but a hash map keyed by product ID would reduce lookups to O(1) amortized.

### Cart Operations

| Operation | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| `addProduct(id, qty)` | O(n) | O(1) | Scan inventory + scan cart for duplicates |
| `removeProduct(id)` | O(n) | O(1) | Linear scan through cart items |
| `clear()` | O(n) | O(1) | Restores stock for each item |
| `calculateSubtotal()` | O(n) | O(1) | Iterates all cart items |
| `calculateProductDiscounts()` | O(n) | O(1) | Iterates all cart items |
| `calculateCartDiscount()` | O(n) | O(1) | Calls `calculateSubtotal` |
| `getItemCount()` | O(n) | O(1) | Sums quantities |

### Bill & Coupon

| Operation | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| `Bill::calculate()` | O(n) | O(1) | Calls cart subtotal/discount methods |
| `Coupon::apply()` | O(1) | O(1) | Map lookup in fixed VALID_COUPONS |

### Order & Order History

| Operation | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| `Order` construction | O(m) | O(m) | m = number of cart items |
| `Order::serialize()` | O(m) | O(m) | Builds pipe-delimited string |
| `Order::deserialize()` | O(m) | O(m) | Parses pipe-delimited string |
| `OrderHistory::addOrder()` | O(1) amortized | O(1) | `vector::push_back` |
| `OrderHistory::saveToFile()` | O(n×m) | O(1) | Serializes all orders |
| `OrderHistory::loadFromFile()` | O(n×m) | O(n) | Deserializes all orders into memory |
| `getMostPurchasedProduct()` | O(n×m) | O(k) | k = distinct product names |
| `getTotalRevenue()` | O(n) | O(1) | Sums all order totals |
| `getTotalGST()` | O(n) | O(1) | Delegates to `getTotalRevenue` |

### Admin Operations

| Operation | Time Complexity | Space Complexity | Notes |
|---|---|---|---|
| `authenticate()` | O(1) | O(1) | String comparison |
| `addNewProduct()` | O(1) amortized | O(1) | Delegates to `Inventory::addProduct` |
| `updateProductPrice()` | O(n) | O(1) | Delegates to `Inventory::updatePrice` |
| `updateProductStock()` | O(n) | O(1) | Delegates to `Inventory::updateStock` |
| `removeProduct()` | O(n) | O(1) | Delegates to `Inventory::removeProduct` |

---

## Memory Footprint Estimates

Sizes are estimates for a 64-bit system with typical ABI padding. `std::string` uses SSO (small string optimization) — short strings (~15 chars) are stored inline; longer strings heap-allocate.

### Per-Object Estimates

| Object | Fields | Estimated Size |
|---|---|---|
| `Product` (base) | `int` + 3× `std::string` + `double` + `int` | ~120 bytes |
| `Electronics` | Product base + 2× `std::string` | ~200 bytes |
| `Clothing` | Product base + 2× `std::string` | ~200 bytes |
| `Book` | Product base + 2× `std::string` | ~200 bytes |
| `unique_ptr<Product>` | 8 bytes (pointer) | 8 bytes |
| `CartItem` | `unique_ptr<Product>` + `int` + padding | ~220 bytes (plus pointed-to Product) |
| `std::pair<string,int>` (order item) | 2× `std::string` + `int` | ~80 bytes |
| `Order` | `int` + `vector` + `double` + 4× `std::string` | ~500 bytes + (items × 80 bytes) |

### Catalog-Level Estimates

| Catalog Size | Inventory Memory | Notes |
|---|---|---|
| 10 products | ~3 KB | Typical demo/test |
| 100 products | ~25 KB | Small store |
| 1,000 products | ~250 KB | Medium store |
| 10,000 products | ~2.5 MB | Recommended maximum |

### Order History Estimates

| Orders in History | In-Memory Size | Notes |
|---|---|---|
| 100 orders | ~100 KB | Typical session |
| 10,000 orders | ~10 MB | Heavy usage |
| 100,000 orders | ~100 MB | Approaching practical limit |

---

## Benchmark Estimates

Estimated wall-clock times on modern hardware (x86-64, ~3 GHz, compiled with `-O2`). These are order-of-magnitude estimates for a C++17 application; actual times depend on hardware, OS scheduling, and catalog size.

| Operation | Catalog Size | Estimated Time | Bottleneck |
|---|---|---|---|
| Add product to cart | 100 products | < 1 µs | `findProduct` linear scan |
| Add product to cart | 10,000 products | ~10 µs | `findProduct` linear scan |
| Search by name | 100 products | < 5 µs | String comparison loop |
| Search by name | 10,000 products | ~100 µs | String comparison loop |
| Calculate bill | 10 cart items | < 1 µs | Arithmetic only |
| Checkout (full flow) | 10 cart items | < 5 µs | In-memory only |
| Save order history | 1,000 orders | ~5 ms | File I/O |
| Save order history | 100,000 orders | ~500 ms | File I/O (sequential write) |
| Load order history | 1,000 orders | ~10 ms | File I/O + string parsing |
| Load order history | 100,000 orders | ~1 s | File I/O + string parsing |
| Display all products | 100 products | ~1 ms | Terminal output |
| Display all products | 10,000 products | ~100 ms | Terminal output |

---

## Scalability Limits

| Resource | Recommended Maximum | Hard Limit | Notes |
|---|---|---|---|
| Products in inventory | 10,000 | ~1,000,000 (RAM) | Linear search degrades above 10k |
| Items in a single cart | 100 | Unbounded (RAM) | UX degrades; no enforced cap |
| Orders in history (in memory) | 100,000 | ~500,000 (RAM) | ~100 MB at 100k orders |
| Orders in history (file I/O) | 100,000 | Filesystem limit | Save/load becomes slow (>500 ms) |
| Distinct product categories | Unlimited | Practical: ~20 | No index; filter is O(n) |
| Receipt files | Filesystem limit | Filesystem limit | One file per order in `data/receipts/` |

### Recommendations for Scaling Beyond These Limits

- **Product lookup:** Replace `std::vector` linear scan with `std::unordered_map<int, unique_ptr<Product>>` to achieve O(1) average-case lookups.
- **Order persistence:** Switch from a flat text file to a binary format or embedded database (e.g., SQLite) once order count exceeds ~10,000.
- **Search:** Add an inverted index or sorted structure for name-based search to reduce from O(n) to O(log n) or O(1).

---

## Known Bottlenecks

1. **Linear search in Inventory** — `findProduct()`, `searchByName()`, and `filterByCategory()` all iterate the full product vector. This is the single most impactful bottleneck for large catalogs.

2. **Full file rewrite on save** — `OrderHistory::saveToFile()` rewrites the entire `orders.txt` file on every save. For large histories this is slow; an append-only log would be more efficient.

3. **Terminal output for large catalogs** — `displayAllProducts()` writes every product to stdout. Displaying 10,000+ products will be slow and unusable without pagination.
