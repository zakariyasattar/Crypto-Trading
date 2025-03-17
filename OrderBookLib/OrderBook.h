/**
 * @file OrderBook.h
 * @author Zakariya Sattar
 *
 * Store and Display OrderBook
 */

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <sstream>

class OrderBook {

private:
    // key: price, value: shares
    std::map<double, double, std::greater<double>> mBids;
    std::map<double, double> mAsks;

    double mCurrentPrice;

    std::mutex mtx;

public:
    enum class Side {
        Buy,
        Sell
    };
    
    struct Order {
        double price;
        double size;

        friend std::ostream& operator<<(std::ostream& os, const Order& o) {
            os << o.size << " @ " << o.price;
            return os;
        }
    };

    struct TradeDecision {
        Side side;
        double stop_loss;
        double exit_price;
    };

    // class constructor
    OrderBook() { };

    // Pull data from source and populate member vectors
    void initData();

    // Print out OrderBook
    void DisplayOrderBook();

    auto& GetAsks() { return mAsks; }
    auto& GetBids() { return mBids; }

    // return current timestamp
    std::string getCurrentTimestamp();

    // Analyze OrderBook and decide what kind of trade to make
    TradeDecision AnalyzeOrderBook();

    // Calc VWAP deviation in order book, return prob of trade success
    double CalcVWAPDev();

    // must use template instead of auto
    template <typename T>
    Order GetTopOrder(const T& orderMap) {
        auto it { orderMap.begin() };

        double price { it->first };
        double size{ it->second };

        return {price, size};
    }

    // first = price
    double GetMidPrice() { return (GetTopOrder(mAsks).price + GetTopOrder(mBids).price) / 2; };

    double CalcVWAP();

    // Getters and Setters with mutual exclusion for current price variable
    // Current price is modified concurrently, so mutex is needed
    void SetCurrentPrice(double price) {
        std::unique_lock<std::mutex> lock(mtx);
        mCurrentPrice = price;
    }

    double GetCurrentPrice() {
        std::unique_lock<std::mutex> lock(mtx);
        return mCurrentPrice;
    }
};

