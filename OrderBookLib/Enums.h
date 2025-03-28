#pragma once

#include <iostream>
#include <stdlib.h>
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

        friend std::ostream& operator<<(std::ostream& os, const TradeDecision& td) {
            std::string str { td.side == Side::Buy ? "Buy" : "Sell" };
            os << str << " " << td.exit_price << " " << td.stop_loss << " " << td.weight;
            return os;
        }
    };

    using Analysis = std::tuple<TradeDecision, TradeDecision, TradeDecision>;
};