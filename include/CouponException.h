#ifndef COUPON_EXCEPTION_H
#define COUPON_EXCEPTION_H

#include "ShoppingException.h"

/**
 * Base exception for coupon validation errors.
 */
class InvalidCouponException : public ShoppingException {
protected:
    std::string couponCode;
    
public:
    explicit InvalidCouponException(const std::string& code)
        : ShoppingException("Invalid coupon code: " + code, "COUPON_ERR")
        , couponCode(code) {}
    
    std::string getCouponCode() const { return couponCode; }
};

/**
 * Exception thrown when a coupon has expired.
 */
class ExpiredCouponException : public InvalidCouponException {
private:
    std::string expiryDate;
    
public:
    ExpiredCouponException(const std::string& code, const std::string& expiry)
        : InvalidCouponException(code)
        , expiryDate(expiry) {}
    
    std::string getExpiryDate() const { return expiryDate; }
    
    std::string getDetailedMessage() const override {
        return "[COUPON_ERR] Coupon " + couponCode + " expired on " + expiryDate;
    }
};

/**
 * Exception thrown when a coupon is not applicable to the current purchase.
 */
class CouponNotApplicableException : public InvalidCouponException {
private:
    std::string reason;
    
public:
    CouponNotApplicableException(const std::string& code, const std::string& why)
        : InvalidCouponException(code)
        , reason(why) {}
    
    std::string getReason() const { return reason; }
    
    std::string getDetailedMessage() const override {
        return "[COUPON_ERR] Coupon " + couponCode + " not applicable: " + reason;
    }
};

#endif // COUPON_EXCEPTION_H
