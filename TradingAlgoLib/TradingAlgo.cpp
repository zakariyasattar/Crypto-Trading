/**
 * @file TradingAlgo.cpp
 * @author Zakariya Sattar
 */


#include "TradingAlgo.h"
#include "TradeExecution.h"

#include <OrderBook.h>
#include <Order.h>
#include <Enums.h>

#include <thread>
#include <iostream>

using namespace std;

using Analysis = std::tuple<OrderBook::TradeDecision, OrderBook::TradeDecision, OrderBook::TradeDecision>;
using Position = Enums::Position;

void TradingAlgo::StartTrading() {
    while(true) {
        Analysis analysis { mOrderBook.AnalyzeOrderBook() };

        // Find TradeDecision with max weight
        auto bestDecision = std::apply([](auto&&... decisions) {
            return std::max({decisions...}, [](const auto& a, const auto& b) {
                return a.weight < b.weight;
            });
        }, analysis);

        // if no positions are open, open pos
        // otherwise, check if exit price 

        mOpenPosition = mExecutionEngine.GetOpenPosition();
        // cout << openPosition.asset_id << endl;

        if(mOpenPosition.asset_id == "") { // No open position
            mExecutionEngine.ExecuteTrade(bestDecision);   
        }
        else {
            double currPrice { mOrderBook.GetCurrentPrice() };

            if(currPrice >= mOpenPosition.td.exit_price || currPrice <= mOpenPosition.td.stop_loss) {
                int percentToLiquidate { 100 };

                mExecutionEngine.ClosePosition(mOpenPosition.asset_id);
            }
        }

        DisplayCurrentPosition();
        
        // mOrderBook.DisplayOrderBook();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void TradingAlgo::DisplayCurrentPosition() {
    cout << "Current Pos: " << endl;
    cout << mOpenPosition << endl;
}

TradingAlgo::~TradingAlgo() {
    mExecutionEngine.CloseAllPositions();
}