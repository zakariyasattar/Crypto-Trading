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

        Order topAsk { mOrderBook.GetTopOrder(mOrderBook.GetAsks()) };
        Order topBid { mOrderBook.GetTopOrder(mOrderBook.GetBids()) };

        // cout << get<0>(analysis) << endl;
        // cout << get<1>(analysis) << endl;
        // cout << get<2>(analysis) << endl;
        
        mOrderBook.DisplayOrderBook();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}