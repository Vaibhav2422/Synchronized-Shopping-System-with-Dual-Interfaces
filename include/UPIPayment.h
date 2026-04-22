#ifndef UPI_PAYMENT_H
#define UPI_PAYMENT_H

#include "Payment.h"
#include <string>

/**
 * UPI payment — concrete Strategy for the Strategy design pattern.
 * Implements the Payment interface to process payments through a UPI ID.
 * Interchangeable at runtime with CardPayment and CashPayment.
 * Requirements: 8.1, 8.3
 */
class UPIPayment : public Payment {
private:
    std::string upiId;
    
public:
    UPIPayment(double amt, const std::string& upiId);
    
    bool pay() override;
    std::string getPaymentMethod() const override;
};

#endif // UPI_PAYMENT_H
