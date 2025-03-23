/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include <thread>
#include <mutex>

#include "OrderBook.h"
#include "DataIngestion.h"
#include "Timer.h"
#include "Enums.h"

using namespace std;

// ANSI escape sequence to clear screen and move cursor to top-left
#define CLEAR_SCREEN "\033[2J\033[1;1H"

void OrderBook::InitData() {
    // Instantiate a thread to init the connection
    std::thread connectionThread { [&]() {
        // Instantiate OrderBook with Data from API
        DataIngestion engine { *this };

        engine.Connect();
    }};

    // detach thread so it runs separately from the program
    connectionThread.detach();
}

void OrderBook::DeletePricePoint(double price, Enums::Side side) {
    std::lock_guard<std::mutex> lock(mMtx);

    if(side == Enums::Side::Buy)
        mBids.erase(price);
    else
        mAsks.erase(price);
}

void OrderBook::SetPricePoint(double price, double size, Enums::Side side) {
    std::lock_guard<std::mutex> lock(mMtx);

    int desiredMapSize { 12 };

    if(side == Enums::Side::Buy) {
        mBids[price] = size;
        Shrink(mBids, desiredMapSize);
    }
    else {
        mAsks[price] = size;
        Shrink(mAsks, desiredMapSize);
    }

    // Notify if both have at least one entry
    if (!mAsks.empty() && !mBids.empty()) {
        mCv.notify_one();
    }
}

void OrderBook::Shrink(auto& orderMap, int desiredMapSize) {
    // keep map at desiredMapSize price points only
    if(orderMap.size() > desiredMapSize) {
        auto it = orderMap.end();
        it--;

        orderMap.erase(it);
    }
}

void OrderBook::DisplayOrderBook() {
    // Clear the screen
    std::cout << CLEAR_SCREEN << std::flush;
    
    // Display header
    std::cout << "======== BTC/USD ORDER BOOK ========" << std::endl;
    std::cout << "Updated: " << getCurrentTimestamp() << std::endl << endl;
    cout << "Current Price: " << GetCurrentPrice() << endl; 
    
    // Format column headers
    std::cout << std::left
              << std::setw(10) << "Order Type" << " | "
              << std::right
              << std::setw(12) << "Price" << " | "
              << std::setw(12) << "Size" << std::endl;
    
    std::cout << "---------------------------------------" << std::endl;
    
    auto asksIt { mAsks.end() };
    asksIt--;

    // Display asks
    for(; asksIt != mAsks.begin(); asksIt--) {
        auto price { asksIt->first };
        auto size { asksIt->second };

        std::cout << std::left 
                  << std::setw(10) << "[Ask]" << " | "
                  << std::right
                  << std::fixed << std::setprecision(2) << std::setw(12) << price << " | "
                  << std::fixed << std::setprecision(6) << std::setw(12) << size << std::endl;
    }
    
    // Display bids
    for(const auto& [price, size]: mBids) {
        std::cout << std::left 
                  << std::setw(10) << "[BID]" << " | "
                  << std::right
                  << std::fixed << std::setprecision(2) << std::setw(12) << price << " | "
                  << std::fixed << std::setprecision(6) << std::setw(12) << size << std::endl;
    }

    std::cout << std::flush;
}

// Helper function to get current timestamp
string OrderBook::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time);

    // Remove newline character
    timestamp.pop_back();
    return timestamp;
}

// function that will analyze OrderBook and will return whether to buy or sell
// and at what price to do so. Also stop-loss price
OrderBook::Analysis OrderBook::AnalyzeOrderBook() {
    // 3 strategies:
    //  1. VWAP Deviation
    //  2. Current Price near to top Bid/Ask
    //  3. Order Book Imbalance

    // each strat can return a prob of success,
    // whichever has the highest prob should 
    // be executed (converted to a TradeDecision)

    OrderBook::TradeDecision VWAPAnalysis { CalcVWAPDev() };
    OrderBook::TradeDecision largeNearbyOrder { FindLargeNearbyOrder() };
    // Pass largeNearbyOrder for stop-loss purposes TODO
    OrderBook::TradeDecision orderBookImbalance { CalcOrderBookImbalance() };

    // cout << GetCurrentPrice() << endl;
    // cout << largeNearbyOrder << endl;

    // cout << orderBookImbalance << endl;

    return {VWAPAnalysis, largeNearbyOrder, orderBookImbalance};
}

// TODO: make functions to check if position should be maintained or terminated?

OrderBook::TradeDecision OrderBook::CalcOrderBookImbalance() {
    auto [askVWAP, askTotalVol] = CalcVWAP(mAsks);
    auto [bidVWAP, bidTotalVol] = CalcVWAP(mBids);
    
    OrderBook::TradeDecision decision;
    
    // Determine which side has more volume
    bool bidSideHeavy = bidTotalVol > askTotalVol;
    double totalDiff = abs(askTotalVol - bidTotalVol);
    
    // Calculate imbalance ratio (how many times larger one side is than the other)
    double imbalanceRatio = totalDiff / (bidSideHeavy ? askTotalVol : bidTotalVol);
        
    // Set trade direction based on imbalance
    if (bidSideHeavy) {
        decision.side = Enums::Side::Buy;  // More buyers than sellers
    } else {
        decision.side = Enums::Side::Sell; // More sellers than buyers
    }

    // Convert ratio to confidence percentage (capped at 100%)
    // A ratio of 2.0 means one side has 200% the volume of the other
    double confidencePercentage = std::min(imbalanceRatio * 100.0 / 3.0, 100.0);
    decision.weight = confidencePercentage / 100.0;
    
    // Apply minimum threshold to avoid acting on small imbalances
    if (decision.weight < 0.15) { // Minimum 15% confidence required
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }
    
    return decision;
}

// find largest order, within 3 levels
OrderBook::TradeDecision OrderBook::FindLargeNearbyOrder(int levelsToCheck) {
    auto asksIt { mAsks.begin() };
    auto bidsIt { mBids.begin() };

    Enums::Order maxOrderWithinLevels {};

    while(levelsToCheck-- > 0 && asksIt != mAsks.end() && bidsIt != mBids.end()) {
        Enums::Order curr {};

        if(asksIt->second > bidsIt->second) {
            curr = Enums::Order(asksIt->first, asksIt->second, Enums::Side::Buy);
        }
        else {
            curr = Enums::Order(asksIt->first, asksIt->second, Enums::Side::Sell);
        }

        maxOrderWithinLevels = maxOrderWithinLevels.size > curr.size ? maxOrderWithinLevels : curr;

        asksIt++;
        bidsIt++;
    }

    Enums::Side side { maxOrderWithinLevels.side };
    double size { maxOrderWithinLevels.size };
    double price { maxOrderWithinLevels.price };

    auto [VWAP, totalVol] { side == Enums::Side::Buy ? CalcVWAP(mAsks) : CalcVWAP(mBids) };
    auto [VWAPop, totalVolOp] { side == Enums::Side::Buy ? CalcVWAP(mBids) : CalcVWAP(mAsks) };

    // Impact Score = (Order Size / Total Volume) * (1 - |Order Price - Relevant VWAP| / |Ask VWAP - Bid VWAP|)
    double weight { (size / totalVol) * (1 - abs(price - VWAP) / abs(VWAP - VWAPop)) };

    OrderBook::TradeDecision decision { side, GetCurrentPrice() * .995, price, weight };

    if(decision.weight < 0.15) {
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }

    return decision;
}

// Calculates VWAP deviation within order book
// returns probability of trade success
OrderBook::TradeDecision OrderBook::CalcVWAPDev() {
    double currPrice { GetCurrentPrice()};

    double bidVWAP { CalcVWAP(mBids).first };
    double askVWAP { CalcVWAP(mAsks).first };

    double bidDev { currPrice - bidVWAP };
    double askDev { currPrice - askVWAP };

    OrderBook::TradeDecision decision {};
    
    // Strongly overbought case
    if(bidDev > 0 && askDev > 0) {
        decision.side = Enums::Side::Sell;
        decision.weight = 0.85;
        return decision;
    }
    
    // Strongly oversold case
    if(bidDev < 0 && askDev < 0) {
        decision.side = Enums::Side::Buy;
        decision.weight = 0.85;
        return decision;
    }
    
    // Handle the in-between case
    double vwapChannelWidth = askVWAP - bidVWAP;
    
    // Prevent division by zero
    if(vwapChannelWidth <= 0) {
        decision.side = Enums::Side::None;
        decision.weight = 0;
        return decision;
    }
    
    double vwapPosition = (currPrice - bidVWAP) / vwapChannelWidth;
    
    // Correct the trading logic
    if(vwapPosition > 0.5) {
        decision.side = Enums::Side::Sell;
        decision.weight = vwapPosition; // Higher as it approaches 1
    } else {
        decision.side = Enums::Side::Buy;
        decision.weight = 1 - vwapPosition; // Higher as it approaches 0
    }

    decision.stop_loss = GetCurrentPrice() * .995;

    if(decision.weight < .15) {
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }
    
    return decision;
}


// Calculate ask/bid VWAP, depending on whats passed in
std::pair<double, double> OrderBook::CalcVWAP(const auto& orderMap) {
    double priceVolSum {};
    double totalVol {};

    for(const auto& [price, size] : orderMap) {
        priceVolSum += (price * size);
        totalVol += size;
    }

    return { priceVolSum / totalVol, totalVol };
}