#include "UPIPayment.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>

UPIPayment::UPIPayment(double amt, const std::string& upiId)
    : Payment(amt), upiId(upiId) {}

bool UPIPayment::pay() {
    std::cout << ANSIColors::CYAN << "\n=== UPI Payment Processing ===" << ANSIColors::RESET << "\n";
    std::cout << "UPI ID: " << ANSIColors::YELLOW << upiId << ANSIColors::RESET << "\n";
    std::cout << "Amount: " << ANSIColors::YELLOW << "Rs. " << amount << ANSIColors::RESET << "\n";
    
    std::cout << "\nProcessing payment";
    std::cout << "..." << std::flush;
    std::cout << "\n";
    
    std::cout << ANSIColors::GREEN << "Payment successful!" << ANSIColors::RESET << "\n";
    Logger::logInfo("Payment", "UPI payment processed: Rs." + std::to_string(amount));
    return true;
}

std::string UPIPayment::getPaymentMethod() const {
    return "UPI";
}
