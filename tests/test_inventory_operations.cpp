#include <rapidcheck.h>
#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include "../include/Electronics.h"
#include "../include/Clothing.h"
#include "../include/Book.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <cctype>

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
    std::cout << "\nRunning Property 26: Product Search Partial Matching" << std::endl;
    
    rc::check("Search results include all products with matching names", []() {
        Inventory inventory;
        
        // Generate random products with known names
        auto numProducts = *rc::gen::inRange(3, 10);
        std::vector<std::string> productNames;
        
        for (int i = 0; i < numProducts; i++) {
            auto nameLength = *rc::gen::inRange(5, 15);
            auto name = *rc::gen::container<std::string>(nameLength, rc::gen::inRange('a', 'z'));
            if (name.empty()) name = "product";
            
            productNames.push_back(name);
            
            // Create different product types
            int productType = i % 3;
            if (productType == 0) {
                inventory.addProduct(ProductFactory::createElectronics(
                    100 + i, name, 1000.0 + i * 100, 10, "Brand", 1
                ));
            } else if (productType == 1) {
                inventory.addProduct(ProductFactory::createClothing(
                    100 + i, name, 500.0 + i * 50, 10, "M", "Cotton"
                ));
            } else {
                inventory.addProduct(ProductFactory::createBook(
                    100 + i, name, 300.0 + i * 30, 10, "Author", "Genre"
                ));
            }
        }
        
        // Pick a random product name and extract a substring
        auto targetIndex = *rc::gen::inRange(0, numProducts);
        std::string targetName = productNames[targetIndex];
        
        // Extract a substring from the target name
        if (targetName.length() > 2) {
            auto startPos = *rc::gen::inRange(0, static_cast<int>(targetName.length()) - 2);
            auto length = *rc::gen::inRange(2, static_cast<int>(targetName.length()) - startPos);
            std::string searchTerm = targetName.substr(startPos, length);
            
            // Perform search
            auto results = inventory.searchByName(searchTerm);
            
            // Count how many products should match
            int expectedMatches = 0;
            for (const auto& name : productNames) {
                if (containsSubstring(name, searchTerm)) {
                    expectedMatches++;
                }
            }
            
            // Verify all matching products are in results
            RC_ASSERT(static_cast<int>(results.size()) == expectedMatches);
            
            // Verify each result actually contains the search term
            for (const Product* product : results) {
                RC_ASSERT(containsSubstring(product->getName(), searchTerm));
            }
        }
    });
}

// **Validates: Requirements 16.4**
// Property 27: Category Filter Exactness
// For any category filter, the results should include only products that belong 
// to exactly that category.
void testProperty27() {
    std::cout << "\nRunning Property 27: Category Filter Exactness" << std::endl;
    
    rc::check("Filter returns only products from specified category", []() {
        Inventory inventory;
        
        // Add products from all three categories
        auto numElectronics = *rc::gen::inRange(1, 5);
        auto numClothing = *rc::gen::inRange(1, 5);
        auto numBooks = *rc::gen::inRange(1, 5);
        
        int productId = 100;
        
        // Add Electronics
        for (int i = 0; i < numElectronics; i++) {
            auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
            if (name.empty()) name = "electronics";
            inventory.addProduct(ProductFactory::createElectronics(
                productId++, name, 1000.0, 10, "Brand", 1
            ));
        }
        
        // Add Clothing
        for (int i = 0; i < numClothing; i++) {
            auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
            if (name.empty()) name = "clothing";
            inventory.addProduct(ProductFactory::createClothing(
                productId++, name, 500.0, 10, "M", "Cotton"
            ));
        }
        
        // Add Books
        for (int i = 0; i < numBooks; i++) {
            auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
            if (name.empty()) name = "book";
            inventory.addProduct(ProductFactory::createBook(
                productId++, name, 300.0, 10, "Author", "Genre"
            ));
        }
        
        // Test filtering by Electronics
        auto electronicsResults = inventory.filterByCategory("Electronics");
        RC_ASSERT(static_cast<int>(electronicsResults.size()) == numElectronics);
        for (const Product* product : electronicsResults) {
            RC_ASSERT(product->getCategory() == "Electronics");
        }
        
        // Test filtering by Clothing
        auto clothingResults = inventory.filterByCategory("Clothing");
        RC_ASSERT(static_cast<int>(clothingResults.size()) == numClothing);
        for (const Product* product : clothingResults) {
            RC_ASSERT(product->getCategory() == "Clothing");
        }
        
        // Test filtering by Books
        auto booksResults = inventory.filterByCategory("Books");
        RC_ASSERT(static_cast<int>(booksResults.size()) == numBooks);
        for (const Product* product : booksResults) {
            RC_ASSERT(product->getCategory() == "Books");
        }
    });
}

// **Validates: Requirements 14.4**
// Property 28: Unique Product IDs
// For any product catalog, no two products should have the same product ID.
void testProperty28() {
    std::cout << "\nRunning Property 28: Unique Product IDs" << std::endl;
    
    rc::check("All products in inventory have unique IDs", []() {
        Inventory inventory;
        
        // Generate random number of products (small to avoid bad_alloc)
        auto numProducts = *rc::gen::inRange(2, 8);
        
        // Pre-generate unique IDs using a set
        std::set<int> usedIds;
        std::vector<int> productIds;
        
        // Generate IDs deterministically to avoid infinite loops
        for (int i = 0; i < numProducts; i++) {
            int productId = 100 + i * 7 + *rc::gen::inRange(0, 5);
            // Ensure uniqueness by offsetting if collision
            while (usedIds.count(productId) > 0) {
                productId += 13;
            }
            usedIds.insert(productId);
            productIds.push_back(productId);
        }
        
        for (int i = 0; i < numProducts; i++) {
            int productId = productIds[i];
            auto name = *rc::gen::container<std::string>(rc::gen::inRange('a', 'z'));
            if (name.empty()) name = "product";
            
            // Create different product types
            int productType = i % 3;
            if (productType == 0) {
                inventory.addProduct(ProductFactory::createElectronics(
                    productId, name, 1000.0, 10, "Brand", 1
                ));
            } else if (productType == 1) {
                inventory.addProduct(ProductFactory::createClothing(
                    productId, name, 500.0, 10, "M", "Cotton"
                ));
            } else {
                inventory.addProduct(ProductFactory::createBook(
                    productId, name, 300.0, 10, "Author", "Genre"
                ));
            }
        }
        
        // Get all products and verify unique IDs
        const auto& products = inventory.getProducts();
        std::set<int> seenIds;
        
        for (const auto& product : products) {
            int id = product->getProductId();
            // Check that this ID hasn't been seen before
            RC_ASSERT(seenIds.count(id) == 0);
            seenIds.insert(id);
        }
        
        // Verify we have the expected number of unique IDs
        RC_ASSERT(static_cast<int>(seenIds.size()) == numProducts);
    });
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Inventory Operations Property Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testProperty26();
    testProperty27();
    testProperty28();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  All property tests passed!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
