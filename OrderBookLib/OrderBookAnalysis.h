#pragma once

#include "Enums.h"
#include "OrderBook.h"

using TradeDecision = Enums::TradeDecision;
using LargestNearbyOrder = Enums::LargestNearbyOrder;

class OrderBookAnalysis {
private:
    OrderBook& mOrderBook;
    std::map<double, double, std::greater<double>>& mBids;
    std::map<double, double>& mAsks;

public:
    OrderBookAnalysis(OrderBook& orderBook) : mOrderBook(orderBook), mBids(mOrderBook.GetBids()), mAsks(mOrderBook.GetAsks()) {}

    // 3 strategies

    // Calc VWAP deviation in order book, return prob of trade success
    TradeDecision CalcVWAPDev();

    // Calc Imbalance between bid and ask volume in order book
    TradeDecision CalcOrderBookImbalance(LargestNearbyOrder largestOrder);

    // Find largest order within x amount of levels
    std::pair<TradeDecision, LargestNearbyOrder> FindLargeNearbyOrder(int levelsToCheck = 3);

    std::pair<double, double> CalcVWAP(const auto& orderMap);

    Enums::Analysis AnalyzeOrderBook();

};