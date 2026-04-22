#include "Payment.h"

Payment::Payment(double amt) : amount(amt) {}

double Payment::getAmount() const {
    return amount;
}
