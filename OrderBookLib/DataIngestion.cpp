/**
 * @file DataIngestion.cpp
 * @author Zakariya Sattar
 */

#include "DataIngestion.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <ctime>
#include <map>

#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#define ASIO_STANDALONE // Standalone ASIO instead of Boost ASIO

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/client.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;
using namespace std;

const string alpaca_api_key { "PKWTH6N3SM1XALKB870E" };
const string alpaca_api_secret { "sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK" };

const string binance_api_key { "X0ESxm8Kx5pak1LMvJSyqYek4dSKMWx3rfzKh3fWC146FujBmd8Tts08nT6bgTbW" };
const string binance_api_secret { "Ws41QPliZk0t6LU3eE0AtKTy3mN33qbIZGKpWQnz7P5ff3r7m9wUtUn4x6DVfBxi" };

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<asio::ssl::context> context_ptr;

// Binance US WebSocket Endpoint
const std::string binance_ws_url = "wss://stream.binance.us:9443/ws/btcusdt@depth";

// Connect to Binance exchange for order book data
void DataIngestion::Connect() {
    // Build initial data snapshot
    json initialOrderBookJSON { GetInitialOrderBook() };
    BuildOrderBook(initialOrderBookJSON);

    // Receive WebSocket updates
    UpdateBook();
}

json DataIngestion::GetInitialOrderBook() {
    // RAII cleanup
    curlpp::Cleanup myCleanup;

    // Set URL for Binance initial orderbook
    curlpp::options::Url initialOrderBookURL { std::string{"https://api.binance.us/api/v3/depth?symbol=BTCUSDT&limit=1000"} };
    curlpp::Easy initialOrderBookRequest { };
    initialOrderBookRequest.setOpt(initialOrderBookURL);

    // Route output to custom ostringstream
    ostringstream os {};
    initialOrderBookRequest.setOpt(curlpp::options::WriteStream(&os));

    // Perform intial order book request
    initialOrderBookRequest.perform();

    // Convert to string and parse as json
    std::string response = os.str();
    const json initialOrderBookJSON { json::parse(response) };

    return initialOrderBookJSON;
}

// Function to initialize SSL context (needed for Binance)
context_ptr DataIngestion::on_tls_init(websocketpp::connection_hdl hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(
        websocketpp::lib::asio::ssl::context::tlsv12
    );

    try {
        ctx->set_options(asio::ssl::context::default_workarounds | 
                         asio::ssl::context::no_sslv2 | 
                         asio::ssl::context::no_sslv3 | 
                         asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::cerr << "TLS Init Error: " << e.what() << std::endl;
    }
    return ctx;
}

void DataIngestion::UpdateBook() {
    try {
        // Initialize WebSocket++ client
        ws_client.init_asio();

        // Bind member functions to allow access to class members
        ws_client.set_tls_init_handler(std::bind(&DataIngestion::on_tls_init, this, std::placeholders::_1));
        ws_client.set_message_handler(std::bind(&DataIngestion::on_message, this, std::placeholders::_1, std::placeholders::_2));

        // Create WebSocket connection
        websocketpp::lib::error_code ec;
        tls_client::connection_ptr con = ws_client.get_connection(binance_ws_url, ec);
        if (ec) {
            std::cerr << "Connection Error: " << ec.message() << std::endl;
            return;
        }

        // Connect and run in a separate thread
        ws_client.connect(con);
        std::thread ws_thread([&]() { ws_client.run(); });

        std::cout << "Connected to Binance WebSocket. Listening for updates..." << std::endl;

        // Keep main thread alive
        ws_thread.join();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void DataIngestion::on_message(websocketpp::connection_hdl hdl, tls_client::message_ptr msg) {
    const json response { json::parse(msg->get_payload()) };
    BuildOrderBook(response);
}

// 
void DataIngestion::BuildOrderBook(const json& response) {
    // this is bringing in an empty map everytime
    map<double, double>& asks { mOrderBook.GetAsks() };
    map<double, double, greater<double>>& bids { mOrderBook.GetBids() };

    string askStr { response.contains("a") ? "a" : "asks" };
    string bidStr { response.contains("b") ? "b" : "bids" };

    json asksObj { json::parse(response.dump(2))[askStr] };
    json bidsObj { json::parse(response.dump(2))[bidStr] };
    
    // Populate Asks
    Populate(asksObj, asks);

    // Populate Bids
    Populate(bidsObj, bids);

    // Display Orderbook
    mOrderBook.DisplayOrderBook();
}

// Populate/Update multimaps according to webhook data
void DataIngestion::Populate(const json& obj, auto& orderMap) {
    int desiredMapSize { 10 };

    for(const auto& o : obj) {
        double price { std::stod(o[0].get<std::string>()) };
        double size { std::stod(o[1].get<std::string>()) };

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