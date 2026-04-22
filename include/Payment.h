#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>

/**
 * Abstract base class for payment processing.
 * Cannot be instantiated directly — derived classes (UPIPayment, CardPayment,
 * CashPayment) must implement the two pure virtual methods: pay() and
 * getPaymentMethod(). This implements the Strategy design pattern and
 * demonstrates the OOP principle of abstraction with runtime polymorphism.
 *
 * OOP Principles demonstrated:
 *   - Abstraction:    Pure virtual interface prevents direct instantiation
 *   - Encapsulation:  amount is protected (accessible to derived classes only)
 *   - Inheritance:    UPIPayment, CardPayment, CashPayment extend this class
 *   - Polymorphism:   pay() dispatched at runtime via Payment* (Strategy pattern)
 *
 * Design Patterns:
 *   - Strategy:  pay() is the strategy method — concrete subclasses (UPIPayment,
 *                CardPayment, CashPayment) are interchangeable algorithms selected
 *                at runtime based on the user's payment choice.
 *
 * Requirements: 8.2, 8.6
 */
class Payment {
protected:
    double amount;
    
public:
    explicit Payment(double amt);
    virtual ~Payment() = default;
    
    // Pure virtual methods
    virtual bool pay() = 0;
    virtual std::string getPaymentMethod() const = 0;
    
    // Getter
    double getAmount() const;
};

#endif // PAYMENT_H
