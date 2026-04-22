#include "Order.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

int Order::nextOrderId = 1;

Order::Order()
    : orderId(0)
    , totalAmount(0.0)
    , paymentMethod("")
    , orderDate("")
    , orderTime("") {
}

Order::Order(const Cart& cart, double total, const std::string& payment)
    : orderId(nextOrderId++)
    , totalAmount(total)
    , paymentMethod(payment) {
    // Extract product names and quantities from cart
    for (const auto& item : cart.getItems()) {
        products.emplace_back(item.product->getName(), item.quantity);
    }

    // Generate current date and time
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);

    char dateBuf[11];
    char timeBuf[9];
    std::strftime(dateBuf, sizeof(dateBuf), "%d/%m/%Y", tm_info);
    std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", tm_info);

    orderDate = dateBuf;
    orderTime = timeBuf;

    Logger::logInfo("Order", "Order created: ID " + std::to_string(orderId)
        + ", Total: " + std::to_string(totalAmount)
        + ", Payment: " + paymentMethod);
}

int Order::getOrderId() const {
    return orderId;
}

double Order::getTotalAmount() const {
    return totalAmount;
}

std::string Order::getPaymentMethod() const {
    return paymentMethod;
}

std::string Order::getOrderDate() const {
    return orderDate;
}

std::string Order::getOrderTime() const {
    return orderTime;
}

const std::vector<std::pair<std::string, int>>& Order::getProducts() const {
    return products;
}

void Order::display() const {
    using namespace ANSIColors;

    std::cout << "\n";
    std::cout << CYAN << BOLD
              << "╔════════════════════════════════════════════════════════╗\n"
              << "║              ORDER #" << std::setw(4) << std::setfill('0') << orderId
              << std::setfill(' ')
              << "                                  ║\n"
              << "╚════════════════════════════════════════════════════════╝"
              << RESET << "\n\n";

    std::cout << WHITE << "Date: " << orderDate
              << "   Time: " << orderTime << RESET << "\n\n";

    std::cout << CYAN << BOLD << "Products Ordered:\n" << RESET;
    std::cout << CYAN << "────────────────────────────────────────────────────────\n" << RESET;

    for (const auto& p : products) {
        std::cout << WHITE << "  " << p.first
                  << YELLOW << "  x" << p.second << RESET << "\n";
    }

    std::cout << CYAN << "────────────────────────────────────────────────────────\n" << RESET;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << WHITE << "Total Amount:  " << YELLOW << "₹" << totalAmount << RESET << "\n";
    std::cout << WHITE << "Payment Method: " << GREEN << paymentMethod << RESET << "\n\n";
}

std::string Order::serialize() const {
    // Serialization format (pipe-delimited fields):
    //   orderId|date|time|paymentMethod|totalAmount|name1:qty1,name2:qty2,...
    // The products field uses comma-separated "name:qty" pairs.
    // Colons in product names are not escaped; rfind(':') is used on deserialization
    // to correctly split on the last colon, preserving names with colons.
    std::ostringstream oss;
    oss << orderId << "|"
        << orderDate << "|"
        << orderTime << "|"
        << paymentMethod << "|"
        << std::fixed << std::setprecision(2) << totalAmount << "|";

    for (std::size_t i = 0; i < products.size(); ++i) {
        if (i > 0) oss << ",";
        oss << products[i].first << ":" << products[i].second;
    }

    return oss.str();
}

Order Order::deserialize(const std::string& data) {
    Order order;

    std::istringstream ss(data);
    std::string token;

    // orderId
    std::getline(ss, token, '|');
    order.orderId = std::stoi(token);

    // date
    std::getline(ss, order.orderDate, '|');

    // time
    std::getline(ss, order.orderTime, '|');

    // paymentMethod
    std::getline(ss, order.paymentMethod, '|');

    // totalAmount
    std::getline(ss, token, '|');
    order.totalAmount = std::stod(token);

    // products: name1:qty1,name2:qty2,...
    std::string productsStr;
    std::getline(ss, productsStr);

    if (!productsStr.empty()) {
        std::istringstream ps(productsStr);
        std::string entry;
        while (std::getline(ps, entry, ',')) {
            auto colonPos = entry.rfind(':');
            if (colonPos != std::string::npos) {
                std::string name = entry.substr(0, colonPos);
                int qty = std::stoi(entry.substr(colonPos + 1));
                order.products.emplace_back(name, qty);
            }
        }
    }

    return order;
}
