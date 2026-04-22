#include <rapidcheck.h>
#include "../include/Cart.h"
#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include "../include/StockException.h"
#include "../include/InvalidInputException.h"
#include <sstream>
#include <iostream>

// Helper function to create a test inventory with sample products
Inventory createTestInventory() {
    Inventory inv;
    
    // Add Electronics
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 5, "Dell", 2));
    inv.addProduct(ProductFactory::createElectronics(102, "Smartphone", 25000.0, 10, "Samsung", 1));
    inv.addProduct(ProductFactory::createElectronics(103, "Headphones", 3000.0, 15, "Sony", 1));
    
    // Add Clothing
    inv.addProduct(ProductFactory::createClothing(201, "T-Shirt", 500.0, 20, "M", "Cotton"));
    inv.addProduct(ProductFactory::createClothing(202, "Jeans", 1500.0, 15, "32", "Denim"));
    inv.addProduct(ProductFactory::createClothing(203, "Jacket", 3000.0, 8, "L", "Leather"));
    
    // Add Books
    inv.addProduct(ProductFactory::createBook(301, "C++ Programming", 600.0, 12, "Bjarne Stroustrup", "Technical"));
    inv.addProduct(ProductFactory::createBook(302, "Clean Code", 800.0, 10, "Robert Martin", "Technical"));
    inv.addProduct(ProductFactory::createBook(303, "Pragmatic Programmer", 700.0, 8, "Hunt & Thomas", "Technical"));
    
    return inv;
}

// Generator for valid product IDs from test inventory
rc::Gen<int> genValidProductId() {
    return rc::gen::element(101, 102, 103, 201, 202, 203, 301, 302, 303);
}

// Generator for invalid product IDs (not in inventory)
rc::Gen<int> genInvalidProductId() {
    return rc::gen::suchThat(
        rc::gen::inRange(1, 1000),
        [](int id) {
            return id != 101 && id != 102 && id != 103 &&
                   id != 201 && id != 202 && id != 203 &&
                   id != 301 && id != 302 && id != 303;
        }
    );
}

// Generator for positive quantities
rc::Gen<int> genPositiveQuantity() {
    return rc::gen::inRange(1, 10);
}

// **Validates: Requirements 3.3, 4.1, 17.5**
// Property 4: Cart-Inventory Synchronization on Add
void testCartInventorySynchronizationOnAdd() {
    std::cout << "\nRunning Property 4: Cart-Inventory Synchronization on Add" << std::endl;
    
    rc::check("Adding product to cart decrements inventory by exact quantity", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        const int quantity = *genPositiveQuantity();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        const int initialStock = product->getQuantityAvailable();
        
        // Skip if insufficient stock
        if (initialStock < quantity) {
            RC_DISCARD("Insufficient stock for test");
        }
        
        // Add product to cart
        bool success = cart.addProduct(productId, quantity);
        RC_ASSERT(success);
        
        // Verify inventory was decremented by exactly the quantity
        const int finalStock = product->getQuantityAvailable();
        RC_ASSERT(finalStock == initialStock - quantity);
    });
}

// **Validates: Requirements 3.4, 4.2, 17.6**
// Property 5: Cart-Inventory Round Trip
void testCartInventoryRoundTrip() {
    std::cout << "\nRunning Property 5: Cart-Inventory Round Trip" << std::endl;
    
    rc::check("Adding then removing product restores inventory", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        const int quantity = *genPositiveQuantity();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        const int initialStock = product->getQuantityAvailable();
        
        // Skip if insufficient stock
        if (initialStock < quantity) {
            RC_DISCARD("Insufficient stock for test");
        }
        
        // Add product to cart
        cart.addProduct(productId, quantity);
        
        // Remove product from cart
        bool removed = cart.removeProduct(productId);
        RC_ASSERT(removed);
        
        // Verify inventory was restored to original quantity
        const int finalStock = product->getQuantityAvailable();
        RC_ASSERT(finalStock == initialStock);
    });
}

// **Validates: Requirements 3.5, 11.1**
// Property 6: Product ID Validation
void testProductIdValidation() {
    std::cout << "\nRunning Property 6: Product ID Validation" << std::endl;
    
    rc::check("Invalid product ID throws exception", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int invalidId = *genInvalidProductId();
        const int quantity = *genPositiveQuantity();
        
        // Attempting to add invalid product ID should throw exception
        bool exceptionThrown = false;
        try {
            cart.addProduct(invalidId, quantity);
        } catch (const InvalidProductIdException&) {
            exceptionThrown = true;
        }
        
        RC_ASSERT(exceptionThrown);
    });
}

// **Validates: Requirements 3.2, 4.3**
// Property 7: Out-of-Stock Prevention
void testOutOfStockPrevention() {
    std::cout << "\nRunning Property 7: Out-of-Stock Prevention" << std::endl;
    
    rc::check("Out-of-stock product throws exception", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        // Deplete the stock completely
        const int initialStock = product->getQuantityAvailable();
        for (int i = 0; i < initialStock; ++i) {
            product->decrementStock(1);
        }
        
        RC_ASSERT(product->getQuantityAvailable() == 0);
        
        // Attempting to add out-of-stock product should throw exception
        bool exceptionThrown = false;
        try {
            cart.addProduct(productId, 1);
        } catch (const ProductUnavailableException&) {
            exceptionThrown = true;
        }
        
        RC_ASSERT(exceptionThrown);
    });
}

// **Validates: Requirements 17.4**
// Property 8: Insufficient Stock Prevention
void testInsufficientStockPrevention() {
    std::cout << "\nRunning Property 8: Insufficient Stock Prevention" << std::endl;
    
    rc::check("Insufficient stock throws exception", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        const int availableStock = product->getQuantityAvailable();
        
        // Skip if stock is too high (we want to test insufficient stock)
        if (availableStock > 50) {
            RC_DISCARD("Stock too high for test");
        }
        
        const int excessiveQuantity = availableStock + *rc::gen::inRange(1, 10);
        
        // Attempting to add more than available should throw exception
        bool exceptionThrown = false;
        try {
            cart.addProduct(productId, excessiveQuantity);
        } catch (const InsufficientStockException&) {
            exceptionThrown = true;
        }
        
        RC_ASSERT(exceptionThrown);
    });
}

// **Validates: Requirements 3.7, 17.3**
// Property 9: Cart Display Completeness
void testCartDisplayCompleteness() {
    std::cout << "\nRunning Property 9: Cart Display Completeness" << std::endl;
    
    rc::check("Cart display contains product details and quantities", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        const int quantity = *genPositiveQuantity();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        // Skip if insufficient stock
        if (product->getQuantityAvailable() < quantity) {
            RC_DISCARD("Insufficient stock for test");
        }
        
        cart.addProduct(productId, quantity);
        
        // Capture display output
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        cart.displayCart();
        
        std::cout.rdbuf(old);
        
        std::string output = buffer.str();
        
        // Verify output contains product name and quantity
        RC_ASSERT(output.find(product->getName()) != std::string::npos);
        RC_ASSERT(output.find("x" + std::to_string(quantity)) != std::string::npos);
    });
}

// **Validates: Requirements 17.2**
// Property 10: Quantity Increment on Duplicate Add
void testQuantityIncrementOnDuplicateAdd() {
    std::cout << "\nRunning Property 10: Quantity Increment on Duplicate Add" << std::endl;
    
    rc::check("Adding same product twice increments quantity", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        const int productId = *genValidProductId();
        const int quantity1 = *genPositiveQuantity();
        const int quantity2 = *genPositiveQuantity();
        
        Product* product = inv.findProduct(productId);
        RC_ASSERT(product != nullptr);
        
        // Skip if insufficient stock
        if (product->getQuantityAvailable() < quantity1 + quantity2) {
            RC_DISCARD("Insufficient stock for test");
        }
        
        // Add product first time
        cart.addProduct(productId, quantity1);
        
        const size_t itemsAfterFirst = cart.getItems().size();
        
        // Add same product again
        cart.addProduct(productId, quantity2);
        
        const size_t itemsAfterSecond = cart.getItems().size();
        
        // Verify no duplicate entry was created
        RC_ASSERT(itemsAfterFirst == itemsAfterSecond);
        
        // Verify quantity was incremented
        const auto& items = cart.getItems();
        bool found = false;
        for (const auto& item : items) {
            if (item.product->getProductId() == productId) {
                RC_ASSERT(item.quantity == quantity1 + quantity2);
                found = true;
                break;
            }
        }
        RC_ASSERT(found);
    });
}

// **Validates: Requirements 7.1**
// Property 13: Subtotal Calculation
void testSubtotalCalculation() {
    std::cout << "\nRunning Property 13: Subtotal Calculation" << std::endl;
    
    rc::check("Subtotal equals sum of price × quantity", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        // Generate 1-3 different products to add
        const int numProducts = *rc::gen::inRange(1, 4);
        std::vector<int> productIds;
        std::vector<int> quantities;
        double expectedSubtotal = 0.0;
        
        for (int i = 0; i < numProducts; ++i) {
            const int productId = *genValidProductId();
            const int quantity = *genPositiveQuantity();
            
            Product* product = inv.findProduct(productId);
            if (!product || product->getQuantityAvailable() < quantity) {
                RC_DISCARD("Insufficient stock for test");
            }
            
            productIds.push_back(productId);
            quantities.push_back(quantity);
            expectedSubtotal += product->getPrice() * quantity;
            
            cart.addProduct(productId, quantity);
        }
        
        const double actualSubtotal = cart.calculateSubtotal();
        
        // Allow small floating point error
        RC_ASSERT(std::abs(actualSubtotal - expectedSubtotal) < 0.01);
    });
}

// **Validates: Requirements 7.2**
// Property 14: Product Discount Summation
void testProductDiscountSummation() {
    std::cout << "\nRunning Property 14: Product Discount Summation" << std::endl;
    
    rc::check("Product discounts equal sum of discount × quantity", []() {
        Inventory inv = createTestInventory();
        Cart cart(&inv);
        
        // Generate 1-3 different products to add
        const int numProducts = *rc::gen::inRange(1, 4);
        double expectedDiscounts = 0.0;
        
        for (int i = 0; i < numProducts; ++i) {
            const int productId = *genValidProductId();
            const int quantity = *genPositiveQuantity();
            
            Product* product = inv.findProduct(productId);
            if (!product || product->getQuantityAvailable() < quantity) {
                RC_DISCARD("Insufficient stock for test");
            }
            
            expectedDiscounts += product->calculateDiscount() * quantity;
            cart.addProduct(productId, quantity);
        }
        
        const double actualDiscounts = cart.calculateProductDiscounts();
        
        // Allow small floating point error
        RC_ASSERT(std::abs(actualDiscounts - expectedDiscounts) < 0.01);
    });
}

int main() {
    std::cout << "=== Cart Operations Property Tests ===" << std::endl;
    
    testCartInventorySynchronizationOnAdd();
    testCartInventoryRoundTrip();
    testProductIdValidation();
    testOutOfStockPrevention();
    testInsufficientStockPrevention();
    testCartDisplayCompleteness();
    testQuantityIncrementOnDuplicateAdd();
    testSubtotalCalculation();
    testProductDiscountSummation();
    
    std::cout << "\n=== All Cart Property Tests Completed ===" << std::endl;
    
    return 0;
}
