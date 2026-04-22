/**
 * Property 5: Data Isolation
 * Feature: dual-interface-customer-system, Property 5: Data Isolation
 * For any two distinct customers A and B, a session authenticated as A
 * must never return data belonging to B (orders, stats, loyalty points).
 * **Validates: Requirements 3.4, 3.5**
 */

#include <rapidcheck.h>
#include "../include/CustomerManager.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Minimal session store mirroring customer_server.cpp
static std::map<std::string, std::string> g_sessions;

static std::string fakeLogin(const std::string& customerId, const std::string& password) {
    if (!CustomerManager::getInstance().authenticate(customerId, password))
        return "";
    std::string token = "tok_" + customerId + "_" + std::to_string(rand() % 100000);
    g_sessions[token] = customerId;
    return token;
}

static std::string resolveCustomer(const std::string& token) {
    auto it = g_sessions.find(token);
    return (it != g_sessions.end()) ? it->second : "";
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 5: Data Isolation
// ─────────────────────────────────────────────────────────────────────────────
void testProperty5() {
    std::cout << "\nRunning Property 5: Data Isolation" << std::endl;

    rc::check("For any two distinct customers, session A never returns data from customer B", []() {
        CustomerManager::getInstance().reset();
        g_sessions.clear();
        auto& cm = CustomerManager::getInstance();

        // Generate two distinct customers
        auto suffixA = *rc::gen::inRange(0, 4999);
        auto suffixB = *rc::gen::inRange(5000, 9999);

        std::string passA = "passA" + std::to_string(suffixA);
        std::string passB = "passB" + std::to_string(suffixB);

        auto recA = cm.registerCustomer("UserA" + std::to_string(suffixA),
                                        "91" + std::to_string(100000000 + suffixA),
                                        passA);
        auto recB = cm.registerCustomer("UserB" + std::to_string(suffixB),
                                        "92" + std::to_string(100000000 + suffixB),
                                        passB);

        // Add different loyalty points to each
        cm.addLoyaltyPoints(recA.customerId, 10 + suffixA % 50);
        cm.addLoyaltyPoints(recB.customerId, 60 + suffixB % 50);

        // Link different fake order IDs
        cm.linkOrder(recA.customerId, 1000 + suffixA);
        cm.linkOrder(recB.customerId, 2000 + suffixB);

        // Login as A
        std::string tokenA = fakeLogin(recA.customerId, passA);
        RC_ASSERT(!tokenA.empty());

        // Login as B
        std::string tokenB = fakeLogin(recB.customerId, passB);
        RC_ASSERT(!tokenB.empty());

        // Token A must resolve to A, not B
        std::string resolvedA = resolveCustomer(tokenA);
        RC_ASSERT(resolvedA == recA.customerId);
        RC_ASSERT(resolvedA != recB.customerId);

        // Token B must resolve to B, not A
        std::string resolvedB = resolveCustomer(tokenB);
        RC_ASSERT(resolvedB == recB.customerId);
        RC_ASSERT(resolvedB != recA.customerId);

        // Data fetched via token A must be A's data
        const CustomerRecord* custA = cm.findById(resolvedA);
        RC_ASSERT(custA != nullptr);
        RC_ASSERT(custA->customerId == recA.customerId);
        RC_ASSERT(custA->name != recB.name);

        // Data fetched via token B must be B's data
        const CustomerRecord* custB = cm.findById(resolvedB);
        RC_ASSERT(custB != nullptr);
        RC_ASSERT(custB->customerId == recB.customerId);
        RC_ASSERT(custB->name != recA.name);

        // Loyalty points must not cross-contaminate
        RC_ASSERT(custA->loyaltyPoints != custB->loyaltyPoints);

        // Order IDs must be distinct
        RC_ASSERT(custA->orderIds != custB->orderIds);

        CustomerManager::getInstance().reset();
        g_sessions.clear();
    });
}

int main() {
    std::cout << "Running Data Isolation PBT" << std::endl;
    std::cout << "==========================" << std::endl;

    testProperty5();

    std::cout << "\n==========================" << std::endl;
    std::cout << "Property 5 PBT complete." << std::endl;
    return 0;
}
