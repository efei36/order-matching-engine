/*orderbook.h*/

/**
 * Author: Eric Fei
 * Version 0.0.1
 * 
 * Defines the Orderbook class
 *     Contains a register of all currently unfilled buy and sell orders for a given financial instrument
 *     Buy and sell orders are organized by price level and time
*/

/**
 * MIT License
 * Copyright (c) 2023 Eric Fei
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#pragma once

#include <string>
#include <queue>

#include "order.h"

/**--------------------------------------------------------------------------------------
 * ProcessedOrder struct
 * 
 * Records information about a processed pair of buy and sell orders
 * --------------------------------------------------------------------------------------
*/
typedef struct ProcessedOrder
{
    int buyID;
    int sellID;
    int fillAmount;

    ProcessedOrder(int buyNum, int sellNum, int fillAmt)
        : buyID(buyNum), sellID(sellNum), fillAmount(fillAmt)
    {} 
} ProcessedOrder;



/**--------------------------------------------------------------------------------------
 * Orderbook class
 * 
 * Contains all currently unfilled buy and sell orders for a given financial instrument
 * --------------------------------------------------------------------------------------
*/
class Orderbook 
{
public:
    /**--------------------------------------------------------------------------------------
     * Constructor
     * 
     * Creates an order book
     * 
     * @param[in] ticker    Ticker used to identify a which financial insturment the order book
     *                      is tracking
     * --------------------------------------------------------------------------------------
    */
    Orderbook(std::string ticker);

    /**--------------------------------------------------------------------------------------
     * Destructor
     * 
     * Destroys m_buyOrders and m_sellOrders
     * --------------------------------------------------------------------------------------
    */
    ~Orderbook();

    /**--------------------------------------------------------------------------------------
     * addOrder()
     * 
     * Adds a new order to the order book, either on the buy or sell side
     * 
     * @param[in] newOrder  new order to be added
     * --------------------------------------------------------------------------------------
    */
    void addOrder(Order newOrder);

    /**--------------------------------------------------------------------------------------
     * matchOrdersFIFO()
     * 
     * Matches a buy and sell order at the top of their respective piles if the buy price is
     * greater than or equal to the sell price
     * --------------------------------------------------------------------------------------
    */
    void matchOrdersFIFO();

    /**--------------------------------------------------------------------------------------
     * matchOrdersProRata()
     * 
     * Matches the best buy order to all sell orders at all matching price levels, split by the
     * proportion each sell order comprises of the total amount of sell orders at the exact
     * same price level
     * --------------------------------------------------------------------------------------
    */
    void matchOrdersProRata();

    /**--------------------------------------------------------------------------------------
     * printOrderHistory()
     * 
     * Prints the order history, from oldest processed fill to newest
     * --------------------------------------------------------------------------------------
    */
    void printOrderHistory();

    /**--------------------------------------------------------------------------------------
     * printOrderbookContents()
     * 
     * Prints the current buy and sell orders inside the order book. 
     *     Sell orders are arranged in descending order, based on price, then time. 
     *     Buy orders are arranged in ascending order, based on price, then time.
     * --------------------------------------------------------------------------------------
    */
    void printOrderbookContents();

private:
    /**--------------------------------------------------------------------------------------
     * prioritizeMax
     * 
     * Functor used to sort sell orders in the Maxheap according to price(greatest on top), 
     * then time(least on top)
     * --------------------------------------------------------------------------------------
    */
    class prioritizeMax
    {
    public:
        bool operator()(const Order& order1, const Order& order2) const
        {
            if(order1.getPrice() < order2.getPrice())
            {
                return true;
            }
            else if(order1.getPrice() > order2.getPrice())
            {
                return false;
            }
            else
            {
                return order1.getTime() > order2.getTime();
            }
        }
    };

    /**--------------------------------------------------------------------------------------
     * prioritizeMin
     * 
     * Functor used to sort sell orders in the minheap according to price (least on top), 
     * then time(least on top)
     * --------------------------------------------------------------------------------------
    */
    class prioritizeMin
    {
    public:
        bool operator()(const Order& order1, const Order& order2) const
        {
            if(order1.getPrice() > order2.getPrice())
            {
                return true;
            }
            else if(order1.getPrice() < order2.getPrice())
            {
                return false;
            }
            else
            {
                return order1.getTime() > order2.getTime();
            }
        }
    };

    std::string m_ticker = "";

    // NOTE: Memory pointed to by m_buyOrders and m_sellOrders is released in the destructor
    //       This can also be replaced by smart pointers
    std::priority_queue<Order, std::vector<Order>, prioritizeMax>* m_buyOrders;    // Maxheap (binheap) for buy orders
    std::priority_queue<Order, std::vector<Order>, prioritizeMin>* m_sellOrders;   // Minheap (binheap) for sell orders
    std::queue<ProcessedOrder> m_orderHistory;  // Contains history of all filled buy and sell orders
};