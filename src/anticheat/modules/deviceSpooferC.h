#pragma once

#include <fmt/core.h>
#include "json.hpp"

#include "util/packets/login.h"
#include "util/decoder/util.h"
#include "../check.h"

using json = nlohmann::json;

class DeviceSpooferC : public AntiCheatCheck {
public:
    std::string getName() const override { return "deviceSpoofer/C"; }
    std::string getDescription() const override { return "Checks for an invalid input mode against their device"; }
    int getDecaySeconds() const override { return -1; }
    int getMaxViolations() const override { return 1; }
    uint64_t getTargetPacketId() const override { return MinecraftPackets::Login; }

    CheckResult run(const PlayerData& ctx, const Packet& packet) override {
        auto* loginPkt = dynamic_cast<const LoginPacket*>(&packet);
        if (!loginPkt) return { CheckStatus::Error, "Packet cast failed" };


        int deviceOS = std::stoi(ctx.deviceOS);
        int defaultInputMode = DeviceOS::getDefaultInput(deviceOS);
        if (defaultInputMode == -1) return { CheckStatus::Pass, "Unknown device input mode" };

        json clientData = loginPkt->clientData;
        auto currentDefaultInputMode = verifyJsonKey<int>(clientData, "DefaultInputMode");
        if (!currentDefaultInputMode.success) {
            return { CheckStatus::NoValue, currentDefaultInputMode.error };
        }
        
        if (currentDefaultInputMode.value != defaultInputMode) {
            CheckResult res{ CheckStatus::Fail, "Not allowed" };
            res.extraData["currentInput"] = std::to_string(currentDefaultInputMode.value);
            res.extraData["defaultInput"] = std::to_string(defaultInputMode); 
            return res;
        }

        return { CheckStatus::Pass, "" };
    }
};
