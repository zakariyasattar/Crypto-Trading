#pragma once

#include <iostream>
#include <stdlib.h>

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
};