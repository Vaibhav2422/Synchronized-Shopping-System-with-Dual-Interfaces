#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <string>
#include "Inventory.h"
#include "Cart.h"
#include "OrderHistory.h"
#include "Admin.h"
#include "Coupon.h"
#include "Transaction.h"
#include "Version.h"

/**
 * UIManager orchestrates all user-facing menus and interactions.
 * Holds non-owning pointers to the core domain objects and routes
 * menu selections to the appropriate handler methods.
 *
 * DIP compliance: Admin is injected as a dependency rather than
 * instantiated internally, so UIManager depends on the Admin
 * abstraction rather than constructing a concrete instance.
 *
 * Requirements: 10.1–10.8, 11.2–11.5, 18.2, 22.1–22.5
 */
class UIManager {
private:
    Inventory* inventory;
    Cart* cart;
    OrderHistory* orderHistory;
    Admin* admin;   // Non-owning pointer — injected for DIP compliance
    Coupon coupon;

public:
    /**
     * Constructs UIManager with all required dependencies injected.
     * @param inv  Non-owning pointer to the product inventory
     * @param c    Non-owning pointer to the shopping cart
     * @param oh   Non-owning pointer to the order history store
     * @param adm  Non-owning pointer to the admin panel (DIP: injected, not created here)
     */
    UIManager(Inventory* inv, Cart* c, OrderHistory* oh, Admin* adm);

    void run();

private:
    void displayMainMenu();
    void handleViewProducts();
    void handleAddToCart();
    void handleRemoveFromCart();
    void handleViewCart();
    void handleApplyCoupon();
    void handleCheckout();
    void handleSearchProducts();
    void handleFilterProducts();
    void handleViewOrderHistory();
    void handleAdminPanel();
    void handleAbout();

    void clearScreen();
    void displayHeader(const std::string& title);
    void displayLoadingAnimation();
    void waitForEnter();
};

#endif // UIMANAGER_H
