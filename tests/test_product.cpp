#include <rapidcheck.h>
#include "../include/Product.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <sstream>
#include <iostream>

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

int main() {
    std::cout << "Running Property 2: Product Display Completeness" << std::endl;
    
    // Test Electronics products
    rc::check("Electronics display contains all required fields", []() {
        auto id = *rc::gen::inRange(1, 1000);
        auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (name.empty()) name = "Product";
        // Use integer range and cast to double (GCC 6.x doesn't support inRange<double>)
        auto price = static_cast<double>(*rc::gen::inRange(1, 100000));
        auto qty = *rc::gen::inRange(0, 100);
        auto brand = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (brand.empty()) brand = "Brand";
        auto warranty = *rc::gen::inRange(1, 5);
        
        Electronics product(id, name, price, qty, brand, warranty);
        std::string output = captureDisplay(product);
        
        // Check that output contains product ID
        RC_ASSERT(contains(output, std::to_string(id)));
        
        // Check that output contains name
        RC_ASSERT(contains(output, name));
        
        // Check that output contains price (as string)
        RC_ASSERT(contains(output, "Rs.") || contains(output, std::to_string(static_cast<int>(price))));
        
        // Check that output contains quantity
        RC_ASSERT(contains(output, std::to_string(qty)));
    });
    
    // Test Clothing products
    rc::check("Clothing display contains all required fields", []() {
        auto id = *rc::gen::inRange(1, 1000);
        auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (name.empty()) name = "Product";
        auto price = static_cast<double>(*rc::gen::inRange(1, 100000));
        auto qty = *rc::gen::inRange(0, 100);
        auto size = *rc::gen::element("S", "M", "L", "XL");
        auto fabric = *rc::gen::element("Cotton", "Polyester", "Denim");
        
        Clothing product(id, name, price, qty, size, fabric);
        std::string output = captureDisplay(product);
        
        // Check that output contains product ID
        RC_ASSERT(contains(output, std::to_string(id)));
        
        // Check that output contains name
        RC_ASSERT(contains(output, name));
        
        // Check that output contains price
        RC_ASSERT(contains(output, "Rs.") || contains(output, std::to_string(static_cast<int>(price))));
        
        // Check that output contains quantity
        RC_ASSERT(contains(output, std::to_string(qty)));
    });
    
    // Test Book products
    rc::check("Book display contains all required fields", []() {
        auto id = *rc::gen::inRange(1, 1000);
        auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (name.empty()) name = "Product";
        auto price = static_cast<double>(*rc::gen::inRange(1, 100000));
        auto qty = *rc::gen::inRange(0, 100);
        auto author = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
        if (author.empty()) author = "Author";
        auto genre = *rc::gen::element("Fiction", "Non-Fiction", "Technical");
        
        Book product(id, name, price, qty, author, genre);
        std::string output = captureDisplay(product);
        
        // Check that output contains product ID
        RC_ASSERT(contains(output, std::to_string(id)));
        
        // Check that output contains name
        RC_ASSERT(contains(output, name));
        
        // Check that output contains price
        RC_ASSERT(contains(output, "Rs.") || contains(output, std::to_string(static_cast<int>(price))));
        
        // Check that output contains quantity
        RC_ASSERT(contains(output, std::to_string(qty)));
    });
    
    std::cout << "All property tests passed!" << std::endl;
    return 0;
}
