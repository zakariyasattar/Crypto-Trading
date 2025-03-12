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

#define ASIO_STANDALONE // Standalone ASIO instead of Boost ASIO

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;
typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;

class DataIngestion {
private:
    std::string api_key;
    std::string api_secret;
    websocketpp::connection_hdl hdl;
    tls_client ws_client;

public:

    DataIngestion(const std::string& api_key, const std::string& api_secret) : api_key (api_key), api_secret (api_secret) {};

    // Populate bids and asks of OrderBook by reference
    void populate(std::multimap<double, double>& bids, std::multimap<double, double>& asks);

    // Get raw order book data as JSON string
    std::string fetchOrderBookStr();

    void build(std::multimap<double, double>& orders, json obj, bool bids);

    // Establish connection to Alpaca WebSocket
    void connect();
};