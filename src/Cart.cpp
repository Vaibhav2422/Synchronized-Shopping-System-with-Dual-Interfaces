#include "Cart.h"
#include "ANSIColors.h"
#include "StockException.h"
#include "InvalidInputException.h"
#include <iostream>
#include <iomanip>

// CartItem implementation
CartItem::CartItem(std::unique_ptr<Product> prod, int qty)
    : product(std::move(prod)), quantity(qty) {}

CartItem::CartItem(CartItem&& other) noexcept
    : product(std::move(other.product)), quantity(other.quantity) {}

CartItem& CartItem::operator=(CartItem&& other) noexcept {
    if (this != &other) {
        product = std::move(other.product);
        quantity = other.quantity;
    }
    return *this;
}

double CartItem::getLineTotal() const {
    return product->getPrice() * quantity;
}

double CartItem::getLineDiscount() const {
    return product->calculateDiscount() * quantity;
}

// Cart implementation
Cart::Cart(Inventory* inv) : inventory(inv) {}

// Time Complexity: O(n) - calls inventory->findProduct (O(n) scan) then scans items for duplicates
// Space Complexity: O(1) - no additional storage beyond the new cart item
bool Cart::addProduct(int productId, int quantity) {
    if (quantity <= 0) {
        throw InvalidQuantityException(quantity);
    }
    
    // Find product in inventory
    Product* inventoryProduct = inventory->findProduct(productId);
    if (!inventoryProduct) {
        throw InvalidProductIdException(productId);
    }
    
    // Check if product is available with sufficient stock
    if (!inventoryProduct->isAvailable(quantity)) {
        int available = inventoryProduct->getQuantityAvailable();
        if (available == 0) {
            throw ProductUnavailableException(inventoryProduct->getName());
        } else {
            throw InsufficientStockException(
                inventoryProduct->getName(), 
                quantity, 
                available
            );
        }
    }
    
    // Check if product already exists in cart
    for (auto& item : items) {
        if (item.product->getProductId() == productId) {
            // Increment quantity for existing product
            item.quantity += quantity;
            inventoryProduct->decrementStock(quantity);
            return true;
        }
    }
    
    // Clone product and add to cart
    std::unique_ptr<Product> clone = inventoryProduct->clone();
    inventoryProduct->decrementStock(quantity);
    items.emplace_back(std::move(clone), quantity);
    
    return true;
}

// Time Complexity: O(n) - linear scan through cart items to find matching product ID
// Space Complexity: O(1) - no additional storage required
bool Cart::removeProduct(int productId) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->product->getProductId() == productId) {
            // Restore inventory stock
            Product* inventoryProduct = inventory->findProduct(productId);
            if (inventoryProduct) {
                inventoryProduct->incrementStock(it->quantity);
            }
            
            // Remove from cart
            items.erase(it);
            return true;
        }
    }
    
    return false;
}

void Cart::clear() {
    // Restore all inventory stock
    for (auto& item : items) {
        Product* inventoryProduct = inventory->findProduct(item.product->getProductId());
        if (inventoryProduct) {
            inventoryProduct->incrementStock(item.quantity);
        }
    }
    
    items.clear();
}

bool Cart::isEmpty() const {
    return items.empty();
}

int Cart::getItemCount() const {
    int count = 0;
    for (const auto& item : items) {
        count += item.quantity;
    }
    return count;
}

// Time Complexity: O(n) - iterates all cart items to sum line totals
// Space Complexity: O(1) - no additional storage required
double Cart::calculateSubtotal() const {
    double subtotal = 0.0;
    for (const auto& item : items) {
        subtotal += item.getLineTotal();
    }
    return subtotal;
}

// Time Complexity: O(n) - iterates all cart items to sum per-item discounts
// Space Complexity: O(1) - no additional storage required
double Cart::calculateProductDiscounts() const {
    double totalDiscounts = 0.0;
    for (const auto& item : items) {
        totalDiscounts += item.getLineDiscount();
    }
    return totalDiscounts;
}

// Time Complexity: O(n) - calls calculateSubtotal which iterates all cart items
// Space Complexity: O(1) - no additional storage required
double Cart::calculateCartDiscount() const {
    // Apply a 5% cart-level discount when the subtotal exceeds the threshold.
    // This discount is calculated on the raw subtotal (before product discounts)
    // and is applied automatically without any user action.
    double subtotal = calculateSubtotal();
    if (subtotal > 5000.0) {
        return subtotal * 0.05;
    }
    return 0.0;
}

void Cart::displayCart() const {
    using namespace ANSIColors;
    
    std::cout << CYAN << "\n========================================\n";
    std::cout << "           SHOPPING CART\n";
    std::cout << "========================================\n" << RESET;
    
    if (isEmpty()) {
        std::cout << YELLOW << "Your cart is empty.\n" << RESET;
        std::cout << CYAN << "========================================\n" << RESET;
        return;
    }
    
    int itemNumber = 1;
    for (const auto& item : items) {
        std::cout << WHITE << itemNumber++ << ". ";
        std::cout << item.product->getName() << " x" << item.quantity;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Rs. " << YELLOW << item.getLineTotal() << RESET << "\n";
    }
    
    std::cout << CYAN << "----------------------------------------\n";
    std::cout << WHITE << "Total Items: " << YELLOW << getItemCount() << RESET << "\n";
    std::cout << WHITE << "Subtotal: Rs. " << YELLOW << calculateSubtotal() << RESET << "\n";
    std::cout << CYAN << "========================================\n" << RESET;
}

const std::vector<CartItem>& Cart::getItems() const {
    return items;
}
