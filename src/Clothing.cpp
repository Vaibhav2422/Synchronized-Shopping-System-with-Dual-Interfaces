#include "Clothing.h"
#include "ANSIColors.h"
#include <iostream>
#include <iomanip>

Clothing::Clothing(int id, const std::string& n, double p, int q,
                   const std::string& s, const std::string& f)
    : Product(id, n, p, q), size(s), fabric(f) {
}

void Clothing::display() const {
    std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN
              << "ID: " << productId << " | "
              << "Name: " << name << " | "
              << "Price: Rs. " << std::fixed << std::setprecision(2) << price << " | "
              << "Size: " << size << " | "
              << "Fabric: " << fabric << " | "
              << "Available: " << quantityAvailable
              << ANSIColors::RESET << std::endl;
}

double Clothing::calculateDiscount() const {
    return price * 0.20;   // 20% discount
}

std::string Clothing::getCategory() const {
    return "Clothing";
}

std::unique_ptr<Product> Clothing::clone() const {
    return std::make_unique<Clothing>(*this);
}

std::string Clothing::getSize() const {
    return size;
}

std::string Clothing::getFabric() const {
    return fabric;
}