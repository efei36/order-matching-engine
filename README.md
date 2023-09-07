# Order matching engine

## Project description
This program simulates a functional order matching engine for a financial exchange. The order matching engine is responsible for algorithmically matching buy and sell orders for a particular financial instrument in an efficient and unbiased manner. Working implementations of two of the most popular order matching algorithms are provided: the Price/Time (FIFO) algorithm, and the Pro-Rata algorithm. The user can choose which algorithm to use.

Buy orders are sorted with priority to maximum price, then earliest time. Sell orders are sorted with priority to minimum price, then earliest time. Buy orders and sell orders can be matched if the price of the buy order is greater than or equal to that of the sell order.

Price/Time (FIFO) algorithm:
- Matches the top buy order with the top sell order, removes filled orders after completion. Repeats until either there are no more buy/sell orders, or the top buy order cannot be matched with the top sell order due to incompatible prices.

Pro-Rata algorithm:
- Matches the top buy order with all sell orders at the minimum sell price level. Sell orders are filled based on the proportion they make up of the total amount of sell orders at their price level. Repeats until there are either no more buy/sell orders, or the top buy order cannot fill any sell orders due to incompatible prices.

## Status
This program has been run and tested with the following. You will need these to emulate the development environment:
```
C++17 and newer
Python 3.10.11 and newer
g++ 12.2.0 and newer
If using an IDE, preferably VSCode version 1.81.1 and newer
Windows 11
```

## Instructions
- Download all header and source files into a folder of your choice, e.g., `<order-matching-folder>`.
- Compile the source files using a C++ compiler.
    - To access debugging statements, pass the additional compiler flag `-DDO_DEBUG` to the compiler.
- Create/download a CSV file containing all the orders you want to process
    - CSV files should have the following columns: Ticker, ID, IsMarket, IsBuy, Price, Time, Amount
        - Ticker: ticker symbol representing the traded financial instrument
        - ID:        integer representing the order ID
        - IsMarket:  `true` if the order is a market order, `false` if the order is a limit order
        - IsBuy:     `true` if the order is a buy order, `false` if the order is a sell order
        - Price:     float with a maximum of two decimal places representing the price at which the order should be filled
        - Time:      integer representing the time the order was place in military time
        - Amount:    integer representing the amount of the order to be filled
    - To see an example of how the CSV should be formatted, look at `sampleOrders.csv`
- Run the resulting EXE file with the appropriate arguments:
    - 3 necessary arguments:
        - CSV file containing order data: `path\to\<your-csv>.csv` 
        - Ticker symbol of the financial instrument
        - Type of matching algorithm (1: FIFO or 2: Pro-Rata)
    - Example: to process the orders in `sampleOrders.csv` with the Pro-Rata algorithm, run the following from the command line:<br />
        `order-matching-folder> ./<your-executable>.exe "sampleOrders.csv" "AAPL" "2"`