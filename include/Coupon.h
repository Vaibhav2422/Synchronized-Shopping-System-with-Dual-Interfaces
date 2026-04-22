#ifndef COUPON_H
#define COUPON_H

#include <string>
#include <unordered_map>

/**
 * Coupon class for managing discount coupons.
 * Supports validation and discount calculation for promotional codes.
 */
class Coupon {
private:
    std::string code;
    double discountPercentage;
    bool applied;
    
public:
    /**
     * Default constructor - initializes with no coupon applied.
     */
    Coupon();
    
    /**
     * Applies a coupon code if valid.
     * Performs an O(1) lookup in the unordered_map of valid coupons.
     * Time Complexity: O(1) average - unordered_map hash lookup
     * Space Complexity: O(1) - no additional storage required
     * @param couponCode The coupon code to apply
     * @return true if coupon is valid and applied, false otherwise
     * @throws InvalidCouponException if coupon code is invalid
     */
    bool apply(const std::string& couponCode);
    
    /**
     * Calculates the discount amount for a given price.
     * Time Complexity: O(1) - simple arithmetic on stored percentage
     * Space Complexity: O(1) - no additional storage required
     * @param amount The amount to calculate discount on
     * @return The discount amount
     */
    double calculateDiscount(double amount) const;
    
    /**
     * Checks if a coupon is currently applied.
     * @return true if a coupon is applied, false otherwise
     */
    bool isApplied() const;
    
    /**
     * Gets the currently applied coupon code.
     * @return The coupon code, or empty string if none applied
     */
    std::string getCode() const;
    
    /**
     * Gets the discount percentage of the applied coupon.
     * @return The discount percentage (0.0 to 1.0)
     */
    double getDiscountPercentage() const;
    
    /**
     * Resets the coupon, removing any applied coupon.
     */
    void reset();
};

#endif // COUPON_H
