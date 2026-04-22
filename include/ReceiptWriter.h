#ifndef RECEIPT_WRITER_H
#define RECEIPT_WRITER_H

#include <string>
#include "Order.h"
#include "Bill.h"

/**
 * ReceiptWriter is a static utility class for generating and saving receipts.
 * No instances are needed — all methods are static.
 */
class ReceiptWriter {
public:
    // Prevent instantiation
    ReceiptWriter() = delete;

    /**
     * Generates a formatted receipt string from an order and bill.
     * Includes ASCII borders, transaction details, item list, and bill summary.
     * Optionally includes customer name and ID in the header.
     *
     * @param order    The completed order
     * @param bill     The calculated bill
     * @param custName Optional customer name (empty = guest)
     * @param custId   Optional customer ID (empty = guest)
     * @return A formatted receipt string
     */
    static std::string generateReceipt(const Order& order, const Bill& bill,
                                       const std::string& custName = "",
                                       const std::string& custId   = "");

    /**
     * Saves receipt content to data/receipts/receipt_XXXX.txt.
     * Returns true on success, throws FileWriteException on failure.
     *
     * @param content  The receipt string to write
     * @param orderId  The order ID (used for filename, zero-padded to 4 digits)
     * @return true on success
     */
    static bool saveReceipt(const std::string& content, int orderId);

    /**
     * Returns the receipt filename for a given order ID.
     * Format: "data/receipts/receipt_XXXX.txt" (zero-padded 4-digit ID)
     *
     * @param orderId The order ID
     * @return The receipt file path
     */
    static std::string getReceiptFilename(int orderId);
};

#endif // RECEIPT_WRITER_H
