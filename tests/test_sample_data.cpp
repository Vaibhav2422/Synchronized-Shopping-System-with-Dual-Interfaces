#include "Inventory.h"
#include "ANSIColors.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
    std::cout << "Testing Sample Data Initialization\n";
    std::cout << "===================================\n" << ANSIColors::RESET;
    
    // Create inventory and initialize sample data
    Inventory inventory;
    inventory.initializeSampleData();
    
    // Verify that products were added
    const auto& products = inventory.getProducts();
    
    std::cout << ANSIColors::BG_BLACK << ANSIColors::YELLOW;
    std::cout << "\nTotal products initialized: " << products.size() << "\n";
    std::cout << ANSIColors::RESET;
    
    // Should have 9 products (3 Electronics + 3 Clothing + 3 Books)
    assert(products.size() == 9);
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Correct number of products (9)\n";
    std::cout << ANSIColors::RESET;
    
    // Verify Electronics products (IDs 101-103)
    assert(inventory.findProduct(101) != nullptr);
    assert(inventory.findProduct(102) != nullptr);
    assert(inventory.findProduct(103) != nullptr);
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Electronics products initialized (IDs 101-103)\n";
    std::cout << ANSIColors::RESET;
    
    // Verify Clothing products (IDs 201-203)
    assert(inventory.findProduct(201) != nullptr);
    assert(inventory.findProduct(202) != nullptr);
    assert(inventory.findProduct(203) != nullptr);
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Clothing products initialized (IDs 201-203)\n";
    std::cout << ANSIColors::RESET;
    
    // Verify Book products (IDs 301-303)
    assert(inventory.findProduct(301) != nullptr);
    assert(inventory.findProduct(302) != nullptr);
    assert(inventory.findProduct(303) != nullptr);
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Book products initialized (IDs 301-303)\n";
    std::cout << ANSIColors::RESET;
    
    // Verify product names
    assert(inventory.findProduct(101)->getName() == "Laptop");
    assert(inventory.findProduct(102)->getName() == "Smartphone");
    assert(inventory.findProduct(103)->getName() == "Headphones");
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Electronics product names correct\n";
    std::cout << ANSIColors::RESET;
    
    assert(inventory.findProduct(201)->getName() == "T-Shirt");
    assert(inventory.findProduct(202)->getName() == "Jeans");
    assert(inventory.findProduct(203)->getName() == "Jacket");
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Clothing product names correct\n";
    std::cout << ANSIColors::RESET;
    
    assert(inventory.findProduct(301)->getName() == "C++ Programming");
    assert(inventory.findProduct(302)->getName() == "Clean Code");
    assert(inventory.findProduct(303)->getName() == "Pragmatic Programmer");
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Book product names correct\n";
    std::cout << ANSIColors::RESET;
    
    // Verify categories
    assert(inventory.findProduct(101)->getCategory() == "Electronics");
    assert(inventory.findProduct(201)->getCategory() == "Clothing");
    assert(inventory.findProduct(301)->getCategory() == "Books");
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ Product categories correct\n";
    std::cout << ANSIColors::RESET;
    
    // Verify prices are realistic (all > 0)
    for (const auto& product : products) {
        assert(product->getPrice() > 0);
    }
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ All products have realistic prices\n";
    std::cout << ANSIColors::RESET;
    
    // Verify quantities are realistic (all > 0)
    for (const auto& product : products) {
        assert(product->getQuantityAvailable() > 0);
    }
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN;
    std::cout << "✓ All products have realistic quantities\n";
    std::cout << ANSIColors::RESET;
    
    // Display all initialized products
    std::cout << ANSIColors::BG_BLACK << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "\n\nInitialized Product Catalog:\n";
    std::cout << ANSIColors::RESET;
    inventory.displayAllProducts();
    
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\n✓ All tests passed! Sample data initialization successful!\n";
    std::cout << ANSIColors::RESET;
    
    return 0;
}
