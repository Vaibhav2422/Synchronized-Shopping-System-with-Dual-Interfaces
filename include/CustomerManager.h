#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include <string>
#include <vector>

/**
 * CustomerRecord holds all data for a single registered customer.
 */
struct CustomerRecord {
    std::string customerId;    // e.g. "CUST0001"
    std::string name;
    std::string phone;
    std::string passwordHash;  // SHA-256 hex of password
    int loyaltyPoints;
    std::vector<int> orderIds; // linked Order IDs
    std::string createdAt;
};

/**
 * CustomerManager singleton class for customer account management.
 * Handles registration, search, authentication, loyalty points, and persistence.
 * Requirements: 1.3, 1.5, 2.2, 2.6, 3.2, 3.6, 8.1-8.5, 9.1, 9.2
 */
class CustomerManager {
public:
    static CustomerManager& getInstance();

    CustomerManager(const CustomerManager&) = delete;
    CustomerManager& operator=(const CustomerManager&) = delete;
    CustomerManager(CustomerManager&&) = delete;
    CustomerManager& operator=(CustomerManager&&) = delete;

    // Registration
    CustomerRecord registerCustomer(const std::string& name,
                                    const std::string& phone,
                                    const std::string& password);

    // Search (by ID, phone, or name — case-insensitive partial)
    std::vector<CustomerRecord> search(const std::string& query) const;
    CustomerRecord* findById(const std::string& id);
    const CustomerRecord* findById(const std::string& id) const;
    CustomerRecord* findByPhone(const std::string& phone);

    // Authentication
    bool authenticate(const std::string& id, const std::string& password) const;

    // Loyalty
    void addLoyaltyPoints(const std::string& customerId, int points);
    void redeemLoyaltyPoints(const std::string& customerId, int points);

    // Compute loyalty points earned for a given purchase amount: floor(amount / 100)
    static int computePointsEarned(double amount);

    // Order linking
    void linkOrder(const std::string& customerId, int orderId);

    // Persistence
    void saveToFile() const;
    void loadFromFile();

    // Reset all customers (used in tests)
    void reset();

    // Access all customers (read-only)
    const std::vector<CustomerRecord>& getAllCustomers() const;

    // Static helpers (public for testing)
    static std::string hashPassword(const std::string& password);

private:
    CustomerManager();

    std::vector<CustomerRecord> customers;
    int nextIndex;

    std::string generateId() const;
    static std::string toLower(const std::string& s);

    static constexpr const char* DATA_FILE = "data/customers.json";
};

#endif // CUSTOMERMANAGER_H
