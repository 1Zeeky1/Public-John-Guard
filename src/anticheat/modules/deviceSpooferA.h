#pragma once

#include <unordered_set>
#include <fmt/core.h>

#include "util/packets/login.h"
#include "../check.h"

static const std::unordered_set<int> allowedOS = { 1, 2, 4, 8, 11, 12, 13, 16 };

class DeviceSpooferA : public AntiCheatCheck {
public:
    std::string getName() const override { return "deviceSpoofer/A"; }
    std::string getDescription() const override { return "Checks the player isn't on an unsupported device"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        int deviceOS = std::stoi(ctx.deviceOS);
        if (allowedOS.find(deviceOS) == allowedOS.end()) {
            CheckResult res{ CheckStatus::Fail, "Hacking isn't allowed" };
            res.extraData["fakeDevice"] = DeviceOS::getName(deviceOS);
            return res;
        }

        return { CheckStatus::Pass, "" };
    }
};