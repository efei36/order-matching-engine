/*main.cpp*/

/**
 * Author: Eric Fei
 * Version 0.0.1
 * 
 * Simulates a working order-matching-engine
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
#include <fstream>
#include <sstream>

#include "orderbook.h"
#include "logger.h"

namespace { 
    const int FIFOCHOICE = 1;
    const int PRORATACHOICE = 2;
}

/**--------------------------------------------------------------------------------------
 * readOrderData()
 * 
 * Write the completed and solved maze data to a csv file
 * 
 * @param[in]       infile          CSV file to be read in, filled with unprocessed orders
 * @param[in,out]   blankOrderbook  Orderbook object that is empty, to be filled with the
 *                                  orders from infile
 * --------------------------------------------------------------------------------------
*/
void readOrderData(std::fstream& infile, Orderbook& blankOrderbook)
{
    std::string line;

    if(infile.is_open())
    {
        std::string ticker = "";
        std::string orderID = "";
        std::string isMarket = "";
        std::string isBuy = "";
        std::string price = "";
        std::string time = "";
        std::string amount = "";

        // Skipping first line of CSV (contains column headers)
        std::getline(infile, line);

        while(std::getline(infile, line))
        {
            std::istringstream curString(line);

            std::getline(curString, ticker, ',');
            std::getline(curString, orderID, ',');
            std::getline(curString, isMarket, ',');
            std::getline(curString, isBuy, ',');
            std::getline(curString, price, ',');
            std::getline(curString, time, ',');
            std::getline(curString, amount, ',');

            bool boolIsMarket = (isMarket == "true");
            bool boolIsBuy = (isBuy == "true");

            blankOrderbook.addOrder(Order(ticker, std::stoll(orderID), boolIsMarket, boolIsBuy, std::stof(price), std::stoi(time), std::stoi(amount)));
        }
    }
    else
    {
        std::cerr << "ERROR: Could not open the given file" << std::endl;
    }
}

/**--------------------------------------------------------------------------------------
 * printUsage()
 * 
 * Checks if the arguments is passed to main() are correct/usable
 * 
 * @param[in] argc Number of arguments passed
 * @param[in] argv String vector of arguments passed
 * @return true if the overall program should be terminated
 * --------------------------------------------------------------------------------------
*/
bool printUsage(int argc, const char** argv)
{
    bool shouldTerminate = false;

    if(argc != 4) // Should be four arguments: 1: name of program, 2: name of input csv file, 3: name of ticker, 4: type of matching algorithm (1: FIFO or 2: PRORATA)
    {
        std::cerr << "ERROR: Incorrect number of arguments passed to main(), need in following order: #1 Name of CSV File\n" \
                  << "                                                                                #2 Name of ticker\n" \
                  << "                                                                                #3 Choice of matching algorithm (1 for FIFO, 2 for Pro-Rata)\n" << std::endl;
        shouldTerminate = true;
    }
    else
    {
        int choice = atoi(argv[3]);

        if(choice != FIFOCHOICE && choice != PRORATACHOICE)
        {
            std::cerr << "ERROR: Invalid choice of algorithm, please pick from the following (FIFO: 1, Pro-Rata: 2)" << std::endl;
            shouldTerminate = true;
        }
    }

    return shouldTerminate;
}

int main(int argc, const char** argv)
{
    if(printUsage(argc, argv))
    {
        return -1;
    }

    Orderbook myOrderbook(argv[2]);

    std::fstream csvData(argv[1], std::fstream::in);
    readOrderData(csvData, myOrderbook);
    csvData.close();

    int choice = atoi(argv[3]);
    if(FIFOCHOICE == choice)  // User chose to use FIFO algorithm for order-matching
    {
        myOrderbook.matchOrdersFIFO();
    }
    else             // User chose to use Pro-Rata algorithm for order-matching
    {
        myOrderbook.matchOrdersProRata();
    }

    // Printing all processed orders
    myOrderbook.printOrderHistory();

    std::cout << "\nDisplaying remaining contents of the order book:" << std::endl;

    // Printing remaining contents of order book
    myOrderbook.printOrderbookContents();

    std::cout << "\nProgram finished" <<std::endl;

    return 0;
}