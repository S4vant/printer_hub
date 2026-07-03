#include "HttpSender.h"

#include <curl/curl.h>

bool HttpSender::send(
    const std::string& url,
    const nlohmann::json& json)
{
    CURL* curl = curl_easy_init();

    if (!curl)
        return false;

    std::string payload =
        json.dump();

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json");

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url.c_str());

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers);

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        payload.c_str());

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDSIZE,
        payload.size());

    CURLcode result =
        curl_easy_perform(curl);

    long responseCode = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &responseCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result == CURLE_OK
        && responseCode >= 200
        && responseCode < 300;
}