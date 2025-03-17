#include <stdlib.h>
#include <OrderBook.h>

class TradingAlgo {
private:
    OrderBook& mOrderBook;
public:
    TradingAlgo(OrderBook& orderBook) : mOrderBook(orderBook) {}
    void StartTrading();
};