#ifndef PAYMENT_EXCEPTION_H
#define PAYMENT_EXCEPTION_H

#include "ShoppingException.h"

/**
 * Base exception for payment processing errors.
 */
class PaymentFailedException : public ShoppingException {
protected:
    std::string paymentMethod;
    double amount;
    
public:
    PaymentFailedException(const std::string& reason, const std::string& method, double amt)
        : ShoppingException("Payment failed: " + reason, "PAY_ERR")
        , paymentMethod(method)
        , amount(amt) {}
    
    std::string getPaymentMethod() const { return paymentMethod; }
    double getAmount() const { return amount; }
};

/**
 * Exception thrown when cash payment has insufficient funds.
 */
class InsufficientFundsException : public PaymentFailedException {
private:
    double tendered;
    
public:
    InsufficientFundsException(double required, double provided)
        : PaymentFailedException("Insufficient funds", "Cash", required)
        , tendered(provided) {}
    
    double getTendered() const { return tendered; }
    
    std::string getDetailedMessage() const override {
        return "[PAY_ERR] Insufficient funds. Required: Rs. " + 
               std::to_string(amount) + ", Tendered: Rs. " + std::to_string(tendered);
    }
};

/**
 * Exception thrown when card details are invalid.
 */
class InvalidCardException : public PaymentFailedException {
private:
    std::string cardNumber;
    
public:
    InvalidCardException(const std::string& cardNum, double amt)
        : PaymentFailedException("Invalid card details", "Card", amt)
        , cardNumber(cardNum) {}
    
    std::string getCardNumber() const { return cardNumber; }
};

/**
 * Exception thrown when payment processing times out.
 */
class PaymentTimeoutException : public PaymentFailedException {
public:
    PaymentTimeoutException(const std::string& method, double amt)
        : PaymentFailedException("Payment timeout", method, amt) {}
};

#endif // PAYMENT_EXCEPTION_H
