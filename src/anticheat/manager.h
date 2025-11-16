#pragma once

#include <unordered_map>
#include <sstream>
#include <chrono>

#include <endstone/plugin/plugin.h>
#include <endstone/player.h>
#include <fmt/core.h>

#include "./extra/manualFlagger.h"
#include "./check.h"

struct ChecksResult {
    bool passed;
    bool kick;
    std::string message{};
};

class AntiCheatManager {
    struct PlayerViolation {
        int count = 0;
        std::chrono::steady_clock::time_point lastViolation;
    };

    std::unordered_map<std::string, PlayerViolation> violations;
    std::unordered_map<uint64_t, std::vector<std::unique_ptr<AntiCheatCheck>>> checks;

public:
    void registerCheck(std::unique_ptr<AntiCheatCheck> check) {
        checks[check->getTargetPacketId()].push_back(std::move(check));
    }

    ChecksResult runChecks(const PlayerData& ctx, const Packet& packet, endstone::Plugin* plugin) {
        auto it = checks.find(packet.getPacketId());
        if (it == checks.end()) return { true, false };

        for (auto& check : it->second) {
            CheckResult result = check->run(ctx, packet);

            if (result.status == CheckStatus::Fail) {
                auto& record = violations[ctx.uuid];

                auto now = std::chrono::steady_clock::now();
                int decay = check->getDecaySeconds();
                if (
                    decay > 0 && 
                    now - record.lastViolation > std::chrono::seconds(decay)
                ) {
                    record.count = 0;
                }

                record.count++;
                record.lastViolation = now;

                std::ostringstream log;
                std::ostringstream extraInfo;

                log << "§l§8[§r§cJohn Guard§l§8]§r " 
                    << ctx.gamertag << " (" << ctx.xuid << ") §6failed §4"
                    << check->getName() << " §7(vl: " << record.count << "/"
                    << check->getMaxViolations() << ")";

                if (!result.extraData.empty()) {
                    log << " [";
                    bool first = true;
                    for (const auto& [key, val] : result.extraData) {
                        if (!first) {
                            log << ", ";
                            extraInfo << ", ";
                        }
                        log << key << ": " << val;
                        extraInfo << key << ": " << val;
                        first = false;
                    }
                    log << "]";
                }

                const std::string logStr = log.str();
                plugin->getServer().broadcast(logStr, "john.notify");

                const std::string extraStr = extraInfo.str();

                if (record.count >= check->getMaxViolations()) {
                    AntiCheat::logAlert(ctx, check->getName(), extraStr, "kicked");
                    clearViolations(ctx.uuid);
                    return { false, true, result.message };
                } 
                
                AntiCheat::logAlert(ctx, check->getName(), extraStr);

                return { false, false, result.message };
            }
        }
        return { true, false };
    }

    void clearViolations(std::string uuid) {
        violations.erase(uuid);
    }
};