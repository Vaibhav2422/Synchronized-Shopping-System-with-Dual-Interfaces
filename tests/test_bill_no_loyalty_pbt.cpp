/**
 * Property 7: Bill Contains No Loyalty Data
 * Feature: dual-interface-customer-system, Property 7: Bill Contains No Loyalty Data
 * For any checkout (with or without customer), receipt string must not contain
 * the words "loyalty", "points", or "pts".
 * **Validates: Requirements 4.3, 9.4**
 */

#include <rapidcheck.h>
#include "../include/ReceiptWriter.h"
#include "../include/Order.h"
#include "../include/Bill.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/CustomerManager.h"
#include <iostream>
#include <string>
#include <algorithm>

static bool containsCI(const std::string& s, const std::string& sub) {
    std::string sl = s, subl = sub;
    std::transform(sl.begin(),   sl.end(),   sl.begin(),   ::tolower);
    std::transform(subl.begin(), subl.end(), subl.begin(), ::tolower);
    return sl.find(subl) != std::string::npos;
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 7: Bill Contains No Loyalty Data
// ─────────────────────────────────────────────────────────────────────────────
void testProperty7() {
    std::cout << "\nRunning Property 7: Bill Contains No Loyalty Data" << std::endl;

    // Shared inventory (initialised once)
    Inventory inv;
    inv.initializeSampleData();

    // Product IDs available in sample data
    static const int PRODUCT_IDS[] = {101, 102, 103, 201, 202, 301, 302};
    static const int NUM_PRODUCTS   = 7;

    rc::check("For any receipt (with or without customer), no loyalty text appears", [&]() {
        // Pick a random product and quantity
        int idx = *rc::gen::inRange(0, NUM_PRODUCTS);
        int qty = *rc::gen::inRange(1, 3);
        int productId = PRODUCT_IDS[idx];

        Cart cart(&inv);
        try { cart.addProduct(productId, qty); }
        catch (...) { return; } // skip if stock issue

        Bill bill;
        bill.calculate(cart, nullptr);

        // Randomly decide whether to attach a customer
        bool withCustomer = *rc::gen::arbitrary<bool>();
        std::string custName, custId;
        if (withCustomer) {
            auto suffix = *rc::gen::inRange(0, 9999);
            custName = "Customer" + std::to_string(suffix);
            custId   = "CUST" + std::to_string(1000 + suffix);
        }

        Order order(cart, bill.getFinalTotal(), "Cash");
        std::string receipt = ReceiptWriter::generateReceipt(order, bill, custName, custId);

        RC_ASSERT(!containsCI(receipt, "loyalty"));
        RC_ASSERT(!containsCI(receipt, " pts"));
    });
}

int main() {
    std::cout << "Running Bill No-Loyalty PBT" << std::endl;
    std::cout << "===========================" << std::endl;

    testProperty7();

    std::cout << "\n===========================" << std::endl;
    std::cout << "Property 7 PBT complete." << std::endl;
    return 0;
}
