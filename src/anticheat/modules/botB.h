#pragma once

#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "util/decoder/util.h"
#include "../check.h"

using json = nlohmann::json;

class BotB : public AntiCheatCheck {
public:
    std::string getName() const override { return "bot/B"; }
    std::string getDescription() const override { return "Checks for bedrock protocol bots"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        json clientData = loginPkt->clientData;
        auto deviceModel = verifyJsonKey<std::string>(clientData, "DeviceModel");
        if (!deviceModel.success) {
            return { CheckStatus::NoValue, deviceModel.error };
        }

        if (deviceModel.value == "PrismarineJS") {
            return { CheckStatus::Fail, "Not allowed" };
        } 

        return { CheckStatus::Pass, "" };
    }
};
