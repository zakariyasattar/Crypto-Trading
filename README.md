# Order Book Trading Engine

This project is a multi-threaded, lock-free trading engine that pulls live Level II data (from Coinbase) and makes real-time trading decisions based on the order book. It's built from the ground up in C++, with low-level memory management and concurrency in mind.

### Why this project?

I wanted to build a real trading engine—not just a simulator. Something that could realistically ingest live data from a crypto exchange, build and maintain an order book in real time, and then make trading decisions based on actual market structure, not just lagging indicators. This also gave me an excuse to dive into topics like hazard pointers, lock-free queues, and Level II market data.

---

## Architecture

**Data Ingestion**  
Uses WebSocket connections to fetch live Level II order book data from CoinAPI (BTC/USD specifically). It parses JSON payloads and updates the internal book with millisecond latency.

**Order Book**  
Built around two std::map(s) (bids and asks) and kept thread-safe through an internal lock-free queue (see below). Every price level and order size is preserved, so we can do proper book analysis.

**Lock-Free Queue**  
This is the backbone of the pipeline. New orders are pushed to the queue by the WebSocket thread and popped off by the main processing thread. It uses atomic pointers and hazard pointers to avoid data races and safely reclaim memory without needing locks or mutexes.

**Hazard Pointers**  
For safe memory reclamation in concurrent environments. Retired nodes are only deleted when we’re sure no other thread is using them.

**OrderBookAnalysis**  
Three strategies run in parallel:
1. **VWAP Deviation** – Is the current price stretched too far from fair value?
2. **Order Imbalance** – Are buyers or sellers outweighing each other?
3. **Large Orders Nearby** – Are there any whales sitting close to the spread?

Each strategy returns a `TradeDecision` struct with a recommended side, stop loss, exit price, and confidence weight.

---

## Flow

1. WebSocket connection is established.
2. Incoming orders are parsed and pushed to a lock-free queue.
3. Queue processor thread pops orders and updates the order book.
4. Once enough data is present, the trading engine runs analysis and decides whether to buy, sell, or hold.

---

## Example Output

Here's a snapshot of what a trade decision looks like, along with a live view of the order book:

![Order Book Output](https://github.com/user-attachments/assets/285c25be-4d51-4a03-ab9b-e1badc1d25e0)

---

## Dependencies

- `websocketpp` for WebSocket communication  
- `nlohmann/json` for JSON parsing  
- `OpenSSL` for TLS  
- `curlpp` for REST API calls (not heavily used right now)  
- C++17  

---

## Run it

Just run the provided shell script:

```bash
./run.sh
```

That will build and launch the engine. You should see the order book populate in real time and get trade decisions printed to the terminal.

---

## Things to improve

- Proper stop-loss enforcement and position tracking  
- Multi-asset support  
- Add backtest mode with historical data  
