#include "Transaction.h"
#include "Logger.h"
#include <iostream>

void Transaction::captureCartState(const Cart& cart) {
    cartSnapshot.clear();
    for (const auto& item : cart.getItems()) {
        // Deep-copy each product via the virtual clone() method (Prototype pattern)
        cartSnapshot.emplace_back(item.product->clone(), item.quantity);
    }
}

void Transaction::captureInventoryState(const Inventory& inventory) {
    inventorySnapshot.clear();
    for (const auto& productPtr : inventory.getProducts()) {
        inventorySnapshot[productPtr->getProductId()] = productPtr->getQuantityAvailable();
    }
    snapshotTaken = true;
    committed = false;
    Logger::logInfo("Transaction", "Checkpoint captured: "
        + std::to_string(cartSnapshot.size()) + " cart item(s), "
        + std::to_string(inventorySnapshot.size()) + " inventory product(s)");
}

void Transaction::commit() {
    committed = true;
    Logger::logInfo("Transaction", "Transaction committed successfully");
}

void Transaction::rollback(Cart& cart, Inventory& inventory, const std::string& reason) {
    if (!snapshotTaken) {
        Logger::logWarning("Transaction", "rollback() called but no snapshot was taken");
        return;
    }

    Logger::logWarning("Transaction",
        "Rolling back transaction. Reason: " + (reason.empty() ? "unspecified" : reason));

    // ── Step 1: Clear the cart items.
    //    Cart::clear() calls incrementStock for each item, temporarily inflating
    //    inventory quantities. We will overwrite those with the snapshot values
    //    in the next step, so this is safe.
    cart.clear();

    // ── Step 2: Restore inventory quantities from snapshot.
    //    The snapshot was taken AFTER items were already in the cart, so the
    //    snapshot quantities already reflect the "items in cart" state (i.e.,
    //    inventory was decremented when items were added). After cart.clear()
    //    the inventory was incremented back. We now set inventory to the exact
    //    snapshot values to undo any changes made during the failed checkout.
    for (const auto& entry : inventorySnapshot) {
        int productId = entry.first;
        int savedQty  = entry.second;
        Product* p = inventory.findProduct(productId);
        if (p) {
            inventory.updateStock(productId, savedQty);
        }
    }

    // ── Step 3: Re-populate the cart from the snapshot.
    //    The inventory is now at snapshot values (items-in-cart state).
    //    We need to temporarily increase each product's stock by the snapshot
    //    quantity so that Cart::addProduct() can decrement it back, leaving
    //    inventory at the correct snapshot values.
    for (const auto& snap : cartSnapshot) {
        int productId = snap.product->getProductId();
        Product* p = inventory.findProduct(productId);
        if (p) {
            p->incrementStock(snap.quantity);
        }
    }

    for (const auto& snap : cartSnapshot) {
        cart.addProduct(snap.product->getProductId(), snap.quantity);
    }

    Logger::logInfo("Transaction", "Rollback complete: cart restored with "
        + std::to_string(cartSnapshot.size()) + " item type(s)");
}

bool Transaction::hasSnapshot() const {
    return snapshotTaken && !committed;
}
