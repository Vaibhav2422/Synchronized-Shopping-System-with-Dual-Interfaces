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
                                           const std::string& custId) {
    std::ostringstream receipt;

    // Header border
    receipt << BORDER << "\n";
    receipt << "    SHOPPING MANAGEMENT SYSTEM - RECEIPT\n";
    receipt << BORDER << "\n";

    // Customer info (when linked)
    if (!custName.empty()) {
        receipt << "Customer      : " << custName << "\n";
        receipt << "Customer ID   : " << custId   << "\n";
    }

    // Order details
    receipt << "Order ID      : " << order.getOrderId() << "\n";
    receipt << "Date          : " << order.getOrderDate() << "\n";
    receipt << "Time          : " << order.getOrderTime() << "\n";
    receipt << "Payment Method: " << order.getPaymentMethod() << "\n";

    receipt << SEPARATOR << "\n";

    // Item list
    receipt << "Items Purchased:\n";
    for (const auto& item : order.getProducts()) {
        receipt << "  " << item.first << " x" << item.second << "\n";
    }

    receipt << SEPARATOR << "\n";

    // Bill summary
    receipt << bill.getSummary();

    // Footer
    receipt << BORDER << "\n";
    receipt << "         Thank you for shopping!\n";
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
