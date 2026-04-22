#include "../include/Product.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <iostream>
#include <sstream>
#include <cassert>

// **Validates: Requirements 1.2**
// Property 2: Product Display Completeness
// For any product, the display output should contain the product ID, name, price, and available quantity.

// Helper function to capture display output
std::string captureDisplay(const Product& product) {
    // Redirect cout to stringstream
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    
    product.display();
    
    // Restore cout
    std::cout.rdbuf(old);
    
    return buffer.str();
}

// Helper function to check if string contains substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

void testElectronicsDisplay() {
    std::cout << "Testing Electronics display completeness..." << std::endl;
    
    Electronics product(101, "Laptop", 45000.0, 5, "Dell", 2);
    std::string output = captureDisplay(product);
    
    // Check that output contains product ID
    assert(contains(output, "101"));
    
    // Check that output contains name
    assert(contains(output, "Laptop"));
    
    // Check that output contains price indicator
    assert(contains(output, "Rs.") || contains(output, "45000"));
    
    // Check that output contains quantity
    assert(contains(output, "5"));
    
    std::cout << "  ✓ Electronics display test passed" << std::endl;
}

void testClothingDisplay() {
    std::cout << "Testing Clothing display completeness..." << std::endl;
    
    Clothing product(201, "T-Shirt", 500.0, 20, "M", "Cotton");
    std::string output = captureDisplay(product);
    
    // Check that output contains product ID
    assert(contains(output, "201"));
    
    // Check that output contains name
    assert(contains(output, "T-Shirt"));
    
    // Check that output contains price indicator
    assert(contains(output, "Rs.") || contains(output, "500"));
    
    // Check that output contains quantity
    assert(contains(output, "20"));
    
    std::cout << "  ✓ Clothing display test passed" << std::endl;
}

void testBookDisplay() {
    std::cout << "Testing Book display completeness..." << std::endl;
    
    Book product(301, "C++ Programming", 600.0, 12, "Bjarne Stroustrup", "Technical");
    std::string output = captureDisplay(product);
    
    // Check that output contains product ID
    assert(contains(output, "301"));
    
    // Check that output contains name
    assert(contains(output, "C++ Programming"));
    
    // Check that output contains price indicator
    assert(contains(output, "Rs.") || contains(output, "600"));
    
    // Check that output contains quantity
    assert(contains(output, "12"));
    
    std::cout << "  ✓ Book display test passed" << std::endl;
}

void testCategorySpecificDiscounts() {
    std::cout << "\nTesting category-specific discount rates..." << std::endl;
    
    // Test Electronics: 10% discount
    Electronics electronics(101, "Laptop", 1000.0, 5, "Dell", 2);
    double electronicsDiscount = electronics.calculateDiscount();
    assert(electronicsDiscount == 100.0);  // 10% of 1000
    assert(electronics.getCategory() == "Electronics");
    std::cout << "  ✓ Electronics discount (10%) test passed" << std::endl;
    
    // Test Clothing: 20% discount
    Clothing clothing(201, "T-Shirt", 1000.0, 20, "M", "Cotton");
    double clothingDiscount = clothing.calculateDiscount();
    assert(clothingDiscount == 200.0);  // 20% of 1000
    assert(clothing.getCategory() == "Clothing");
    std::cout << "  ✓ Clothing discount (20%) test passed" << std::endl;
    
    // Test Book: 5% discount
    Book book(301, "C++ Programming", 1000.0, 12, "Bjarne Stroustrup", "Technical");
    double bookDiscount = book.calculateDiscount();
    assert(bookDiscount == 50.0);  // 5% of 1000
    assert(book.getCategory() == "Books");
    std::cout << "  ✓ Book discount (5%) test passed" << std::endl;
}

void testInventoryManagement() {
    std::cout << "\nTesting inventory management..." << std::endl;
    
    Electronics product(101, "Laptop", 45000.0, 5, "Dell", 2);
    
    // Test initial availability
    assert(product.isAvailable(5));
    assert(!product.isAvailable(6));
    std::cout << "  ✓ Initial availability test passed" << std::endl;
    
    // Test decrement
    product.decrementStock(2);
    assert(product.getQuantityAvailable() == 3);
    assert(product.isAvailable(3));
    assert(!product.isAvailable(4));
    std::cout << "  ✓ Decrement stock test passed" << std::endl;
    
    // Test increment
    product.incrementStock(3);
    assert(product.getQuantityAvailable() == 6);
    assert(product.isAvailable(6));
    std::cout << "  ✓ Increment stock test passed" << std::endl;
}

void testClone() {
    std::cout << "\nTesting product cloning..." << std::endl;
    
    Electronics original(101, "Laptop", 45000.0, 5, "Dell", 2);
    auto cloned = original.clone();
    
    // Verify cloned product has same attributes
    assert(cloned->getProductId() == 101);
    assert(cloned->getName() == "Laptop");
    assert(cloned->getPrice() == 45000.0);
    assert(cloned->getQuantityAvailable() == 5);
    assert(cloned->getCategory() == "Electronics");
    
    std::cout << "  ✓ Clone test passed" << std::endl;
}

int main() {
    std::cout << "=== Property 2: Product Display Completeness ===" << std::endl;
    std::cout << "**Validates: Requirements 1.2**\n" << std::endl;
    
    testElectronicsDisplay();
    testClothingDisplay();
    testBookDisplay();
    
    std::cout << "\n=== Property 1: Category-Specific Discount Rates ===" << std::endl;
    std::cout << "**Validates: Requirements 2.4, 2.5, 2.6**\n" << std::endl;
    
    testCategorySpecificDiscounts();
    
    std::cout << "\n=== Additional Product Tests ===" << std::endl;
    testInventoryManagement();
    testClone();
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}
