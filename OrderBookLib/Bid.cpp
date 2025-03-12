#include "Bid.h"
#include <sstream>

using namespace std;

void Bid::print(ostream& os) const {
    os << "Bid: " << shares << " " << price;
}