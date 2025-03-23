/**
 * @file TradingAlgo.cpp
 * @author Zakariya Sattar
 */


#include "TradingAlgo.h"
#include <OrderBook.h>
#include <thread>
#include <iostream>

using namespace std;

using Analysis = std::tuple<OrderBook::TradeDecision, OrderBook::TradeDecision, OrderBook::TradeDecision>;

void TradingAlgo::StartTrading() {
    while(true) {
        // Analysis analysis { mOrderBook.AnalyzeOrderBook() };

        // OrderBook::Order topAsk { mOrderBook.GetTopOrder(mOrderBook.GetAsks()) };
        // OrderBook::Order topBid { mOrderBook.GetTopOrder(mOrderBook.GetBids()) };
        
        mOrderBook.DisplayOrderBook();

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}