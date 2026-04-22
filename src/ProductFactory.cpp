#include "ProductFactory.h"
#include <stdexcept>

std::unique_ptr<Product> ProductFactory::createProduct(
    ProductType type,
    int id,
    const std::string& name,
    double price,
    int quantity,
    const std::map<std::string, std::string>& attributes
) {
    switch (type) {
        case ProductType::ELECTRONICS: {
            auto brandIt = attributes.find("brand");
            auto warrantyIt = attributes.find("warranty");
            
            if (brandIt == attributes.end() || warrantyIt == attributes.end()) {
                throw std::invalid_argument("Electronics requires 'brand' and 'warranty' attributes");
            }
            
            int warranty = std::stoi(warrantyIt->second);
            return std::make_unique<Electronics>(id, name, price, quantity, brandIt->second, warranty);
        }
        
        case ProductType::CLOTHING: {
            auto sizeIt = attributes.find("size");
            auto fabricIt = attributes.find("fabric");
            
            if (sizeIt == attributes.end() || fabricIt == attributes.end()) {
                throw std::invalid_argument("Clothing requires 'size' and 'fabric' attributes");
            }
            
            return std::make_unique<Clothing>(id, name, price, quantity, sizeIt->second, fabricIt->second);
        }
        
        case ProductType::BOOK: {
            auto authorIt = attributes.find("author");
            auto genreIt = attributes.find("genre");
            
            if (authorIt == attributes.end() || genreIt == attributes.end()) {
                throw std::invalid_argument("Book requires 'author' and 'genre' attributes");
            }
            
            return std::make_unique<Book>(id, name, price, quantity, authorIt->second, genreIt->second);
        }
        
        default:
            throw std::invalid_argument("Unknown product type");
    }
}

std::unique_ptr<Product> ProductFactory::createElectronics(
    int id,
    const std::string& name,
    double price,
    int qty,
    const std::string& brand,
    int warranty
) {
    return std::make_unique<Electronics>(id, name, price, qty, brand, warranty);
}

std::unique_ptr<Product> ProductFactory::createClothing(
    int id,
    const std::string& name,
    double price,
    int qty,
    const std::string& size,
    const std::string& fabric
) {
    return std::make_unique<Clothing>(id, name, price, qty, size, fabric);
}

std::unique_ptr<Product> ProductFactory::createBook(
    int id,
    const std::string& name,
    double price,
    int qty,
    const std::string& author,
    const std::string& genre
) {
    return std::make_unique<Book>(id, name, price, qty, author, genre);
}
