/**
 * Property 9: Customer Stats Accuracy
 * Feature: dual-interface-customer-system, Property 9: Customer Stats Accuracy
 * For any customer with N linked orders, totalSpent == sum of order totals,
 * and totalProducts == sum of all item quantities across those orders.
 * **Validates: Requirements 5.2, 5.4, 5.5**
 */

#include <rapidcheck.h>
#include "../include/CustomerManager.h"
#include "../include/OrderHistory.h"
#include "../include/Order.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

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

// ─────────────────────────────────────────────────────────────────────────────
// Property 9: Customer Stats Accuracy
// ─────────────────────────────────────────────────────────────────────────────
void testProperty9() {
    std::cout << "\nRunning Property 9: Customer Stats Accuracy" << std::endl;

    // Available product IDs in sample data
    static const int PRODUCT_IDS[] = {101, 102, 103, 201, 202, 301, 302};
    static const int NUM_PRODUCTS   = 7;

    rc::check("For any customer with N orders, totalSpent == sum of order totals, "
              "totalProducts == sum of quantities", [&]() {
        CustomerManager::getInstance().reset();
        auto& cm = CustomerManager::getInstance();

        // Fresh inventory per iteration to avoid stock exhaustion
        Inventory localInv;
        localInv.initializeSampleData();

        auto suffix = *rc::gen::inRange(0, 9999);
        auto rec = cm.registerCustomer("StatsUser" + std::to_string(suffix),
                                       "93" + std::to_string(100000000 + suffix),
                                       "pw");

        // Generate between 1 and 5 orders
        int numOrders = *rc::gen::inRange(1, 5);

        OrderHistory hist;
        double expectedTotalSpent    = 0.0;
        int    expectedTotalProducts = 0;

        for (int i = 0; i < numOrders; ++i) {
            int pidx = *rc::gen::inRange(0, NUM_PRODUCTS);
            int qty  = *rc::gen::inRange(1, 3);
            int productId = PRODUCT_IDS[pidx];

            Cart cart(&localInv);
            try { cart.addProduct(productId, qty); }
            catch (...) {
                // If stock is exhausted, use product 101 qty 1
                try { cart.addProduct(101, 1); qty = 1; }
                catch (...) { continue; }
            }

            double orderTotal = static_cast<double>(*rc::gen::inRange(100, 5000));
            Order order(cart, orderTotal, "Cash");

            hist.addOrder(order);
            cm.linkOrder(rec.customerId, order.getOrderId());

            expectedTotalSpent    += orderTotal;
            expectedTotalProducts += qty;
        }

        const CustomerRecord* cust = cm.findById(rec.customerId);
        RC_ASSERT(cust != nullptr);

        CustomerStats stats = computeStats(*cust, hist);

        // totalSpent must equal exact sum of order totals
        RC_ASSERT(std::abs(stats.totalSpent - expectedTotalSpent) < 0.01);

        // totalProducts must equal sum of all item quantities
        RC_ASSERT(stats.totalProducts == expectedTotalProducts);

        CustomerManager::getInstance().reset();
    });
}

int main() {
    std::cout << "Running Customer Stats Accuracy PBT" << std::endl;
    std::cout << "====================================" << std::endl;

    testProperty9();

    std::cout << "\n====================================" << std::endl;
    std::cout << "Property 9 PBT complete." << std::endl;
    return 0;
}
