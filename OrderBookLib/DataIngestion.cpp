/**
 * @file DataIngestion.cpp
 * @author Zakariya Sattar
 */

#include "Order.h"
#include "DataIngestion.h"
#include "Bid.h"
#include "Ask.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <memory>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#define ASIO_STANDALONE // Standalone ASIO instead of Boost ASIO

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;
using namespace std;

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;

void DataIngestion::connect() {
    try {
        tls_client ws_client;
        ws_client.init_asio();

        // TLS initialization function
        ws_client.set_tls_init_handler([](websocketpp::connection_hdl) {
            return std::make_shared<websocketpp::lib::asio::ssl::context>(
                websocketpp::lib::asio::ssl::context::tlsv12
            );
        });

        // Set open handler (on connection)
        ws_client.set_open_handler([this, &ws_client](websocketpp::connection_hdl hdl) {
            std::cout << "✅ Connected to WebSocket!" << std::endl;
            this->hdl = hdl;

            // Send authentication
            json auth_msg = {
                {"action", "auth"},
                {"key", api_key},
                {"secret", api_secret}
            };
            ws_client.send(hdl, auth_msg.dump(), websocketpp::frame::opcode::text);
        });

        // Set message handler (when a message is received)
        ws_client.set_message_handler([this](websocketpp::connection_hdl, tls_client::message_ptr msg) {
            json response = json::parse(msg->get_payload());
            std::cout << "📩 Received: " << response.dump(2) << std::endl;

            // Subscribe after authentication success
            if (response[0]["T"] == "success" && response[0]["msg"] == "authenticated") {
                json sub_msg = {
                    {"action", "subscribe"},
                    {"bars", {"BTC/USD"}}
                };
                this->ws_client.send(this->hdl, sub_msg.dump(), websocketpp::frame::opcode::text);
            }
        });

        // Connect to Secure WebSocket (wss://)
        websocketpp::lib::error_code ec;
        auto con = ws_client.get_connection("wss://stream.data.alpaca.markets/v1beta3/crypto/us", ec);
        if (ec) {
            std::cerr << "❌ Connection failed: " << ec.message() << std::endl;
            return;
        }

        ws_client.connect(con);
        ws_client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << std::endl;
    }
}


void DataIngestion::populate(std::multimap<double, double>& bids, std::multimap<double, double>& asks) {
    connect();
    // // Get raw JSON from API
    // string orderBookStr { fetchOrderBookStr() };
    
    // // Convert raw JSON to Object
    // json jsonObject { json::parse(orderBookStr) };

    // json JSONAsks { jsonObject["orderbooks"]["BTC/USD"]["a"] };
    // json JSONBids { jsonObject["orderbooks"]["BTC/USD"]["b"] };

    // // build bids and asks from respective JSON objects
    // build(bids, JSONBids, true);
    // build(asks, JSONAsks, false);
}

void DataIngestion::build(std::multimap<double, double>& orders, json obj, bool bids) {
    for(const auto& o : obj) {
        double shares { o["s"] };
        double price { o["p"] };

        orders.insert({ price, shares });
    }
}

string DataIngestion::fetchOrderBookStr() {
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        // Set API endpoint
        request.setOpt<curlpp::options::Url>("https://data.alpaca.markets/v1beta3/crypto/us/latest/orderbooks?symbols=BTC%2FUSD");

        // Set headers
        std::list<std::string> headers;
        headers.push_back("accept: application/json");
        request.setOpt<curlpp::options::HttpHeader>(headers);

        // Capture response
        std::ostringstream responseStream;
        request.setOpt<curlpp::options::WriteStream>(&responseStream);

        // Perform request
        request.perform();

        // Return raw JSON response
        return responseStream.str();
        
    } catch (curlpp::LogicError& e) {
        return "{\"error\": \"Logic error: " + std::string(e.what()) + "\"}";
    } catch (curlpp::RuntimeError& e) {
        return "{\"error\": \"Runtime error: " + std::string(e.what()) + "\"}";
    }
}