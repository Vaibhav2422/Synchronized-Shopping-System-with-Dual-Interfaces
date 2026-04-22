#include "CashPayment.h"
#include "PaymentException.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>

CashPayment::CashPayment(double amt, double tendered)
    : Payment(amt), cashTendered(tendered) {}

double CashPayment::calculateChange() const {
    return cashTendered - amount;
}

bool CashPayment::pay() {
    std::cout << ANSIColors::CYAN << "\n=== Cash Payment Processing ===" << ANSIColors::RESET << "\n";
    std::cout << "Amount Due: " << ANSIColors::YELLOW << "Rs. " << std::fixed << std::setprecision(2) << amount << ANSIColors::RESET << "\n";
    std::cout << "Cash Tendered: " << ANSIColors::YELLOW << "Rs. " << std::fixed << std::setprecision(2) << cashTendered << ANSIColors::RESET << "\n";
    
    if (cashTendered < amount) {
        Logger::logError("Payment", "Cash payment failed - insufficient funds: Rs." + std::to_string(amount));
        throw InsufficientFundsException(amount, cashTendered);
    }
    
    double change = calculateChange();
    if (change > 0) {
        std::cout << "Change: " << ANSIColors::YELLOW << "Rs. " << std::fixed << std::setprecision(2) << change << ANSIColors::RESET << "\n";
    } else {
        std::cout << "Exact amount received.\n";
    }
    
    std::cout << ANSIColors::GREEN << "Payment successful!" << ANSIColors::RESET << "\n";
    Logger::logInfo("Payment", "Cash payment processed: Rs." + std::to_string(amount));
    return true;
}

std::string CashPayment::getPaymentMethod() const {
    return "Cash";
}
