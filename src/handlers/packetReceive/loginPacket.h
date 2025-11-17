#include <fmt/core.h>
#include <endstone/event/server/packet_receive_event.h>
#include <BinaryStream/ReadOnlyBinaryStream.hpp>

#include "../../anticheat/extra/syn.h"
#include "../../util/packets/login.h"
#include "../../anticheat/manager.h"
#include "util/decoder/util.h"
#include "util/discord.h"

using namespace bedrock_protocol;

namespace LoginHandler {
    static std::unordered_map<std::string, std::vector<std::chrono::steady_clock::time_point>> recentConnections;
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastFloodFlag;

    inline void handle(endstone::PacketReceiveEvent& event, endstone::Plugin* plugin, AntiCheatManager& acManager) {
        bool synCheckResult = SYNCheck::check(event, plugin);
        if (synCheckResult) return;

        endstone::SocketAddress address = event.getAddress();
        std::string host = address.getHostname();
        
        std::cout << "Connection from: " << host << "\n";

        LoginPacket packet(event.getPayload());
        PacketResult decoderResult = packet.result;

        if (LoginPacket::NoCertificate == decoderResult.code) return;

        json identityData = packet.identityData;

        if (decoderResult.status != PacketStatus::OK) {
            endstone::SocketAddress address = event.getAddress();

            PlayerData playerFlagData{
                "Unknown", "0", host, host, "Unknown", "Unknown", "0"
            };

            if (!identityData.is_null()) {
                auto uuid =     verifyJsonKey<std::string>(identityData, "identity");
                auto xuid =     verifyJsonKey<std::string>(identityData, "XUID");
                auto name =     verifyJsonKey<std::string>(identityData, "displayName");
                auto tid =      verifyJsonKey<std::string>(identityData, "titleId");

                std::string titleId = tid.success ? tid.value : "0";
                uint32_t titleIdInt = std::strtoul(titleId.c_str(), nullptr, 10);
                std::string deviceName = DeviceOS::getName(titleIdInt);

                playerFlagData = {
                    uuid.success    ? uuid.value.substr(0, 36) : "Unknown",
                    xuid.success    ? xuid.value.substr(0, 36) : "Unknown",
                    name.success    ? name.value.substr(0, 36) : "Unknown",
                    host, "Unknown", deviceName, titleId.substr(0, 36)
                };
            }

            AntiCheat::flagPlayerOnLogin(plugin, decoderResult.message, playerFlagData, &event, "invalid login");
            event.setCancelled(true);
            return;
        }

        json clientData = packet.clientData;

        auto uuid       = verifyJsonKey<std::string>(identityData, "identity");
        auto xuid       = verifyJsonKey<std::string>(identityData, "XUID");
        auto name       = verifyJsonKey<std::string>(identityData, "displayName");
        auto titleId    = verifyJsonKey<std::string>(identityData, "titleId");
        auto deviceId   = verifyJsonKey<std::string>(clientData,  "DeviceId");
        auto deviceOS   = verifyJsonKey<int>(clientData, "DeviceOS");

        PlayerData playerData = {
            uuid.success    ? uuid.value    : "Unknown",
            xuid.success    ? xuid.value    : "0",
            name.success    ? name.value    : "Unknown",
            host,
            deviceId.success   ? deviceId.value : "Unknown",
            deviceOS.success   ? std::to_string(deviceOS.value) : "0",
            titleId.success ? titleId.value : "0"
        };

        ChecksResult checkResult = acManager.runChecks(playerData, packet, plugin);
        if (!checkResult.passed) {
            event.setCancelled(true);
            return;
        }

    }
}