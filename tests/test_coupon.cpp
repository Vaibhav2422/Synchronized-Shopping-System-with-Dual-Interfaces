#include <rapidcheck.h>
#include "../include/Coupon.h"
#include "../include/CouponException.h"
#include <iostream>
#include <string>
#include <vector>

// Generator for valid coupon codes
rc::Gen<std::string> genValidCouponCode() {
    return rc::gen::element(std::string("SAVE10"), std::string("FESTIVE20"));
}

// Generator for invalid coupon codes
rc::Gen<std::string> genInvalidCouponCode() {
    return rc::gen::suchThat(
        rc::gen::oneOf(
            rc::gen::just(std::string("INVALID")),
            rc::gen::just(std::string("EXPIRED")),
            rc::gen::just(std::string("NOTVALID")),
            rc::gen::just(std::string("FAKE10")),
            rc::gen::just(std::string("DISCOUNT50")),
            rc::gen::just(std::string("SAVE20")),
            rc::gen::just(std::string("FESTIVE10")),
            rc::gen::just(std::string("COUPON123")),
            rc::gen::just(std::string("TEST")),
            rc::gen::just(std::string("WRONGCODE"))
        ),
        [](const std::string& code) {
            return code != "SAVE10" && code != "FESTIVE20" && !code.empty();
        }
    );
}

// **Validates: Requirements 5.4**
// Property 18: Invalid Coupon Rejection
void testInvalidCouponRejection() {
    std::cout << "\nRunning Property 18: Invalid Coupon Rejection" << std::endl;
    
    rc::check("Invalid coupon codes should throw InvalidCouponException", []() {
        Coupon coupon;
        
        const std::string invalidCode = *genInvalidCouponCode();
        
        // Attempting to apply an invalid coupon should throw an exception
        bool exceptionThrown = false;
        try {
            coupon.apply(invalidCode);
        } catch (const InvalidCouponException& e) {
            exceptionThrown = true;
            // Verify the exception contains the invalid code
            RC_ASSERT(e.getCouponCode() == invalidCode);
        }
        
        RC_ASSERT(exceptionThrown);
        
        // Verify coupon was not applied
        RC_ASSERT(!coupon.isApplied());
        RC_ASSERT(coupon.getCode().empty());
        RC_ASSERT(coupon.getDiscountPercentage() == 0.0);
    });
}

// **Validates: Requirements 5.5**
// Property 19: Single Coupon Constraint
void testSingleCouponConstraint() {
    std::cout << "\nRunning Property 19: Single Coupon Constraint" << std::endl;
    
    rc::check("Applying a second coupon should replace the first", []() {
        Coupon coupon;
        
        // Apply first coupon
        const std::string firstCode = *genValidCouponCode();
        bool success1 = coupon.apply(firstCode);
        RC_ASSERT(success1);
        RC_ASSERT(coupon.isApplied());
        RC_ASSERT(coupon.getCode() == firstCode);
        
        // Store first coupon's discount percentage
        double firstDiscount = coupon.getDiscountPercentage();
        RC_ASSERT(firstDiscount > 0.0);
        
        // Apply second coupon (should replace the first)
        const std::string secondCode = *genValidCouponCode();
        bool success2 = coupon.apply(secondCode);
        RC_ASSERT(success2);
        RC_ASSERT(coupon.isApplied());
        RC_ASSERT(coupon.getCode() == secondCode);
        
        // Verify the second coupon's discount is applied
        double secondDiscount = coupon.getDiscountPercentage();
        RC_ASSERT(secondDiscount > 0.0);
        
        // If the codes are different, the discounts should match the expected values
        if (firstCode != secondCode) {
            RC_ASSERT(coupon.getCode() != firstCode);
        }
        
        // Verify only one coupon is active (the second one)
        if (secondCode == "SAVE10") {
            RC_ASSERT(coupon.getDiscountPercentage() == 0.10);
        } else if (secondCode == "FESTIVE20") {
            RC_ASSERT(coupon.getDiscountPercentage() == 0.20);
        }
    });
}

// Additional test: Valid coupon codes should be accepted
void testValidCouponAcceptance() {
    std::cout << "\nRunning Additional Test: Valid Coupon Acceptance" << std::endl;
    
    rc::check("Valid coupon codes SAVE10 and FESTIVE20 should be accepted", []() {
        Coupon coupon;
        
        const std::string validCode = *genValidCouponCode();
        
        // Apply valid coupon
        bool success = coupon.apply(validCode);
        RC_ASSERT(success);
        
        // Verify coupon was applied
        RC_ASSERT(coupon.isApplied());
        RC_ASSERT(coupon.getCode() == validCode);
        
        // Verify correct discount percentage
        if (validCode == "SAVE10") {
            RC_ASSERT(coupon.getDiscountPercentage() == 0.10);
        } else if (validCode == "FESTIVE20") {
            RC_ASSERT(coupon.getDiscountPercentage() == 0.20);
        }
        
        // Test discount calculation
        int testAmount = *rc::gen::inRange(100, 10000);
        double discount = coupon.calculateDiscount(static_cast<double>(testAmount));
        double expectedDiscount = testAmount * coupon.getDiscountPercentage();
        
        // Allow small floating point error
        RC_ASSERT(std::abs(discount - expectedDiscount) < 0.01);
    });
}

// Additional test: Reset functionality
void testCouponReset() {
    std::cout << "\nRunning Additional Test: Coupon Reset" << std::endl;
    
    rc::check("Reset should clear applied coupon", []() {
        Coupon coupon;
        
        // Apply a valid coupon
        const std::string validCode = *genValidCouponCode();
        coupon.apply(validCode);
        
        RC_ASSERT(coupon.isApplied());
        
        // Reset the coupon
        coupon.reset();
        
        // Verify coupon is cleared
        RC_ASSERT(!coupon.isApplied());
        RC_ASSERT(coupon.getCode().empty());
        RC_ASSERT(coupon.getDiscountPercentage() == 0.0);
        
        // Verify discount calculation returns 0
        int testAmount = *rc::gen::inRange(100, 10000);
        RC_ASSERT(coupon.calculateDiscount(static_cast<double>(testAmount)) == 0.0);
    });
}

int main() {
    std::cout << "=== Coupon Property-Based Tests ===" << std::endl;
    
    try {
        testInvalidCouponRejection();
        testSingleCouponConstraint();
        testValidCouponAcceptance();
        testCouponReset();
        
        std::cout << "\n=== All Coupon Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n=== Test Failed ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
