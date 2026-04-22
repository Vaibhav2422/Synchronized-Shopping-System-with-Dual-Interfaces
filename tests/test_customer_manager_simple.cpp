#include "../include/CustomerManager.h"
#include <iostream>
#include <cassert>
#include <set>
#include <cmath>

// Reset singleton state between test groups
static void resetManager() {
    CustomerManager::getInstance().reset();
}

// ─── 1. ID uniqueness across sequential registrations ────────────────────────
void testIdUniqueness() {
    std::cout << "Testing ID uniqueness..." << std::endl;
    resetManager();
    auto& cm = CustomerManager::getInstance();

    std::set<std::string> ids;
    for (int i = 0; i < 10; ++i) {
        auto rec = cm.registerCustomer("Name" + std::to_string(i),
                                       "900000000" + std::to_string(i),
                                       "pass" + std::to_string(i));
        assert(ids.find(rec.customerId) == ids.end() && "Duplicate ID detected");
        ids.insert(rec.customerId);
    }
    std::cout << "  ✓ ID uniqueness test passed" << std::endl;
}

// ─── 2. Password hash is not plaintext ───────────────────────────────────────
void testPasswordNotPlaintext() {
    std::cout << "Testing password not stored in plaintext..." << std::endl;
    resetManager();
    auto& cm = CustomerManager::getInstance();

    std::string password = "MySecret123";
    auto rec = cm.registerCustomer("Alice", "9000000001", password);

    assert(rec.passwordHash != password && "Password stored in plaintext!");
    assert(!rec.passwordHash.empty() && "Password hash is empty!");
    // SHA-256 hex is always 64 chars
    assert(rec.passwordHash.size() == 64 && "Hash length unexpected");
    std::cout << "  ✓ Password not plaintext test passed" << std::endl;
}

// ─── 3. Search by phone, ID, name (exact and partial, case-insensitive) ──────
void testSearch() {
    std::cout << "Testing search functionality..." << std::endl;
    resetManager();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Rahul Sharma", "9876543210", "pass1");

    // Search by exact phone
    auto r1 = cm.search("9876543210");
    assert(!r1.empty() && "Search by phone failed");
    assert(r1[0].customerId == rec.customerId);

    // Search by exact ID
    auto r2 = cm.search(rec.customerId);
    assert(!r2.empty() && "Search by ID failed");
    assert(r2[0].customerId == rec.customerId);

    // Search by partial name (case-insensitive)
    auto r3 = cm.search("rahul");
    assert(!r3.empty() && "Search by partial name (lower) failed");

    auto r4 = cm.search("SHARMA");
    assert(!r4.empty() && "Search by partial name (upper) failed");

    // Search that should return nothing
    auto r5 = cm.search("zzznomatch");
    assert(r5.empty() && "Search should return empty for no match");

    std::cout << "  ✓ Search tests passed" << std::endl;
}

// ─── 4. Authentication success and failure ───────────────────────────────────
void testAuthentication() {
    std::cout << "Testing authentication..." << std::endl;
    resetManager();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Bob", "9111111111", "correctPass");

    assert(cm.authenticate(rec.customerId, "correctPass") && "Auth should succeed with correct password");
    assert(!cm.authenticate(rec.customerId, "wrongPass")  && "Auth should fail with wrong password");
    assert(!cm.authenticate("CUST9999",    "correctPass") && "Auth should fail for unknown ID");

    std::cout << "  ✓ Authentication tests passed" << std::endl;
}

// ─── 5. Loyalty points calculation: floor(amount / 100) ──────────────────────
void testLoyaltyPointsCalculation() {
    std::cout << "Testing loyalty points calculation..." << std::endl;

    assert(CustomerManager::computePointsEarned(0.0)    == 0);
    assert(CustomerManager::computePointsEarned(99.99)  == 0);
    assert(CustomerManager::computePointsEarned(100.0)  == 1);
    assert(CustomerManager::computePointsEarned(199.99) == 1);
    assert(CustomerManager::computePointsEarned(200.0)  == 2);
    assert(CustomerManager::computePointsEarned(1500.0) == 15);
    assert(CustomerManager::computePointsEarned(999.0)  == 9);

    std::cout << "  ✓ Loyalty points calculation tests passed" << std::endl;
}

// ─── 6. Loyalty points add/deduct ────────────────────────────────────────────
void testLoyaltyPointsAddDeduct() {
    std::cout << "Testing loyalty points add/deduct..." << std::endl;
    resetManager();
    auto& cm = CustomerManager::getInstance();

    auto rec = cm.registerCustomer("Carol", "9222222222", "pass");
    cm.addLoyaltyPoints(rec.customerId, 10);
    cm.addLoyaltyPoints(rec.customerId, 5);

    auto* r = cm.findById(rec.customerId);
    assert(r && r->loyaltyPoints == 15 && "Loyalty points add failed");

    cm.redeemLoyaltyPoints(rec.customerId, 7);
    r = cm.findById(rec.customerId);
    assert(r && r->loyaltyPoints == 8 && "Loyalty points deduct failed");

    // Redeem more than balance should throw
    bool threw = false;
    try { cm.redeemLoyaltyPoints(rec.customerId, 100); }
    catch (...) { threw = true; }
    assert(threw && "Should throw on insufficient points");

    std::cout << "  ✓ Loyalty points add/deduct tests passed" << std::endl;
}

int main() {
    std::cout << "=== CustomerManager Unit Tests ===" << std::endl;
    std::cout << "_Requirements: 1.3, 2.2, 3.2, 9.1_\n" << std::endl;

    testIdUniqueness();
    testPasswordNotPlaintext();
    testSearch();
    testAuthentication();
    testLoyaltyPointsCalculation();
    testLoyaltyPointsAddDeduct();

    std::cout << "\n✅ All CustomerManager unit tests passed!" << std::endl;
    return 0;
}
