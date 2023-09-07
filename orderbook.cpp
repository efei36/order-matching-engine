/*orderbook.cpp*/

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

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>

#include "orderbook.h"
#include "logger.h"

/**--------------------------------------------------------------------------------------
 * Constructor
 * 
 * Creates an order book
 * 
 * @param[in] ticker    Ticker used to identify a which financial insturment the order book
 *                      is tracking
 * --------------------------------------------------------------------------------------
*/
Orderbook::Orderbook(std::string ticker)
  : m_ticker(ticker)
{
    // Reserving extra space for the underlying containers of the priority_queuess beforehand, thereby saving time on resizing the vectors
    std::vector<Order> buyContainer;
    buyContainer.reserve(2048);
    m_buyOrders = new std::priority_queue<Order, std::vector<Order>, prioritizeMax> (prioritizeMax(), std::move(buyContainer));

    std::vector<Order> sellContainer;
    sellContainer.reserve(2048);
    m_sellOrders = new std::priority_queue<Order, std::vector<Order>, prioritizeMin> (prioritizeMin(), std::move(sellContainer));
}

/**--------------------------------------------------------------------------------------
 * Destructor
 * 
 * Destroys m_buyOrders and m_sellOrders
 * --------------------------------------------------------------------------------------
*/
Orderbook::~Orderbook()
{
    delete m_buyOrders;
    delete m_sellOrders;
}

/**--------------------------------------------------------------------------------------
 * addOrder()
 * 
 * Adds a new order to the order book, either on the buy or sell side
 * 
 * @param[in] newOrder  new order to be added
 * --------------------------------------------------------------------------------------
*/
void Orderbook::addOrder(Order newOrder)
{
    if(newOrder.checkIsBuy())
    {
        (*m_buyOrders).push(newOrder);
    }
    else
    {
        (*m_sellOrders).push(newOrder);
    }
}

/**--------------------------------------------------------------------------------------
 * matchOrdersFIFO()
 * 
 * Matches a buy and sell order at the top of their respective piles if the buy price is
 * greater than or equal to the sell price
 * --------------------------------------------------------------------------------------
*/
void Orderbook::matchOrdersFIFO()
{
    std::cout << "Initiating FIFO order-matching" << std::endl;

    while(!(*m_buyOrders).empty() && !(*m_sellOrders).empty())
    {
        Order bestBuy = (*m_buyOrders).top();
        Order bestSell = (*m_sellOrders).top();

        (*m_buyOrders).pop();
        (*m_sellOrders).pop();

        if(bestBuy.getPrice() >= bestSell.getPrice())   // Buy and sell price levels are compatible
        {
            int buyAmount = bestBuy.getAmount();
            int sellAmount = bestSell.getAmount();
            int amountFilled = 0;

            if(buyAmount > sellAmount)      // Sell order should be completely filled
            {
                bestBuy.setAmount(buyAmount - sellAmount);

                amountFilled = sellAmount;
                (*m_buyOrders).push(bestBuy); // Re-adding partially filled buy order to maxheap of buy orders
            }
            else if(buyAmount < sellAmount) // Buy order should be completely filled
            {
                bestSell.setAmount(sellAmount - buyAmount);

                amountFilled = buyAmount;
                (*m_sellOrders).push(bestSell); // Re-adding partially filled sell order to minheap of sell orders
            }
            else                            // Both orders should be completely filled
            {
                amountFilled = buyAmount;
            }

            m_orderHistory.emplace(bestBuy.getID(), bestSell.getID(), amountFilled); // Updating order book history
        }
        else
        {
            (*m_buyOrders).push(bestBuy);
            (*m_sellOrders).push(bestSell);

            LOG_DEBUG("NOTE - matchOrdersFIFO(): Best buy price does not fulfill best sell price, waiting for new orders");
            break;
        }
    }
}

/**--------------------------------------------------------------------------------------
 * matchOrdersProRata()
 * 
 * Matches the best buy order to all sell orders at all matching price levels, split by the
 * proportion each sell order comprises of the total amount of sell orders at the exact
 * same price level
 * --------------------------------------------------------------------------------------
*/
void Orderbook::matchOrdersProRata()
{
    std::cout << "Initiating Pro-Rata order-matching" << std::endl;

    while(!(*m_buyOrders).empty() && !(*m_sellOrders).empty())
    {
        if((*m_buyOrders).top().getPrice() < (*m_sellOrders).top().getPrice())
        {
            LOG_DEBUG("NOTE - matchOrdersProRata(): Best buy price does not fulfill best sell price, waiting for new orders");
            break;
        }

        Order bestBuy = (*m_buyOrders).top();
        int buyAmount = bestBuy.getAmount();
        (*m_buyOrders).pop();

        std::vector<Order> matchingOrders;

        // Finding all sell orders at a matching price level with the best buy order
        while(bestBuy.getPrice() >= (*m_sellOrders).top().getPrice())
        {
            // NOTE: This is creating two copies of the same Order object (push_back() creates a copy of the object it is, passed, which it then adds to the vector)
            //       See if this can be improved
            Order bestSell = (*m_sellOrders).top();
            matchingOrders.push_back(bestSell);
            (*m_sellOrders).pop();

            if((*m_sellOrders).empty())
            {
                LOG_DEBUG("NOTE - matchOrdersProRata(): All remaining sell orders in the orderbook are being filled");
                break;
            }
        }
        
        // Partially filling all sell orders at each successive price level, until there are either no more buy orders remaining or no more sell orders to fill
        int curIndex = 0;
        int indexNextPriceLevel = 0;
        const int numMatchingSells = matchingOrders.size();
        bool buyOrdersStillRemaining = true;
        while(curIndex < numMatchingSells && buyOrdersStillRemaining)
        {
            // Finding the index of the earliest sell order at the next price level
            for(int i = curIndex; i < numMatchingSells; i++)
            {
                indexNextPriceLevel++;

                if(matchingOrders[indexNextPriceLevel].getPrice() > matchingOrders[curIndex].getPrice())
                {
                    break;
                }
            }

            // Finding the total amount of sell orders at the current price level
            int curTotalSellAmount = 0;
            for(int i = curIndex; i < indexNextPriceLevel; i++)
            {
                curTotalSellAmount += matchingOrders[i].getAmount();
            }

            LOG_DEBUG("matchingOrdersProRata: Current price level of sell orders: " << matchingOrders[curIndex].getPrice() \
                      << ", Total number of sell orders at current price level: " << curTotalSellAmount);

            int buyAmountBeforeFillingCurPriceLevel = buyAmount;
            // Partially filling each sell order at the current price level according to the proportion they make up of the current price level
            for(int i = curIndex; i < indexNextPriceLevel; i++)
            {    
                if(i >= numMatchingSells)
                {
                    std::cerr << "ERROR - matchOrdersProRata: index" << i << "out of bounds (total matching sell orders: " << numMatchingSells \
                              << ") while filling sell orders" << std::endl;
                    buyOrdersStillRemaining = false; // Doing this to exit the current while loop
                    break;
                }
                
                int sellAmount = matchingOrders[i].getAmount();
                float proportion = (float)sellAmount / (float)curTotalSellAmount;
                int amountFilled = std::min(std::min(buyAmount, sellAmount), (int)std::ceil(buyAmountBeforeFillingCurPriceLevel * proportion));

                buyAmount -= amountFilled;
                sellAmount -= amountFilled;
                matchingOrders[i].setAmount(sellAmount);

                LOG_DEBUG("    matchingOrdersProRata - processed order:    Amount filled: " << amountFilled \
                          << "\n                                                Proportion of total sell orders at current price level: " << proportion \
                          << "\n                                                Seller: ID: " << matchingOrders[i].getID() << ", Amount remaining: " << sellAmount \
                          << "\n                                                Buyer: ID: " << bestBuy.getID() << ", Amount remaining: " << buyAmount)
                m_orderHistory.emplace(ProcessedOrder(bestBuy.getID(), matchingOrders[i].getID(), amountFilled)); // Updating order book history

                if(buyAmount == 0)
                {
                    buyOrdersStillRemaining = false;
                    break;
                }
            }

            // Move on to the next price level
            curIndex = indexNextPriceLevel;
        }
        
        // Re-adding any partially filled sell orders to the minheap of sell orders
        for(Order& matchingSell : matchingOrders)
        {
            if(matchingSell.getAmount() > 0)
            {
                (*m_sellOrders).push(matchingSell);
            }
        }

        // Re-adding the best buy order to the minheap of buy orders if it is not completely filled
        if(buyAmount > 0)
        {
            bestBuy.setAmount(buyAmount);
            (*m_buyOrders).push(bestBuy);
        }
    }
}

/**--------------------------------------------------------------------------------------
 * printOrderHistory()
 * 
 * Prints the order history, from oldest processed fill to newest
 * --------------------------------------------------------------------------------------
*/
void Orderbook::printOrderHistory()
{
    while(!m_orderHistory.empty())
    {
        ProcessedOrder& curProcessed = m_orderHistory.front();
        std::cout << "    ORDER PROCESSED:   Buyer ID: " << curProcessed.buyID << ",   Amount filled: " << curProcessed.fillAmount \
                  << ",   Seller ID: " << curProcessed.sellID << std::endl;

        m_orderHistory.pop();
    }
}

/**--------------------------------------------------------------------------------------
 * printOrderbookContents()
 * 
 * Prints the current buy and sell orders inside the order book. 
 *     Sell orders are arranged in descending order, based on price, then time. 
 *     Buy orders are arranged in ascending order, based on price, then time.
 * --------------------------------------------------------------------------------------
*/
void Orderbook::printOrderbookContents()
{
    std::stack<Order> sellOrdersAscending;

    std::cout << "    Id   Side    Time   Qty   Price   Qty    Time   Side\n    ---+------+-------+-----+-------+-----+-------+------" << std::endl;

    // Arranging sell orders into descending order, based on price, then time
    while(!(*m_sellOrders).empty())
    {
        sellOrdersAscending.push((*m_sellOrders).top());
        (*m_sellOrders).pop();
    }

    // Printing sell orders remaining in the order book
    while(!sellOrdersAscending.empty())
    {
        const Order& curSell = sellOrdersAscending.top();
        int intTime = curSell.getTime();
        std::string stringTime = "";

        stringTime = std::to_string((intTime / 1000) % 10) + std::to_string((intTime / 100) % 10) + ":" + std::to_string((intTime / 10) % 10) + std::to_string(intTime % 10);

        std::cout << "    #" << curSell.getID() << "                        " << std::fixed << std::setprecision(2) << curSell.getPrice() << "   " \
                  << curSell.getAmount() << "   " << stringTime << "   SELL" << std::endl;

        sellOrdersAscending.pop();
    }

    // Printing buy orders remaining in the order book
    while(!(*m_buyOrders).empty())
    {
        const Order& curBuy = (*m_buyOrders).top();
        int intTime = curBuy.getTime();
        std::string stringTime = "";

        stringTime = std::to_string((intTime / 1000) % 10) + std::to_string((intTime / 100) % 10) + ":" + std::to_string((intTime / 10) % 10) + std::to_string(intTime % 10);

        std::cout << "    #" << curBuy.getID() << "   BUY    " << stringTime << "   " << curBuy.getAmount() << "   " << std::fixed << std::setprecision(2) \
                  << curBuy.getPrice() << std::endl;

        (*m_buyOrders).pop();
    }
}