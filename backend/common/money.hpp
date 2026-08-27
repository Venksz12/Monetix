#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>

namespace monetix {
struct Money {
    int64_t minor{0};
    std::string currency{"INR"};
    Money operator+(Money rhs) const {
        if (currency != rhs.currency) throw std::invalid_argument("currency mismatch");
        return {minor + rhs.minor, currency};
    }
    Money operator-(Money rhs) const {
        if (currency != rhs.currency) throw std::invalid_argument("currency mismatch");
        return {minor - rhs.minor, currency};
    }
};
}
