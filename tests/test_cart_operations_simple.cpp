#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include "../include/StockException.h"
#include "../include/InvalidInputException.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>

// Helper function to create a test inventory with sample products
void initializeTestInventory(Inventory& inv) {
    // Add Electronics
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 5, "Dell", 2));
    inv.addProduct(ProductFactory::createElectronics(102, "Smartphone", 25000.0, 10, "Samsung", 1));
    inv.addProduct(ProductFactory::createElectronics(103, "Headphones", 3000.0, 15, "Sony", 1));
    
    // Add Clothing
    inv.addProduct(ProductFactory::createClothing(201, "T-Shirt", 500.0, 20, "M", "Cotton"));
    inv.addProduct(ProductFactory::createClothing(202, "Jeans", 1500.0, 15, "32", "Denim"));
    inv.addProduct(ProductFactory::createClothing(203, "Jacket", 3000.0, 8, "L", "Leather"));
    
    // Add Books
    inv.addProduct(ProductFactory::createBook(301, "C++ Programming", 600.0, 12, "Bjarne Stroustrup", "Technical"));
    inv.addProduct(ProductFactory::createBook(302, "Clean Code", 800.0, 10, "Robert Martin", "Technical"));
    inv.addProduct(ProductFactory::createBook(303, "Pragmatic Programmer", 700.0, 8, "Hunt & Thomas", "Technical"));
}

// **Validates: Requirements 3.3, 4.1, 17.5**
// Property 4: Cart-Inventory Synchronization on Add
void testCartInventorySynchronizationOnAdd() {
    std::cout << "\nProperty 4: Cart-Inventory Synchronization on Add" << std::endl;
    std::cout << "**Validates: Requirements 3.3, 4.1, 17.5**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    Product* product = inv.findProduct(101);
    int initialStock = product->getQuantityAvailable();
    
    // Add 2 units to cart
    cart.addProduct(101, 2);
    
    int finalStock = product->getQuantityAvailable();
    assert(finalStock == initialStock - 2);
    
    std::cout << "  ✓ Inventory decremented by exact quantity" << std::endl;
}

// **Validates: Requirements 3.4, 4.2, 17.6**
// Property 5: Cart-Inventory Round Trip
void testCartInventoryRoundTrip() {
    std::cout << "\nProperty 5: Cart-Inventory Round Trip" << std::endl;
    std::cout << "**Validates: Requirements 3.4, 4.2, 17.6**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    Product* product = inv.findProduct(102);
    int initialStock = product->getQuantityAvailable();
    
    // Add product to cart
    cart.addProduct(102, 3);
    
    // Remove product from cart
    cart.removeProduct(102);
    
    int finalStock = product->getQuantityAvailable();
    assert(finalStock == initialStock);
    
    std::cout << "  ✓ Inventory restored after add/remove round trip" << std::endl;
}

// **Validates: Requirements 3.5, 11.1**
// Property 6: Product ID Validation
void testProductIdValidation() {
    std::cout << "\nProperty 6: Product ID Validation" << std::endl;
    std::cout << "**Validates: Requirements 3.5, 11.1**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    bool exceptionThrown = false;
    try {
        cart.addProduct(999, 1);  // Invalid product ID
    } catch (const InvalidProductIdException&) {
        exceptionThrown = true;
    }
    
    assert(exceptionThrown);
    std::cout << "  ✓ Invalid product ID throws exception" << std::endl;
}

// **Validates: Requirements 3.2, 4.3**
// Property 7: Out-of-Stock Prevention
void testOutOfStockPrevention() {
    std::cout << "\nProperty 7: Out-of-Stock Prevention" << std::endl;
    std::cout << "**Validates: Requirements 3.2, 4.3**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    Product* product = inv.findProduct(103);
    int initialStock = product->getQuantityAvailable();
    
    // Deplete stock completely
    for (int i = 0; i < initialStock; ++i) {
        product->decrementStock(1);
    }
    
    bool exceptionThrown = false;
    try {
        cart.addProduct(103, 1);
    } catch (const ProductUnavailableException&) {
        exceptionThrown = true;
    }
    
    assert(exceptionThrown);
    std::cout << "  ✓ Out-of-stock product throws exception" << std::endl;
}

// **Validates: Requirements 17.4**
// Property 8: Insufficient Stock Prevention
void testInsufficientStockPrevention() {
    std::cout << "\nProperty 8: Insufficient Stock Prevention" << std::endl;
    std::cout << "**Validates: Requirements 17.4**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    Product* product = inv.findProduct(203);
    int availableStock = product->getQuantityAvailable();
    
    bool exceptionThrown = false;
    try {
        cart.addProduct(203, availableStock + 5);  // Request more than available
    } catch (const InsufficientStockException&) {
        exceptionThrown = true;
    }
    
    assert(exceptionThrown);
    std::cout << "  ✓ Insufficient stock throws exception" << std::endl;
}

// **Validates: Requirements 3.7, 17.3**
// Property 9: Cart Display Completeness
void testCartDisplayCompleteness() {
    std::cout << "\nProperty 9: Cart Display Completeness" << std::endl;
    std::cout << "**Validates: Requirements 3.7, 17.3**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    cart.addProduct(201, 3);
    
    // Capture display output
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    cart.displayCart();
    
    std::cout.rdbuf(old);
    
    std::string output = buffer.str();
    
    // Verify output contains product name and quantity
    assert(output.find("T-Shirt") != std::string::npos);
    assert(output.find("x3") != std::string::npos);
    
    std::cout << "  ✓ Cart display contains product details and quantities" << std::endl;
}

// **Validates: Requirements 17.2**
// Property 10: Quantity Increment on Duplicate Add
void testQuantityIncrementOnDuplicateAdd() {
    std::cout << "\nProperty 10: Quantity Increment on Duplicate Add" << std::endl;
    std::cout << "**Validates: Requirements 17.2**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    // Add product first time
    cart.addProduct(301, 2);
    size_t itemsAfterFirst = cart.getItems().size();
    
    // Add same product again
    cart.addProduct(301, 3);
    size_t itemsAfterSecond = cart.getItems().size();
    
    // Verify no duplicate entry was created
    assert(itemsAfterFirst == itemsAfterSecond);
    
    // Verify quantity was incremented
    const auto& items = cart.getItems();
    bool found = false;
    for (const auto& item : items) {
        if (item.product->getProductId() == 301) {
            assert(item.quantity == 5);  // 2 + 3
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "  ✓ Adding same product increments quantity instead of duplicating" << std::endl;
}

// **Validates: Requirements 7.1**
// Property 13: Subtotal Calculation
void testSubtotalCalculation() {
    std::cout << "\nProperty 13: Subtotal Calculation" << std::endl;
    std::cout << "**Validates: Requirements 7.1**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    // Add multiple products
    cart.addProduct(101, 1);  // Laptop: 45000
    cart.addProduct(201, 2);  // T-Shirt: 500 x 2 = 1000
    cart.addProduct(301, 3);  // C++ Programming: 600 x 3 = 1800
    
    double expectedSubtotal = 45000.0 + 1000.0 + 1800.0;
    double actualSubtotal = cart.calculateSubtotal();
    
    assert(fabs(actualSubtotal - expectedSubtotal) < 0.01);
    
    std::cout << "  ✓ Subtotal equals sum of (price × quantity)" << std::endl;
}

// **Validates: Requirements 7.2**
// Property 14: Product Discount Summation
void testProductDiscountSummation() {
    std::cout << "\nProperty 14: Product Discount Summation" << std::endl;
    std::cout << "**Validates: Requirements 7.2**" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    // Add products with known discounts
    cart.addProduct(101, 1);  // Electronics: 10% of 45000 = 4500
    cart.addProduct(201, 2);  // Clothing: 20% of 500 = 100, x2 = 200
    cart.addProduct(301, 1);  // Book: 5% of 600 = 30
    
    double expectedDiscounts = 4500.0 + 200.0 + 30.0;
    double actualDiscounts = cart.calculateProductDiscounts();
    
    assert(fabs(actualDiscounts - expectedDiscounts) < 0.01);
    
    std::cout << "  ✓ Product discounts equal sum of (discount × quantity)" << std::endl;
}

// Additional test for cart discount threshold
void testCartDiscountThreshold() {
    std::cout << "\nAdditional Test: Cart Discount Threshold" << std::endl;
    std::cout << "**Validates: Requirements 6.1, 6.4**" << std::endl;
    
    Inventory inv1;
    initializeTestInventory(inv1);
    Cart cart1(&inv1);
    
    Inventory inv2;
    initializeTestInventory(inv2);
    Cart cart2(&inv2);
    
    // Cart with subtotal < 5000
    cart1.addProduct(201, 2);  // 1000
    double discount1 = cart1.calculateCartDiscount();
    assert(discount1 == 0.0);
    std::cout << "  ✓ No cart discount when subtotal ≤ 5000" << std::endl;
    
    // Cart with subtotal > 5000
    cart2.addProduct(101, 1);  // 45000
    double discount2 = cart2.calculateCartDiscount();
    double expectedDiscount = 45000.0 * 0.05;
    assert(fabs(discount2 - expectedDiscount) < 0.01);
    std::cout << "  ✓ 5% cart discount when subtotal > 5000" << std::endl;
}

// Test isEmpty and getItemCount
void testCartQueryMethods() {
    std::cout << "\nAdditional Test: Cart Query Methods" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    // Test empty cart
    assert(cart.isEmpty());
    assert(cart.getItemCount() == 0);
    std::cout << "  ✓ Empty cart detected correctly" << std::endl;
    
    // Add items
    cart.addProduct(101, 2);
    cart.addProduct(201, 3);
    
    assert(!cart.isEmpty());
    assert(cart.getItemCount() == 5);  // 2 + 3
    std::cout << "  ✓ Item count calculated correctly" << std::endl;
}

// Test clear method
void testCartClear() {
    std::cout << "\nAdditional Test: Cart Clear" << std::endl;
    
    Inventory inv;
    initializeTestInventory(inv);
    Cart cart(&inv);
    
    Product* product1 = inv.findProduct(101);
    Product* product2 = inv.findProduct(201);
    
    int initialStock1 = product1->getQuantityAvailable();
    int initialStock2 = product2->getQuantityAvailable();
    
    // Add items
    cart.addProduct(101, 2);
    cart.addProduct(201, 3);
    
    // Clear cart
    cart.clear();
    
    // Verify cart is empty
    assert(cart.isEmpty());
    
    // Verify inventory restored
    assert(product1->getQuantityAvailable() == initialStock1);
    assert(product2->getQuantityAvailable() == initialStock2);
    
    std::cout << "  ✓ Clear restores all inventory and empties cart" << std::endl;
}

int main() {
    std::cout << "=== Cart Operations Property Tests ===" << std::endl;
    
    testCartInventorySynchronizationOnAdd();
    testCartInventoryRoundTrip();
    testProductIdValidation();
    testOutOfStockPrevention();
    testInsufficientStockPrevention();
    testCartDisplayCompleteness();
    testQuantityIncrementOnDuplicateAdd();
    testSubtotalCalculation();
    testProductDiscountSummation();
    
    std::cout << "\n=== Additional Cart Tests ===" << std::endl;
    testCartDiscountThreshold();
    testCartQueryMethods();
    testCartClear();
    
    std::cout << "\n✅ All Cart tests passed!" << std::endl;
    return 0;
}
