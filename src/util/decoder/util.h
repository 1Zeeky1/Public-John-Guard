#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "json.hpp"

using json = nlohmann::json;

const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Decoder(const std::string &in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
        T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

json parseJwtPayload(const std::string& jwt) {
    size_t dot1 = jwt.find('.');
    size_t dot2 = jwt.find('.', dot1 + 1);
    std::string payloadB64 = jwt.substr(dot1 + 1, dot2 - dot1 - 1);
    std::string payloadJson = base64Decoder(payloadB64);
    
    return json::parse(payloadJson);
}