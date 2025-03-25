#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sstream>

enum class Side {
    Bid,
    Ask,
    None
};

class Order {
private:
    double mPrice {};
    double mSize {};
    Side mSide {};
    static uint64_t mOrderId;

public:
    Order() = default;
    Order(double price, double size, Side side)
        : mPrice(price), mSize(size), mSide(side) {
            mOrderId++;
        }

    // Getters
    uint64_t GetOrderID() { return mOrderId; }
    Side GetSide() { return mSide; }
    double GetPrice() { return mPrice; }
    double GetSize() { return mSize; }

    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        os << order.mPrice;
        return os;
    }
};