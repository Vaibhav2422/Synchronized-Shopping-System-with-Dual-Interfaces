/**
 * Unit tests for Customer Server logic.
 * Tests login, /api/me, /api/me/stats, /api/me/loyalty.
 * Requirements: 3.2, 3.3, 5.2–5.6
 */

#include "../include/CustomerManager.h"
#include "../include/OrderHistory.h"
#include "../include/Order.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include <iostream>
#include <cassert>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

static int passed = 0;
static int failed = 0;

#define TEST(name) void name()
#define RUN(name) do { \
    try { name(); std::cout << "[PASS] " #name "\n"; ++passed; } \
    catch (const std::exception& e) { std::cout << "[FAIL] " #name ": " << e.what() << "\n"; ++failed; } \
    catch (...) { std::cout << "[FAIL] " #name ": unknown exception\n"; ++failed; } \
} while(0)

// ─── Helpers mirroring customer_server.cpp logic ─────────────────────────────

// Session store (mirrors server's g_sessions)
static std::map<std::string, std::string> sessions;

static std::string fakeLogin(const std::string& customerId, const std::string& password) {
    if (!CustomerManager::getInstance().authenticate(customerId, password))
        return "";  // empty = auth failed
    std::string token = "tok_" + customerId;
    sessions[token] = customerId;
    return token;
}

static std::string resolveCustomer(const std::string& token) {
    auto it = sessions.find(token);
    return (it != sessions.end()) ? it->second : "";
}

// Mirrors computeStats from customer_server.cpp
struct CustomerStats {
    double totalSpent    = 0.0;
    int    totalProducts = 0;
    int    loyaltyPoints = 0;
};

static CustomerStats computeStats(const CustomerRecord& cust, const OrderHistory& hist) {
    CustomerStats stats;
    stats.loyaltyPoints = cust.loyaltyPoints;
    const auto& orders = hist.getOrders();
    for (int oid : cust.orderIds) {
        for (const auto& o : orders) {
            if (o.getOrderId() == oid) {
                stats.totalSpent += o.getTotalAmount();
                for (const auto& prod : o.getProducts())
                    stats.totalProducts += prod.second;
                break;
            }
        }
    }
    return stats;
}

// ─── Tests ────────────────────────────────────────────────────────────────────

// Test: login returns token on valid credentials (Req 3.2)
TEST(test_login_valid_credentials_returns_token) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Alice", "9100000001", "secret123");

    std::string token = fakeLogin(rec.customerId, "secret123");
    assert(!token.empty() && "Valid login must return a non-empty token");
    assert(resolveCustomer(token) == rec.customerId && "Token must resolve to correct customer");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

// Test: login returns 401 on invalid credentials (Req 3.3)
TEST(test_login_invalid_credentials_returns_failure) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Bob", "9100000002", "correctPass");

    // Wrong password
    std::string token = fakeLogin(rec.customerId, "wrongPass");
    assert(token.empty() && "Wrong password must not return a token");

    // Non-existent customer
    std::string token2 = fakeLogin("CUST9999", "anyPass");
    assert(token2.empty() && "Unknown customer must not return a token");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

// Test: /api/me returns only the authenticated customer's data (Req 3.4, 3.5)
TEST(test_me_returns_only_authenticated_customer_data) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto recA = cm.registerCustomer("Alice", "9200000001", "passA");
    auto recB = cm.registerCustomer("Bob",   "9200000002", "passB");

    std::string tokenA = fakeLogin(recA.customerId, "passA");
    std::string tokenB = fakeLogin(recB.customerId, "passB");

    // Token A resolves to Alice, not Bob
    std::string resolvedA = resolveCustomer(tokenA);
    assert(resolvedA == recA.customerId && "Token A must resolve to Alice");
    assert(resolvedA != recB.customerId && "Token A must NOT resolve to Bob");

    // Token B resolves to Bob, not Alice
    std::string resolvedB = resolveCustomer(tokenB);
    assert(resolvedB == recB.customerId && "Token B must resolve to Bob");
    assert(resolvedB != recA.customerId && "Token B must NOT resolve to Alice");

    // Verify the actual customer data returned is correct
    const CustomerRecord* custA = cm.findById(resolvedA);
    assert(custA != nullptr && "Customer A must be found");
    assert(custA->name == "Alice" && "Customer A name must be Alice");
    assert(custA->phone == "9200000001" && "Customer A phone must match");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

// Test: /api/me/stats computes correct totals from linked orders (Req 5.2, 5.4, 5.5)
TEST(test_stats_computes_correct_totals) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Carol", "9300000001", "pass");

    // Build a minimal order history in memory
    Inventory inv;
    inv.initializeSampleData();
    Cart cart1(&inv);
    cart1.addProduct(101, 2);  // 2 units of product 101
    Cart cart2(&inv);
    cart2.addProduct(201, 1);  // 1 unit of product 201

    Order o1(cart1, 500.0, "Cash");
    Order o2(cart2, 300.0, "UPI");

    OrderHistory hist;
    hist.addOrder(o1);
    hist.addOrder(o2);

    // Link orders to customer
    cm.linkOrder(rec.customerId, o1.getOrderId());
    cm.linkOrder(rec.customerId, o2.getOrderId());

    const CustomerRecord* cust = cm.findById(rec.customerId);
    assert(cust != nullptr);

    CustomerStats stats = computeStats(*cust, hist);

    // totalSpent must equal sum of order totals
    assert(std::abs(stats.totalSpent - 800.0) < 0.01 && "totalSpent must be 800.0");

    // totalProducts: 2 + 1 = 3
    assert(stats.totalProducts == 3 && "totalProducts must be 3");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

// Test: /api/me/loyalty returns correct points balance (Req 5.6)
TEST(test_loyalty_returns_correct_balance) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Dave", "9400000001", "pass");
    cm.addLoyaltyPoints(rec.customerId, 42);

    std::string token = fakeLogin(rec.customerId, "pass");
    std::string custId = resolveCustomer(token);

    const CustomerRecord* cust = cm.findById(custId);
    assert(cust != nullptr);
    assert(cust->loyaltyPoints == 42 && "Loyalty points must be 42");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

// Test: unauthorized request (no token) returns 401 equivalent
TEST(test_no_token_is_unauthorized) {
    sessions.clear();
    std::string custId = resolveCustomer("");
    assert(custId.empty() && "Empty token must not resolve to any customer");

    std::string custId2 = resolveCustomer("nonexistent_token");
    assert(custId2.empty() && "Unknown token must not resolve to any customer");
}

// Test: redeem loyalty points deducts correctly (Req 9.5)
TEST(test_redeem_loyalty_points) {
    CustomerManager::getInstance().reset();
    sessions.clear();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Eve", "9500000001", "pass");
    cm.addLoyaltyPoints(rec.customerId, 100);

    cm.redeemLoyaltyPoints(rec.customerId, 30);
    const CustomerRecord* cust = cm.findById(rec.customerId);
    assert(cust != nullptr);
    assert(cust->loyaltyPoints == 70 && "After redeeming 30 from 100, balance must be 70");

    // Redeeming more than balance must throw
    bool threw = false;
    try { cm.redeemLoyaltyPoints(rec.customerId, 200); }
    catch (...) { threw = true; }
    assert(threw && "Redeeming more than balance must throw");

    CustomerManager::getInstance().reset();
    sessions.clear();
}

int main() {
    std::cout << "=== Customer Server Unit Tests ===\n\n";

    RUN(test_login_valid_credentials_returns_token);
    RUN(test_login_invalid_credentials_returns_failure);
    RUN(test_me_returns_only_authenticated_customer_data);
    RUN(test_stats_computes_correct_totals);
    RUN(test_loyalty_returns_correct_balance);
    RUN(test_no_token_is_unauthorized);
    RUN(test_redeem_loyalty_points);

    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed > 0 ? 1 : 0;
}
