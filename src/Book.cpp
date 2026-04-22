#include "Book.h"
#include "ANSIColors.h"
#include <iostream>
#include <iomanip>

Book::Book(int id, const std::string& n, double p, int q,
           const std::string& a, const std::string& g)
    : Product(id, n, p, q), author(a), genre(g) {
}

void Book::display() const {
    std::cout << ANSIColors::BG_BLACK << ANSIColors::CYAN
              << "ID: " << productId << " | "
              << "Name: " << name << " | "
              << "Price: Rs. " << std::fixed << std::setprecision(2) << price << " | "
              << "Author: " << author << " | "
              << "Genre: " << genre << " | "
              << "Available: " << quantityAvailable
              << ANSIColors::RESET << std::endl;
}

double Book::calculateDiscount() const {
    return price * 0.05;   // 5% discount
}

std::string Book::getCategory() const {
    return "Books";
}

std::unique_ptr<Product> Book::clone() const {
    return std::make_unique<Book>(*this);
}

std::string Book::getAuthor() const {
    return author;
}

std::string Book::getGenre() const {
    return genre;
}