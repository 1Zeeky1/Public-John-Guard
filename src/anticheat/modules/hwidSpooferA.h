#pragma once

#include <regex>
#include <fmt/core.h>

#include "util/packets/login.h"
#include "../check.h"

class HwidSpooferA : public AntiCheatCheck {
public:
    std::string getName() const override { return "hwidSpoofer/A"; }
    std::string getDescription() const override { return "Checks for an invalid HWID against their device."; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        int deviceOS = std::stoi(ctx.deviceOS);
        auto& validators = getPlatformValidators();
        auto it = validators.find(deviceOS);
        if (it != validators.end()) {
            bool isValid = std::regex_match(ctx.deviceId, it->second);
            if (!isValid) {
                // players on unsupports devices can download android minecraft on other devices
                if (deviceOS == DeviceOS::Android) return { CheckStatus::Pass, "" };

                if (deviceOS == DeviceOS::Windows) { 
                    CheckResult res{ CheckStatus::Fail, "Please turn off your hack client!" };
                    res.extraData["type"] = "Horion"; 
                    return res;
                } else {
                    CheckResult res{ CheckStatus::Fail, "Please join our discord for help." };
                    res.extraData["type"] = "device mismatch"; 
                    return res;
                }
            }
        } else {
            std::cout << "Unhandled deviceOS (" << deviceOS << ") for " << ctx.deviceId << "\n";
        }

        return { CheckStatus::Pass, "" };
    }

    const std::unordered_map<int, std::regex>& getPlatformValidators() {
        static const std::unordered_map<int, std::regex> validators = {
            {DeviceOS::PlayStation,     std::regex(R"(^[0-9a-f]{8}-[0-9a-f]{4}-3[0-9a-f]{3}-[0-9a-f]{4}-[0-9a-f]{12}$)", std::regex::icase)},
            {DeviceOS::IOS,             std::regex(R"(^[0-9A-F]{32}$)", std::regex::icase)},
            // {DeviceOS::Windows,          std::regex(R"(^[0-9a-f]{8}-[0-9a-f]{4}-3[0-9a-f]{3}-[0-9a-f]{4}-[0-9a-f]{12}$)", std::regex::icase)},
            // {DeviceOS::Windows32Bit,     std::regex(R"(^[A-Za-z0-9+/]{39,50}={0,2}$)")}, 
            {DeviceOS::Windows32Bit,    std::regex(R"(^[0-9a-f]{32}$)", std::regex::icase)},
            {DeviceOS::Xbox,            std::regex(R"(^[A-Za-z0-9+/=]{39,50}$)")},
            {DeviceOS::NintendoSwitch,  std::regex(R"(^[0-9a-f]{8}-[0-9a-f]{4}-5[0-9a-f]{3}-[0-9a-f]{4}-[0-9a-f]{12}$)", std::regex::icase)},
            {DeviceOS::Android,         std::regex(R"(^[0-9a-f]{8}[0-9a-f]{4}[1-5][0-9a-f]{3}[89ab][0-9a-f]{3}[0-9a-f]{12}$)", std::regex::icase)},
            {DeviceOS::AmazonGear,      std::regex(R"(^[a-z]{32}$)")}
        };
        return validators;
    }
};
