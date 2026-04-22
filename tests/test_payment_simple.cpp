#include "../include/Payment.h"
#include "../include/UPIPayment.h"
#include "../include/CardPayment.h"
#include "../include/CashPayment.h"
#include "../include/PaymentException.h"
#include <iostream>
#include <memory>
#include <cassert>

void testUPIPayment() {
    std::cout << "Testing UPI Payment..." << std::endl;
    
    UPIPayment payment(1000.0, "user@upi");
    assert(payment.getAmount() == 1000.0);
    assert(payment.getPaymentMethod() == "UPI");
    
    bool result = payment.pay();
    assert(result == true);
    
    std::cout << "UPI Payment test passed!" << std::endl;
}

void testCardPayment() {
    std::cout << "\nTesting Card Payment..." << std::endl;
    
    CardPayment payment(2500.0, "1234567890123456", "John Doe");
    assert(payment.getAmount() == 2500.0);
    assert(payment.getPaymentMethod() == "Card");
    
    bool result = payment.pay();
    assert(result == true);
    
    std::cout << "Card Payment test passed!" << std::endl;
}

void testCashPaymentSufficient() {
    std::cout << "\nTesting Cash Payment with sufficient funds..." << std::endl;
    
    CashPayment payment(500.0, 1000.0);
    assert(payment.getAmount() == 500.0);
    assert(payment.getPaymentMethod() == "Cash");
    assert(payment.calculateChange() == 500.0);
    
    bool result = payment.pay();
    assert(result == true);
    
    std::cout << "Cash Payment (sufficient) test passed!" << std::endl;
}

void testCashPaymentInsufficient() {
    std::cout << "\nTesting Cash Payment with insufficient funds..." << std::endl;
    
    CashPayment payment(1000.0, 500.0);
    
    bool exceptionThrown = false;
    try {
        payment.pay();
    } catch (const InsufficientFundsException& e) {
        exceptionThrown = true;
        assert(e.getAmount() == 1000.0);
        assert(e.getTendered() == 500.0);
        std::cout << "Exception caught: " << e.getDetailedMessage() << std::endl;
    }
    
    assert(exceptionThrown);
    std::cout << "Cash Payment (insufficient) test passed!" << std::endl;
}

void testPolymorphicPayment() {
    std::cout << "\nTesting polymorphic payment processing..." << std::endl;
    
    // Test through base class pointer
    std::unique_ptr<Payment> payment1 = std::make_unique<UPIPayment>(1500.0, "test@upi");
    assert(payment1->getAmount() == 1500.0);
    assert(payment1->getPaymentMethod() == "UPI");
    bool result1 = payment1->pay();
    assert(result1 == true);
    
    std::unique_ptr<Payment> payment2 = std::make_unique<CardPayment>(2000.0, "9876543210987654", "Jane Smith");
    assert(payment2->getAmount() == 2000.0);
    assert(payment2->getPaymentMethod() == "Card");
    bool result2 = payment2->pay();
    assert(result2 == true);
    
    std::unique_ptr<Payment> payment3 = std::make_unique<CashPayment>(750.0, 1000.0);
    assert(payment3->getAmount() == 750.0);
    assert(payment3->getPaymentMethod() == "Cash");
    bool result3 = payment3->pay();
    assert(result3 == true);
    
    std::cout << "Polymorphic payment test passed!" << std::endl;
}

int main() {
    std::cout << "=== Payment Class Hierarchy Tests ===" << std::endl;
    
    testUPIPayment();
    testCardPayment();
    testCashPaymentSufficient();
    testCashPaymentInsufficient();
    testPolymorphicPayment();
    
    std::cout << "\n=== All Payment Tests Passed! ===" << std::endl;
    return 0;
}
