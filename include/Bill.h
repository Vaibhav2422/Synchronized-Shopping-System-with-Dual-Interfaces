#ifndef BILL_H
#define BILL_H

#include <string>
#include "Cart.h"
#include "Coupon.h"

/**
 * Bill class for calculating and displaying bill breakdown.
 * Handles discount calculations, GST, and final total computation.
 */
class Bill {
private:
    double subtotal;
    double productDiscounts;
    double cartDiscount;
    double couponDiscount;
    double gstAmount;
    double finalTotal;
    std::string couponCode;
    
public:
    /**
     * Default constructor - initializes all amounts to zero.
     */
    Bill();
    
    /**
     * Calculates the bill based on cart contents and optional coupon.
     * Applies discounts in the correct order:
     * 1. Subtotal (sum of product prices × quantities)
     * 2. Product discounts (category-specific)
     * 3. Cart discount (5% if subtotal > 5000)
     * 4. Coupon discount (applied to amount after other discounts)
     * 5. GST (18% on amount after all discounts)
     * 6. Final total
     * 
     * @param cart The shopping cart containing products
     * @param coupon Optional coupon to apply (nullptr if no coupon)
     */
    void calculate(const Cart& cart, const Coupon* coupon = nullptr);
    
    /**
     * Displays the complete bill breakdown with ANSI formatting.
     * Shows all components: subtotal, discounts, GST, and final total.
     */
    void display() const;
    
    /**
     * Gets the subtotal amount (before discounts).
     * @return The subtotal amount
     */
    double getSubtotal() const;
    
    /**
     * Gets the final total amount (after all discounts and GST).
     * @return The final total amount
     */
    double getFinalTotal() const;
    
    /**
     * Gets the GST amount.
     * @return The GST amount
     */
    double getGSTAmount() const;
    
    /**
     * Gets a summary string of the bill for receipt generation.
     * @return A formatted string containing the bill breakdown
     */
    std::string getSummary() const;
};

#endif // BILL_H
