#pragma once

#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "../check.h"

using json = nlohmann::json;

class BotA : public AntiCheatCheck {
public:
    std::string getName() const override { return "bot/A"; }
    std::string getDescription() const override { return "Checks for an invalid view distance"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };


        json clientData = loginPkt->clientData;
        int maxViewDistance = clientData["MaxViewDistance"].get<int>();
        if (maxViewDistance == 0) {
            return { CheckStatus::Fail, "Not allowed" };
        }

        return { CheckStatus::Pass, "" };
    }
};