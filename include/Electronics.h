#ifndef ELECTRONICS_H
#define ELECTRONICS_H

#include "Product.h"
#include <string>
#include <memory>

/**
 * Electronics product specialization.
 * Extends Product with brand and warrantyYears attributes.
 * Applies a 10% category discount on calculateDiscount().
 * Implements clone() for the Prototype design pattern — returns a deep copy
 * of this Electronics instance, used when adding products to the cart.
 */
class Electronics : public Product {
private:
    std::string brand;
    int warrantyYears;
    
public:
    Electronics(int id, const std::string& name, double price, int qty,
                const std::string& brand, int warranty);
    
    void display() const override;
    double calculateDiscount() const override;
    std::string getCategory() const override;
    std::unique_ptr<Product> clone() const override;
    
    std::string getBrand() const;
    int getWarrantyYears() const;
};

#endif // ELECTRONICS_H
