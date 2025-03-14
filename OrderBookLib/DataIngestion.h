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

#include "OrderBook.h"

#define ASIO_STANDALONE // Standalone ASIO instead of Boost ASIO

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;
typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;

class DataIngestion {
private:
    OrderBook& mOrderBook;

    std::string api_key;
    std::string api_secret;
    websocketpp::connection_hdl hdl;
    tls_client ws_client;

    // key: price, value: size
    std::map<double, double>& mBids;
    std::map<double, double>& mAsks;

public:

    DataIngestion(OrderBook& ob, const std::string& api_key, const std::string& api_secret, std::map<double, double>& bids, std::map<double, double>& asks) : 
        mOrderBook(ob), api_key (api_key), api_secret (api_secret), mBids (bids), mAsks (asks) { 
        };

    // Populate bids and asks of OrderBook by reference
    void populate(json obj);

    // Establish connection to Alpaca WebSocket
    void connect();
};