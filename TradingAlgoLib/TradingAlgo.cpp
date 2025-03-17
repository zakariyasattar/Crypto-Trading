#include "TradingAlgo.h"
#include <OrderBook.h>
#include <thread>
#include <iostream>

using namespace std;

void TradingAlgo::StartTrading() {
    while(true) {
        OrderBook::TradeDecision analysis { mOrderBook.AnalyzeOrderBook() };

        // mOrderBook.DisplayOrderBook();
        OrderBook::Order topAsk { mOrderBook.GetTopOrder(mOrderBook.GetAsks()) };
        OrderBook::Order topBid { mOrderBook.GetTopOrder(mOrderBook.GetBids()) };

        cout << mOrderBook.GetCurrentPrice() << endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}