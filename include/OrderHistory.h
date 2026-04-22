#ifndef ORDER_HISTORY_H
#define ORDER_HISTORY_H

#include <vector>
#include <string>
#include "Order.h"

/**
 * OrderHistory class manages a collection of completed orders.
 * Provides persistence, display, and sales analytics functionality.
 * Requirements: 15.3, 15.4, 15.5, 15.6, 19.1-19.5
 */
class OrderHistory {
private:
    std::vector<Order> orders;
    std::string storageFile;

public:
    OrderHistory();
    explicit OrderHistory(const std::string& file);

    // Order management
    void addOrder(const Order& order);
    void displayHistory() const;

    // Persistence
    void saveToFile() const;
    void loadFromFile();

    // Analytics
    // Time Complexity: O(n×m) - iterates all orders and all items per order to build frequency map
    // Space Complexity: O(k) - unordered_map holds k distinct product names
    std::string getMostPurchasedProduct() const;

    // Time Complexity: O(n) - linear scan through all orders to sum totals
    // Space Complexity: O(1) - no additional storage required
    double getTotalRevenue() const;

    // Time Complexity: O(n) - delegates to getTotalRevenue (linear scan)
    // Space Complexity: O(1) - no additional storage required
    double getTotalGST() const;
    void displaySalesAnalytics() const;

    // Access
    const std::vector<Order>& getOrders() const;
};

#endif // ORDER_HISTORY_H
