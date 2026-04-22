#include "Inventory.h"
#include "ProductFactory.h"
#include "ANSIColors.h"
#include <iostream>

int main() {
    std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
    std::cout << "Testing Inventory Display Methods\n";
    std::cout << "==================================\n" << ANSIColors::RESET;
    
    // Create inventory
    Inventory inventory;
    
    // Add sample products using ProductFactory
    inventory.addProduct(ProductFactory::createElectronics(
        101, "Laptop", 45000.0, 5, "Dell", 2
    ));
    inventory.addProduct(ProductFactory::createElectronics(
        102, "Smartphone", 25000.0, 10, "Samsung", 1
    ));
    inventory.addProduct(ProductFactory::createElectronics(
        103, "Headphones", 2500.0, 15, "Sony", 1
    ));
    
    inventory.addProduct(ProductFactory::createClothing(
        201, "T-Shirt", 500.0, 20, "L", "Cotton"
    ));
    inventory.addProduct(ProductFactory::createClothing(
        202, "Jeans", 1500.0, 15, "M", "Denim"
    ));
    inventory.addProduct(ProductFactory::createClothing(
        203, "Jacket", 3000.0, 8, "XL", "Leather"
    ));
    
    inventory.addProduct(ProductFactory::createBook(
        301, "C++ Programming", 800.0, 12, "Bjarne Stroustrup", "Programming"
    ));
    inventory.addProduct(ProductFactory::createBook(
        302, "Clean Code", 600.0, 10, "Robert Martin", "Software Engineering"
    ));
    inventory.addProduct(ProductFactory::createBook(
        303, "Design Patterns", 900.0, 8, "Gang of Four", "Software Engineering"
    ));
    
    // Test 1: Display all products with category grouping
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 1: Display All Products (with category grouping)\n";
    std::cout << ANSIColors::RESET;
    inventory.displayAllProducts();
    
    // Test 2: Display Electronics category
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 2: Display Electronics Category\n";
    std::cout << ANSIColors::RESET;
    inventory.displayByCategory("Electronics");
    
    // Test 3: Display Clothing category
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 3: Display Clothing Category\n";
    std::cout << ANSIColors::RESET;
    inventory.displayByCategory("Clothing");
    
    // Test 4: Display Books category
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 4: Display Books Category\n";
    std::cout << ANSIColors::RESET;
    inventory.displayByCategory("Books");
    
    // Test 5: Display non-existent category
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 5: Display Non-existent Category (Furniture)\n";
    std::cout << ANSIColors::RESET;
    inventory.displayByCategory("Furniture");
    
    // Test 6: Display empty inventory
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nTest 6: Display Empty Inventory\n";
    std::cout << ANSIColors::RESET;
    Inventory emptyInventory;
    emptyInventory.displayAllProducts();
    
    std::cout << ANSIColors::BG_BLACK << ANSIColors::GREEN << ANSIColors::BOLD;
    std::cout << "\n\nAll tests completed successfully!\n";
    std::cout << ANSIColors::RESET;
    
    return 0;
}
