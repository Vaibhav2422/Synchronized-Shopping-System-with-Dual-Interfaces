#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include "Inventory.h"

/**
 * Admin class for managing inventory through an authenticated admin panel.
 * Provides CRUD operations on products with credential-based access control.
 */
class Admin {
private:
    std::string username;
    std::string password;
    Inventory* inventory;  // Non-owning pointer
    bool authenticated;

    // Private admin operations (require authentication)
    void addNewProduct();
    void updateProductPrice();
    void updateProductStock();
    void removeProduct();
    void viewInventory();

public:
    explicit Admin(Inventory* inv);

    bool authenticate(const std::string& user, const std::string& pass);
    bool isAuthenticated() const;

    void showAdminMenu();
    void run();  // Main admin loop after authentication
};

#endif // ADMIN_H
