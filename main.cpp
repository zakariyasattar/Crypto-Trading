/**
 * @file main.cpp
 * @author Zakariya Sattar
 *
 * Initialize Application
 */

#include <iostream>
#include "OrderBookLib/OrderBook.h"

int main() {
    OrderBook orderBook;
    orderBook.DisplayOrderBook();

    return 0;
}