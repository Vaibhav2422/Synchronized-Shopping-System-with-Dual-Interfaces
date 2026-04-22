#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <memory>
#include <string>
#include "Product.h"

/**
 * Inventory class manages the product catalog.
 * Owns all Product instances via unique_ptr and provides CRUD operations,
 * search/filter capabilities, and display methods.
 * Requirements: 4.5, 16.1–16.4
 */
class Inventory {
private:
    std::vector<std::unique_ptr<Product>> products;
    
public:
    Inventory();
    ~Inventory() = default;
    
    // Move constructor and assignment (unique_ptr requires move semantics)
    Inventory(Inventory&&) = default;
    Inventory& operator=(Inventory&&) = default;
    
    // Disable copy (unique_ptr is not copyable)
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;
    
    // Initialization
    void initializeSampleData();
    
    // Product management
    // Time Complexity: O(1) amortized - vector push_back
    // Space Complexity: O(1) - no additional storage beyond the element itself
    void addProduct(std::unique_ptr<Product> product);

    // Time Complexity: O(n) - linear scan through products vector to find by ID
    // Space Complexity: O(1) - no additional storage required
    bool removeProduct(int productId);

    // Time Complexity: O(n) - delegates to findProduct (linear scan)
    // Space Complexity: O(1) - no additional storage required
    bool updatePrice(int productId, double newPrice);

    // Time Complexity: O(n) - delegates to findProduct (linear scan)
    // Space Complexity: O(1) - no additional storage required
    bool updateStock(int productId, int newQuantity);
    
    // Query operations
    // Time Complexity: O(n) - linear scan through products vector
    // Space Complexity: O(1) - no additional storage required
    Product* findProduct(int productId);

    // Time Complexity: O(n) - linear scan through products vector
    // Space Complexity: O(1) - no additional storage required
    const Product* findProduct(int productId) const;

    // Time Complexity: O(n) - linear scan through all products with substring match
    // Space Complexity: O(k) - result vector holds k matching product pointers
    std::vector<const Product*> searchByName(const std::string& name) const;

    // Time Complexity: O(n) - linear scan through all products comparing category
    // Space Complexity: O(k) - result vector holds k matching product pointers
    std::vector<const Product*> filterByCategory(const std::string& category) const;
    
    // Display
    void displayAllProducts() const;
    void displayByCategory(const std::string& category) const;
    
    // Access
    const std::vector<std::unique_ptr<Product>>& getProducts() const;
};

#endif // INVENTORY_H
