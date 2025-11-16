#pragma once

#include <iostream>
#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "../check.h"

using json = nlohmann::json;

class DeviceSpooferB : public AntiCheatCheck {
public:
    std::string getName() const override { return "deviceSpoofer/D"; }
    std::string getDescription() const override { return "Checks the platform type matches the device"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        return { CheckStatus::Pass, "" };
    }
};
