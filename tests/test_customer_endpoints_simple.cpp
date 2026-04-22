/**
 * Unit tests for customer-related server logic.
 * Tests CustomerManager search, register, checkout-with-customer,
 * receipt customer header, and receipt no-loyalty-text.
 * Requirements: 1.6, 4.2, 4.3
 */

#include "../include/CustomerManager.h"
#include "../include/ReceiptWriter.h"
#include "../include/Order.h"
#include "../include/Bill.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Coupon.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>

static int passed = 0;
static int failed = 0;

#define TEST(name) void name()
#define RUN(name) do { \
    try { name(); std::cout << "[PASS] " #name "\n"; ++passed; } \
    catch (const std::exception& e) { std::cout << "[FAIL] " #name ": " << e.what() << "\n"; ++failed; } \
    catch (...) { std::cout << "[FAIL] " #name ": unknown exception\n"; ++failed; } \
} while(0)

static bool contains(const std::string& s, const std::string& sub) {
    return s.find(sub) != std::string::npos;
}

// ─── Test: search returns correct matches ─────────────────────────────────────
TEST(test_search_returns_matches) {
    CustomerManager::getInstance().reset();
    auto& cm = CustomerManager::getInstance();

    auto r1 = cm.registerCustomer("Alice Kumar", "9111111111", "pass1");
    auto r2 = cm.registerCustomer("Bob Singh",   "9222222222", "pass2");

    // Search by phone
    auto byPhone = cm.search("9111111111");
    bool found = false;
    for (const auto& r : byPhone) if (r.customerId == r1.customerId) { found = true; break; }
    assert(found && "search by phone should find Alice");

    // Search by ID
    auto byId = cm.search(r2.customerId);
    found = false;
    for (const auto& r : byId) if (r.customerId == r2.customerId) { found = true; break; }
    assert(found && "search by ID should find Bob");

    // Search by name substring (case-insensitive)
    auto byName = cm.search("alice");
    found = false;
    for (const auto& r : byName) if (r.customerId == r1.customerId) { found = true; break; }
    assert(found && "case-insensitive name search should find Alice");

    // Search with no match returns empty
    auto noMatch = cm.search("ZZZNOMATCH999");
    assert(noMatch.empty() && "non-matching query should return empty");

    CustomerManager::getInstance().reset();
}

// ─── Test: register returns generated ID ──────────────────────────────────────
TEST(test_register_returns_generated_id) {
    CustomerManager::getInstance().reset();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Charlie", "9333333333", "pw");
    assert(!rec.customerId.empty() && "customerId must not be empty");
    assert(rec.customerId.substr(0, 4) == "CUST" && "ID must start with CUST");
    assert(rec.customerId.size() == 8 && "ID must be 8 chars (CUST + 4 digits)");

    CustomerManager::getInstance().reset();
}

// ─── Test: checkout with customerId links order ────────────────────────────────
TEST(test_checkout_links_order_to_customer) {
    CustomerManager::getInstance().reset();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Diana", "9444444444", "pw");
    int fakeOrderId = 9001;

    cm.linkOrder(rec.customerId, fakeOrderId);
    cm.addLoyaltyPoints(rec.customerId, 5);

    auto* updated = cm.findById(rec.customerId);
    assert(updated != nullptr);
    assert(!updated->orderIds.empty() && "order should be linked");
    assert(updated->orderIds[0] == fakeOrderId && "linked order ID must match");
    assert(updated->loyaltyPoints == 5 && "loyalty points should be added");

    CustomerManager::getInstance().reset();
}

// ─── Test: receipt contains customer name when customer linked ─────────────────
TEST(test_receipt_contains_customer_name) {
    Inventory inv;
    inv.initializeSampleData();
    Cart cart(&inv);
    cart.addProduct(101, 1);

    Bill bill;
    bill.calculate(cart, nullptr);

    Order order(cart, bill.getFinalTotal(), "Cash");
    std::string receipt = ReceiptWriter::generateReceipt(order, bill, "Diana Patel", "CUST0001");

    assert(contains(receipt, "Diana Patel") && "receipt must contain customer name");
    assert(contains(receipt, "CUST0001")    && "receipt must contain customer ID");
}

// ─── Test: receipt without customer has no customer header ────────────────────
TEST(test_receipt_guest_has_no_customer_header) {
    Inventory inv;
    inv.initializeSampleData();
    Cart cart(&inv);
    cart.addProduct(101, 1);

    Bill bill;
    bill.calculate(cart, nullptr);

    Order order(cart, bill.getFinalTotal(), "Cash");
    std::string receipt = ReceiptWriter::generateReceipt(order, bill);

    assert(!contains(receipt, "Customer      :") && "guest receipt must not have customer line");
    assert(!contains(receipt, "Customer ID   :") && "guest receipt must not have customer ID line");
}

// ─── Test: receipt does NOT contain loyalty or points ─────────────────────────
TEST(test_receipt_no_loyalty_text) {
    Inventory inv;
    inv.initializeSampleData();
    Cart cart(&inv);
    cart.addProduct(101, 1);

    Bill bill;
    bill.calculate(cart, nullptr);

    Order order(cart, bill.getFinalTotal(), "UPI");

    // With customer
    std::string receiptWithCust = ReceiptWriter::generateReceipt(order, bill, "Eve", "CUST0002");
    std::string lower = receiptWithCust;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    assert(!contains(lower, "loyalty") && "receipt must not contain 'loyalty'");
    assert(!contains(lower, " pts")    && "receipt must not contain 'pts'");

    // Without customer (guest)
    std::string receiptGuest = ReceiptWriter::generateReceipt(order, bill);
    lower = receiptGuest;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    assert(!contains(lower, "loyalty") && "guest receipt must not contain 'loyalty'");
}

int main() {
    std::cout << "=== Customer Endpoint Unit Tests ===\n\n";

    RUN(test_search_returns_matches);
    RUN(test_register_returns_generated_id);
    RUN(test_checkout_links_order_to_customer);
    RUN(test_receipt_contains_customer_name);
    RUN(test_receipt_guest_has_no_customer_header);
    RUN(test_receipt_no_loyalty_text);

    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed > 0 ? 1 : 0;
}
