#include "OrderHistory.h"
#include "FileIOException.h"
#include "InputValidator.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>

// SECURITY: Set restrictive file permissions after creation (Requirement 27.5).
// On POSIX systems, chmod() restricts access to owner only (mode 0600).
// On Windows, NTFS ACLs control access; no action is taken here.
#ifndef _WIN32
#include <sys/stat.h>
static void setFilePermissions(const std::string& path, int mode) {
    chmod(path.c_str(), static_cast<mode_t>(mode));
}
#else
static void setFilePermissions(const std::string&, int) {
    // Windows: rely on NTFS ACLs configured by the administrator.
}
#endif

OrderHistory::OrderHistory()
    : storageFile("data/orders.txt") {
}

OrderHistory::OrderHistory(const std::string& file)
    : storageFile(file) {
}

void OrderHistory::addOrder(const Order& order) {
    orders.push_back(order);
    Logger::logInfo("OrderHistory", "Order added: ID " + std::to_string(order.getOrderId()));
}

void OrderHistory::displayHistory() const {
    using namespace ANSIColors;

    std::cout << "\n" << CYAN << BOLD
              << "╔════════════════════════════════════════════════════════╗\n"
              << "║                    ORDER HISTORY                       ║\n"
              << "╚════════════════════════════════════════════════════════╝"
              << RESET << "\n\n";

    if (orders.empty()) {
        std::cout << YELLOW << "  No orders found.\n" << RESET << "\n";
        return;
    }

    for (const auto& order : orders) {
        order.display();
    }
}

void OrderHistory::saveToFile() const {
    // SECURITY: Validate file path to prevent directory traversal (Requirement 27.3)
    if (!InputValidator::validateFilePath(storageFile)) {
        throw FileWriteException(storageFile);
    }

    std::ofstream file(storageFile);
    if (!file.is_open()) {
        throw FileWriteException(storageFile);
    }

    for (const auto& order : orders) {
        file << order.serialize() << "\n";
    }

    if (file.fail()) {
        throw FileWriteException(storageFile);
    }

    // SECURITY: Set restrictive permissions (600) on the orders file so only
    // the owner can read/write it (Requirement 27.5).
    setFilePermissions(storageFile, 0600);

    Logger::logInfo("OrderHistory", "Orders saved to " + storageFile);
}

void OrderHistory::loadFromFile() {
    // SECURITY: Validate file path to prevent directory traversal (Requirement 27.3)
    if (!InputValidator::validateFilePath(storageFile)) {
        // Silently skip loading if path is invalid
        return;
    }

    std::ifstream file(storageFile);
    if (!file.is_open()) {
        // File may not exist on first run — silently ignore
        return;
    }

    orders.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            orders.push_back(Order::deserialize(line));
        } catch (...) {
            // Skip malformed lines
        }
    }

    if (file.bad()) {
        throw FileReadException(storageFile);
    }

    Logger::logInfo("OrderHistory", "Orders loaded from " + storageFile);
}

// Time Complexity: O(n×m) - iterates all n orders and all m items per order to build frequency map,
//                  then a second O(k) pass to find the max (k = distinct product names)
// Space Complexity: O(k) - unordered_map holds k distinct product name entries
std::string OrderHistory::getMostPurchasedProduct() const {
    if (orders.empty()) return "N/A";

    // Build a frequency map: product name → total units sold across all orders.
    // Then perform a single linear scan to find the entry with the highest count.
    // Time complexity: O(n × m) where n = orders, m = items per order.
    std::unordered_map<std::string, int> productCounts;

    for (const auto& order : orders) {
        for (const auto& item : order.getProducts()) {
            productCounts[item.first] += item.second;
        }
    }

    std::string maxProduct;
    int maxCount = 0;
    for (const auto& entry : productCounts) {
        if (entry.second > maxCount) {
            maxCount = entry.second;
            maxProduct = entry.first;
        }
    }

    return maxProduct.empty() ? "N/A" : maxProduct;
}

// Time Complexity: O(n) - linear scan through all orders to sum total amounts
// Space Complexity: O(1) - no additional storage required
double OrderHistory::getTotalRevenue() const {
    double total = 0.0;
    for (const auto& order : orders) {
        total += order.getTotalAmount();
    }
    return total;
}

// Time Complexity: O(n) - delegates to getTotalRevenue which performs a linear scan
// Space Complexity: O(1) - no additional storage required
double OrderHistory::getTotalGST() const {
    // Orders store the GST-inclusive final total, so we back-calculate the GST
    // component using the formula: GST = total × (rate / (1 + rate)).
    // With GST_RATE = 0.18: GST = total × (0.18 / 1.18)
    return getTotalRevenue() * (0.18 / 1.18);
}

void OrderHistory::displaySalesAnalytics() const {
    using namespace ANSIColors;

    std::cout << "\n" << CYAN << BOLD
              << "╔════════════════════════════════════════════════════════╗\n"
              << "║                  SALES ANALYTICS                       ║\n"
              << "╚════════════════════════════════════════════════════════╝"
              << RESET << "\n\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << WHITE << "  Most Purchased Product: "
              << YELLOW << getMostPurchasedProduct() << RESET << "\n";

    std::cout << WHITE << "  Total Revenue:          "
              << YELLOW << "₹" << getTotalRevenue() << RESET << "\n";

    std::cout << WHITE << "  Total GST Collected:    "
              << YELLOW << "₹" << getTotalGST() << RESET << "\n\n";
}

const std::vector<Order>& OrderHistory::getOrders() const {
    return orders;
}
