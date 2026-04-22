#ifndef PRODUCTFACTORY_H
#define PRODUCTFACTORY_H

#include "Product.h"
#include "Electronics.h"
#include "Clothing.h"
#include "Book.h"
#include <memory>
#include <string>
#include <map>

/**
 * ProductFactory implements the Factory design pattern for product creation.
 * Centralizes construction logic so client code depends only on the Product
 * interface, not on concrete subclasses.
 * Requirements: 1.1, 2.1, 2.2, 2.3
 */
class ProductFactory {
public:
    enum class ProductType {
        ELECTRONICS,
        CLOTHING,
        BOOK
    };
    
    // Main factory method with attributes map
    static std::unique_ptr<Product> createProduct(
        ProductType type,
        int id,
        const std::string& name,
        double price,
        int quantity,
        const std::map<std::string, std::string>& attributes
    );
    
    // Convenience methods for specific product types
    static std::unique_ptr<Product> createElectronics(
        int id,
        const std::string& name,
        double price,
        int qty,
        const std::string& brand,
        int warranty
    );
    
    static std::unique_ptr<Product> createClothing(
        int id,
        const std::string& name,
        double price,
        int qty,
        const std::string& size,
        const std::string& fabric
    );
    
    static std::unique_ptr<Product> createBook(
        int id,
        const std::string& name,
        double price,
        int qty,
        const std::string& author,
        const std::string& genre
    );
};

#endif // PRODUCTFACTORY_H
