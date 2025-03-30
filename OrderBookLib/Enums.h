#pragma once

#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>

class Enums {
public:
    enum class Side {
        Buy,
        Sell,
        None
    };

    enum class Operation {
        Set,
        Delete,
        None
    };

    struct LargestNearbyOrder {
        double bid;
        double ask;
    };

    struct TradeDecision {
        Side side;
        double stop_loss;
        double exit_price;
        double weight;
        std::string strategy;
    };

    struct Position {
        std::string asset_id;
        TradeDecision td;

        // Display Position
        friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
            Side side { pos.td.side };
            double exit_price { pos.td.exit_price };
            double stop_price { pos.td.stop_loss };
            std::string strategy { pos.td.strategy };

            os << "Side: " << (side == Side::Buy ? "Buy" : "Sell") << std::endl;
            os << "Exiting At: " << exit_price << std::endl;
            os << "Stop Loss Set At: " << stop_price << std::endl;
            os << "Strategy: " << strategy << std::endl;

            return os;
        }
    };

    using Analysis = std::tuple<TradeDecision, TradeDecision, TradeDecision>;
};