#ifndef CARD_PAYMENT_H
#define CARD_PAYMENT_H

#include "Payment.h"
#include <string>

/**
 * Card payment — concrete Strategy for the Strategy design pattern.
 * Implements the Payment interface to process credit/debit card payments
 * with masked card number display for security.
 * Interchangeable at runtime with UPIPayment and CashPayment.
 * Requirements: 8.1, 8.4
 */
class CardPayment : public Payment {
private:
    std::string cardNumber;
    std::string cardHolderName;
    
    std::string maskCardNumber() const;
    
public:
    CardPayment(double amt, const std::string& cardNum, const std::string& holderName);
    
    bool pay() override;
    std::string getPaymentMethod() const override;
};

#endif // CARD_PAYMENT_H
