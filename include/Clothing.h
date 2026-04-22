#ifndef CLOTHING_H
#define CLOTHING_H

#include "Product.h"
#include <string>
#include <memory>

/**
 * Clothing product specialization.
 * Extends Product with size and fabric attributes.
 * Applies a 20% category discount on calculateDiscount().
 * Implements clone() for the Prototype design pattern — returns a deep copy
 * of this Clothing instance, used when adding products to the cart.
 */
class Clothing : public Product {
private:
    std::string size;
    std::string fabric;
    
public:
    Clothing(int id, const std::string& name, double price, int qty,
             const std::string& size, const std::string& fabric);
    
    void display() const override;
    double calculateDiscount() const override;
    std::string getCategory() const override;
    std::unique_ptr<Product> clone() const override;
    
    std::string getSize() const;
    std::string getFabric() const;
};

#endif // CLOTHING_H
