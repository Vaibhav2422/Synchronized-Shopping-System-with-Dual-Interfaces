#ifndef STOCK_EXCEPTION_H
#define STOCK_EXCEPTION_H

#include "ShoppingException.h"

/**
 * Base exception for inventory availability errors.
 */
class OutOfStockException : public ShoppingException {
protected:
    std::string productName;
    int availableQuantity;
    
public:
    OutOfStockException(const std::string& name, int available)
        : ShoppingException("Product out of stock: " + name, "STOCK_ERR")
        , productName(name)
        , availableQuantity(available) {}
    
    std::string getProductName() const { return productName; }
    int getAvailableQuantity() const { return availableQuantity; }
};

/**
 * Exception thrown when a product is completely unavailable (zero stock).
 */
class ProductUnavailableException : public OutOfStockException {
public:
    explicit ProductUnavailableException(const std::string& name)
        : OutOfStockException(name, 0) {}
};

/**
 * Exception thrown when requested quantity exceeds available stock.
 */
class InsufficientStockException : public OutOfStockException {
private:
    int requestedQuantity;
    
public:
    InsufficientStockException(const std::string& name, int requested, int available)
        : OutOfStockException(name, available)
        , requestedQuantity(requested) {}
    
    int getRequestedQuantity() const { return requestedQuantity; }
    
    std::string getDetailedMessage() const override {
        return "[STOCK_ERR] Insufficient stock for " + productName + 
               ". Requested: " + std::to_string(requestedQuantity) + 
               ", Available: " + std::to_string(availableQuantity);
    }
};

#endif // STOCK_EXCEPTION_H
