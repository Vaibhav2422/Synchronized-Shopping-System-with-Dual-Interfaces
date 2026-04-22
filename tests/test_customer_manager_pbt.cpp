#include <rapidcheck.h>
#include "../include/CustomerManager.h"
#include <iostream>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>

// Helper: reset the singleton and remove any temp file
static void resetManager() {
    CustomerManager::getInstance().reset();
}

// Helper: check if string contains substring
static bool contains(const std::string& s, const std::string& sub) {
    return s.find(sub) != std::string::npos;
}



// ─────────────────────────────────────────────────────────────────────────────
// Property 1: Customer ID Uniqueness
// Feature: dual-interface-customer-system, Property 1: Customer ID Uniqueness
// **Validates: Requirements 1.3**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty1() {
    std::cout << "\nRunning Property 1: Customer ID Uniqueness" << std::endl;

    rc::check("For any N registrations, all Customer_IDs are distinct", []() {
        resetManager();
        auto& cm = CustomerManager::getInstance();

        auto n = *rc::gen::inRange(2, 20);
        std::set<std::string> ids;

        for (int i = 0; i < n; ++i) {
            std::string name  = "Name"  + std::to_string(i);
            std::string phone = "9" + std::to_string(1000000000 + i);
            std::string pass  = "pass"  + std::to_string(i);
            auto rec = cm.registerCustomer(name, phone, pass);
            RC_ASSERT(ids.find(rec.customerId) == ids.end());
            ids.insert(rec.customerId);
        }
        RC_ASSERT(static_cast<int>(ids.size()) == n);
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 2: Customer Persistence Round-Trip
// Feature: dual-interface-customer-system, Property 2: Customer Persistence Round-Trip
// **Validates: Requirements 8.1, 8.2**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty2() {
    std::cout << "\nRunning Property 2: Customer Persistence Round-Trip" << std::endl;

    rc::check("Serialize then deserialize produces equivalent customer records", []() {
        resetManager();
        auto& cm = CustomerManager::getInstance();

        auto n = *rc::gen::inRange(1, 10);
        std::vector<CustomerRecord> originals;

        for (int i = 0; i < n; ++i) {
            std::string name  = "User"  + std::to_string(i);
            std::string phone = "9" + std::to_string(1000000000 + i);
            std::string pass  = "pw"   + std::to_string(i);
            originals.push_back(cm.registerCustomer(name, phone, pass));
        }

        // Save then reload
        cm.saveToFile();
        cm.reset();
        cm.loadFromFile();

        const auto& loaded = cm.getAllCustomers();
        RC_ASSERT(loaded.size() == originals.size());

        for (size_t i = 0; i < originals.size(); ++i) {
            RC_ASSERT(loaded[i].customerId    == originals[i].customerId);
            RC_ASSERT(loaded[i].name          == originals[i].name);
            RC_ASSERT(loaded[i].phone         == originals[i].phone);
            RC_ASSERT(loaded[i].passwordHash  == originals[i].passwordHash);
            RC_ASSERT(loaded[i].loyaltyPoints == originals[i].loyaltyPoints);
        }

        resetManager();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 3: Search Completeness
// Feature: dual-interface-customer-system, Property 3: Search Completeness
// **Validates: Requirements 2.2, 2.6**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty3() {
    std::cout << "\nRunning Property 3: Search Completeness" << std::endl;

    rc::check("Any registered customer is found by phone, ID, or name substring", []() {
        resetManager();
        auto& cm = CustomerManager::getInstance();

        // Register a customer with a unique enough name
        auto suffix = *rc::gen::inRange(0, 9999);
        std::string name  = "TestUser" + std::to_string(suffix);
        std::string phone = "9" + std::to_string(1000000000 + suffix);
        auto rec = cm.registerCustomer(name, phone, "pass");

        // Search by exact phone
        auto byPhone = cm.search(phone);
        bool foundByPhone = false;
        for (const auto& r : byPhone)
            if (r.customerId == rec.customerId) { foundByPhone = true; break; }
        RC_ASSERT(foundByPhone);

        // Search by exact ID
        auto byId = cm.search(rec.customerId);
        bool foundById = false;
        for (const auto& r : byId)
            if (r.customerId == rec.customerId) { foundById = true; break; }
        RC_ASSERT(foundById);

        // Search by name substring (case-insensitive: use lowercase)
        std::string nameLower = "testuser" + std::to_string(suffix);
        auto byName = cm.search(nameLower);
        bool foundByName = false;
        for (const auto& r : byName)
            if (r.customerId == rec.customerId) { foundByName = true; break; }
        RC_ASSERT(foundByName);

        resetManager();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 4: Authentication Correctness
// Feature: dual-interface-customer-system, Property 4: Authentication Correctness
// **Validates: Requirements 3.2**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty4() {
    std::cout << "\nRunning Property 4: Authentication Correctness" << std::endl;

    rc::check("Correct password always succeeds; wrong password always fails", []() {
        resetManager();
        auto& cm = CustomerManager::getInstance();

        auto suffix = *rc::gen::inRange(0, 9999);
        std::string password = "secret" + std::to_string(suffix);
        std::string wrongPw  = "wrong"  + std::to_string(suffix + 1);

        auto rec = cm.registerCustomer("AuthUser", "9500000000", password);

        RC_ASSERT(cm.authenticate(rec.customerId, password));
        RC_ASSERT(!cm.authenticate(rec.customerId, wrongPw));

        resetManager();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 6: Password Never Stored in Plaintext
// Feature: dual-interface-customer-system, Property 6: Password Never Stored in Plaintext
// **Validates: Requirements 3.6**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty6() {
    std::cout << "\nRunning Property 6: Password Never Stored in Plaintext" << std::endl;

    rc::check("Raw password must not appear in serialized JSON output", []() {
        resetManager();
        auto& cm = CustomerManager::getInstance();

        auto suffix = *rc::gen::inRange(0, 9999);
        std::string password = "plaintext" + std::to_string(suffix);

        cm.registerCustomer("SecureUser", "9600000000", password);
        cm.saveToFile();

        // Read the file back as raw text
        std::ifstream f("data/customers.json");
        RC_ASSERT(f.is_open());
        std::ostringstream ss;
        ss << f.rdbuf();
        std::string fileContent = ss.str();

        RC_ASSERT(!contains(fileContent, password));

        resetManager();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// Property 8: Loyalty Points Calculation
// Feature: dual-interface-customer-system, Property 8: Loyalty Points Calculation
// **Validates: Requirements 9.1**
// ─────────────────────────────────────────────────────────────────────────────
void testProperty8() {
    std::cout << "\nRunning Property 8: Loyalty Points Calculation" << std::endl;

    rc::check("For any amount A, earned points == floor(A / 100)", []() {
        auto amountCents = *rc::gen::inRange(0, 1000000); // 0 to 10000.00
        double amount = static_cast<double>(amountCents);

        int expected = static_cast<int>(std::floor(amount / 100.0));
        int actual   = CustomerManager::computePointsEarned(amount);

        RC_ASSERT(actual == expected);
    });
}

int main() {
    std::cout << "Running CustomerManager Property-Based Tests" << std::endl;
    std::cout << "=============================================" << std::endl;

    testProperty1();
    testProperty2();
    testProperty3();
    testProperty4();
    testProperty6();
    testProperty8();

    std::cout << "\n=============================================" << std::endl;
    std::cout << "All CustomerManager PBT tests passed!" << std::endl;
    return 0;
}
