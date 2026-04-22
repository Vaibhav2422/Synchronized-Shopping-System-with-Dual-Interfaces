/**
 * Admin Property Tests
 * Tests for Admin class authentication and inventory operations.
 * Properties 33-37, 51, 52
 * Validates: Requirements 18.1-18.8
 */
#include "../include/Admin.h"
#include "../include/Inventory.h"
#include "../include/ProductFactory.h"
#include <iostream>
#include <cassert>

// **Validates: Requirements 18.1, 18.7**
// Property 33: Admin Authentication
// Valid credentials authenticate; invalid ones don't.
void testProperty33() {
    std::cout << "\n=== Property 33: Admin Authentication ===" << std::endl;
    std::cout << "**Validates: Requirements 18.1, 18.7**\n" << std::endl;

    Inventory inv;
    Admin admin(&inv);

    // Before authentication, isAuthenticated() should be false
    assert(!admin.isAuthenticated());
    std::cout << "  ✓ Not authenticated before login" << std::endl;

    // Wrong username
    assert(!admin.authenticate("wronguser", "admin123"));
    assert(!admin.isAuthenticated());
    std::cout << "  ✓ Wrong username rejected" << std::endl;

    // Wrong password
    assert(!admin.authenticate("admin", "wrongpass"));
    assert(!admin.isAuthenticated());
    std::cout << "  ✓ Wrong password rejected" << std::endl;

    // Both wrong
    assert(!admin.authenticate("hacker", "password"));
    assert(!admin.isAuthenticated());
    std::cout << "  ✓ Both wrong credentials rejected" << std::endl;

    // Empty credentials
    assert(!admin.authenticate("", ""));
    assert(!admin.isAuthenticated());
    std::cout << "  ✓ Empty credentials rejected" << std::endl;

    // Valid credentials
    assert(admin.authenticate("admin", "admin123"));
    assert(admin.isAuthenticated());
    std::cout << "  ✓ Valid credentials accepted" << std::endl;
}

// **Validates: Requirements 18.3**
// Property 34: Admin Product Addition
// Adding a product via inventory->addProduct() (simulating addNewProduct) makes it findable.
void testProperty34() {
    std::cout << "\n=== Property 34: Admin Product Addition ===" << std::endl;
    std::cout << "**Validates: Requirements 18.3**\n" << std::endl;

    Inventory inv;
    Admin admin(&inv);
    admin.authenticate("admin", "admin123");

    // Simulate what addNewProduct() does: create via factory and add to inventory
    auto product = ProductFactory::createElectronics(501, "Test Laptop", 50000.0, 5, "TestBrand", 2);
    inv.addProduct(std::move(product));

    // Product should now be findable
    Product* found = inv.findProduct(501);
    assert(found != nullptr);
    assert(found->getProductId() == 501);
    assert(found->getName() == "Test Laptop");
    assert(found->getPrice() == 50000.0);
    assert(found->getQuantityAvailable() == 5);
    std::cout << "  ✓ Added product is findable in inventory" << std::endl;

    // Inventory size should reflect the addition
    assert(inv.getProducts().size() == 1);
    std::cout << "  ✓ Inventory size increased after product addition" << std::endl;

    // Add a clothing product
    auto clothing = ProductFactory::createClothing(502, "Test Shirt", 800.0, 10, "M", "Cotton");
    inv.addProduct(std::move(clothing));
    assert(inv.getProducts().size() == 2);
    assert(inv.findProduct(502) != nullptr);
    std::cout << "  ✓ Multiple products can be added" << std::endl;
}

// **Validates: Requirements 18.4**
// Property 35: Admin Price Update
// updateProductPrice changes the price correctly.
void testProperty35() {
    std::cout << "\n=== Property 35: Admin Price Update ===" << std::endl;
    std::cout << "**Validates: Requirements 18.4**\n" << std::endl;

    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));

    // Verify initial price
    Product* p = inv.findProduct(101);
    assert(p != nullptr);
    assert(p->getPrice() == 45000.0);
    std::cout << "  ✓ Initial price is 45000.0" << std::endl;

    // Update price
    bool updated = inv.updatePrice(101, 40000.0);
    assert(updated);
    assert(p->getPrice() == 40000.0);
    std::cout << "  ✓ Price updated to 40000.0" << std::endl;

    // Update to a different price
    updated = inv.updatePrice(101, 55000.0);
    assert(updated);
    assert(p->getPrice() == 55000.0);
    std::cout << "  ✓ Price updated again to 55000.0" << std::endl;

    // Non-existent product returns false
    bool notFound = inv.updatePrice(999, 100.0);
    assert(!notFound);
    std::cout << "  ✓ Update on non-existent product returns false" << std::endl;

    // Invalid price (zero or negative) returns false
    bool invalidPrice = inv.updatePrice(101, 0.0);
    assert(!invalidPrice);
    std::cout << "  ✓ Zero price update rejected" << std::endl;
}

// **Validates: Requirements 18.5**
// Property 36: Admin Stock Update
// updateProductStock changes stock correctly.
void testProperty36() {
    std::cout << "\n=== Property 36: Admin Stock Update ===" << std::endl;
    std::cout << "**Validates: Requirements 18.5**\n" << std::endl;

    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));

    Product* p = inv.findProduct(101);
    assert(p != nullptr);
    assert(p->getQuantityAvailable() == 10);
    std::cout << "  ✓ Initial stock is 10" << std::endl;

    // Increase stock
    bool updated = inv.updateStock(101, 20);
    assert(updated);
    assert(p->getQuantityAvailable() == 20);
    std::cout << "  ✓ Stock increased to 20" << std::endl;

    // Decrease stock
    updated = inv.updateStock(101, 5);
    assert(updated);
    assert(p->getQuantityAvailable() == 5);
    std::cout << "  ✓ Stock decreased to 5" << std::endl;

    // Set to zero (out of stock)
    updated = inv.updateStock(101, 0);
    assert(updated);
    assert(p->getQuantityAvailable() == 0);
    std::cout << "  ✓ Stock set to 0 (out of stock)" << std::endl;

    // Non-existent product returns false
    bool notFound = inv.updateStock(999, 10);
    assert(!notFound);
    std::cout << "  ✓ Update on non-existent product returns false" << std::endl;
}

// **Validates: Requirements 18.6**
// Property 37: Admin Product Removal
// removeProduct removes the product from inventory.
void testProperty37() {
    std::cout << "\n=== Property 37: Admin Product Removal ===" << std::endl;
    std::cout << "**Validates: Requirements 18.6**\n" << std::endl;

    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));
    inv.addProduct(ProductFactory::createClothing(201, "T-Shirt", 800.0, 25, "L", "Cotton"));
    inv.addProduct(ProductFactory::createBook(301, "C++ Book", 650.0, 30, "Author", "Programming"));

    assert(inv.getProducts().size() == 3);
    std::cout << "  ✓ Inventory has 3 products initially" << std::endl;

    // Remove existing product
    bool removed = inv.removeProduct(201);
    assert(removed);
    assert(inv.getProducts().size() == 2);
    assert(inv.findProduct(201) == nullptr);
    std::cout << "  ✓ Product 201 removed, inventory has 2 products" << std::endl;

    // Remove another product
    removed = inv.removeProduct(101);
    assert(removed);
    assert(inv.getProducts().size() == 1);
    assert(inv.findProduct(101) == nullptr);
    std::cout << "  ✓ Product 101 removed, inventory has 1 product" << std::endl;

    // Non-existent product returns false
    bool notFound = inv.removeProduct(999);
    assert(!notFound);
    assert(inv.getProducts().size() == 1);
    std::cout << "  ✓ Removing non-existent product returns false, size unchanged" << std::endl;

    // Remove last product
    removed = inv.removeProduct(301);
    assert(removed);
    assert(inv.getProducts().empty());
    std::cout << "  ✓ Last product removed, inventory is empty" << std::endl;
}

// **Validates: Requirements 18.1, 18.7**
// Property 51: Admin Menu Display After Authentication
// isAuthenticated() returns false before auth and true after valid auth.
// run() should only proceed when authenticated.
void testProperty51() {
    std::cout << "\n=== Property 51: Admin Menu Display After Authentication ===" << std::endl;
    std::cout << "**Validates: Requirements 18.1, 18.7**\n" << std::endl;

    Inventory inv;
    Admin admin1(&inv);

    // Not authenticated initially
    assert(!admin1.isAuthenticated());
    std::cout << "  ✓ Admin not authenticated initially" << std::endl;

    // Failed auth keeps unauthenticated
    admin1.authenticate("wrong", "wrong");
    assert(!admin1.isAuthenticated());
    std::cout << "  ✓ Failed auth keeps unauthenticated state" << std::endl;

    // Successful auth sets authenticated
    admin1.authenticate("admin", "admin123");
    assert(admin1.isAuthenticated());
    std::cout << "  ✓ Successful auth sets authenticated state" << std::endl;

    // A fresh admin instance is not authenticated
    Admin admin2(&inv);
    assert(!admin2.isAuthenticated());
    std::cout << "  ✓ New Admin instance starts unauthenticated" << std::endl;

    // Multiple failed attempts before success
    Admin admin3(&inv);
    admin3.authenticate("admin", "wrong1");
    admin3.authenticate("admin", "wrong2");
    admin3.authenticate("admin", "wrong3");
    assert(!admin3.isAuthenticated());
    admin3.authenticate("admin", "admin123");
    assert(admin3.isAuthenticated());
    std::cout << "  ✓ Auth succeeds after multiple failed attempts" << std::endl;
}

// **Validates: Requirements 18.8**
// Property 52: Admin Inventory Persistence
// Changes made via admin operations persist in inventory (reflected in subsequent findProduct calls).
void testProperty52() {
    std::cout << "\n=== Property 52: Admin Inventory Persistence ===" << std::endl;
    std::cout << "**Validates: Requirements 18.8**\n" << std::endl;

    Inventory inv;
    Admin admin(&inv);
    admin.authenticate("admin", "admin123");

    // Add product and verify it persists
    inv.addProduct(ProductFactory::createElectronics(101, "Laptop", 45000.0, 10, "Dell", 2));
    Product* p = inv.findProduct(101);
    assert(p != nullptr);
    std::cout << "  ✓ Added product persists in inventory" << std::endl;

    // Update price and verify it persists
    inv.updatePrice(101, 42000.0);
    p = inv.findProduct(101);
    assert(p != nullptr);
    assert(p->getPrice() == 42000.0);
    std::cout << "  ✓ Price update persists in inventory" << std::endl;

    // Update stock and verify it persists
    inv.updateStock(101, 15);
    p = inv.findProduct(101);
    assert(p != nullptr);
    assert(p->getQuantityAvailable() == 15);
    std::cout << "  ✓ Stock update persists in inventory" << std::endl;

    // Add more products and verify all persist
    inv.addProduct(ProductFactory::createClothing(201, "T-Shirt", 800.0, 25, "L", "Cotton"));
    inv.addProduct(ProductFactory::createBook(301, "C++ Book", 650.0, 30, "Author", "Programming"));
    assert(inv.getProducts().size() == 3);
    assert(inv.findProduct(201) != nullptr);
    assert(inv.findProduct(301) != nullptr);
    std::cout << "  ✓ Multiple additions persist in inventory" << std::endl;

    // Remove product and verify it no longer persists
    inv.removeProduct(201);
    assert(inv.findProduct(201) == nullptr);
    assert(inv.getProducts().size() == 2);
    std::cout << "  ✓ Removal persists (product no longer findable)" << std::endl;

    // Verify remaining products still accessible
    assert(inv.findProduct(101) != nullptr);
    assert(inv.findProduct(301) != nullptr);
    std::cout << "  ✓ Remaining products still accessible after removal" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "     Admin Operations Property Tests" << std::endl;
    std::cout << "========================================" << std::endl;

    testProperty33();
    testProperty34();
    testProperty35();
    testProperty36();
    testProperty37();
    testProperty51();
    testProperty52();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  ✅ All admin property tests passed!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
