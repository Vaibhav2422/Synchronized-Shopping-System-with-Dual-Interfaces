/**
 * Shopping Management System - Entry Point
 * Requirements: 13.3, 14.1, 14.2, 14.3, 15.6, 19.6, 21.2, 21.5
 */

#include "Inventory.h"
#include "Cart.h"
#include "OrderHistory.h"
#include "Admin.h"
#include "UIManager.h"
#include "ANSIColors.h"
#include "Logger.h"
#include "ShoppingException.h"
#include "Version.h"

#include <iostream>

int main() {
    // Clear screen on startup
    ANSIColors::clearScreen();

    // Display version banner (Requirement 25.1, 25.2, 25.3)
    Version::displayBanner();

    try {
        // Initialize core components
        Inventory inventory;
        inventory.initializeSampleData();

        Cart cart(&inventory);

        OrderHistory orderHistory("data/orders.txt");
        orderHistory.loadFromFile();

        // DIP: Admin is constructed here (composition root) and injected into UIManager
        Admin admin(&inventory);

        // Launch UI — all dependencies injected from the composition root
        UIManager ui(&inventory, &cart, &orderHistory, &admin);
        ui.run();

        // On clean exit: persist orders and show analytics
        orderHistory.saveToFile();
        orderHistory.displaySalesAnalytics();

        Logger::logInfo("Main", "Application exited normally");
        return 0;

    } catch (const ShoppingException& ex) {
        std::cerr << ANSIColors::RED
                  << "[FATAL] " << ex.getDetailedMessage()
                  << ANSIColors::RESET << std::endl;
        Logger::logError("Main", std::string("Fatal ShoppingException: ") + ex.what());
        return 1;

    } catch (const std::exception& ex) {
        std::cerr << ANSIColors::RED
                  << "[FATAL] Unexpected error: " << ex.what()
                  << ANSIColors::RESET << std::endl;
        Logger::logError("Main", std::string("Fatal std::exception: ") + ex.what());
        return 1;
    }
}
