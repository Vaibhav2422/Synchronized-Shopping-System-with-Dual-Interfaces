#include "ProductFactory.h"
#include <iostream>
#include <cassert>

void testConvenienceMethods() {
    std::cout << "Testing convenience methods..." << std::endl;
    
    // Test createElectronics
    auto electronics = ProductFactory::createElectronics(1, "Laptop", 50000.0, 10, "Dell", 2);
    assert(electronics != nullptr);
    assert(electronics->getProductId() == 1);
    assert(electronics->getName() == "Laptop");
    assert(electronics->getPrice() == 50000.0);
    assert(electronics->getCategory() == "Electronics");
    std::cout << "  ✓ createElectronics works" << std::endl;
    
    // Test createClothing
    auto clothing = ProductFactory::createClothing(2, "T-Shirt", 500.0, 20, "L", "Cotton");
    assert(clothing != nullptr);
    assert(clothing->getProductId() == 2);
    assert(clothing->getName() == "T-Shirt");
    assert(clothing->getPrice() == 500.0);
    assert(clothing->getCategory() == "Clothing");
    std::cout << "  ✓ createClothing works" << std::endl;
    
    // Test createBook
    auto book = ProductFactory::createBook(3, "C++ Primer", 800.0, 15, "Stanley Lippman", "Programming");
    assert(book != nullptr);
    assert(book->getProductId() == 3);
    assert(book->getName() == "C++ Primer");
    assert(book->getPrice() == 800.0);
    assert(book->getCategory() == "Books");
    std::cout << "  ✓ createBook works" << std::endl;
}

void testCreateProductWithMap() {
    std::cout << "\nTesting createProduct with attributes map..." << std::endl;
    
    // Test Electronics creation
    std::map<std::string, std::string> electronicsAttrs = {
        {"brand", "Apple"},
        {"warranty", "3"}
    };
    auto electronics = ProductFactory::createProduct(
        ProductFactory::ProductType::ELECTRONICS,
        101, "iPhone", 70000.0, 5, electronicsAttrs
    );
    assert(electronics != nullptr);
    assert(electronics->getCategory() == "Electronics");
    std::cout << "  ✓ createProduct for Electronics works" << std::endl;
    
    // Test Clothing creation
    std::map<std::string, std::string> clothingAttrs = {
        {"size", "M"},
        {"fabric", "Polyester"}
    };
    auto clothing = ProductFactory::createProduct(
        ProductFactory::ProductType::CLOTHING,
        102, "Jeans", 1500.0, 8, clothingAttrs
    );
    assert(clothing != nullptr);
    assert(clothing->getCategory() == "Clothing");
    std::cout << "  ✓ createProduct for Clothing works" << std::endl;
    
    // Test Book creation
    std::map<std::string, std::string> bookAttrs = {
        {"author", "Robert Martin"},
        {"genre", "Software Engineering"}
    };
    auto book = ProductFactory::createProduct(
        ProductFactory::ProductType::BOOK,
        103, "Clean Code", 900.0, 12, bookAttrs
    );
    assert(book != nullptr);
    assert(book->getCategory() == "Books");
    std::cout << "  ✓ createProduct for Book works" << std::endl;
}

void testErrorHandling() {
    std::cout << "\nTesting error handling..." << std::endl;
    
    // Test missing attributes for Electronics
    try {
        std::map<std::string, std::string> incompleteAttrs = {{"brand", "Dell"}};
        auto product = ProductFactory::createProduct(
            ProductFactory::ProductType::ELECTRONICS,
            1, "Test", 100.0, 1, incompleteAttrs
        );
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        std::cout << "  ✓ Missing warranty attribute caught: " << e.what() << std::endl;
    }
    
    // Test missing attributes for Clothing
    try {
        std::map<std::string, std::string> incompleteAttrs = {{"size", "M"}};
        auto product = ProductFactory::createProduct(
            ProductFactory::ProductType::CLOTHING,
            2, "Test", 100.0, 1, incompleteAttrs
        );
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        std::cout << "  ✓ Missing fabric attribute caught: " << e.what() << std::endl;
    }
    
    // Test missing attributes for Book
    try {
        std::map<std::string, std::string> incompleteAttrs = {{"author", "Test Author"}};
        auto product = ProductFactory::createProduct(
            ProductFactory::ProductType::BOOK,
            3, "Test", 100.0, 1, incompleteAttrs
        );
        assert(false && "Should have thrown exception");
    } catch (const std::invalid_argument& e) {
        std::cout << "  ✓ Missing genre attribute caught: " << e.what() << std::endl;
    }
}

void testPolymorphism() {
    std::cout << "\nTesting polymorphism..." << std::endl;
    
    // Create products using factory
    auto electronics = ProductFactory::createElectronics(1, "Laptop", 50000.0, 10, "Dell", 2);
    auto clothing = ProductFactory::createClothing(2, "T-Shirt", 500.0, 20, "L", "Cotton");
    auto book = ProductFactory::createBook(3, "C++ Primer", 800.0, 15, "Stanley Lippman", "Programming");
    
    // Test polymorphic behavior through Product pointer
    Product* products[] = {electronics.get(), clothing.get(), book.get()};
    
    for (int i = 0; i < 3; i++) {
        std::cout << "  Product " << (i+1) << ": " << products[i]->getCategory() << std::endl;
        double discount = products[i]->calculateDiscount();
        assert(discount > 0);
    }
    
    std::cout << "  ✓ Polymorphic behavior works correctly" << std::endl;
}

int main() {
    std::cout << "=== ProductFactory Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testConvenienceMethods();
        testCreateProductWithMap();
        testErrorHandling();
        testPolymorphism();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
