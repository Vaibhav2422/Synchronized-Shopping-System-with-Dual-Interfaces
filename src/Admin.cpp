#include "Admin.h"
#include "ANSIColors.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "ProductFactory.h"
#include <iostream>
#include <limits>
#include <stdexcept>

Admin::Admin(Inventory* inv)
    : username(""), password(""), inventory(inv), authenticated(false) {}

bool Admin::authenticate(const std::string& user, const std::string& pass) {
    if (user == ConfigManager::getInstance().getAdminUsername() &&
        pass == ConfigManager::getInstance().getAdminPassword()) {
        authenticated = true;
        username = user;
        Logger::logInfo("Admin", "Authentication successful for user: " + user);
        return true;
    }
    authenticated = false;
    Logger::logWarning("Admin", "Authentication failed for user: " + user);
    return false;
}

bool Admin::isAuthenticated() const {
    return authenticated;
}

void Admin::showAdminMenu() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << std::endl;
    std::cout << "  ╔══════════════════════════════════╗" << std::endl;
    std::cout << "  ║        ADMIN PANEL MENU          ║" << std::endl;
    std::cout << "  ╚══════════════════════════════════╝" << std::endl;
    std::cout << ANSIColors::RESET;
    std::cout << ANSIColors::CYAN;
    std::cout << "  1. View Inventory" << std::endl;
    std::cout << "  2. Add New Product" << std::endl;
    std::cout << "  3. Update Product Price" << std::endl;
    std::cout << "  4. Update Product Stock" << std::endl;
    std::cout << "  5. Remove Product" << std::endl;
    std::cout << "  6. Exit Admin Panel" << std::endl;
    std::cout << ANSIColors::RESET;
    std::cout << ANSIColors::YELLOW << "  Enter choice: " << ANSIColors::RESET;
}

void Admin::viewInventory() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << "\n  === Current Inventory ===" << ANSIColors::RESET << std::endl;
    inventory->displayAllProducts();
    Logger::logInfo("Admin", "Viewed inventory");
}

void Admin::addNewProduct() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << "\n  === Add New Product ===" << ANSIColors::RESET << std::endl;
    std::cout << ANSIColors::CYAN << "  Product Type:" << ANSIColors::RESET << std::endl;
    std::cout << ANSIColors::CYAN << "  1. Electronics" << ANSIColors::RESET << std::endl;
    std::cout << ANSIColors::CYAN << "  2. Clothing" << ANSIColors::RESET << std::endl;
    std::cout << ANSIColors::CYAN << "  3. Book" << ANSIColors::RESET << std::endl;
    std::cout << ANSIColors::YELLOW << "  Enter type: " << ANSIColors::RESET;

    int typeChoice = 0;
    std::cin >> typeChoice;
    if (std::cin.fail() || typeChoice < 1 || typeChoice > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid product type." << ANSIColors::RESET << std::endl;
        Logger::logWarning("Admin", "Invalid product type entered");
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int id = 0;
    std::cout << ANSIColors::YELLOW << "  Product ID: " << ANSIColors::RESET;
    std::cin >> id;
    if (std::cin.fail() || id <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid product ID." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string name;
    std::cout << ANSIColors::YELLOW << "  Product Name: " << ANSIColors::RESET;
    std::getline(std::cin, name);
    if (name.empty()) {
        std::cout << ANSIColors::RED << "  Product name cannot be empty." << ANSIColors::RESET << std::endl;
        return;
    }

    double price = 0.0;
    std::cout << ANSIColors::YELLOW << "  Price: " << ANSIColors::RESET;
    std::cin >> price;
    if (std::cin.fail() || price <= 0.0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid price." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int qty = 0;
    std::cout << ANSIColors::YELLOW << "  Quantity: " << ANSIColors::RESET;
    std::cin >> qty;
    if (std::cin.fail() || qty < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid quantity." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
        std::unique_ptr<Product> product;

        if (typeChoice == 1) {
            // Electronics: prompt for brand and warranty period
            std::string brand;
            int warranty = 0;
            std::cout << ANSIColors::YELLOW << "  Brand: " << ANSIColors::RESET;
            std::getline(std::cin, brand);
            std::cout << ANSIColors::YELLOW << "  Warranty (years): " << ANSIColors::RESET;
            std::cin >> warranty;
            if (std::cin.fail() || warranty < 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ANSIColors::RED << "  Invalid warranty." << ANSIColors::RESET << std::endl;
                return;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            product = ProductFactory::createElectronics(id, name, price, qty, brand, warranty);

        } else if (typeChoice == 2) {
            // Clothing: prompt for size and fabric type
            std::string size, fabric;
            std::cout << ANSIColors::YELLOW << "  Size: " << ANSIColors::RESET;
            std::getline(std::cin, size);
            std::cout << ANSIColors::YELLOW << "  Fabric: " << ANSIColors::RESET;
            std::getline(std::cin, fabric);
            product = ProductFactory::createClothing(id, name, price, qty, size, fabric);

        } else {
            // Book: prompt for author and genre
            std::string author, genre;
            std::cout << ANSIColors::YELLOW << "  Author: " << ANSIColors::RESET;
            std::getline(std::cin, author);
            std::cout << ANSIColors::YELLOW << "  Genre: " << ANSIColors::RESET;
            std::getline(std::cin, genre);
            product = ProductFactory::createBook(id, name, price, qty, author, genre);
        }

        inventory->addProduct(std::move(product));
        std::cout << ANSIColors::GREEN << "  Product added successfully!" << ANSIColors::RESET << std::endl;
        Logger::logInfo("Admin", "Added new product ID=" + std::to_string(id) + " Name=" + name);

    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "  Error adding product: " << e.what() << ANSIColors::RESET << std::endl;
        Logger::logError("Admin", std::string("Failed to add product: ") + e.what());
    }
}

void Admin::updateProductPrice() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << "\n  === Update Product Price ===" << ANSIColors::RESET << std::endl;

    int id = 0;
    std::cout << ANSIColors::YELLOW << "  Product ID: " << ANSIColors::RESET;
    std::cin >> id;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid product ID." << ANSIColors::RESET << std::endl;
        return;
    }

    double newPrice = 0.0;
    std::cout << ANSIColors::YELLOW << "  New Price: " << ANSIColors::RESET;
    std::cin >> newPrice;
    if (std::cin.fail() || newPrice <= 0.0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid price. Must be positive." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (inventory->updatePrice(id, newPrice)) {
        std::cout << ANSIColors::GREEN << "  Price updated successfully!" << ANSIColors::RESET << std::endl;
        Logger::logInfo("Admin", "Updated price for product ID=" + std::to_string(id)
                        + " to " + std::to_string(newPrice));
    } else {
        std::cout << ANSIColors::RED << "  Product not found." << ANSIColors::RESET << std::endl;
        Logger::logWarning("Admin", "Product not found for price update, ID=" + std::to_string(id));
    }
}

void Admin::updateProductStock() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << "\n  === Update Product Stock ===" << ANSIColors::RESET << std::endl;

    int id = 0;
    std::cout << ANSIColors::YELLOW << "  Product ID: " << ANSIColors::RESET;
    std::cin >> id;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid product ID." << ANSIColors::RESET << std::endl;
        return;
    }

    int newQty = 0;
    std::cout << ANSIColors::YELLOW << "  New Quantity: " << ANSIColors::RESET;
    std::cin >> newQty;
    if (std::cin.fail() || newQty < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid quantity. Must be non-negative." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (inventory->updateStock(id, newQty)) {
        std::cout << ANSIColors::GREEN << "  Stock updated successfully!" << ANSIColors::RESET << std::endl;
        Logger::logInfo("Admin", "Updated stock for product ID=" + std::to_string(id)
                        + " to " + std::to_string(newQty));
    } else {
        std::cout << ANSIColors::RED << "  Product not found." << ANSIColors::RESET << std::endl;
        Logger::logWarning("Admin", "Product not found for stock update, ID=" + std::to_string(id));
    }
}

void Admin::removeProduct() {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD << "\n  === Remove Product ===" << ANSIColors::RESET << std::endl;

    int id = 0;
    std::cout << ANSIColors::YELLOW << "  Product ID to remove: " << ANSIColors::RESET;
    std::cin >> id;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << ANSIColors::RED << "  Invalid product ID." << ANSIColors::RESET << std::endl;
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (inventory->removeProduct(id)) {
        std::cout << ANSIColors::GREEN << "  Product removed successfully!" << ANSIColors::RESET << std::endl;
        Logger::logInfo("Admin", "Removed product ID=" + std::to_string(id));
    } else {
        std::cout << ANSIColors::RED << "  Product not found." << ANSIColors::RESET << std::endl;
        Logger::logWarning("Admin", "Product not found for removal, ID=" + std::to_string(id));
    }
}

void Admin::run() {
    if (!authenticated) {
        std::cout << ANSIColors::RED << "  Access denied. Please authenticate first." << ANSIColors::RESET << std::endl;
        Logger::logWarning("Admin", "Attempted to run admin panel without authentication");
        return;
    }

    Logger::logInfo("Admin", "Admin panel session started");
    int choice = 0;

    do {
        showAdminMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "  Invalid input. Please enter a number." << ANSIColors::RESET << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: viewInventory();       break;
            case 2: addNewProduct();       break;
            case 3: updateProductPrice();  break;
            case 4: updateProductStock();  break;
            case 5: removeProduct();       break;
            case 6:
                std::cout << ANSIColors::GREEN << "  Exiting admin panel." << ANSIColors::RESET << std::endl;
                Logger::logInfo("Admin", "Admin panel session ended");
                break;
            default:
                std::cout << ANSIColors::RED << "  Invalid choice. Please enter 1-6." << ANSIColors::RESET << std::endl;
                break;
        }
    } while (choice != 6);
}
