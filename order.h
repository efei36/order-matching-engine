/*order.h*/

/**
 * Author: Eric Fei
 * Version 0.0.1
 * 
 * Defines the Order class
 *     Either a market order or limit order, either buy or sell, for a specific financial instrument
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

class Order
{
public:
    /**--------------------------------------------------------------------------------------
     * Constructor
     * 
     * Creates an order
     * 
     * @param[in] ticker    Ticker used to identify a which financial insturment the order is for
     * @param[in] isMarket  True if it is a market order, false if it is a limit order
     * @param[in] isBuy     True if it is a buy order, false if it is a sell order
     * @param[in] price     Price at which the order is intended to be filled
     * @param[in] time      Time (hr:min) at which the order is made.
     *                      NOTE: the time is stored as an int according to military time
     *                            i.e. 12 AM would be 0, 7:45 AM would be 745, and 10:20 PM
     *                            would be 2220
     * @param[in] amount    Amount the order intended to be filled
     * --------------------------------------------------------------------------------------
    */
    Order(std::string ticker, long long orderID, bool isMarket, bool isBuy, float price, int time, int amount);

    /**--------------------------------------------------------------------------------------
     * checkIsBuy()
     * 
     * Checks if the order is a buy order or sell order
     * 
     * @return true if the order is a buy order, false if it is a sell order
     * --------------------------------------------------------------------------------------
    */
    bool checkIsBuy() const
    {
        return m_isBuy;
    }

    /**--------------------------------------------------------------------------------------
     * getID()
     * 
     * Returns the ID of the order
     * 
     * @return an int representing the ID of the order
     * --------------------------------------------------------------------------------------
    */
    int getID() const
    {
        return m_orderID;
    }

    /**--------------------------------------------------------------------------------------
     * getPrice()
     * 
     * Returns the price at which the order is intended to be filled
     * 
     * @return an float representing the price at which the order is intended to be filled
     * --------------------------------------------------------------------------------------
    */
    float getPrice() const
    {
        return m_price;
    }

    /**--------------------------------------------------------------------------------------
     * getTime()
     * 
     * Returns the time at which the order was made
     * 
     * @return an int representing the time at which the order was made
     * --------------------------------------------------------------------------------------
    */
    int getTime() const
    {
        return m_time;
    }

    /**--------------------------------------------------------------------------------------
     * getAmount()
     * 
     * Returns the amount of the order waiting to be filled
     * 
     * @return an int representing the amount of the order waiting to be filled
     * --------------------------------------------------------------------------------------
    */
    int getAmount() const
    {
        return m_amount;
    }

    /**--------------------------------------------------------------------------------------
     * setAmount()
     * 
     * Changes the amount of the order waiting to be filled
     * 
     * @param[in] newAmount An int representing the new amount of the order waiting to be filled
     * --------------------------------------------------------------------------------------
    */
    void setAmount(int newAmount)
    {
        m_amount = newAmount;
    }

private:
    std::string m_ticker;
    unsigned long long m_orderID;
    bool m_isMarket;
    bool m_isBuy;
    float m_price;
    int m_time; // int formatted along military time (0 -> 2359)
    int m_amount;
};