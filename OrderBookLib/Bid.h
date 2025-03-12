#pragma once

#include "Order.h"
#include <stdlib.h>

class Bid : public Order {

public:
    // Construct Ask from Order
    Bid(double s, double p) : Order (s, p) {};

    // Virtual print function
    void print(std::ostream& os) const override;
};