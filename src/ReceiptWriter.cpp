#include "ReceiptWriter.h"
#include "FileIOException.h"
#include "InputValidator.h"
#include "Version.h"
#include <fstream>
#include <sstream>
#include <iomanip>

// SECURITY: Set file permissions after creation (Requirement 27.5).
// Receipts use mode 644 (owner read/write, group/others read-only).
// On Windows, NTFS ACLs control access; no action is taken here.
#ifndef _WIN32
#include <sys/stat.h>
static void setReceiptPermissions(const std::string& path) {
    chmod(path.c_str(), 0644);
}
#else
static void setReceiptPermissions(const std::string&) {
    // Windows: rely on NTFS ACLs configured by the administrator.
}
#endif

static const int BORDER_WIDTH = 60;
static const std::string BORDER(BORDER_WIDTH, '=');
static const std::string SEPARATOR(BORDER_WIDTH, '-');

std::string ReceiptWriter::getReceiptFilename(int orderId) {
    std::ostringstream oss;
    oss << "data/receipts/receipt_" << std::setw(4) << std::setfill('0') << orderId << ".txt";
    return oss.str();
}

std::string ReceiptWriter::generateReceipt(const Order& order, const Bill& bill,
                                           const std::string& custName,
                                           const std::string& custId,
                                           const PaymentDetails& pd) {
    std::ostringstream receipt;
    receipt << std::fixed << std::setprecision(2);

    // Header
    receipt << BORDER << "\n";
    receipt << "    SHOPPING MANAGEMENT SYSTEM - RECEIPT\n";
    receipt << BORDER << "\n";

    // Customer info
    if (!custName.empty()) {
        receipt << "Customer      : " << custName << "\n";
        receipt << "Customer ID   : " << custId   << "\n";
        if (!pd.phone.empty())
            receipt << "Mobile        : " << pd.phone << "\n";
    } else {
        receipt << "Customer      : Guest\n";
    }

    // Order details
    receipt << "Order ID      : #" << std::setw(4) << std::setfill('0') << order.getOrderId() << "\n";
    receipt << std::setfill(' ');
    receipt << "Date          : " << order.getOrderDate() << "\n";
    receipt << "Time          : " << order.getOrderTime() << "\n";

    receipt << SEPARATOR << "\n";

    // Payment details
    receipt << "PAYMENT DETAILS\n";
    receipt << "Method        : " << order.getPaymentMethod() << "\n";
    if (pd.method == "UPI" && !pd.upiId.empty()) {
        receipt << "UPI ID        : " << pd.upiId << "\n";
    } else if (pd.method == "Card") {
        if (!pd.cardHolder.empty())
            receipt << "Card Holder   : " << pd.cardHolder << "\n";
        if (!pd.cardNumber.empty()) {
            // Mask: show only last 4 digits
            std::string masked = "XXXX-XXXX-XXXX-";
            masked += pd.cardNumber.size() >= 4
                      ? pd.cardNumber.substr(pd.cardNumber.size() - 4)
                      : pd.cardNumber;
            receipt << "Card Number   : " << masked << "\n";
        }
    } else if (pd.method == "Cash") {
        receipt << "Cash Tendered : Rs." << pd.cashTendered << "\n";
        receipt << "Change        : Rs." << pd.cashChange   << "\n";
    }

    receipt << SEPARATOR << "\n";

    // Item list
    receipt << "Items Purchased:\n";
    for (const auto& item : order.getProducts()) {
        receipt << "  " << item.first << " x" << item.second << "\n";
    }

    receipt << SEPARATOR << "\n";

    // Bill summary
    receipt << bill.getSummary();

    // Loyalty points
    if (pd.loyaltyEarned > 0 || pd.loyaltyTotal > 0) {
        receipt << SEPARATOR << "\n";
        receipt << "LOYALTY POINTS\n";
        if (pd.loyaltyEarned > 0)
            receipt << "  Points Earned  : +" << pd.loyaltyEarned << " pts\n";
        receipt << "  Total Balance  :  " << pd.loyaltyTotal  << " pts\n";
    }

    // Footer
    receipt << BORDER << "\n";
    receipt << "     *** THANK YOU FOR SHOPPING! ***\n";
    receipt << "  " << Version::PROJECT_NAME << " v" << Version::VERSION << "\n";
    receipt << BORDER << "\n";

    return receipt.str();
}

bool ReceiptWriter::saveReceipt(const std::string& content, int orderId) {
    std::string filename = getReceiptFilename(orderId);

    // SECURITY: Validate file path to prevent directory traversal (Requirement 27.3)
    if (!InputValidator::validateFilePath(filename)) {
        throw FileWriteException(filename);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw FileWriteException(filename);
    }

    file << content;

    if (file.fail()) {
        throw FileWriteException(filename);
    }

    // SECURITY: Set permissions (644) on the receipt file — owner can read/write,
    // group and others can only read (Requirement 27.5).
    file.close();
    setReceiptPermissions(filename);

    return true;
}
