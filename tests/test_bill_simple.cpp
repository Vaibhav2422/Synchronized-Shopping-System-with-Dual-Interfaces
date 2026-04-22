#include "../include/Bill.h"
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/Coupon.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "Bill Class Simple Test\n";
    std::cout << "======================\n\n";
    
    // Test 1: Basic bill without coupon
    std::cout << "Test 1: Basic bill calculation\n";
    std::cout << "-------------------------------\n";
    {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Add a laptop (Electronics: 10% discount)
        auto laptop = std::make_unique<Electronics>(1, "Laptop", 50000.0, 10, "Dell", 2);
        inventory.addProduct(std::move(laptop));
        cart.addProduct(1, 1);
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        std::cout << "Expected subtotal: 50000.00\n";
        std::cout << "Actual subtotal: " << std::fixed << std::setprecision(2) << bill.getSubtotal() << "\n";
        
        // Expected: subtotal=50000, product_discount=5000, cart_discount=2500
        // Amount after discounts = 50000 - 5000 - 2500 = 42500
        // GST = 42500 * 0.18 = 7650
        // Final = 42500 + 7650 = 50150
        std::cout << "Expected final total: 50150.00\n";
        std::cout << "Actual final total: " << bill.getFinalTotal() << "\n\n";
        
        bill.display();
    }
    
    // Test 2: Bill with coupon
    std::cout << "\nTest 2: Bill with SAVE10 coupon\n";
    std::cout << "--------------------------------\n";
    {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Add a laptop
        auto laptop = std::make_unique<Electronics>(2, "Laptop", 50000.0, 10, "Dell", 2);
        inventory.addProduct(std::move(laptop));
        cart.addProduct(2, 1);
        
        Coupon coupon;
        coupon.apply("SAVE10");
        
        Bill bill;
        bill.calculate(cart, &coupon);
        
        // Expected: subtotal=50000, product_discount=5000, cart_discount=2500
        // Amount after product and cart discounts = 42500
        // Coupon discount = 42500 * 0.10 = 4250
        // Amount after all discounts = 42500 - 4250 = 38250
        // GST = 38250 * 0.18 = 6885
        // Final = 38250 + 6885 = 45135
        std::cout << "Expected final total: 45135.00\n";
        std::cout << "Actual final total: " << std::fixed << std::setprecision(2) << bill.getFinalTotal() << "\n\n";
        
        bill.display();
    }
    
    // Test 3: Bill below cart discount threshold
    std::cout << "\nTest 3: Bill below cart discount threshold\n";
    std::cout << "-------------------------------------------\n";
    {
        Inventory inventory;
        Cart cart(&inventory);
        
        // Add a book (Books: 5% discount)
        auto book = std::make_unique<Book>(3, "C++ Programming", 500.0, 10, "Bjarne", "Technical");
        inventory.addProduct(std::move(book));
        cart.addProduct(3, 1);
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        // Expected: subtotal=500, product_discount=25, cart_discount=0 (below threshold)
        // Amount after discounts = 500 - 25 = 475
        // GST = 475 * 0.18 = 85.50
        // Final = 475 + 85.50 = 560.50
        std::cout << "Expected final total: 560.50\n";
        std::cout << "Actual final total: " << std::fixed << std::setprecision(2) << bill.getFinalTotal() << "\n\n";
        
        bill.display();
    }
    
    // Test 4: getSummary() method
    std::cout << "\nTest 4: Bill summary for receipt\n";
    std::cout << "---------------------------------\n";
    {
        Inventory inventory;
        Cart cart(&inventory);
        
        auto shirt = std::make_unique<Clothing>(4, "T-Shirt", 1000.0, 10, "L", "Cotton");
        inventory.addProduct(std::move(shirt));
        cart.addProduct(4, 2);
        
        Bill bill;
        bill.calculate(cart, nullptr);
        
        std::cout << bill.getSummary() << "\n";
    }
    
    std::cout << "\n======================\n";
    std::cout << "All simple tests completed!\n";
    
    return 0;
}
