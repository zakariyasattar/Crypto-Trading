#pragma once

#include "Order.h"
#include <stdlib.h>

class Ask : public Order {

public:
    // Construct Ask from Order
    Ask(double s, double p) : Order (s, p) {};

    // Virtual print function
    void print(std::ostream& os) const override;
};