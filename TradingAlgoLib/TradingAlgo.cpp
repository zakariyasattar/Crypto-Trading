/**
 * @file TradingAlgo.cpp
 * @author Zakariya Sattar
 */


#include "TradingAlgo.h"
#include <OrderBook.h>
#include <Order.h>

#include <thread>
#include <iostream>

using namespace std;

using Analysis = std::tuple<OrderBook::TradeDecision, OrderBook::TradeDecision, OrderBook::TradeDecision>;

void TradingAlgo::StartTrading() {
    while(true) {
        Analysis analysis { mOrderBook.AnalyzeOrderBook() };

        // Find TradeDecision with max weight
        auto bestDecision = std::apply([](auto&&... decisions) {
            return std::max({decisions...}, [](const auto& a, const auto& b) {
                return a.weight < b.weight;
            });
        }, analysis);

        cout << bestDecision << endl;

        Order topAsk { mOrderBook.GetTopOrder(mOrderBook.GetAsks()) };
        Order topBid { mOrderBook.GetTopOrder(mOrderBook.GetBids()) };
        
        mOrderBook.DisplayOrderBook();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}