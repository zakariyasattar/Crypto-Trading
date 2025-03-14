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

const string api_key { "PKWTH6N3SM1XALKB870E" };
const string api_secret { "sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK" };

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;

void DataIngestion::Connect() {
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

            BuildOrderBook(response);

            // Subscribe after authentication success
            if (response[0]["T"] == "success" && response[0]["msg"] == "authenticated") {
                json sub_msg = {
                    {"action", "subscribe"},
                    {"orderbooks", {"BTC/USD"}}
                };
                this->ws_client.send(this->hdl, sub_msg.dump(), websocketpp::frame::opcode::text);
            }
        });

        // Connect to Secure WebSocket (wss://)
        websocketpp::lib::error_code ec;
        auto con = ws_client.get_connection("wss://stream.data.alpaca.markets/v1beta3/crypto/us", ec);
        if (ec) {
            std::cerr << "Connection failed: " << ec.message() << std::endl;
            return;
        }

        ws_client.connect(con);
        ws_client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

// 
void DataIngestion::BuildOrderBook(json response) {
    // this is bringing in an empty map everytime
    map<double, double>& asks { mOrderBook.GetAsks() };
    map<double, double, greater<double>>& bids { mOrderBook.GetBids() };

    json asksObj { json::parse(response.dump(2))[0]["a"] };
    json bidsObj { json::parse(response.dump(2))[0]["b"] };
    
    // Populate Asks
    Populate(asksObj, asks);

    // Populate Bids
    Populate(bidsObj, bids);


    // Display Orderbook
    mOrderBook.DisplayOrderBook();
}

// Populate/Update multimaps according to webhook data
void DataIngestion::Populate(json obj, auto& orderMap) {
    int desiredMapSize { 10 };

    for(const auto& o : obj) {
        double size { o["s"] };
        double price { o["p"] };

        if(size == 0) {
            orderMap.erase(price);
        }
        else orderMap[price] = size;

        // keep map at desiredMapSize price points only
        if(orderMap.size() > desiredMapSize) {
            auto it = orderMap.end();
            it--;

            orderMap.erase(it);
        }
    }
}