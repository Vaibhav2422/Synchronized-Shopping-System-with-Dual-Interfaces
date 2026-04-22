#include "Bill.h"
#include "ConfigManager.h"
#include "ANSIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Bill::Bill() 
    : subtotal(0.0)
    , productDiscounts(0.0)
    , cartDiscount(0.0)
    , couponDiscount(0.0)
    , gstAmount(0.0)
    , finalTotal(0.0)
    , couponCode("") {
}

void Bill::calculate(const Cart& cart, const Coupon* coupon) {
    // Reset all values
    subtotal = 0.0;
    productDiscounts = 0.0;
    cartDiscount = 0.0;
    couponDiscount = 0.0;
    gstAmount = 0.0;
    finalTotal = 0.0;
    couponCode = "";
    
    // Step 1: Calculate subtotal (sum of product prices × quantities)
    subtotal = cart.calculateSubtotal();
    
    // Step 2: Calculate product discounts (category-specific)
    productDiscounts = cart.calculateProductDiscounts();
    
    // Step 3: Calculate cart discount (5% if subtotal > threshold)
    // Cart discount is applied to subtotal, not after product discounts
    if (subtotal > ConfigManager::getInstance().getCartDiscountThreshold()) {
        cartDiscount = subtotal * ConfigManager::getInstance().getCartDiscountRate();
    }
    
    // Step 4: Calculate coupon discount (applied to amount after other discounts)
    double amountAfterDiscounts = subtotal - productDiscounts - cartDiscount;
    if (coupon != nullptr && coupon->isApplied()) {
        couponDiscount = coupon->calculateDiscount(amountAfterDiscounts);
        couponCode = coupon->getCode();
    }
    
    // Step 5: Calculate GST on amount after all discounts
    double taxableAmount = amountAfterDiscounts - couponDiscount;
    gstAmount = taxableAmount * ConfigManager::getInstance().getGSTRate();
    
    // Step 6: Calculate final total
    finalTotal = taxableAmount + gstAmount;
}

void Bill::display() const {
    using namespace ANSIColors;
    
    std::cout << "\n";
    std::cout << CYAN << BOLD << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    BILL BREAKDOWN                      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝" << RESET << "\n\n";
    
    // Set precision for monetary values
    std::cout << std::fixed << std::setprecision(2);
    
    // Subtotal
    std::cout << WHITE << "Subtotal:                           " 
              << YELLOW << "₹" << std::setw(10) << subtotal << RESET << "\n";
    
    // Product discounts
    if (productDiscounts > 0.0) {
        std::cout << WHITE << "Product Discounts:                  " 
                  << GREEN << "-₹" << std::setw(9) << productDiscounts << RESET << "\n";
    }
    
    // Cart discount
    if (cartDiscount > 0.0) {
        std::cout << WHITE << "Cart Discount (5%):                 " 
                  << GREEN << "-₹" << std::setw(9) << cartDiscount << RESET << "\n";
    }
    
    // Coupon discount
    if (couponDiscount > 0.0) {
        std::cout << WHITE << "Coupon Discount (" << couponCode << "):           " 
                  << GREEN << "-₹" << std::setw(9) << couponDiscount << RESET << "\n";
    }
    
    // Separator
    std::cout << CYAN << "────────────────────────────────────────────────────────" << RESET << "\n";
    
    // Amount after discounts
    double amountAfterDiscounts = subtotal - productDiscounts - cartDiscount - couponDiscount;
    std::cout << WHITE << "Amount after discounts:             " 
              << YELLOW << "₹" << std::setw(10) << amountAfterDiscounts << RESET << "\n";
    
    // GST
    int gstPct = static_cast<int>(ConfigManager::getInstance().getGSTRate() * 100 + 0.5);
    std::cout << WHITE << "GST (" << gstPct << "%):                          " 
              << YELLOW << "₹" << std::setw(10) << gstAmount << RESET << "\n";
    
    // Separator
    std::cout << CYAN << "════════════════════════════════════════════════════════" << RESET << "\n";
    
    // Final total
    std::cout << WHITE << BOLD << "FINAL TOTAL:                        " 
              << YELLOW << "₹" << std::setw(10) << finalTotal << RESET << "\n\n";
}

double Bill::getSubtotal() const {
    return subtotal;
}

double Bill::getFinalTotal() const {
    return finalTotal;
}

double Bill::getGSTAmount() const {
    return gstAmount;
}

std::string Bill::getSummary() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    oss << "BILL BREAKDOWN\n";
    oss << "==============\n\n";
    
    oss << "Subtotal:                    ₹" << subtotal << "\n";
    
    if (productDiscounts > 0.0) {
        oss << "Product Discounts:          -₹" << productDiscounts << "\n";
    }
    
    if (cartDiscount > 0.0) {
        oss << "Cart Discount (5%):         -₹" << cartDiscount << "\n";
    }
    
    if (couponDiscount > 0.0) {
        oss << "Coupon Discount (" << couponCode << "):    -₹" << couponDiscount << "\n";
    }
    
    oss << "----------------------------------------\n";
    
    double amountAfterDiscounts = subtotal - productDiscounts - cartDiscount - couponDiscount;
    oss << "Amount after discounts:      ₹" << amountAfterDiscounts << "\n";
    int gstPct = static_cast<int>(ConfigManager::getInstance().getGSTRate() * 100 + 0.5);
    oss << "GST (" << gstPct << "%):                   ₹" << gstAmount << "\n";
    
    oss << "========================================\n";
    oss << "FINAL TOTAL:                 ₹" << finalTotal << "\n";
    
    return oss.str();
}
