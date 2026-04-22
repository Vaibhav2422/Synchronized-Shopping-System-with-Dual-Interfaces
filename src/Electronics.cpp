#include "Electronics.h"
#include "ANSIColors.h"
#include <iostream>
#include <iomanip>

Electronics::Electronics(int id, const std::string& n, double p, int q,
                         const std::string& b, int w)
    : Product(id, n, p, q), brand(b), warrantyYears(w) {
}

void Electronics::display() const {
    std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN
              << "ID: " << productId << " | "
              << "Name: " << name << " | "
              << "Price: Rs. " << std::fixed << std::setprecision(2) << price << " | "
              << "Brand: " << brand << " | "
              << "Warranty: " << warrantyYears << " year(s) | "
              << "Available: " << quantityAvailable
              << ANSIColors::RESET << std::endl;
}

double Electronics::calculateDiscount() const {
    return price * 0.10;   // 10% discount
}

std::string Electronics::getCategory() const {
    return "Electronics";
}

std::unique_ptr<Product> Electronics::clone() const {
    return std::make_unique<Electronics>(*this);
}

std::string Electronics::getBrand() const {
    return brand;
}

int Electronics::getWarrantyYears() const {
    return warrantyYears;
}