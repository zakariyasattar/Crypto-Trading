/**
 * @file Order.h
 * @author Zakariya Sattar
 *
 * Order Object
 */


#pragma once

#include <stdlib.h>
#include <iostream>
#include <sstream>

class Order {
protected:

    // shares of BTC are double
    double shares;
    double price;

public:
    Order(double s, double p) : shares (s), price (p) {};

    virtual ~Order() = default;

    virtual void print(std::ostream& os) const {}

    friend std::ostream& operator<<(std::ostream& os, Order& order) {
        order.print(os);
        return os;
    }

};