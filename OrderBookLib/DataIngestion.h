/**
 * @file DataIngestion.h
 * @author Zakariya Sattar
 * 
 * Data ingestion engine to connect to API and populate order book
 */

#pragma once

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "Order.h"
#include "Bid.h"
#include "Ask.h"

#include <../include/json.hpp>

using json = nlohmann::json;

class DataIngestion {
public:

    // Populate bids and asks of OrderBook by reference
    void populate(std::vector<std::unique_ptr<Order>>& bids, std::vector<std::unique_ptr<Order>>& asks);

    // Get raw order book data as JSON string
    std::string fetchOrderBookStr();

    void build(std::vector<std::unique_ptr<Order>>& orders, json obj, bool bids);
};