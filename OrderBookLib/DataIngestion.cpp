/**
 * @file DataIngestion.cpp
 * @author Zakariya Sattar
 */

#include "DataIngestion.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <ctime>
#include <map>
#include <typeinfo>

#include "Enums.h"

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

const string coin_api_key { "e508df71-b919-4e7c-b062-79a517f1c54d" };

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<asio::ssl::context> context_ptr;

// Connect to Binance exchange for order book data
void DataIngestion::Connect() {
    // Build initial data snapshot
    // json initialOrderBookJSON { GetInitialOrderBook() };
    // BuildOrderBook(initialOrderBookJSON);

    // Receive WebSocket updates
    UpdateBook();
}

json DataIngestion::GetInitialOrderBook() {
    // RAII cleanup
    curlpp::Cleanup myCleanup;

    // Set URL for Binance initial orderbook
    curlpp::options::Url initialOrderBookURL { std::string{"https://rest.coinapi.io/v1/orderbooks/BTC_USD/current"} };

    curlpp::Easy initialOrderBookRequest { };
    initialOrderBookRequest.setOpt(initialOrderBookURL);

    std::list<std::string> headers;
    headers.push_back("X-CoinAPI-Key: " + coin_api_key);
    headers.push_back("Accept: application/json");  // Ensures JSON response
    initialOrderBookRequest.setOpt<curlpp::options::HttpHeader>(headers);

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

// Function to initialize SSL context
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

        // Define on_open handler
        ws_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
            std::cout << "WebSocket connection established.\n";

            nlohmann::json subscribe_json = {
                {"type", "subscribe"},
                {"heartbeat", true},
                {"subscribe_data_type", {"book20", "quote"}},
                {"subscribe_filter_asset_id", {"BTC/USD"}}
            };

            std::string subscribe_message = subscribe_json.dump();
            ws_client.send(hdl, subscribe_message, websocketpp::frame::opcode::text);
        });

        // WebSocket Endpoint
        const std::string ws_url = "wss://coinbase.ws-ds.md.coinapi.io/APIKEY-e508df71-b919-4e7c-b062-79a517f1c54d";

        // Create connection
        websocketpp::lib::error_code ec;
        tls_client::connection_ptr con = ws_client.get_connection(ws_url, ec);
        if (ec) {
            std::cerr << "Connection Error: " << ec.message() << std::endl;
            return;
        }

        ws_client.connect(con);

        // Run WebSocket in its own thread
        std::thread ws_thread([&]() {
            ws_client.run();
        });

        ws_thread.join();
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void DataIngestion::on_message(websocketpp::connection_hdl hdl, tls_client::message_ptr msg) {
    const json response { json::parse(msg->get_payload()) };

    // cout << response << endl;

    string type { response["type"] };

    // ticker stream (current price)
    if(type == "quote") {
        double latestAskPrice { response["ask_price"].get<double>() };
        double latestBidPrice { response["bid_price"].get<double>() };

        // Calcuate current price from most recent bid/ask
        double currPrice { (latestAskPrice + latestBidPrice) / 2 };

        mOrderBook.SetCurrentPrice(currPrice);
    }
    else if(type == "book20") { // depth stream (order book)
        BuildOrderBook(response);
    }
}

// 
void DataIngestion::BuildOrderBook(const json& response) {
    // this is bringing in an empty map everytime
    map<double, double>& asks { mOrderBook.GetAsks() };
    map<double, double, greater<double>>& bids { mOrderBook.GetBids() };

    json asksObj { response["asks"] };
    json bidsObj { response["bids"] };
    
    // Populate Asks
    Populate(asksObj, Enums::Side::Sell);

    // Populate Bids
    Populate(bidsObj, Enums::Side::Buy);

    // Display Orderbook
    // mOrderBook.DisplayOrderBook();
}

// Populate/Update multimaps according to webhook data
void DataIngestion::Populate(const json& obj, Enums::Side side) {
    int desiredMapSize { 10 };

    for(const auto& o : obj) {
        double price { o["price"].get<double>() };
        double size { o["size"].get<double>() };

        if(size == 0) {
            // Delete element at price in either asks/bids 
            mOrderBook.DeletePricePoint(price, side);
        }
        else {
            mOrderBook.SetPricePoint(price, size, side);
        }
    }
}