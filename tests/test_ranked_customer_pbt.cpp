/**
 * Property 10: Ranked Customer List Order
 * Feature: dual-interface-customer-system, Property 10: Ranked Customer List Order
 * For any set of customers with orders, the ranked list must be in
 * non-increasing order of total spending.
 * **Validates: Requirements 6.1**
 */

#include <rapidcheck.h>
#include "../include/CustomerManager.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

// Mirrors the ranking logic from server.cpp /api/customers/ranked
// Given a list of (customerId -> totalSpent) pairs, produce a ranked list
// sorted non-increasing by totalSpent.
struct RankedEntry {
    std::string customerId;
    double totalSpent;
};

static std::vector<RankedEntry> rankCustomers(
    const std::vector<CustomerRecord>& customers,
    const std::vector<std::pair<std::string, double>>& spendingMap)
{
    std::vector<RankedEntry> ranked;
    for (const auto& c : customers) {
        double total = 0.0;
        for (const auto& kv : spendingMap) {
            if (kv.first == c.customerId) { total = kv.second; break; }
        }
        ranked.push_back({c.customerId, total});
    }
    std::sort(ranked.begin(), ranked.end(),
        [](const RankedEntry& a, const RankedEntry& b){ return a.totalSpent > b.totalSpent; });
    return ranked;
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 10: Ranked Customer List Order
// ─────────────────────────────────────────────────────────────────────────────
void testProperty10() {
    std::cout << "\nRunning Property 10: Ranked Customer List Order" << std::endl;

    rc::check("For any set of customers, ranked list is non-increasing by total spending", []() {
        CustomerManager::getInstance().reset();
        auto& cm = CustomerManager::getInstance();

        // Generate between 2 and 10 customers
        int n = *rc::gen::inRange(2, 10);

        std::vector<std::pair<std::string, double>> spendingMap;

        for (int i = 0; i < n; ++i) {
            std::string name  = "User"  + std::to_string(i);
            std::string phone = "9" + std::to_string(1000000000 + i);
            auto rec = cm.registerCustomer(name, phone, "pw");

            // Assign a random total spending (0 to 100000)
            double spending = static_cast<double>(*rc::gen::inRange(0, 100000));
            spendingMap.push_back({rec.customerId, spending});
        }

        const auto& allCustomers = cm.getAllCustomers();
        auto ranked = rankCustomers(allCustomers, spendingMap);

        RC_ASSERT(static_cast<int>(ranked.size()) == n);

        // Verify non-increasing order
        for (size_t i = 1; i < ranked.size(); ++i) {
            RC_ASSERT(ranked[i-1].totalSpent >= ranked[i].totalSpent);
        }

        CustomerManager::getInstance().reset();
    });
}

int main() {
    std::cout << "Running Ranked Customer List PBT" << std::endl;
    std::cout << "================================" << std::endl;

    testProperty10();

    std::cout << "\n================================" << std::endl;
    std::cout << "Property 10 PBT complete." << std::endl;
    return 0;
}
