/**
 * @file Order.h
 * @author Zakariya Sattar
 *
 * Order Object
 */


#pragma once

#include <stdlib.h>
#include <iostream>

class Order {
private:
    uint64_t shares;
    double price;
    bool bid;

public:

    // Constructor
    Order(uint64_t shares, float price, bool bid) : shares (shares), price (price), bid (bid) {};

    // Overload << operator to allow direct printing of object
    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        std::string str { order.bid ? "Bid" : "Ask" };
        
        os << order.shares << " " << order.price << " " << str;
        return os;
    }

};