#include "CardPayment.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>

CardPayment::CardPayment(double amt, const std::string& cardNum, const std::string& holderName)
    : Payment(amt), cardNumber(cardNum), cardHolderName(holderName) {}

std::string CardPayment::maskCardNumber() const {
    // SECURITY: Never expose full card number in display or logs.
    // Only the last 4 digits are shown; all preceding digits are masked.
    if (cardNumber.length() <= 4) {
        return cardNumber;
    }
    
    std::string masked = "****-****-****-";
    masked += cardNumber.substr(cardNumber.length() - 4);
    return masked;
}

bool CardPayment::pay() {
    std::cout << ANSIColors::CYAN << "\n=== Card Payment Processing ===" << ANSIColors::RESET << "\n";
    std::cout << "Card Number: " << ANSIColors::YELLOW << maskCardNumber() << ANSIColors::RESET << "\n";
    std::cout << "Cardholder: " << ANSIColors::YELLOW << cardHolderName << ANSIColors::RESET << "\n";
    std::cout << "Amount: " << ANSIColors::YELLOW << "Rs. " << amount << ANSIColors::RESET << "\n";
    
    std::cout << "\nAuthorizing payment";
    std::cout << "..." << std::flush;
    std::cout << "\n";
    
    std::cout << ANSIColors::GREEN << "Payment authorized and processed successfully!" << ANSIColors::RESET << "\n";
    Logger::logInfo("Payment", "Card payment processed: Rs." + std::to_string(amount));
    return true;
}

std::string CardPayment::getPaymentMethod() const {
    return "Card";
}
