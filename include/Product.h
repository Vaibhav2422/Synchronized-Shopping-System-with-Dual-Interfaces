#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <memory>

/**
 * Abstract base class for all products in the catalog.
 * Cannot be instantiated directly — derived classes (Electronics, Clothing, Book)
 * must implement the four pure virtual methods: display(), calculateDiscount(),
 * getCategory(), and clone(). This enforces the OOP principle of abstraction
 * and enables runtime polymorphism through base-class pointers/references.
 *
 * OOP Principles demonstrated:
 *   - Abstraction:    Pure virtual interface prevents direct instantiation
 *   - Encapsulation:  Data members are protected (accessible to derived classes only)
 *   - Inheritance:    Electronics, Clothing, Book extend this class
 *   - Polymorphism:   Virtual functions dispatched at runtime via Product*
 *
 * Design Patterns:
 *   - Prototype:  clone() pure virtual method — each subclass returns a deep copy
 *                 of itself, enabling cart items to be independent of inventory.
 *
 * Requirements: 1.1, 1.5, 2.7, 12.1
 */
class Product {
protected:
    int productId;
    std::string name;
    double price;
    int quantityAvailable;
    
public:
    Product(int id, const std::string& name, double price, int qty);
    virtual ~Product() = default;
    
    // Pure virtual functions
    virtual void display() const = 0;
    virtual double calculateDiscount() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::unique_ptr<Product> clone() const = 0;
    
    // Getters
    int getProductId() const;
    std::string getName() const;
    double getPrice() const;
    int getQuantityAvailable() const;
    
    // Setters
    void setPrice(double newPrice);

    // Inventory management
    void decrementStock(int qty = 1);
    void incrementStock(int qty = 1);
    bool isAvailable(int qty = 1) const;
};

#endif // PRODUCT_H
