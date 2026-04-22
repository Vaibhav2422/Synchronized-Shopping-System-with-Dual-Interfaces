#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <vector>
#include <utility>
#include "Cart.h"

/**
 * Order represents a completed transaction.
 * Captures a snapshot of cart contents, total amount, payment method,
 * and timestamps at the moment of checkout.
 * Supports pipe-delimited serialization for file persistence.
 * Requirements: 15.1, 15.2, 15.4
 */
class Order {
private:
    int orderId;
    std::vector<std::pair<std::string, int>> products;  // name, quantity
    double totalAmount;
    std::string paymentMethod;
    std::string orderDate;
    std::string orderTime;

    static int nextOrderId;

    // Private default constructor used only by deserialize
    Order();

public:
    Order(const Cart& cart, double total, const std::string& payment);

    // Getters
    int getOrderId() const;
    double getTotalAmount() const;
    std::string getPaymentMethod() const;
    std::string getOrderDate() const;
    std::string getOrderTime() const;
    const std::vector<std::pair<std::string, int>>& getProducts() const;

    // Display
    void display() const;

    // Serialization
    std::string serialize() const;
    static Order deserialize(const std::string& data);
};

#endif // ORDER_H
