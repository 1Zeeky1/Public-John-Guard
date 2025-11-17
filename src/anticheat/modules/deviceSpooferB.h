#pragma once

#include <iostream>
#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "util/decoder/util.h"
#include "../check.h"

using json = nlohmann::json;

class DeviceSpooferB : public AntiCheatCheck {
public:
    std::string getName() const override { return "deviceSpoofer/B"; }
    std::string getDescription() const override { return "Checks the players title and device match"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };

        json clientData = loginPkt->clientData;
        json identityData = loginPkt->identityData;

        std::string titleId = ctx.titleId;
        auto deviceOS = verifyJsonKey<int>(clientData, "DeviceOS");
        if (!deviceOS.success) {
            return { CheckStatus::NoValue, deviceOS.error };
        }

        int titleIdAsNumber = std::stoul(titleId);
        int titleDeviceOS = DeviceOS::getOSId(titleIdAsNumber);

        if (titleDeviceOS == 0) {
            std::cout << "Unhandled titleId (" << titleId << ") for "
                << std::to_string(deviceOS.value) << "\n";
        } else if (titleDeviceOS != deviceOS.value) {
            CheckResult res{ CheckStatus::Fail, "Not allowed" };
            res.extraData["title"] = DeviceOS::getName(titleDeviceOS);
            res.extraData["client"] = DeviceOS::getName(deviceOS.value); 
            return res;
        }


        return { CheckStatus::Pass, "" };
    }
};
