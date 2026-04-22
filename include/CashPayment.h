#ifndef CASH_PAYMENT_H
#define CASH_PAYMENT_H

#include "Payment.h"
#include <string>

/**
 * Cash payment — concrete Strategy for the Strategy design pattern.
 * Implements the Payment interface to process cash payments with
 * change calculation. Throws InsufficientFundsException if cash < amount.
 * Interchangeable at runtime with UPIPayment and CardPayment.
 * Requirements: 8.1, 8.5
 */
class CashPayment : public Payment {
private:
    double cashTendered;
    
public:
    CashPayment(double amt, double tendered);
    
    bool pay() override;
    std::string getPaymentMethod() const override;
    double calculateChange() const;
};

#endif // CASH_PAYMENT_H
