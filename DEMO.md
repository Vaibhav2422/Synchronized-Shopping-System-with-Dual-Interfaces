# Sample Run Demonstration

## Overview

This demo walks through a complete end-to-end session of the Shopping Management System, covering product browsing, cart management, coupon application, UPI checkout, order history, and admin panel operations. It is designed to exercise every major feature of the application in a single automated run.

---

## Prerequisites

Build the application first:

```bash
make app
```

This produces the binary at `./bin/shopping_system`.

---

## Running the Demo

### Interactive (manual input)
```bash
./bin/shopping_system
```

### Automated (piped input)
```bash
./bin/shopping_system < demo_input.txt
```

---

## Step-by-Step Walkthrough

### Step 1 — View Products (choice: `1`)

Displays all 9 products across 3 categories:

| Category    | ID  | Product               | Price (₹) |
|-------------|-----|-----------------------|-----------|
| Electronics | 101 | Laptop                | 45,000    |
| Electronics | 102 | Smartphone            | 25,000    |
| Electronics | 103 | Headphones            | 3,500     |
| Clothing    | 201 | T-Shirt               | 800       |
| Clothing    | 202 | Jeans                 | 1,500     |
| Clothing    | 203 | Jacket                | 2,500     |
| Books       | 301 | C++ Programming       | 650       |
| Books       | 302 | Clean Code            | 550       |
| Books       | 303 | Pragmatic Programmer  | 600       |

---

### Step 2 — Search Products (choice: `2`, query: `Laptop`)

Searches by name. Returns the Laptop (ID 101, ₹45,000) as the matching result.

---

### Step 3 — Filter by Category (choice: `3`, category: `Electronics`)

Filters the inventory to show only Electronics products: Laptop, Smartphone, and Headphones.

---

### Step 4 — Add to Cart (choice: `4`, repeated 3 times)

| Product ID | Name            | Qty |
|------------|-----------------|-----|
| 101        | Laptop          | 1   |
| 202        | Jeans           | 2   |
| 301        | C++ Programming | 1   |

Each addition confirms "Product added to cart successfully!"

---

### Step 5 — View Cart (choice: `6`)

Displays the 3 cart items with individual quantities and line totals:

| Product         | Qty | Unit Price | Line Total |
|-----------------|-----|------------|------------|
| Laptop          | 1   | ₹45,000    | ₹45,000    |
| Jeans           | 2   | ₹1,500     | ₹3,000     |
| C++ Programming | 1   | ₹650       | ₹650       |

---

### Step 6 — Apply Coupon (choice: `7`, code: `SAVE10`)

Applies the SAVE10 coupon (10% off the cart total after other discounts). Confirms "Coupon SAVE10 applied successfully! (10% off)".

---

### Step 7 — Checkout with UPI (choice: `8`, payment: `1`, UPI ID: `user@upi`)

Calculates the full bill and processes UPI payment. See the **Sample Bill Calculation** section below for the exact math. On success:
- Order is saved to `data/orders.txt`
- Receipt is written to `data/receipts/receipt_XXXX.txt`
- Cart is cleared and coupon is reset

---

### Step 8 — View Order History (choice: `9`)

Displays the completed order from Step 7, including order ID, items, payment method, and total amount paid.

---

### Step 9 — Admin Panel (choice: `10`)

Login with `admin` / `admin123`, then performs the following admin operations in sequence:

| Admin Choice | Action                                          |
|--------------|-------------------------------------------------|
| `1`          | View Inventory — lists all current products     |
| `2`          | Add Product — adds Smart Watch (Electronics, ID 104, ₹8,000, qty 5, brand Apple, warranty 1 year) |
| `3`          | Update Price — sets Headphones (ID 103) to ₹3,200 |
| `4`          | Update Stock — sets T-Shirt (ID 201) stock to 30  |
| `5`          | Remove Product — removes Smart Watch (ID 104)   |
| `6`          | Exit Admin Panel                                |

---

### Step 10 — Exit (choice: `0`)

Exits the application. Sales analytics are displayed on exit, summarising total orders, revenue, and top-selling products from the session.

---

## Sample Bill Calculation

Cart contents before checkout (SAVE10 coupon active):

| Product         | Qty | Unit Price | Subtotal  | Product Discount | After Discount |
|-----------------|-----|------------|-----------|------------------|----------------|
| Laptop          | 1   | ₹45,000    | ₹45,000   | 10% = ₹4,500     | ₹40,500        |
| Jeans           | 2   | ₹1,500     | ₹3,000    | 20% = ₹600       | ₹2,400         |
| C++ Programming | 1   | ₹650       | ₹650      | 5% = ₹32.50      | ₹617.50        |

```
Subtotal (before product discounts):   ₹48,650.00
Total product discounts:               ₹5,132.50
After product discounts:               ₹43,517.50

Cart discount (5%, cart > ₹5,000):    ₹2,175.88
After cart discount:                   ₹41,341.63

SAVE10 coupon (10%):                   ₹4,134.16
After coupon:                          ₹37,207.46

GST (18%):                             ₹6,697.34
─────────────────────────────────────────────────
Final Total:                          ~₹43,904.80
```

---

## Admin Credentials

| Field    | Value    |
|----------|----------|
| Username | admin    |
| Password | admin123 |

---

## Available Coupons

| Code      | Discount |
|-----------|----------|
| SAVE10    | 10% off  |
| FESTIVE20 | 20% off  |

---

## Output Files

| File                              | Description                              |
|-----------------------------------|------------------------------------------|
| `data/receipts/receipt_XXXX.txt`  | Receipt for each completed order         |
| `data/orders.txt`                 | Persistent order history                 |
| `data/system.log`                 | Application event and error log          |
