#include "Ask.h"
#include <sstream>

using namespace std;

void Ask::print(ostream& os) const {
    os << "Ask: " << shares << " " << price;
}