
#include "OrderBookAnalysis.h"
#include "Enums.h"

// function that will analyze OrderBook and will return whether to buy or sell
// and at what price to do so. Also stop-loss price
Enums::Analysis OrderBookAnalysis::AnalyzeOrderBook() {
    // 3 strategies:
    //  1. VWAP Deviation
    //  2. Current Price near to top Bid/Ask
    //  3. Order Book Imbalance

    // each strat can return a prob of success,
    // whichever has the highest prob should 
    // be executed (converted to a TradeDecision)

    TradeDecision VWAPAnalysis { CalcVWAPDev() };
    std::pair<TradeDecision, LargestNearbyOrder> largeNearbyOrder { FindLargeNearbyOrder() };
    // Pass largeNearbyOrder for stop-loss purposes TODO
    TradeDecision orderBookImbalance { CalcOrderBookImbalance(largeNearbyOrder.second) };

    return {VWAPAnalysis, largeNearbyOrder.first, orderBookImbalance};
}

// TODO: make functions to check if position should be maintained or terminated?

TradeDecision OrderBookAnalysis::CalcOrderBookImbalance(LargestNearbyOrder largestOrder) {
    auto [askVWAP, askTotalVol] = CalcVWAP(mAsks);
    auto [bidVWAP, bidTotalVol] = CalcVWAP(mBids);
    
    TradeDecision decision;
    
    // Determine which side has more volume
    bool bidSideHeavy = bidTotalVol > askTotalVol;
    double totalDiff = abs(askTotalVol - bidTotalVol);
    
    // Calculate imbalance ratio (how many times larger one side is than the other)
    double imbalanceRatio = totalDiff / (bidSideHeavy ? askTotalVol : bidTotalVol);
        
    // Set trade direction based on imbalance
    if (bidSideHeavy) {
        decision.side = Enums::Side::Buy;  // More buyers than sellers
        decision.strategy = "Order Book Imbalance: Bid Side Heavy";
    } else {
        decision.side = Enums::Side::Sell; // More sellers than buyers
        decision.strategy = "Order Book Imbalance: Ask Side Heavy";
    }

    // Convert ratio to confidence percentage (capped at 100%)
    // A ratio of 2.0 means one side has 200% the volume of the other
    double confidencePercentage = std::min(imbalanceRatio * 100.0 / 3.0, 100.0);
    decision.weight = confidencePercentage / 100.0;

    double currPrice { mOrderBook.GetCurrentPrice() };

    decision.exit_price = decision.side == Side::Buy ? largestOrder.bid : largestOrder.ask;
    decision.stop_loss = decision.side == Side::Buy ? currPrice * 1.02 : currPrice * 0.98;
    
    // Apply minimum threshold to avoid acting on small imbalances
    if (decision.weight < 0.15) { // Minimum 15% confidence required
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }
    
    return decision;
}

// find largest order, within 3 levels
std::pair<TradeDecision, LargestNearbyOrder> OrderBookAnalysis::FindLargeNearbyOrder(int levelsToCheck) {
    auto asksIt { mAsks.begin() };
    auto bidsIt { mBids.begin() };

    Order maxOrderWithinLevels {};

    Order maxBid { 0.0, 0.0, Side::None };
    Order maxAsk { 0.0, 0.0, Side::None };

    while(levelsToCheck-- > 0 && asksIt != mAsks.end() && bidsIt != mBids.end()) {
        double asksSize { asksIt->second };
        double asksPrice { asksIt->first };

        double bidsSize { bidsIt->second };
        double bidsPrice { bidsIt->first };

        if(asksSize > bidsSize && asksSize > maxAsk.GetSize()) {
            maxAsk = Order(asksPrice, asksSize, Side::Buy);
        }
        else if(asksSize <= bidsSize && bidsSize > maxBid.GetSize()) {
            maxBid = Order(bidsPrice, bidsSize, Side::Sell);
        }

        asksIt++;
        bidsIt++;
    }

    maxOrderWithinLevels = maxAsk.GetSize() > maxBid.GetSize() ? maxAsk : maxBid;

    Enums::Side side { maxOrderWithinLevels.GetSide() };
    double size { maxOrderWithinLevels.GetSize() };
    double price { maxOrderWithinLevels.GetPrice() };

    auto [VWAP, totalVol] { side == Enums::Side::Buy ? CalcVWAP(mAsks) : CalcVWAP(mBids) };
    auto [VWAPop, totalVolOp] { side == Enums::Side::Buy ? CalcVWAP(mBids) : CalcVWAP(mAsks) };

    // Impact Score = (Order Size / Total Volume) * (1 - |Order Price - Relevant VWAP| / |Ask VWAP - Bid VWAP|)
    double weight { (size / totalVol) * (1 - abs(price - VWAP) / abs(VWAP - VWAPop)) };

    std::string strategy { "Large Order: Size (" + std::to_string(size) + ") Price (" + std::to_string(price) + ") " + (side == Side::Buy ? "Bid" : "Ask") + " Side"};

    TradeDecision decision { side, mOrderBook.GetCurrentPrice() * .995, price, weight, strategy };

    if(decision.weight < 0.15) {
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }

    LargestNearbyOrder largestNearbyOrder { maxBid.GetPrice(), maxAsk.GetPrice() };

    return std::make_pair(decision, largestNearbyOrder);
}

// Calculates VWAP deviation within order book
// returns probability of trade success
TradeDecision OrderBookAnalysis::CalcVWAPDev() {
    double currPrice { mOrderBook.GetCurrentPrice()};

    double bidVWAP { CalcVWAP(mBids).first };
    double askVWAP { CalcVWAP(mAsks).first };

    double bidDev { currPrice - bidVWAP };
    double askDev { currPrice - askVWAP };

    TradeDecision decision {};
    
    // Strongly overbought case
    if(bidDev > 0 && askDev > 0) {
        decision.side = Enums::Side::Sell;
        decision.weight = 0.85;
        decision.strategy = "VWAP Deviation: Strongly Overbought";

        return decision;
    }
    
    // Strongly oversold case
    if(bidDev < 0 && askDev < 0) {
        decision.side = Enums::Side::Buy;
        decision.weight = 0.85;
        decision.strategy = "VWAP Deviation: Strongly Oversold";

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

    decision.stop_loss = mOrderBook.GetCurrentPrice() * .98;
    
    double reward_to_risk = 2.0;  // Target 2x the stop loss distance

    if (decision.side == Enums::Side::Buy) {
        double risk = currPrice - decision.stop_loss;
        decision.exit_price = currPrice + (risk * reward_to_risk);
    } else if (decision.side == Enums::Side::Sell) {
        double risk = decision.stop_loss - currPrice;
        decision.exit_price = currPrice - (risk * reward_to_risk);
    }

    if(decision.weight < .15) {
        decision.side = Enums::Side::None;
        decision.weight = 0.0;
    }
    else {
        decision.strategy = "VWAP Deviation";
    }
    
    return decision;
}


// Calculate ask/bid VWAP, depending on whats passed in
std::pair<double, double> OrderBookAnalysis::CalcVWAP(const auto& orderMap) {
    double priceVolSum {};
    double totalVol {};

    for(const auto& [price, size] : orderMap) {
        priceVolSum += (price * size);
        totalVol += size;
    }

    // prevent div by zero
    if(totalVol == 0.0) {
        return {0, 0};
    }

    return { priceVolSum / totalVol, totalVol };
}