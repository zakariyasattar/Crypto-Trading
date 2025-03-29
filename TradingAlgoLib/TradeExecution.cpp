
#include "TradeExecution.h"
#include <string>
#include <Enums.h>
#include <sstream>

#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <../include/json.hpp>

using json = nlohmann::json;

using namespace std;

using Position = Enums::Position;
using Side = Enums::Side;

const string alpaca_api_key { "PKWTH6N3SM1XALKB870E" };
const string alpaca_api_secret { "sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK" };
// on construction, build hashmap

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);

    return totalSize;
}

void TradeExecution::ExecuteTrade(TradeDecision orderDetails) {
    CURL *hnd = curl_easy_init();

    string responseString;

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://paper-api.alpaca.markets/v2/orders");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "content-type: application/json");
    headers = curl_slist_append(headers, "APCA-API-KEY-ID: PKWTH6N3SM1XALKB870E");
    headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    string side { orderDetails.side == Side::Buy ? "buy" : "sell" };
    mCurrTd = orderDetails;

    std::ostringstream oss;
    oss << R"({"type":"market","time_in_force":"gtc","symbol":"BTC/USD","notional":"50000","side":")"
        << side << R"("})";

    std::string postFields = oss.str();
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postFields.c_str());

    CURLcode ret = curl_easy_perform(hnd);
}

Position TradeExecution::GetOpenPosition() {
    CURL *hnd = curl_easy_init();

    string responseString {};

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseString);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://paper-api.alpaca.markets/v2/positions");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "APCA-API-KEY-ID: PKWTH6N3SM1XALKB870E");
    headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);

    json response { json::parse(responseString) };

    int openPositions { static_cast<int>(response.size()) };
    if(openPositions == 0) return {};

    string assetID { response[0]["asset_id"] };

    // Guaranteed open position so mCurrTd is guaranteed to be filled
    Position res {assetID, mCurrTd };

    return res;
}

void TradeExecution::ClosePosition(string asset_id) {
    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, stdout);

    std::ostringstream oss;
    oss << "https://paper-api.alpaca.markets/v2/positions/";
    oss << asset_id;

    curl_easy_setopt(hnd, CURLOPT_URL, oss.str().c_str());

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "APCA-API-KEY-ID: PKWTH6N3SM1XALKB870E");
    headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);

    mCurrTd = {};
}

void TradeExecution::CloseAllPositions() {
    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, stdout);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://paper-api.alpaca.markets/v2/positions");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "APCA-API-KEY-ID: PKWTH6N3SM1XALKB870E");
    headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: sS8KmCnbdYKcT6y6wZzJ5hhjgTx8svbSM8gTFfuK");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);
}