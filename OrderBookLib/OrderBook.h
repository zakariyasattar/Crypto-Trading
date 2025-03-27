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
#include <mutex>
#include <condition_variable>

#include "Enums.h"
#include "LockFreeQueue.h"

class OrderBook {

private:
    // key: price, value: shares
    std::map<double, double, std::greater<double>> mBids;
    std::map<double, double> mAsks;

    std::mutex& mMtx;
    std::condition_variable& mCv;

    bool mDataReceived {};
    LockFreeQueue mLockFreeQueue;

    std::atomic<double> mCurrentPrice {};

public:
    using TradeDecision = Enums::TradeDecision;

    // class constructor
    OrderBook(std::mutex& mtx, std::condition_variable& cv) : mMtx(mtx), mCv(cv) { };

    ~OrderBook() { 
        mAsks.clear();
        mBids.clear();
    }

    // Pull data from source and populate member vectors
    void InitData();

    // Print out OrderBook
    void DisplayOrderBook();

    // Methods to update OrderBook
    void SetPricePoint(double price, double size, Enums::Side side);
    void DeletePricePoint(double price, Enums::Side side);
    void Shrink(auto& orderMap, int desiredMapSize, Enums::Side side);

    auto& GetAsks() { return mAsks; }
    auto& GetBids() { return mBids; }

    // return current timestamp
    std::string getCurrentTimestamp();

    // Analyze OrderBook and decide what kind of trade to make
    Enums::Analysis AnalyzeOrderBook();

    // must use template instead of auto
    template <typename T>
    Order GetTopOrder(const T& orderMap) {
        auto it { orderMap.begin() };

        double price { it->first };
        double size{ it->second };

        return {price, size, Side::None};
    }

    bool isEmpty() { return mAsks.size() == 0 || mBids.size() == 0; }

    // return instance of LockFreeQueue
    LockFreeQueue& GetLockFreeQueue() {
        return mLockFreeQueue;
    }

    // first = price
    double GetMidPrice() { return (GetTopOrder(mAsks).GetPrice() + GetTopOrder(mBids).GetPrice()) / 2; };

    // Getters and Setters with mutual exclusion for current price variable
    // Current price is modified concurrently, so mutex is needed
    void SetCurrentPrice(double price) { mCurrentPrice.store(price, std::memory_order_relaxed); }
    double GetCurrentPrice() { return mCurrentPrice.load(std::memory_order_relaxed); }
};

