#include <iostream>
#include <cassert>
#include <cmath>
#include "../include/Coupon.h"
#include "../include/CouponException.h"

void testValidCoupons() {
    std::cout << "Testing valid coupons..." << std::endl;
    
    Coupon coupon;
    
    // Test SAVE10
    assert(coupon.apply("SAVE10"));
    assert(coupon.isApplied());
    assert(coupon.getCode() == "SAVE10");
    assert(std::abs(coupon.getDiscountPercentage() - 0.10) < 0.001);
    assert(std::abs(coupon.calculateDiscount(1000.0) - 100.0) < 0.01);
    
    // Reset and test FESTIVE20
    coupon.reset();
    assert(!coupon.isApplied());
    assert(coupon.apply("FESTIVE20"));
    assert(coupon.getCode() == "FESTIVE20");
    assert(std::abs(coupon.getDiscountPercentage() - 0.20) < 0.001);
    assert(std::abs(coupon.calculateDiscount(1000.0) - 200.0) < 0.01);
    
    std::cout << "Valid coupon tests passed!" << std::endl;
}

void testInvalidCoupon() {
    std::cout << "Testing invalid coupon..." << std::endl;
    
    Coupon coupon;
    
    try {
        coupon.apply("INVALID");
        assert(false); // Should not reach here
    } catch (const InvalidCouponException& e) {
        assert(e.getCouponCode() == "INVALID");
        std::cout << "Invalid coupon correctly rejected!" << std::endl;
    }
}

void testCouponReplacement() {
    std::cout << "Testing coupon replacement..." << std::endl;
    
    Coupon coupon;
    
    // Apply first coupon
    coupon.apply("SAVE10");
    assert(coupon.getCode() == "SAVE10");
    
    // Apply second coupon (should replace first)
    coupon.apply("FESTIVE20");
    assert(coupon.getCode() == "FESTIVE20");
    assert(std::abs(coupon.getDiscountPercentage() - 0.20) < 0.001);
    
    std::cout << "Coupon replacement test passed!" << std::endl;
}

int main() {
    std::cout << "=== Coupon Simple Unit Tests ===" << std::endl;
    
    try {
        testValidCoupons();
        testInvalidCoupon();
        testCouponReplacement();
        
        std::cout << "\n=== All Simple Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n=== Test Failed ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
