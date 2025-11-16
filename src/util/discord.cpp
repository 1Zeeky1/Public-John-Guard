#include "discord.h"
#include <curl/curl.h>
#include <json.hpp>
#include <iostream>
#include <fstream>

bool sendWebhook(const std::string& url, const std::string& jsonPayload) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

    curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");

    CURLcode res = curl_easy_perform(curl);
    bool success = (res == CURLE_OK);

    if (!success)
        std::cerr << "Webhook error: " << curl_easy_strerror(res) << std::endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}