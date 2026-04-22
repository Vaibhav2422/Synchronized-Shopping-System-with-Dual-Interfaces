#include "Product.h"
#include <stdexcept>

Product::Product(int id, const std::string& n, double p, int q)
    : productId(id), name(n), price(p), quantityAvailable(q) {

    if (p < 0) {
        throw std::invalid_argument("Price cannot be negative");
    }

    if (q < 0) {
        throw std::invalid_argument("Quantity cannot be negative");
    }
}

int Product::getProductId() const {
    return productId;
}

std::string Product::getName() const {
    return name;
}

double Product::getPrice() const {
    return price;
}

int Product::getQuantityAvailable() const {
    return quantityAvailable;
}

void Product::setPrice(double newPrice) {
    if (newPrice < 0) {
        throw std::invalid_argument("Price cannot be negative");
    }
    price = newPrice;
}

void Product::decrementStock(int q) {
    if (q < 0) {
        throw std::invalid_argument("Quantity to decrement cannot be negative");
    }

    if (quantityAvailable < q) {
        throw std::runtime_error("Insufficient stock to decrement");
    }

    quantityAvailable -= q;
}

void Product::incrementStock(int q) {
    if (q < 0) {
        throw std::invalid_argument("Quantity to increment cannot be negative");
    }

    quantityAvailable += q;
}

bool Product::isAvailable(int q) const {
    return quantityAvailable >= q;
}