#pragma once

#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "../check.h"

using json = nlohmann::json;

class BotC : public AntiCheatCheck {
public:
    std::string getName() const override { return "bot/C"; }
    std::string getDescription() const override { return "Checks for third party name mismatch"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        int deviceOS = std::stoi(ctx.deviceOS);
        if (deviceOS == DeviceOS::PlayStation) return { CheckStatus::Pass, "" };

        json clientData = loginPkt->clientData;
        std::string gamertag = clientData["ThirdPartyName"].get<std::string>();
        if (ctx.gamertag != gamertag) {
            CheckResult res{ CheckStatus::Fail, "Not allowed" };
            res.extraData["fakeName"] = gamertag; 
            return res;
        }

        return { CheckStatus::Pass, "" };
    }
};
