#include "Coupon.h"
#include "ConfigManager.h"
#include "CouponException.h"

Coupon::Coupon() 
    : code("")
    , discountPercentage(0.0)
    , applied(false) {
}

// Time Complexity: O(1) average - unordered_map hash lookup for coupon validation
// Space Complexity: O(1) - no additional storage required
bool Coupon::apply(const std::string& couponCode) {
    const auto& validCoupons = ConfigManager::getInstance().getCoupons();
    auto it = validCoupons.find(couponCode);
    if (it == validCoupons.end()) {
        throw InvalidCouponException(couponCode);
    }
    
    // Apply the coupon (replaces any previously applied coupon)
    code = couponCode;
    discountPercentage = it->second;
    applied = true;
    
    return true;
}

// Time Complexity: O(1) - simple arithmetic on stored discount percentage
// Space Complexity: O(1) - no additional storage required
double Coupon::calculateDiscount(double amount) const {
    if (!applied) {
        return 0.0;
    }
    
    return amount * discountPercentage;
}

bool Coupon::isApplied() const {
    return applied;
}

std::string Coupon::getCode() const {
    return code;
}

double Coupon::getDiscountPercentage() const {
    return discountPercentage;
}

void Coupon::reset() {
    code = "";
    discountPercentage = 0.0;
    applied = false;
}
