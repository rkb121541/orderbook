# Limit Order Book

This is my implementation of a limit order book for a single product  

- Only limit orders are supported (not market orders)

- Trades execute only if certain conditions are met:
  - Buy orders execute if the price $\ge$ lowest sell price/ limit price
  - Sell orders execute if the price $\le$ highest buy price/ limit price

- Only supports single product trade executions; have not looked into implementing/ handling multiple different products. This might be something I can do in the future.

- Orders are matched using price-time priority
  - So, it is ordered by price first, and then time (order in which they were placed)

- The order book displays the aggregated quantities at each price level. What this means is that it groups the quantities together and prints it out like that when you call `printOrderBook()`
  - Ex:
    ```c++
    BUY ORDERS:
    Price: 99 | Quantity: 20 | Orders: [B1, B2]
    SELL ORDERS:
    Price: 100 | Quantity: 5 | Orders: [S3]
    Price: 101 | Quantity: 15 | Orders: [S2]
    ```

- It also prints trades immediately as they happen. This is handled/ done by `addOrder()`. Might have to work on moving this functionality to a separate function.

- Right now my implementation is single-threaded ([single-threaded directory](./single-threaded/))

**TODO:**
- Concurrency with threads (implement it in [multi-threaded directory](./multi-threaded/))
