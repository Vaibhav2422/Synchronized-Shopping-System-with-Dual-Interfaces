#ifndef CART_H
#define CART_H

#include <vector>
#include <memory>
#include "Product.h"
#include "Inventory.h"

/**
 * CartItem structure to hold a product and its quantity in the cart.
 * Uses unique_ptr for exclusive ownership of cloned products.
 */
struct CartItem {
    std::unique_ptr<Product> product;
    int quantity;
    
    // Constructor
    CartItem(std::unique_ptr<Product> prod, int qty);
    
    // Move constructor
    CartItem(CartItem&& other) noexcept;
    
    // Move assignment operator
    CartItem& operator=(CartItem&& other) noexcept;
    
    // Delete copy constructor and copy assignment (unique_ptr is non-copyable)
    CartItem(const CartItem&) = delete;
    CartItem& operator=(const CartItem&) = delete;
    
    // Helper methods
    double getLineTotal() const;
    double getLineDiscount() const;
};

/**
 * Cart class manages shopping cart items and synchronizes with inventory.
 * Supports adding/removing products with quantity tracking.
 */
class Cart {
private:
    std::vector<CartItem> items;
    Inventory* inventory;  // Non-owning pointer
    
public:
    explicit Cart(Inventory* inv);
    ~Cart() = default;
    
    // Cart operations
    // Time Complexity: O(n) - calls findProduct (linear scan) then scans items for duplicates
    // Space Complexity: O(1) - no additional storage beyond the new cart item
    bool addProduct(int productId, int quantity = 1);

    // Time Complexity: O(n) - linear scan through cart items to find by ID
    // Space Complexity: O(1) - no additional storage required
    bool removeProduct(int productId);

    // Time Complexity: O(n) - iterates all items to restore inventory stock
    // Space Complexity: O(1) - no additional storage required
    void clear();
    
    // Query operations
    // Time Complexity: O(1) - checks if items vector is empty
    bool isEmpty() const;

    // Time Complexity: O(n) - iterates all cart items to sum quantities
    int getItemCount() const;

    // Time Complexity: O(n) - iterates all cart items to sum line totals
    // Space Complexity: O(1) - no additional storage required
    double calculateSubtotal() const;

    // Time Complexity: O(n) - iterates all cart items to sum per-item discounts
    // Space Complexity: O(1) - no additional storage required
    double calculateProductDiscounts() const;

    // Time Complexity: O(n) - calls calculateSubtotal which iterates all items
    // Space Complexity: O(1) - no additional storage required
    double calculateCartDiscount() const;
    
    // Display
    void displayCart() const;
    
    // Access
    const std::vector<CartItem>& getItems() const;
};

#endif // CART_H
