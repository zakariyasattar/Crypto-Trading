/**
 * @file DataIngestion.h
 * @author Zakariya Sattar
 * 
 * Data ingestion engine to connect to API and populate order book
 */

#pragma once

#include <iostream>
#include <stdlib.h>

#include "Order.h"
#include <vector>

class DataIngestion {
public:

    // Populate bids and asks of OrderBook by reference
    void populate(std::vector<Order>& bids, std::vector<Order>& asks);
};