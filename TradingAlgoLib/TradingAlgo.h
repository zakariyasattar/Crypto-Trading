#include <stdlib.h>
#include <OrderBook.h>
#include <TradeExecution.h>

class TradingAlgo {
private:
    OrderBook& mOrderBook;
    TradeExecution mExecutionEngine;
    Position mOpenPosition;

public:
    TradingAlgo(OrderBook& orderBook) : mOrderBook(orderBook) {}
    void StartTrading();

    ~TradingAlgo();
};