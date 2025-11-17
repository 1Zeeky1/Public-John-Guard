#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "json.hpp"

using json = nlohmann::json;

const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

template<typename T>
struct VerifyResult {
    bool success;
    std::string error;
    T value;
};

template<typename T>
VerifyResult<T> verifyJsonKey(const json& obj, const std::string& key) {
    VerifyResult<T> result;

    if (!obj.contains(key)) {
        result.success = false;
        result.error = "Key doesn't exist";
        return result;
    }

    try {
        result.value = obj.at(key).get<T>();
        result.success = true;
        return result;
    } 
    catch (...) {
        result.success = false;
        result.error = "Wrong data type";
        return result;
    }
}

inline std::string base64Decoder(const std::string &in) {
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

inline json parseJwtPayload(const std::string& jwt) {
    size_t dot1 = jwt.find('.');
    size_t dot2 = jwt.find('.', dot1 + 1);
    std::string payloadB64 = jwt.substr(dot1 + 1, dot2 - dot1 - 1);
    std::string payloadJson = base64Decoder(payloadB64);
    
    return json::parse(payloadJson);
}