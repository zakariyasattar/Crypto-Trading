/**
 * @file DataIngestion.cpp
 * @author Zakariya Sattar
 */

#include "Order.h"
#include "DataIngestion.h"

#include <iostream>
#include <vector>
#include <stdlib.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;

void DataIngestion::populate(std::vector<Order>& bids, std::vector<Order>& asks) {
    cout << bids.size() << endl;
}