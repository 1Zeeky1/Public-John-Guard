#pragma once

#include <iostream>
#include <endstone/player.h>

#include "util/packets/util.h"

enum class CheckStatus { Pass, Fail, Error };

struct CheckResult {
    CheckStatus status;
    std::string message;
    std::unordered_map<std::string, std::string> extraData;
};

struct PlayerData {
    std::string uuid{};
    std::string xuid{};
    std::string gamertag{};
    std::string ip{};
    std::string deviceId{};
    std::string deviceOS{};
    std::string titleId{};
};

class AntiCheatCheck {
public:
    virtual ~AntiCheatCheck() = default;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual int getDecaySeconds() const = 0;
    virtual int getMaxViolations() const = 0;

    virtual CheckResult run(const PlayerData& ctx, const Packet& packet) = 0;

    virtual uint64_t getTargetPacketId() const = 0;
};