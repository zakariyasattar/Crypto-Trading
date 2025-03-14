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
            
            populate(json::parse(response.dump(2)));
            mOrderBook.DisplayOrderBook();

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

// multimap: elems sorted by key, self-balancing red-black tree

// map will map price-level to size
// map<double, double> asks

// steps:
//  - Build initial maps, for every bid/ask, create map entry for each price level
//  - if certain order gets removed, (s = 0), remove price level
//  - otherwise, if it gets updated, update it

// Populate/Update multimaps according to webhook data
void DataIngestion::populate(json obj) {
    json askObj { obj[0]["a"] };
    json bidObj { obj[0]["b"] };

    for(const auto& o : askObj) {
        double size { o["s"] };
        double price { o["p"] };

        mAsks[price] += size;
    }

    for(const auto& o : bidObj) {
        double size { o["s"] };
        double price { o["p"] };

        mBids[price] += size;
    }
}