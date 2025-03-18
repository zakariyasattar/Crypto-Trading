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
#include <atomic>
#include <iostream>
#include <sstream>

class OrderBook {

private:
    // key: price, value: shares
    std::map<double, double, std::greater<double>> mBids;
    std::map<double, double> mAsks;

    std::atomic<double> mCurrentPrice {};

public:
    enum class Side {
        Buy,
        Sell,
        None
    };
    
    struct Order {
        double price {};
        double size {};
        Side side { Side::None };

        friend std::ostream& operator<<(std::ostream& os, const Order& o) {
            os << o.size << " @ " << o.price;
            return os;
        }
    };

    struct TradeDecision {
        Side side;
        double stop_loss;
        double exit_price;
        double weight;

        friend std::ostream& operator<<(std::ostream& os, const TradeDecision& td) {
            std::string str { td.side == Side::Buy ? "Buy" : "Sell" };
            os << str << " " << td.exit_price << " " << td.stop_loss << " " << td.weight;
            return os;
        }
    };

    using TradeDecision = OrderBook::TradeDecision;
    using Analysis = std::tuple<TradeDecision, TradeDecision, TradeDecision>;

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
    Analysis AnalyzeOrderBook();

    // Calc VWAP deviation in order book, return prob of trade success
    TradeDecision CalcVWAPDev();

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

    std::pair<double, double> CalcVWAP(const auto& orderMap);

    TradeDecision FindLargeNearbyOrder();

    // Getters and Setters with mutual exclusion for current price variable
    // Current price is modified concurrently, so mutex is needed
    void SetCurrentPrice(double price) {
        mCurrentPrice.store(price, std::memory_order_relaxed);
    }

    double GetCurrentPrice() {
        return mCurrentPrice.load(std::memory_order_relaxed);
    }
};

