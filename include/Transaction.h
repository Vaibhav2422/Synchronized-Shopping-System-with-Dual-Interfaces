#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include <map>
#include <memory>
#include <string>
#include "Cart.h"
#include "Inventory.h"

/**
 * Transaction class implements a checkpoint/rollback mechanism for checkout.
 *
 * Before payment processing begins, the UIManager captures a snapshot of both
 * the cart state and the inventory quantities. If any step in the checkout
 * pipeline fails (payment, receipt generation, order save), rollback() restores
 * both the cart and the inventory to the captured state, ensuring atomicity.
 *
 * Design:
 *   - captureCartState()      – deep-copies each CartItem (product clone + qty)
 *   - captureInventoryState() – records productId → quantityAvailable for every product
 *   - commit()                – marks the transaction successful (no-op, discards snapshot)
 *   - rollback()              – restores cart and inventory from the captured snapshots
 *
 * Requirements: 24.1, 24.2, 24.3, 24.4, 24.5, 24.6
 */
class Transaction {
public:
    /**
     * Lightweight snapshot of a single cart item.
     * Stores a cloned product and the quantity at capture time.
     */
    struct CartSnapshot {
        std::unique_ptr<Product> product;  // deep copy via clone()
        int quantity;

        CartSnapshot(std::unique_ptr<Product> p, int q)
            : product(std::move(p)), quantity(q) {}

        // Move-only (unique_ptr is non-copyable)
        CartSnapshot(CartSnapshot&&) = default;
        CartSnapshot& operator=(CartSnapshot&&) = default;
        CartSnapshot(const CartSnapshot&) = delete;
        CartSnapshot& operator=(const CartSnapshot&) = delete;
    };

    Transaction() = default;
    ~Transaction() = default;

    // Non-copyable, movable
    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;
    Transaction(Transaction&&) = default;
    Transaction& operator=(Transaction&&) = default;

    /**
     * Capture a deep copy of the current cart state.
     * Each CartItem's product is cloned so the snapshot is independent.
     * @param cart  The cart whose state should be captured.
     */
    void captureCartState(const Cart& cart);

    /**
     * Capture the current inventory quantities for all products.
     * Stores a map of productId → quantityAvailable.
     * @param inventory  The inventory whose stock levels should be captured.
     */
    void captureInventoryState(const Inventory& inventory);

    /**
     * Mark the transaction as committed.
     * This is a no-op — the snapshots are simply discarded on destruction.
     * Call this after all checkout steps succeed.
     */
    void commit();

    /**
     * Restore cart and inventory to the captured state.
     * - Clears the cart (which would normally restore inventory, but we
     *   bypass that by directly setting inventory quantities from the snapshot).
     * - Re-adds each captured CartItem to the cart without touching inventory
     *   (inventory is restored separately from the inventory snapshot).
     * - Logs the rollback operation with the provided reason.
     *
     * @param cart       The cart to restore.
     * @param inventory  The inventory to restore.
     * @param reason     Human-readable reason for the rollback (for logging).
     */
    void rollback(Cart& cart, Inventory& inventory, const std::string& reason = "");

    /** Returns true if state has been captured and is ready for rollback. */
    bool hasSnapshot() const;

private:
    std::vector<CartSnapshot> cartSnapshot;
    std::map<int, int> inventorySnapshot;  // productId → quantity
    bool snapshotTaken = false;
    bool committed = false;
};

#endif // TRANSACTION_H
