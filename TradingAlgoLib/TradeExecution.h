#pragma once

#include <stdlib.h>

#include "Enums.h"

using TradeDecision = Enums::TradeDecision;
using Position = Enums::Position;

class TradeExecution {
private:
    TradeDecision mCurrTd;

public:
    void ExecuteTrade(TradeDecision td);

    // return open position
    Position GetOpenPosition();

    void CloseAllPositions();

    void ClosePosition(std::string asset_id);

};