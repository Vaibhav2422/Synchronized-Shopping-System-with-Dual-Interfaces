#ifndef SHOPPING_EXCEPTION_H
#define SHOPPING_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <chrono>
#include <ctime>

/**
 * Base exception class for all shopping system errors.
 * Extends std::runtime_error and adds error codes and timestamps.
 */
class ShoppingException : public std::runtime_error {
protected:
    std::string errorCode;
    std::chrono::system_clock::time_point timestamp;
    
public:
    explicit ShoppingException(const std::string& msg, const std::string& code = "SHOP_ERR") 
        : std::runtime_error(msg)
        , errorCode(code)
        , timestamp(std::chrono::system_clock::now()) {}
    
    virtual ~ShoppingException() = default;
    
    std::string getErrorCode() const { return errorCode; }
    
    std::string getTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::string timeStr = std::ctime(&time);
        // Remove trailing newline from ctime
        if (!timeStr.empty() && timeStr.back() == '\n') {
            timeStr.pop_back();
        }
        return timeStr;
    }
    
    virtual std::string getDetailedMessage() const {
        return "[" + errorCode + "] " + what();
    }
};

#endif // SHOPPING_EXCEPTION_H
