/**
 * Unit tests for the Transaction rollback mechanism.
 * Tests Requirements: 24.1 – 24.6
 */
#include "../include/Transaction.h"
#include "../include/Inventory.h"
#include "../include/Cart.h"
#include "../include/ProductFactory.h"
#include "../include/PaymentException.h"
#include <iostream>
#include <cassert>
#include <stdexcept>

// ── Helpers ───────────────────────────────────────────────────────────────────

static Inventory makeInventory() {
    Inventory inv;
    inv.addProduct(ProductFactory::createElectronics(1, "Laptop",  50000.0, 5, "Dell", 2));
    inv.addProduct(ProductFactory::createClothing   (2, "T-Shirt",   500.0, 10, "M", "Cotton"));
    inv.addProduct(ProductFactory::createBook       (3, "C++ Book",  800.0, 3, "Stroustrup", "Tech"));
    return inv;
}

// ── Test 1: Snapshot is captured correctly ────────────────────────────────────
void testSnapshotCapture() {
    std::cout << "Test 1: Snapshot capture..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(1, 2);  // 2 Laptops → inventory[1] = 3
    cart.addProduct(2, 1);  // 1 T-Shirt → inventory[2] = 9

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    assert(txn.hasSnapshot());
    std::cout << "  PASSED" << std::endl;
}

// ── Test 2: Rollback on payment failure (InsufficientFundsException) ──────────
void testRollbackOnInsufficientFunds() {
    std::cout << "Test 2: Rollback on insufficient funds..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(1, 1);  // 1 Laptop → inventory[1] qty becomes 4

    // Capture state: cart has 1 Laptop, inventory[1] = 4
    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    // Simulate payment failure
    try {
        throw InsufficientFundsException(50000.0, 100.0);
    } catch (const InsufficientFundsException& e) {
        txn.rollback(cart, inv, e.what());
    }

    // After rollback: cart should have 1 Laptop again, inventory[1] = 4
    assert(!cart.isEmpty());
    assert(cart.getItemCount() == 1);

    Product* laptop = inv.findProduct(1);
    assert(laptop != nullptr);
    assert(laptop->getQuantityAvailable() == 4);  // same as before rollback

    std::cout << "  PASSED" << std::endl;
}

// ── Test 3: Rollback on payment timeout ──────────────────────────────────────
void testRollbackOnPaymentTimeout() {
    std::cout << "Test 3: Rollback on payment timeout..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(2, 3);  // 3 T-Shirts → inventory[2] = 7

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    try {
        throw PaymentTimeoutException("UPI", 1500.0);
    } catch (const PaymentTimeoutException& e) {
        txn.rollback(cart, inv, e.what());
    }

    // Cart should be restored with 3 T-Shirts
    assert(!cart.isEmpty());
    assert(cart.getItemCount() == 3);

    Product* shirt = inv.findProduct(2);
    assert(shirt != nullptr);
    assert(shirt->getQuantityAvailable() == 7);

    std::cout << "  PASSED" << std::endl;
}

// ── Test 4: Rollback on invalid card ─────────────────────────────────────────
void testRollbackOnInvalidCard() {
    std::cout << "Test 4: Rollback on invalid card..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(3, 2);  // 2 Books → inventory[3] = 1

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    try {
        throw InvalidCardException("0000000000000000", 1600.0);
    } catch (const InvalidCardException& e) {
        txn.rollback(cart, inv, e.what());
    }

    // Cart should be restored with 2 Books
    assert(!cart.isEmpty());
    assert(cart.getItemCount() == 2);

    Product* book = inv.findProduct(3);
    assert(book != nullptr);
    assert(book->getQuantityAvailable() == 1);

    std::cout << "  PASSED" << std::endl;
}

// ── Test 5: Commit — no rollback after commit ─────────────────────────────────
void testCommitPreventsRollback() {
    std::cout << "Test 5: Commit marks transaction as done..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(1, 1);

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    txn.commit();
    assert(!txn.hasSnapshot());  // committed → hasSnapshot() returns false

    std::cout << "  PASSED" << std::endl;
}

// ── Test 6: Verify user can retry checkout after rollback ─────────────────────
void testRetryAfterRollback() {
    std::cout << "Test 6: User can retry checkout after rollback..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(1, 1);  // 1 Laptop

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    // Simulate failure and rollback
    txn.rollback(cart, inv, "simulated failure");

    // Cart is restored — user can attempt checkout again
    assert(!cart.isEmpty());
    assert(cart.getItemCount() == 1);

    // A second transaction can be created for the retry
    Transaction txn2;
    txn2.captureCartState(cart);
    txn2.captureInventoryState(inv);
    assert(txn2.hasSnapshot());

    txn2.commit();
    assert(!txn2.hasSnapshot());

    std::cout << "  PASSED" << std::endl;
}

// ── Test 7: Empty cart rollback is safe ──────────────────────────────────────
void testRollbackEmptyCart() {
    std::cout << "Test 7: Rollback with empty cart is safe..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    txn.rollback(cart, inv, "empty cart test");

    assert(cart.isEmpty());
    // Inventory quantities should be unchanged
    assert(inv.findProduct(1)->getQuantityAvailable() == 5);
    assert(inv.findProduct(2)->getQuantityAvailable() == 10);
    assert(inv.findProduct(3)->getQuantityAvailable() == 3);

    std::cout << "  PASSED" << std::endl;
}

// ── Test 8: Multi-item cart rollback ─────────────────────────────────────────
void testRollbackMultiItemCart() {
    std::cout << "Test 8: Multi-item cart rollback..." << std::endl;

    Inventory inv = makeInventory();
    Cart cart(&inv);
    cart.addProduct(1, 1);  // 1 Laptop  → inv[1] = 4
    cart.addProduct(2, 2);  // 2 T-Shirts → inv[2] = 8
    cart.addProduct(3, 1);  // 1 Book    → inv[3] = 2

    Transaction txn;
    txn.captureCartState(cart);
    txn.captureInventoryState(inv);

    // Simulate a generic exception during checkout
    try {
        throw std::runtime_error("receipt write failed");
    } catch (const std::exception& e) {
        txn.rollback(cart, inv, e.what());
    }

    // All items should be back in the cart
    assert(cart.getItemCount() == 4);  // 1+2+1

    // Inventory should be at snapshot values
    assert(inv.findProduct(1)->getQuantityAvailable() == 4);
    assert(inv.findProduct(2)->getQuantityAvailable() == 8);
    assert(inv.findProduct(3)->getQuantityAvailable() == 2);

    std::cout << "  PASSED" << std::endl;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Transaction Rollback Tests ===" << std::endl;

    testSnapshotCapture();
    testRollbackOnInsufficientFunds();
    testRollbackOnPaymentTimeout();
    testRollbackOnInvalidCard();
    testCommitPreventsRollback();
    testRetryAfterRollback();
    testRollbackEmptyCart();
    testRollbackMultiItemCart();

    std::cout << "\n=== All Transaction Tests Passed! ===" << std::endl;
    return 0;
}
