#pragma once

#include <unordered_map>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>

#include <endstone/event/server/packet_receive_event.h>
#include <BinaryStream/ReadOnlyBinaryStream.hpp>

#include "../anticheat/manager.h"
#include "./manualFlagger.h"
#include "../check.h"

namespace SYNCheck {
    static std::unordered_map<std::string, std::vector<std::chrono::steady_clock::time_point>> recentConnections;
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastFloodFlag;

    bool check(endstone::PacketReceiveEvent& event, endstone::Plugin* plugin) {
		endstone::SocketAddress address = event.getAddress();
        std::string host = address.getHostname();
        int port = address.getPort();

        if (host == "UNASSIGNED_SYSTEM_ADDRESS") {
            event.setCancelled(true);
            return true;
        }

        constexpr size_t FAST_MAX = 2;
        constexpr auto FAST_WINDOW = std::chrono::seconds(2);

        constexpr size_t SLOW_MAX = 3;
        constexpr auto SLOW_WINDOW = std::chrono::seconds(10);

        constexpr auto FLAG_COOLDOWN = std::chrono::seconds(20);

        auto now = std::chrono::steady_clock::now();
        auto& history = recentConnections[host];

        history.erase(std::remove_if(history.begin(), history.end(), [&](auto t) {
            return (now - t) > SLOW_WINDOW;
        }), history.end());

        history.push_back(now);

        auto fastCount = std::count_if(history.begin(), history.end(), [&](auto t) {
            return (now - t) <= FAST_WINDOW;
        });

        if (fastCount > FAST_MAX) {
            event.setCancelled(true);
            auto it = lastFloodFlag.find(host);
            if (it == lastFloodFlag.end() || (now - it->second) > FLAG_COOLDOWN) {
                PlayerData playerData{
                    "Unknown", "0", host, host, "Unknown", "Unknown", "0"
                };

                AntiCheat::flagPlayerOnLogin(plugin, "SYN", playerData, &event, "login flood");
                lastFloodFlag[host] = now;
            }
            return true;
        }

        if (history.size() >= SLOW_MAX) {
            event.setCancelled(true);
            auto it = lastFloodFlag.find(host);
            if (it == lastFloodFlag.end() || (now - it->second) > FLAG_COOLDOWN) {
                PlayerData playerData{
                    "Unknown", "0", host, host, "Unknown", "Unknown", "0"
                };

                AntiCheat::flagPlayerOnLogin(plugin, "SYN", playerData, &event, "Bloat login flood");
                lastFloodFlag[host] = now;
            }
            return true;
        }

        return false;
    };
}