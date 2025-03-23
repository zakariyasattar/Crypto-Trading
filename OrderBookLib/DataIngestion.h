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
#include "Enums.h"
#include "Order.h"

#define ASIO_STANDALONE // Standalone ASIO instead of Boost ASIO

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/connection_hdl.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<asio::ssl::context> context_ptr;

class DataIngestion {
private:
    OrderBook& mOrderBook;

    websocketpp::connection_hdl hdl;

    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    tls_client ws_client;

public:

    // Pass mOrderBook to have access to OrderBook methods
    explicit DataIngestion(OrderBook& ob) : mOrderBook(ob) {};

    // Use auto for different map sorting types
    // Populate bids and asks of OrderBook by reference
    void Populate(const json& obj, Enums::Side side);

    // Establish connection to Alpaca WebSocket
    void Connect();

    // Build OrderBook based on JSON response
    void BuildOrderBook(const json& response);

    void on_message(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);
    context_ptr on_tls_init(websocketpp::connection_hdl hdl);

    void UpdateBook();

    json GetInitialOrderBook();
};