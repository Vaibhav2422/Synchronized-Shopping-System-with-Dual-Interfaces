#ifndef INVALID_INPUT_EXCEPTION_H
#define INVALID_INPUT_EXCEPTION_H

#include "ShoppingException.h"

/**
 * Base exception for user input validation errors.
 */
class InvalidInputException : public ShoppingException {
public:
    explicit InvalidInputException(const std::string& msg)
        : ShoppingException(msg, "INPUT_ERR") {}
};

/**
 * Exception thrown when an invalid product ID is provided.
 */
class InvalidProductIdException : public InvalidInputException {
private:
    int productId;
    
public:
    explicit InvalidProductIdException(int id)
        : InvalidInputException("Invalid product ID: " + std::to_string(id))
        , productId(id) {}
    
    int getProductId() const { return productId; }
};

/**
 * Exception thrown when an invalid quantity is provided.
 */
class InvalidQuantityException : public InvalidInputException {
private:
    int requestedQuantity;
    
public:
    explicit InvalidQuantityException(int qty)
        : InvalidInputException("Invalid quantity: " + std::to_string(qty))
        , requestedQuantity(qty) {}
    
    int getRequestedQuantity() const { return requestedQuantity; }
};

/**
 * Exception thrown when an invalid menu choice is provided.
 */
class InvalidMenuChoiceException : public InvalidInputException {
private:
    int choice;
    
public:
    explicit InvalidMenuChoiceException(int ch)
        : InvalidInputException("Invalid menu choice: " + std::to_string(ch))
        , choice(ch) {}
    
    int getChoice() const { return choice; }
};

#endif // INVALID_INPUT_EXCEPTION_H
