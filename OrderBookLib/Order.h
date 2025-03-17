#include <stdlib.h>

enum class Side {
    Bid,
    Ask
};

class Order {
private:
    double mPrice;
    double mSize;
    Side mSide;
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
};