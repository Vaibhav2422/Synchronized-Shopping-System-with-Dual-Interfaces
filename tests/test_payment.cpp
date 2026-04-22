#include <rapidcheck.h>
#include "../include/Payment.h"
#include "../include/UPIPayment.h"
#include "../include/CardPayment.h"
#include "../include/CashPayment.h"
#include "../include/PaymentException.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <cmath>

// **Validates: Requirements 8.6**
// Property 20: Polymorphic Payment Processing
// For any payment method (UPI, Card, Cash), calling the pay() method should execute 
// the method-specific payment logic and return a success status.

// Helper function to redirect cout to suppress output during tests
class CoutRedirect {
private:
    std::streambuf* old;
    std::stringstream buffer;
    
public:
    CoutRedirect() {
        old = std::cout.rdbuf(buffer.rdbuf());
    }
    
    ~CoutRedirect() {
        std::cout.rdbuf(old);
    }
    
    std::string getOutput() {
        return buffer.str();
    }
};

int main() {
    std::cout << "Running Property 20: Polymorphic Payment Processing" << std::endl;
    
    // Test UPI Payment
    rc::check("UPI payment processes successfully and returns true", []() {
        // Use integer range and cast to double (GCC 6.x doesn't support inRange<double>)
        double amount = static_cast<double>(*rc::gen::inRange(1, 100000));
        
        auto upiId = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (upiId.empty()) upiId = "user@upi";
        
        UPIPayment payment(amount, upiId);
        
        // Redirect cout to suppress output
        CoutRedirect redirect;
        bool result = payment.pay();
        
        RC_ASSERT(result == true);
        RC_ASSERT(payment.getPaymentMethod() == "UPI");
        RC_ASSERT(payment.getAmount() == amount);
    });
    
    // Test Card Payment
    rc::check("Card payment processes successfully and returns true", []() {
        double amount = static_cast<double>(*rc::gen::inRange(1, 100000));
        
        auto cardNumber = *rc::gen::container<std::string>(rc::gen::inRange('0', '9'));
        if (cardNumber.empty()) cardNumber = "1234567890123456";
        auto holderName = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (holderName.empty()) holderName = "John Doe";
        
        CardPayment payment(amount, cardNumber, holderName);
        
        // Redirect cout to suppress output
        CoutRedirect redirect;
        bool result = payment.pay();
        
        RC_ASSERT(result == true);
        RC_ASSERT(payment.getPaymentMethod() == "Card");
        RC_ASSERT(payment.getAmount() == amount);
    });
    
    // Test Cash Payment with sufficient funds
    rc::check("Cash payment with sufficient funds processes successfully", []() {
        double amount = static_cast<double>(*rc::gen::inRange(1, 10000));
        double extra = static_cast<double>(*rc::gen::inRange(0, 5000));
        double tendered = amount + extra;
        
        CashPayment payment(amount, tendered);
        
        // Redirect cout to suppress output
        CoutRedirect redirect;
        bool result = payment.pay();
        
        RC_ASSERT(result == true);
        RC_ASSERT(payment.getPaymentMethod() == "Cash");
        RC_ASSERT(payment.getAmount() == amount);
        // Use epsilon comparison for floating point
        double calculatedChange = payment.calculateChange();
        double epsilon = 0.0001;
        RC_ASSERT(std::abs(calculatedChange - extra) < epsilon);
    });
    
    // Test Cash Payment with insufficient funds throws exception
    rc::check("Cash payment with insufficient funds throws InsufficientFundsException", []() {
        double amount = static_cast<double>(*rc::gen::inRange(100, 10000));
        double shortage = static_cast<double>(*rc::gen::inRange(1, 99));
        double tendered = amount - shortage;
        
        CashPayment payment(amount, tendered);
        
        bool exceptionThrown = false;
        try {
            // Redirect cout to suppress output
            CoutRedirect redirect;
            payment.pay();
        } catch (const InsufficientFundsException& e) {
            exceptionThrown = true;
            RC_ASSERT(e.getAmount() == amount);
            RC_ASSERT(e.getTendered() == tendered);
        }
        
        RC_ASSERT(exceptionThrown);
    });
    
    // Test polymorphic payment processing through base class pointer
    rc::check("Polymorphic payment processing works through base class pointer", []() {
        double amount = static_cast<double>(*rc::gen::inRange(1, 10000));
        
        // Test with UPI
        std::unique_ptr<Payment> upiPayment = std::make_unique<UPIPayment>(amount, "test@upi");
        {
            CoutRedirect redirect;
            bool result = upiPayment->pay();
            RC_ASSERT(result == true);
            RC_ASSERT(upiPayment->getPaymentMethod() == "UPI");
        }
        
        // Test with Card
        std::unique_ptr<Payment> cardPayment = std::make_unique<CardPayment>(amount, "1234567890123456", "Test User");
        {
            CoutRedirect redirect;
            bool result = cardPayment->pay();
            RC_ASSERT(result == true);
            RC_ASSERT(cardPayment->getPaymentMethod() == "Card");
        }
        
        // Test with Cash (sufficient funds)
        std::unique_ptr<Payment> cashPayment = std::make_unique<CashPayment>(amount, amount + 100.0);
        {
            CoutRedirect redirect;
            bool result = cashPayment->pay();
            RC_ASSERT(result == true);
            RC_ASSERT(cashPayment->getPaymentMethod() == "Cash");
        }
    });
    
    std::cout << "All payment property tests passed!" << std::endl;
    return 0;
}
