#ifndef BOOK_H
#define BOOK_H

#include "Product.h"
#include <string>
#include <memory>

/**
 * Book product specialization.
 * Extends Product with author and genre attributes.
 * Applies a 5% category discount on calculateDiscount().
 * Implements clone() for the Prototype design pattern — returns a deep copy
 * of this Book instance, used when adding products to the cart.
 */
class Book : public Product {
private:
    std::string author;
    std::string genre;
    
public:
    Book(int id, const std::string& name, double price, int qty,
         const std::string& author, const std::string& genre);
    
    void display() const override;
    double calculateDiscount() const override;
    std::string getCategory() const override;
    std::unique_ptr<Product> clone() const override;
    
    std::string getAuthor() const;
    std::string getGenre() const;
};

#endif // BOOK_H
