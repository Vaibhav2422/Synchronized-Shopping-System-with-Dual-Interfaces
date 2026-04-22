#include "UIManager.h"
#include "Bill.h"
#include "Order.h"
#include "ReceiptWriter.h"
#include "Payment.h"
#include "UPIPayment.h"
#include "CardPayment.h"
#include "CashPayment.h"
#include "ANSIColors.h"
#include "Logger.h"
#include "ShoppingException.h"
#include "InvalidInputException.h"
#include "StockException.h"
#include "CouponException.h"
#include "PaymentException.h"
#include "Transaction.h"
#include "Version.h"
#include "InputValidator.h"

#include <iostream>
#include <limits>
#include <memory>
#include <chrono>
#include <string>

// MinGW on Windows may lack POSIX thread support; use platform sleep as fallback
#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
#include <windows.h>
static void platformSleep(int ms) { Sleep(static_cast<DWORD>(ms)); }
#else
#include <thread>
static void platformSleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif

UIManager::UIManager(Inventory* inv, Cart* c, OrderHistory* oh, Admin* adm)
    : inventory(inv), cart(c), orderHistory(oh), admin(adm) {}

void UIManager::run() {
    int choice = -1;
    while (true) {
        displayMainMenu();
        std::cout << ANSIColors::CYAN << ANSIColors::BG_BLACK
                  << "Enter your choice: " << ANSIColors::RESET;

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid input. Please enter a number.\n"
                      << ANSIColors::RESET;
            waitForEnter();
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            std::cout << ANSIColors::GREEN << "\nThank you for shopping! Goodbye.\n"
                      << ANSIColors::RESET;
            break;
        }

        try {
            if (choice < 0 || choice > 11) {
                throw InvalidMenuChoiceException(choice);
            }
            switch (choice) {
                case 1:  handleViewProducts();     break;
                case 2:  handleSearchProducts();   break;
                case 3:  handleFilterProducts();   break;
                case 4:  handleAddToCart();        break;
                case 5:  handleRemoveFromCart();   break;
                case 6:  handleViewCart();         break;
                case 7:  handleApplyCoupon();      break;
                case 8:  handleCheckout();         break;
                case 9:  handleViewOrderHistory(); break;
                case 10: handleAdminPanel();       break;
                case 11: handleAbout();            break;
                default: throw InvalidMenuChoiceException(choice);
            }
        } catch (const InvalidMenuChoiceException& e) {
            std::cout << ANSIColors::RED << "Error: " << e.what() << "\n"
                      << ANSIColors::RESET;
            Logger::logException("UIManager", e);
        } catch (const std::exception& e) {
            std::cout << ANSIColors::RED << "Unexpected error: " << e.what() << "\n"
                      << ANSIColors::RESET;
            Logger::logException("UIManager", e);
        }
        waitForEnter();
    }
}

void UIManager::displayMainMenu() {
    clearScreen();
    displayHeader("SHOPPING MANAGEMENT SYSTEM");
    std::cout << ANSIColors::CYAN << ANSIColors::BG_BLACK;
    std::cout << "  1. View Products\n";
    std::cout << "  2. Search Products\n";
    std::cout << "  3. Filter by Category\n";
    std::cout << "  4. Add to Cart\n";
    std::cout << "  5. Remove from Cart\n";
    std::cout << "  6. View Cart\n";
    std::cout << "  7. Apply Coupon\n";
    std::cout << "  8. Checkout\n";
    std::cout << "  9. View Order History\n";
    std::cout << " 10. Admin Panel\n";
    std::cout << " 11. About\n";
    std::cout << "  0. Exit\n";
    std::cout << ANSIColors::RESET;
    std::cout << "============================================================\n";
}

// ── Product viewing handlers ──────────────────────────────────────────────────

void UIManager::handleViewProducts() {
    try {
        clearScreen();
        displayHeader("ALL PRODUCTS");
        inventory->displayAllProducts();
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleViewProducts", e);
    }
}

void UIManager::handleSearchProducts() {
    try {
        clearScreen();
        displayHeader("SEARCH PRODUCTS");
        std::cout << ANSIColors::CYAN << "Enter search term: " << ANSIColors::RESET;
        std::string term;
        std::getline(std::cin, term);
        // SECURITY: Sanitize user input before processing (Requirement 27.2)
        term = InputValidator::sanitizeString(term);

        auto results = inventory->searchByName(term);
        if (results.empty()) {
            std::cout << ANSIColors::YELLOW << "No products found matching \"" << term << "\".\n"
                      << ANSIColors::RESET;
        } else {
            for (const auto* p : results) {
                p->display();
            }
        }
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleSearchProducts", e);
    }
}

void UIManager::handleFilterProducts() {
    try {
        clearScreen();
        displayHeader("FILTER BY CATEGORY");
        std::cout << ANSIColors::CYAN << "Enter category (Electronics/Clothing/Books): "
                  << ANSIColors::RESET;
        std::string category;
        std::getline(std::cin, category);
        // SECURITY: Sanitize user input before processing (Requirement 27.2)
        category = InputValidator::sanitizeString(category);

        auto results = inventory->filterByCategory(category);
        if (results.empty()) {
            std::cout << ANSIColors::YELLOW << "No products found in category \"" << category << "\".\n"
                      << ANSIColors::RESET;
        } else {
            for (const auto* p : results) {
                p->display();
            }
        }
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleFilterProducts", e);
    }
}

// ── Cart management handlers ──────────────────────────────────────────────────

void UIManager::handleAddToCart() {
    try {
        clearScreen();
        displayHeader("ADD TO CART");

        std::cout << ANSIColors::CYAN << "Enter product ID: " << ANSIColors::RESET;
        int id;
        if (!(std::cin >> id)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid product ID. Please enter a number.\n"
                      << ANSIColors::RESET;
            return;
        }
        // SECURITY: Validate product ID range before processing (Requirement 27.2)
        if (!InputValidator::validateProductId(id)) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid product ID. ID must be between 1 and 99999.\n"
                      << ANSIColors::RESET;
            return;
        }

        std::cout << ANSIColors::CYAN << "Enter quantity: " << ANSIColors::RESET;
        int qty;
        if (!(std::cin >> qty) || qty <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid quantity. Please enter a positive number.\n"
                      << ANSIColors::RESET;
            return;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (cart->addProduct(id, qty)) {
            std::cout << ANSIColors::GREEN << "Product added to cart successfully!\n"
                      << ANSIColors::RESET;
            Logger::logInfo("UIManager", "Product " + std::to_string(id) + " added to cart");
        } else {
            std::cout << ANSIColors::RED << "Failed to add product to cart.\n" << ANSIColors::RESET;
        }
    } catch (const OutOfStockException& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleAddToCart", e);
    } catch (const InvalidProductIdException& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleAddToCart", e);
    } catch (const ShoppingException& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleAddToCart", e);
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleAddToCart", e);
    }
}

void UIManager::handleRemoveFromCart() {
    try {
        clearScreen();
        displayHeader("REMOVE FROM CART");

        std::cout << ANSIColors::CYAN << "Enter product ID to remove: " << ANSIColors::RESET;
        int id;
        if (!(std::cin >> id)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid product ID. Please enter a number.\n"
                      << ANSIColors::RESET;
            return;
        }
        // SECURITY: Validate product ID range before processing (Requirement 27.2)
        if (!InputValidator::validateProductId(id)) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid product ID. ID must be between 1 and 99999.\n"
                      << ANSIColors::RESET;
            return;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (cart->removeProduct(id)) {
            std::cout << ANSIColors::GREEN << "Product removed from cart successfully!\n"
                      << ANSIColors::RESET;
            Logger::logInfo("UIManager", "Product " + std::to_string(id) + " removed from cart");
        } else {
            std::cout << ANSIColors::RED << "Product not found in cart.\n" << ANSIColors::RESET;
        }
    } catch (const ShoppingException& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleRemoveFromCart", e);
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleRemoveFromCart", e);
    }
}

void UIManager::handleViewCart() {
    try {
        clearScreen();
        displayHeader("YOUR CART");
        cart->displayCart();
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleViewCart", e);
    }
}

// ── Coupon handler ────────────────────────────────────────────────────────────

void UIManager::handleApplyCoupon() {
    try {
        clearScreen();
        displayHeader("APPLY COUPON");

        if (coupon.isApplied()) {
            std::cout << ANSIColors::YELLOW << "Coupon \"" << coupon.getCode()
                      << "\" is already applied.\n" << ANSIColors::RESET;
            return;
        }

        std::cout << ANSIColors::CYAN << "Enter coupon code: " << ANSIColors::RESET;
        std::string code;
        std::getline(std::cin, code);
        // SECURITY: Sanitize coupon code before validation (Requirement 27.2)
        code = InputValidator::sanitizeString(code);

        coupon.apply(code);
        std::cout << ANSIColors::GREEN << "Coupon \"" << code << "\" applied successfully! ("
                  << static_cast<int>(coupon.getDiscountPercentage() * 100) << "% off)\n"
                  << ANSIColors::RESET;
        Logger::logInfo("UIManager", "Coupon applied: " + code);
    } catch (const InvalidCouponException& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleApplyCoupon", e);
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleApplyCoupon", e);
    }
}

// ── Checkout handler ──────────────────────────────────────────────────────────

void UIManager::handleCheckout() {
    // ── Requirement 24: Transaction checkpoint ────────────────────────────────
    // Create a transaction checkpoint before any state-mutating operations so
    // that we can roll back cart and inventory if any step fails.
    Transaction txn;
    txn.captureCartState(*cart);
    txn.captureInventoryState(*inventory);

    try {
        clearScreen();
        displayHeader("CHECKOUT");

        if (cart->isEmpty()) {
            std::cout << ANSIColors::YELLOW << "Your cart is empty. Add products before checkout.\n"
                      << ANSIColors::RESET;
            return;
        }

        // Display cart and calculate bill
        cart->displayCart();

        Bill bill;
        bill.calculate(*cart, coupon.isApplied() ? &coupon : nullptr);
        bill.display();

        // Payment method selection
        std::cout << ANSIColors::CYAN << "\nSelect payment method:\n"
                  << "  1. UPI\n"
                  << "  2. Card\n"
                  << "  3. Cash\n"
                  << "Enter choice: " << ANSIColors::RESET;

        int payChoice;
        if (!(std::cin >> payChoice) || payChoice < 1 || payChoice > 3) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSIColors::RED << "Invalid payment choice.\n" << ANSIColors::RESET;
            return;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        double total = bill.getFinalTotal();
        std::unique_ptr<Payment> payment;

        if (payChoice == 1) {
            std::cout << ANSIColors::CYAN << "Enter UPI ID: " << ANSIColors::RESET;
            std::string upiId;
            std::getline(std::cin, upiId);
            // SECURITY: Sanitize UPI ID before use (Requirement 27.2)
            upiId = InputValidator::sanitizeString(upiId);
            payment = std::make_unique<UPIPayment>(total, upiId);

        } else if (payChoice == 2) {
            std::cout << ANSIColors::CYAN << "Enter card number: " << ANSIColors::RESET;
            std::string cardNum;
            std::getline(std::cin, cardNum);
            std::cout << ANSIColors::CYAN << "Enter card holder name: " << ANSIColors::RESET;
            std::string holderName;
            std::getline(std::cin, holderName);
            // SECURITY: Sanitize card holder name; card number digits are kept as-is
            // since they are only digits/hyphens (already safe), but we sanitize
            // the holder name which may contain arbitrary text (Requirement 27.2).
            holderName = InputValidator::sanitizeString(holderName);
            payment = std::make_unique<CardPayment>(total, cardNum, holderName);

        } else {
            std::cout << ANSIColors::CYAN << "Enter cash amount: " << ANSIColors::RESET;
            double cash;
            if (!(std::cin >> cash)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << ANSIColors::RED << "Invalid cash amount.\n" << ANSIColors::RESET;
                return;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            payment = std::make_unique<CashPayment>(total, cash);
        }

        displayLoadingAnimation();

        // ── Payment processing (may throw PaymentFailedException) ─────────────
        if (payment->pay()) {
            // Create order and save
            Order order(*cart, total, payment->getPaymentMethod());
            orderHistory->addOrder(order);
            orderHistory->saveToFile();

            // Generate receipt
            std::string receiptContent = ReceiptWriter::generateReceipt(order, bill);
            ReceiptWriter::saveReceipt(receiptContent, order.getOrderId());
            std::string filename = ReceiptWriter::getReceiptFilename(order.getOrderId());

            // ── All steps succeeded — commit the transaction ──────────────────
            txn.commit();

            std::cout << ANSIColors::GREEN << "\nPayment successful! Order #"
                      << order.getOrderId() << " placed.\n"
                      << "Receipt saved to: " << filename << "\n"
                      << ANSIColors::RESET;

            Logger::logInfo("UIManager", "Order " + std::to_string(order.getOrderId()) + " completed");

            // Clear cart and reset coupon
            cart->clear();
            coupon.reset();
        } else {
            // pay() returned false — roll back and let the user retry
            txn.rollback(*cart, *inventory, "Payment returned failure");
            std::cout << ANSIColors::RED
                      << "Payment failed. Your cart and inventory have been restored.\n"
                      << "Please try again.\n"
                      << ANSIColors::RESET;
        }
    } catch (const PaymentFailedException& e) {
        // ── Payment exception — roll back cart and inventory ──────────────────
        txn.rollback(*cart, *inventory, std::string("PaymentFailedException: ") + e.what());
        std::cout << ANSIColors::RED
                  << "Payment error: " << e.what() << "\n"
                  << "Your cart and inventory have been restored. You may retry checkout.\n"
                  << ANSIColors::RESET;
        Logger::logException("UIManager::handleCheckout", e);
    } catch (const ShoppingException& e) {
        txn.rollback(*cart, *inventory, std::string("ShoppingException: ") + e.what());
        std::cout << ANSIColors::RED
                  << "Checkout error: " << e.what() << "\n"
                  << "Your cart and inventory have been restored.\n"
                  << ANSIColors::RESET;
        Logger::logException("UIManager::handleCheckout", e);
    } catch (const std::exception& e) {
        txn.rollback(*cart, *inventory, std::string("std::exception: ") + e.what());
        std::cout << ANSIColors::RED
                  << "Unexpected error: " << e.what() << "\n"
                  << "Your cart and inventory have been restored.\n"
                  << ANSIColors::RESET;
        Logger::logException("UIManager::handleCheckout", e);
    }
}

// ── Order history and admin handlers ─────────────────────────────────────────

void UIManager::handleViewOrderHistory() {
    try {
        clearScreen();
        displayHeader("ORDER HISTORY");
        orderHistory->displayHistory();
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleViewOrderHistory", e);
    }
}

void UIManager::handleAdminPanel() {
    try {
        clearScreen();
        displayHeader("ADMIN PANEL");

        std::cout << ANSIColors::CYAN << "Username: " << ANSIColors::RESET;
        std::string username;
        std::getline(std::cin, username);

        std::cout << ANSIColors::CYAN << "Password: " << ANSIColors::RESET;
        std::string password;
        std::getline(std::cin, password);

        // DIP: use the injected Admin dependency rather than constructing one here
        if (admin->authenticate(username, password)) {
            admin->run();
        } else {
            std::cout << ANSIColors::RED << "Authentication failed. Invalid credentials.\n"
                      << ANSIColors::RESET;
            Logger::logError("UIManager", "Admin authentication failed for user: " + username);
        }
    } catch (const std::exception& e) {
        std::cout << ANSIColors::RED << "Error: " << e.what() << "\n" << ANSIColors::RESET;
        Logger::logException("UIManager::handleAdminPanel", e);
    }
}

void UIManager::handleAbout() {
    clearScreen();
    Version::displayAbout();
}

// ── UI utility methods ────────────────────────────────────────────────────────

void UIManager::clearScreen() {
    ANSIColors::clearScreen();
}

void UIManager::displayHeader(const std::string& title) {
    std::cout << ANSIColors::YELLOW << ANSIColors::BOLD;
    std::cout << "============================================================\n";
    // Center the title within 60 chars
    int padding = (60 - static_cast<int>(title.size())) / 2;
    if (padding < 0) padding = 0;
    std::cout << std::string(padding, ' ') << title << "\n";
    std::cout << "============================================================\n";
    std::cout << ANSIColors::RESET;
}

void UIManager::displayLoadingAnimation() {
    std::cout << ANSIColors::CYAN << "Processing";
    for (int i = 0; i < 3; ++i) {
        std::cout << "." << std::flush;
        platformSleep(300);
    }
    std::cout << ANSIColors::RESET << "\n";
}

void UIManager::waitForEnter() {
    std::cout << ANSIColors::WHITE << "\nPress Enter to continue..." << ANSIColors::RESET;
    // Handle any leftover newline in the buffer
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    std::cin.get();
}
