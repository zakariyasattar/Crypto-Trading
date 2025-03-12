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

#include <../include/json.hpp>

using json = nlohmann::json;
using namespace std;

void DataIngestion::populate(std::vector<unique_ptr<Order>>& bids, std::vector<unique_ptr<Order>>& asks) {
    string orderBookStr { fetchOrderBookStr() };
    json jsonObject { json::parse(orderBookStr) };

    json JSONAsks { jsonObject["orderbooks"]["BTC/USD"]["a"] };
    json JSONBids { jsonObject["orderbooks"]["BTC/USD"]["b"] };

    build(bids, JSONBids, true);
    build(asks, JSONAsks, false);
}

void DataIngestion::build(vector<unique_ptr<Order>>& orders, json obj, bool bids) {
    for(const auto& o : obj) {
        double shares { o["s"] };
        double price { o["p"] };

        if(bids) {
            orders.emplace_back(make_unique<Bid>(Bid(shares, price)));
        }
        else {
            orders.emplace_back(make_unique<Ask>(Ask(shares, price)));
        }
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