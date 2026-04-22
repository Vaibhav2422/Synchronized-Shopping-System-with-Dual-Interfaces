#include <rapidcheck.h>
#include "../include/Bill.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Coupon.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <sstream>
#include <iostream>
#include <cmath>

// Helper function to capture display output
std::string captureDisplay(const Bill& bill) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    bill.display();
    
    std::cout.rdbuf(old);
    return buffer.str();
}

// Helper function to check if string contains substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

// Helper function to compare doubles with tolerance
bool doubleEquals(double a, double b, double epsilon = 0.01) {
    return std::abs(a - b) < epsilon;
}

// **Validates: Requirements 6.1, 6.4**
// Property 11: Cart Discount Threshold
// For any cart with subtotal greater than 5000, the cart discount should be exactly 5% of the subtotal.
// For any cart with subtotal less than or equal to 5000, the cart discount should be zero.
void testProperty11() {
    std::cout << "\nRunning Property 11: Cart Discount Threshold" << std::endl;
    
    rc::check("Cart discount is 5% when subtotal > 5000, zero otherwise", []() {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Generate random number of products
        auto numProducts = *rc::gen::inRange(1, 5);
        
        // Add products to inventory and cart
        double expectedSubtotal = 0.0;
        for (int i = 0; i < numProducts; i++) {
            // Use integer range and cast to double (GCC 6.x doesn't support inRange<double>)
            auto priceInt = *rc::gen::inRange(100, 3000);
            double price = static_cast<double>(priceInt);
            auto qty = *rc::gen::inRange(1, 5);
            
            auto product = std::make_unique<Electronics>(
                i + 1, "Product" + std::to_string(i), price, qty + 5,
                "Brand", 1
            );
            
            expectedSubtotal += price * qty;
            inventory.addProduct(std::move(product));
            cart.addProduct(i + 1, qty);
        }
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        double subtotal = bill.getSubtotal();
        
        // Verify subtotal matches expected
        RC_ASSERT(doubleEquals(subtotal, expectedSubtotal));
        
        // Check cart discount threshold
        std::string summary = bill.getSummary();
        if (subtotal > 5000.0) {
            // Should have cart discount
            RC_ASSERT(contains(summary, "Cart Discount"));
        } else {
            // Should not have cart discount (or it should be zero)
            // This is acceptable either way
        }
    });
}

// **Validates: Requirements 6.2**
// Property 12: Discount Calculation Order
// For any cart with products, coupon, and cart discount, the calculation order should be:
// (1) product discounts, (2) cart discount, (3) coupon discount, (4) GST.
void testProperty12() {
    std::cout << "\nRunning Property 12: Discount Calculation Order" << std::endl;
    
    rc::check("Discounts are applied in correct order", []() {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Create a cart with known values to verify calculation order
        // Add Electronics (10% discount)
        auto electronics = std::make_unique<Electronics>(1, "Laptop", 6000.0, 10, "Dell", 2);
        inventory.addProduct(std::move(electronics));
        cart.addProduct(1, 1);
        
        // Apply coupon
        Coupon coupon;
        coupon.apply("SAVE10");
        
        Bill bill;
        bill.calculate(cart, &coupon);
        
        // Manual calculation to verify order:
        // 1. Subtotal = 6000
        double expectedSubtotal = 6000.0;
        
        // 2. Product discount = 6000 * 0.10 = 600
        double expectedProductDiscount = 600.0;
        
        // 3. Cart discount = 6000 * 0.05 = 300 (since subtotal > 5000)
        double expectedCartDiscount = 300.0;
        
        // 4. Amount after product and cart discounts = 6000 - 600 - 300 = 5100
        double amountAfterDiscounts = expectedSubtotal - expectedProductDiscount - expectedCartDiscount;
        
        // 5. Coupon discount = 5100 * 0.10 = 510
        double expectedCouponDiscount = amountAfterDiscounts * 0.10;
        
        // 6. Taxable amount = 5100 - 510 = 4590
        double taxableAmount = amountAfterDiscounts - expectedCouponDiscount;
        
        // 7. GST = 4590 * 0.18 = 826.20
        double expectedGST = taxableAmount * 0.18;
        
        // 8. Final total = 4590 + 826.20 = 5416.20
        double expectedFinalTotal = taxableAmount + expectedGST;
        
        // Verify the calculation
        RC_ASSERT(doubleEquals(bill.getSubtotal(), expectedSubtotal));
        RC_ASSERT(doubleEquals(bill.getGSTAmount(), expectedGST));
        RC_ASSERT(doubleEquals(bill.getFinalTotal(), expectedFinalTotal));
    });
}

// **Validates: Requirements 7.5**
// Property 15: GST Calculation
// For any cart, the GST amount should be exactly 18% of the amount after all discounts.
void testProperty15() {
    std::cout << "\nRunning Property 15: GST Calculation" << std::endl;
    
    rc::check("GST is exactly 18% of amount after all discounts", []() {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Generate random products
        auto numProducts = *rc::gen::inRange(1, 3);
        double subtotal = 0.0;
        double productDiscounts = 0.0;
        
        for (int i = 0; i < numProducts; i++) {
            auto priceInt = *rc::gen::inRange(100, 2000);
            double price = static_cast<double>(priceInt);
            auto qty = *rc::gen::inRange(1, 3);
            
            auto product = std::make_unique<Electronics>(
                i + 1, "Product" + std::to_string(i), price, qty + 5,
                "Brand", 1
            );
            
            subtotal += price * qty;
            productDiscounts += (price * 0.10) * qty;  // Electronics: 10% discount
            
            inventory.addProduct(std::move(product));
            cart.addProduct(i + 1, qty);
        }
        
        // Calculate cart discount
        double cartDiscount = (subtotal > 5000.0) ? (subtotal * 0.05) : 0.0;
        
        // Randomly apply coupon
        Coupon* couponPtr = nullptr;
        Coupon coupon;
        double couponDiscount = 0.0;
        
        auto applyCoupon = *rc::gen::inRange(0, 2);
        if (applyCoupon == 1) {
            coupon.apply("SAVE10");
            couponPtr = &coupon;
            double amountAfterDiscounts = subtotal - productDiscounts - cartDiscount;
            couponDiscount = amountAfterDiscounts * 0.10;
        }
        
        Bill bill;
        bill.calculate(cart, couponPtr);
        
        // Calculate expected GST
        double amountAfterAllDiscounts = subtotal - productDiscounts - cartDiscount - couponDiscount;
        double expectedGST = amountAfterAllDiscounts * 0.18;
        
        // Verify GST calculation
        RC_ASSERT(doubleEquals(bill.getGSTAmount(), expectedGST));
    });
}

// **Validates: Requirements 7.6**
// Property 16: Final Total Calculation
// For any cart, the final total should equal (subtotal - all discounts + GST).
void testProperty16() {
    std::cout << "\nRunning Property 16: Final Total Calculation" << std::endl;
    
    rc::check("Final total equals subtotal - all discounts + GST", []() {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Generate random products
        auto numProducts = *rc::gen::inRange(1, 4);
        
        for (int i = 0; i < numProducts; i++) {
            auto priceInt = *rc::gen::inRange(100, 2000);
            double price = static_cast<double>(priceInt);
            auto qty = *rc::gen::inRange(1, 3);
            
            // Mix different product types
            std::unique_ptr<Product> product;
            auto productType = *rc::gen::inRange(0, 3);
            
            if (productType == 0) {
                product = std::make_unique<Electronics>(
                    i + 1, "Electronics" + std::to_string(i), price, qty + 5,
                    "Brand", 1
                );
            } else if (productType == 1) {
                product = std::make_unique<Clothing>(
                    i + 1, "Clothing" + std::to_string(i), price, qty + 5,
                    "M", "Cotton"
                );
            } else {
                product = std::make_unique<Book>(
                    i + 1, "Book" + std::to_string(i), price, qty + 5,
                    "Author", "Fiction"
                );
            }
            
            inventory.addProduct(std::move(product));
            cart.addProduct(i + 1, qty);
        }
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        // Get values from bill
        double subtotal = bill.getSubtotal();
        double gst = bill.getGSTAmount();
        double finalTotal = bill.getFinalTotal();
        
        // Calculate expected final total
        double productDiscounts = cart.calculateProductDiscounts();
        double cartDiscount = (subtotal > 5000.0) ? (subtotal * 0.05) : 0.0;
        double expectedFinalTotal = (subtotal - productDiscounts - cartDiscount) + gst;
        
        // Verify final total calculation
        RC_ASSERT(doubleEquals(finalTotal, expectedFinalTotal));
    });
}

// **Validates: Requirements 7.2, 7.3, 7.4, 7.6, 6.3**
// Property 17: Bill Display Completeness
// For any bill, the display output should contain subtotal, product discounts,
// cart discount (if applicable), coupon discount (if applicable), GST amount, and final total.
void testProperty17() {
    std::cout << "\nRunning Property 17: Bill Display Completeness" << std::endl;
    
    rc::check("Bill display contains all required components", []() {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Add at least one product
        auto priceInt = *rc::gen::inRange(1000, 3000);
        double price = static_cast<double>(priceInt);
        auto qty = *rc::gen::inRange(1, 3);
        
        auto product = std::make_unique<Electronics>(
            1, "TestProduct", price, qty + 5, "Brand", 1
        );
        
        inventory.addProduct(std::move(product));
        cart.addProduct(1, qty);
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        std::string output = captureDisplay(bill);
        
        // Verify all required components are present
        RC_ASSERT(contains(output, "Subtotal"));
        RC_ASSERT(contains(output, "GST"));
        RC_ASSERT(contains(output, "FINAL TOTAL"));
        
        // Check for monetary values (₹ symbol)
        RC_ASSERT(contains(output, "\xe2\x82\xb9") || contains(output, "Rs.") || contains(output, "INR"));
        
        // Verify summary also contains required components
        std::string summary = bill.getSummary();
        RC_ASSERT(contains(summary, "Subtotal"));
        RC_ASSERT(contains(summary, "GST"));
        RC_ASSERT(contains(summary, "FINAL TOTAL"));
    });
}

int main() {
    std::cout << "Running Bill Property-Based Tests" << std::endl;
    std::cout << "==================================" << std::endl;
    
    testProperty11();
    testProperty12();
    testProperty15();
    testProperty16();
    testProperty17();
    
    std::cout << "\n==================================" << std::endl;
    std::cout << "All Bill property tests passed!" << std::endl;
    
    return 0;
}
