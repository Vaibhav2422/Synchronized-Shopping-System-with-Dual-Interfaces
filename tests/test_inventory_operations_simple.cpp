#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cctype>
#include <set>

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Helper function to check if a string contains a substring (case-insensitive)
bool containsSubstring(const std::string& str, const std::string& substr) {
    return toLower(str).find(toLower(substr)) != std::string::npos;
}

// **Validates: Requirements 16.2, 16.6**
// Property 26: Product Search Partial Matching
// For any search term, the results should include all products whose names contain 
// the search term as a substring (case-insensitive).
void testProperty26() {
    std::cout << "\n=== Property 26: Product Search Partial Matching ===" << std::endl;
    std::cout << "**Validates: Requirements 16.2, 16.6**\n" << std::endl;
    
    Inventory inventory;
    
    // Add products with known names
    inventory.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));
    inventory.addProduct(ProductFactory::createElectronics(102, "Smartphone", 25000.0, 15, "Samsung", 1));
    inventory.addProduct(ProductFactory::createElectronics(103, "Headphones", 3500.0, 20, "Sony", 1));
    inventory.addProduct(ProductFactory::createClothing(201, "T-Shirt", 800.0, 25, "L", "Cotton"));
    inventory.addProduct(ProductFactory::createClothing(202, "Jeans", 1500.0, 18, "M", "Denim"));
    inventory.addProduct(ProductFactory::createBook(301, "C++ Programming", 650.0, 30, "Bjarne", "Programming"));
    
    std::cout << "Testing partial name matching..." << std::endl;
    
    // Test 1: Search for "phone" should match "Smartphone" and "Headphones"
    auto results1 = inventory.searchByName("phone");
    assert(results1.size() == 2);
    bool foundSmartphone = false, foundHeadphones = false;
    for (const Product* p : results1) {
        if (containsSubstring(p->getName(), "Smartphone")) foundSmartphone = true;
        if (containsSubstring(p->getName(), "Headphones")) foundHeadphones = true;
    }
    assert(foundSmartphone && foundHeadphones);
    std::cout << "  ✓ Search 'phone' found 2 products (Smartphone, Headphones)" << std::endl;
    
    // Test 2: Search for "Lap" should match "Laptop"
    auto results2 = inventory.searchByName("Lap");
    assert(results2.size() == 1);
    assert(containsSubstring(results2[0]->getName(), "Laptop"));
    std::cout << "  ✓ Search 'Lap' found Laptop" << std::endl;
    
    // Test 3: Case-insensitive search - "SHIRT" should match "T-Shirt"
    auto results3 = inventory.searchByName("SHIRT");
    assert(results3.size() == 1);
    assert(containsSubstring(results3[0]->getName(), "T-Shirt"));
    std::cout << "  ✓ Case-insensitive search 'SHIRT' found T-Shirt" << std::endl;
    
    // Test 4: Search for "C++" should match "C++ Programming"
    auto results4 = inventory.searchByName("C++");
    assert(results4.size() == 1);
    assert(containsSubstring(results4[0]->getName(), "C++ Programming"));
    std::cout << "  ✓ Search 'C++' found C++ Programming" << std::endl;
    
    // Test 5: Search for non-existent term should return empty
    auto results5 = inventory.searchByName("xyz123");
    assert(results5.size() == 0);
    std::cout << "  ✓ Search for non-existent term returns empty" << std::endl;
    
    // Test 6: All results should contain the search term
    auto results6 = inventory.searchByName("a");
    for (const Product* p : results6) {
        assert(containsSubstring(p->getName(), "a"));
    }
    std::cout << "  ✓ All search results contain the search term" << std::endl;
}

// **Validates: Requirements 16.4**
// Property 27: Category Filter Exactness
// For any category filter, the results should include only products that belong 
// to exactly that category.
void testProperty27() {
    std::cout << "\n=== Property 27: Category Filter Exactness ===" << std::endl;
    std::cout << "**Validates: Requirements 16.4**\n" << std::endl;
    
    Inventory inventory;
    
    // Add products from all three categories
    inventory.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));
    inventory.addProduct(ProductFactory::createElectronics(102, "Smartphone", 25000.0, 15, "Samsung", 1));
    inventory.addProduct(ProductFactory::createElectronics(103, "Headphones", 3500.0, 20, "Sony", 1));
    
    inventory.addProduct(ProductFactory::createClothing(201, "T-Shirt", 800.0, 25, "L", "Cotton"));
    inventory.addProduct(ProductFactory::createClothing(202, "Jeans", 1500.0, 18, "M", "Denim"));
    
    inventory.addProduct(ProductFactory::createBook(301, "C++ Programming", 650.0, 30, "Bjarne", "Programming"));
    inventory.addProduct(ProductFactory::createBook(302, "Clean Code", 550.0, 25, "Robert", "Software"));
    
    std::cout << "Testing category filtering..." << std::endl;
    
    // Test 1: Filter by Electronics
    auto electronicsResults = inventory.filterByCategory("Electronics");
    assert(electronicsResults.size() == 3);
    for (const Product* p : electronicsResults) {
        assert(p->getCategory() == "Electronics");
    }
    std::cout << "  ✓ Electronics filter returns exactly 3 Electronics products" << std::endl;
    
    // Test 2: Filter by Clothing
    auto clothingResults = inventory.filterByCategory("Clothing");
    assert(clothingResults.size() == 2);
    for (const Product* p : clothingResults) {
        assert(p->getCategory() == "Clothing");
    }
    std::cout << "  ✓ Clothing filter returns exactly 2 Clothing products" << std::endl;
    
    // Test 3: Filter by Books
    auto booksResults = inventory.filterByCategory("Books");
    assert(booksResults.size() == 2);
    for (const Product* p : booksResults) {
        assert(p->getCategory() == "Books");
    }
    std::cout << "  ✓ Books filter returns exactly 2 Books products" << std::endl;
    
    // Test 4: Filter by non-existent category
    auto emptyResults = inventory.filterByCategory("Furniture");
    assert(emptyResults.size() == 0);
    std::cout << "  ✓ Non-existent category returns empty results" << std::endl;
    
    // Test 5: Verify no cross-contamination
    for (const Product* p : electronicsResults) {
        assert(p->getCategory() != "Clothing");
        assert(p->getCategory() != "Books");
    }
    std::cout << "  ✓ Category filter has no cross-contamination" << std::endl;
}

// **Validates: Requirements 14.4**
// Property 28: Unique Product IDs
// For any product catalog, no two products should have the same product ID.
void testProperty28() {
    std::cout << "\n=== Property 28: Unique Product IDs ===" << std::endl;
    std::cout << "**Validates: Requirements 14.4**\n" << std::endl;
    
    Inventory inventory;
    
    // Add products with unique IDs
    inventory.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));
    inventory.addProduct(ProductFactory::createElectronics(102, "Smartphone", 25000.0, 15, "Samsung", 1));
    inventory.addProduct(ProductFactory::createElectronics(103, "Headphones", 3500.0, 20, "Sony", 1));
    inventory.addProduct(ProductFactory::createClothing(201, "T-Shirt", 800.0, 25, "L", "Cotton"));
    inventory.addProduct(ProductFactory::createClothing(202, "Jeans", 1500.0, 18, "M", "Denim"));
    inventory.addProduct(ProductFactory::createClothing(203, "Jacket", 2500.0, 12, "XL", "Leather"));
    inventory.addProduct(ProductFactory::createBook(301, "C++ Programming", 650.0, 30, "Bjarne", "Programming"));
    inventory.addProduct(ProductFactory::createBook(302, "Clean Code", 550.0, 25, "Robert", "Software"));
    inventory.addProduct(ProductFactory::createBook(303, "Pragmatic Programmer", 600.0, 20, "Andrew", "Software"));
    
    std::cout << "Testing product ID uniqueness..." << std::endl;
    
    // Test 1: Collect all product IDs
    const auto& products = inventory.getProducts();
    std::set<int> productIds;
    
    for (const auto& product : products) {
        int id = product->getProductId();
        // Check that this ID hasn't been seen before
        assert(productIds.count(id) == 0);
        productIds.insert(id);
    }
    std::cout << "  ✓ All product IDs are unique" << std::endl;
    
    // Test 2: Verify we have the expected number of unique IDs
    assert(productIds.size() == 9);
    std::cout << "  ✓ Found 9 unique product IDs" << std::endl;
    
    // Test 3: Verify specific IDs exist
    assert(productIds.count(101) == 1);
    assert(productIds.count(102) == 1);
    assert(productIds.count(103) == 1);
    assert(productIds.count(201) == 1);
    assert(productIds.count(202) == 1);
    assert(productIds.count(203) == 1);
    assert(productIds.count(301) == 1);
    assert(productIds.count(302) == 1);
    assert(productIds.count(303) == 1);
    std::cout << "  ✓ All expected IDs are present" << std::endl;
    
    // Test 4: Test with sample data initialization
    Inventory inventory2;
    inventory2.initializeSampleData();
    
    const auto& sampleProducts = inventory2.getProducts();
    std::set<int> sampleIds;
    
    for (const auto& product : sampleProducts) {
        int id = product->getProductId();
        assert(sampleIds.count(id) == 0);
        sampleIds.insert(id);
    }
    std::cout << "  ✓ Sample data has unique product IDs" << std::endl;
    
    // Test 5: Verify findProduct returns correct product by ID
    Product* found101 = inventory.findProduct(101);
    assert(found101 != nullptr);
    assert(found101->getProductId() == 101);
    assert(found101->getName() == "Laptop");
    
    Product* found201 = inventory.findProduct(201);
    assert(found201 != nullptr);
    assert(found201->getProductId() == 201);
    assert(found201->getName() == "T-Shirt");
    
    Product* found301 = inventory.findProduct(301);
    assert(found301 != nullptr);
    assert(found301->getProductId() == 301);
    assert(found301->getName() == "C++ Programming");
    std::cout << "  ✓ findProduct correctly retrieves products by unique ID" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Inventory Operations Property Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testProperty26();
    testProperty27();
    testProperty28();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  ✅ All property tests passed!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
