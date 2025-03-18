/**
 * @file OrderBook.cpp
 * @author Zakariya Sattar
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "OrderBook.h"
#include "DataIngestion.h"
#include "Timer.h"

using namespace std;

// ANSI escape sequence to clear screen and move cursor to top-left
#define CLEAR_SCREEN "\033[2J\033[1;1H"

void OrderBook::initData() {
    // Instantiate OrderBook with Data from API
    DataIngestion engine { *this };

    // Connect to webhook and display data
    engine.Connect();
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
    // be executed (converted to an TradeDecision)

    OrderBook::TradeDecision VWAPAnalysis { CalcVWAPDev() };
    OrderBook::TradeDecision largeNearbyOrder { FindLargeNearbyOrder() };
    OrderBook::TradeDecision orderBookImbalance { CalcVWAPDev() };

    cout << GetCurrentPrice() << endl;
    cout << largeNearbyOrder << endl;

    return {VWAPAnalysis, largeNearbyOrder, orderBookImbalance};
}

// find largest order, within 3 levels
OrderBook::TradeDecision OrderBook::FindLargeNearbyOrder() {
    auto asksIt { mAsks.begin() };
    auto bidsIt { mBids.begin() };

    Order maxOrderWithinLevels {};

    int levelsToCheck { 3 };

    while(levelsToCheck-- > 0 && asksIt != mAsks.end() && bidsIt != mBids.end()) {
        Order curr {};

        if(asksIt->second > bidsIt->second) {
            curr = Order(asksIt->first, asksIt->second, Side::Buy);
        }
        else {
            curr = Order(asksIt->first, asksIt->second, Side::Sell);
        }

        maxOrderWithinLevels = maxOrderWithinLevels.size > curr.size ? maxOrderWithinLevels : curr;

        asksIt++;
        bidsIt++;
    }

    Side side { maxOrderWithinLevels.side };
    double size { maxOrderWithinLevels.size };
    double price { maxOrderWithinLevels.price };

    auto [VWAP, totalVol] { side == Side::Buy ? CalcVWAP(mAsks) : CalcVWAP(mBids) };
    auto [VWAPop, totalVolOp] { side == Side::Buy ? CalcVWAP(mBids) : CalcVWAP(mAsks) };

    // Impact Score = (Order Size / Total Volume) * (1 - |Order Price - Relevant VWAP| / |Ask VWAP - Bid VWAP|)
    double weight { (size / totalVol) * (1 - abs(price - VWAP) / abs(VWAP - VWAPop)) };

    return { side, GetCurrentPrice() * .995, price, weight };
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
        cout << "strongly overbought" << endl;
        decision.side = Side::Sell;
        decision.weight = 0.85;
        return decision;
    }
    
    // Strongly oversold case
    if(bidDev < 0 && askDev < 0) {
        cout << "strongly oversold" << endl;
        decision.side = Side::Buy;
        decision.weight = 0.85;
        return decision;
    }
    
    // Handle the in-between case
    double vwapChannelWidth = askVWAP - bidVWAP;
    
    // Prevent division by zero
    if(vwapChannelWidth <= 0) {
        decision.side = Side::None;
        decision.weight = 0;
        return decision;
    }
    
    double vwapPosition = (currPrice - bidVWAP) / vwapChannelWidth;
    
    // Correct the trading logic
    if(vwapPosition > 0.5) {
        decision.side = Side::Sell;
        decision.weight = vwapPosition; // Higher as it approaches 1
    } else {
        decision.side = Side::Buy;
        decision.weight = 1 - vwapPosition; // Higher as it approaches 0
    }

    decision.stop_loss = GetCurrentPrice() * .995;
    
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