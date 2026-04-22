#include "Inventory.h"
#include "ProductFactory.h"
#include "ANSIColors.h"
#include "Logger.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <map>

Inventory::Inventory() {
    // Constructor
}

void Inventory::initializeSampleData() {
    // Electronics products (IDs starting from 101)
    addProduct(ProductFactory::createElectronics(
        101, "Laptop", 45000.0, 10, "Dell", 2
    ));
    
    addProduct(ProductFactory::createElectronics(
        102, "Smartphone", 25000.0, 15, "Samsung", 1
    ));
    
    addProduct(ProductFactory::createElectronics(
        103, "Headphones", 3500.0, 20, "Sony", 1
    ));
    
    // Clothing products (IDs starting from 201)
    addProduct(ProductFactory::createClothing(
        201, "T-Shirt", 800.0, 25, "L", "Cotton"
    ));
    
    addProduct(ProductFactory::createClothing(
        202, "Jeans", 1500.0, 18, "M", "Denim"
    ));
    
    addProduct(ProductFactory::createClothing(
        203, "Jacket", 2500.0, 12, "XL", "Leather"
    ));
    
    // Book products (IDs starting from 301)
    addProduct(ProductFactory::createBook(
        301, "C++ Programming", 650.0, 30, "Bjarne Stroustrup", "Programming"
    ));
    
    addProduct(ProductFactory::createBook(
        302, "Clean Code", 550.0, 25, "Robert C. Martin", "Software Engineering"
    ));
    
    addProduct(ProductFactory::createBook(
        303, "Pragmatic Programmer", 600.0, 20, "Andrew Hunt", "Software Engineering"
    ));
}

// Time Complexity: O(1) amortized - vector push_back (may trigger O(n) reallocation, amortized O(1))
// Space Complexity: O(1) - no additional storage beyond the element itself
void Inventory::addProduct(std::unique_ptr<Product> product) {
    if (product) {
        std::string name = product->getName();
        int id = product->getProductId();
        products.push_back(std::move(product));
        Logger::logInfo("Inventory", "Product added: " + name + " (ID: " + std::to_string(id) + ")");
    }
}

// Time Complexity: O(n) - linear scan through products vector to find matching ID
// Space Complexity: O(1) - no additional storage required
bool Inventory::removeProduct(int productId) {
    auto it = std::find_if(products.begin(), products.end(),
        [productId](const std::unique_ptr<Product>& p) {
            return p->getProductId() == productId;
        });
    
    if (it != products.end()) {
        products.erase(it);
        Logger::logInfo("Inventory", "Product removed: ID " + std::to_string(productId));
        return true;
    }
    Logger::logWarning("Inventory", "Remove failed - product not found: ID " + std::to_string(productId));
    return false;
}

bool Inventory::updatePrice(int productId, double newPrice) {
    Product* product = findProduct(productId);
    if (product && newPrice > 0) {
        product->setPrice(newPrice);
        Logger::logInfo("Inventory", "Price updated for product ID " + std::to_string(productId));
        return true;
    }
    return false;
}

bool Inventory::updateStock(int productId, int newQuantity) {
    Product* product = findProduct(productId);
    if (product && newQuantity >= 0) {
        // Compute the delta between desired and current stock, then apply
        // incrementStock/decrementStock to keep validation logic in one place.
        int currentStock = product->getQuantityAvailable();
        int difference = newQuantity - currentStock;
        
        if (difference > 0) {
            product->incrementStock(difference);
        } else if (difference < 0) {
            product->decrementStock(-difference);
        }
        Logger::logInfo("Inventory", "Stock updated for product ID " + std::to_string(productId));
        return true;
    }
    return false;
}

// Time Complexity: O(n) - delegates to findProduct which performs a linear scan
// Space Complexity: O(1) - no additional storage required
Product* Inventory::findProduct(int productId) {
    auto it = std::find_if(products.begin(), products.end(),
        [productId](const std::unique_ptr<Product>& p) {
            return p->getProductId() == productId;
        });
    
    return (it != products.end()) ? it->get() : nullptr;
}

// Time Complexity: O(n) - linear scan through products vector to find matching ID
// Space Complexity: O(1) - no additional storage required
const Product* Inventory::findProduct(int productId) const {
    auto it = std::find_if(products.begin(), products.end(),
        [productId](const std::unique_ptr<Product>& p) {
            return p->getProductId() == productId;
        });
    
    return (it != products.end()) ? it->get() : nullptr;
}

// Time Complexity: O(n) - linear scan through all products with case-insensitive substring match
// Space Complexity: O(k) - result vector holds k matching product pointers (k ≤ n)
std::vector<const Product*> Inventory::searchByName(const std::string& name) const {
    std::vector<const Product*> results;
    
    // Convert search term to lowercase for case-insensitive matching.
    // Time complexity: O(n) where n is the number of products in inventory.
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
        [](unsigned char c) { return std::tolower(c); });
    
    for (const auto& product : products) {
        std::string productName = product->getName();
        std::transform(productName.begin(), productName.end(), productName.begin(),
            [](unsigned char c) { return std::tolower(c); });
        
        // Partial matching
        if (productName.find(lowerName) != std::string::npos) {
            results.push_back(product.get());
        }
    }
    
    return results;
}

// Time Complexity: O(n) - linear scan through all products comparing category string
// Space Complexity: O(k) - result vector holds k matching product pointers (k ≤ n)
std::vector<const Product*> Inventory::filterByCategory(const std::string& category) const {
    std::vector<const Product*> results;
    
    for (const auto& product : products) {
        if (product->getCategory() == category) {
            results.push_back(product.get());
        }
    }
    
    return results;
}

void Inventory::displayAllProducts() const {
    std::cout << ANSIColors::BG_BLACK;
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "\n========================================\n";
    std::cout << "          PRODUCT CATALOG\n";
    std::cout << "========================================\n" << ANSIColors::RESET;
    
    if (products.empty()) {
        std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
        std::cout << "No products available in inventory.\n";
        std::cout << ANSIColors::RESET;
        return;
    }
    
    // Group products by category
    std::map<std::string, std::vector<const Product*>> categorizedProducts;
    
    for (const auto& product : products) {
        categorizedProducts[product->getCategory()].push_back(product.get());
    }
    
    // Display products by category
    for (const auto& categoryPair : categorizedProducts) {
        const std::string& category = categoryPair.first;
        const std::vector<const Product*>& productList = categoryPair.second;
        
        std::cout << ANSIColors::BG_BLACK << ANSIColors::YELLOW << ANSIColors::BOLD;
        std::cout << "\n--- " << category << " ---\n" << ANSIColors::RESET;
        
        for (const Product* product : productList) {
            std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
            product->display();
            std::cout << ANSIColors::RESET;
        }
    }
    
    std::cout << ANSIColors::BG_BLACK << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "\n========================================\n" << ANSIColors::RESET;
}

void Inventory::displayByCategory(const std::string& category) const {
    std::cout << ANSIColors::BG_BLACK;
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "\n========================================\n";
    std::cout << "          " << category << "\n";
    std::cout << "========================================\n" << ANSIColors::RESET;
    
    std::vector<const Product*> categoryProducts = filterByCategory(category);
    
    if (categoryProducts.empty()) {
        std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
        std::cout << "No products found in category: " << category << "\n";
        std::cout << ANSIColors::RESET;
        return;
    }
    
    for (const Product* product : categoryProducts) {
        std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN;
        product->display();
        std::cout << ANSIColors::RESET;
    }
    
    std::cout << ANSIColors::BG_BLACK << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "\n========================================\n" << ANSIColors::RESET;
}

const std::vector<std::unique_ptr<Product>>& Inventory::getProducts() const {
    return products;
}
