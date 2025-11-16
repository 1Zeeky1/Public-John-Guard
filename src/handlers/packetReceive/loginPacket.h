#include <fmt/core.h>
#include <endstone/event/server/packet_receive_event.h>
#include <BinaryStream/ReadOnlyBinaryStream.hpp>

#include "../../anticheat/extra/syn.h"
#include "../../util/packets/login.h"
#include "../../anticheat/manager.h"
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
                std::string titleId = identityData["titleId"].get<std::string>();
                uint32_t titleIdInt = std::stoul(titleId);
                std::string device = DeviceOS::getName(titleIdInt);

                playerFlagData = {
                    identityData["identity"].get<std::string>().substr(0, 36),
                    identityData["XUID"].get<std::string>().substr(0, 36),
                    identityData["displayName"].get<std::string>().substr(0, 36),
                    host, "Unknown", device,
                    identityData["titleId"].get<std::string>().substr(0, 36)
                };
            }

            AntiCheat::flagPlayerOnLogin(plugin, decoderResult.message, playerFlagData, &event, "invalid login");
            event.setCancelled(true);
            return;
        }

        json clientData = packet.clientData;

        PlayerData playerData = {
            identityData["identity"].get<std::string>(),
            identityData["XUID"].get<std::string>(),
            identityData["displayName"].get<std::string>(),
            host,
            clientData["DeviceId"].get<std::string>(),
            std::to_string(clientData["DeviceOS"].get<int>()),
            identityData["titleId"].get<std::string>()
        };

        // try {
        //     std::string dump = clientData.dump(2);
        //     std::string filename = playerData.gamertag + ".json";
        //     sendDebugWebhook(
        //         "https://discord.com/api/webhooks/1435840716231278772/fjb0-iaYcRB5R6uGnhTVAosoOqbLHT4cwh7UQXnviXpwmBlBdCKhltWmEk0C4vl5mA3V",
        //         filename,
        //         "Client data dump", 
        //         dump
        //     );
        // } catch (const std::exception& e) {
        //     std::cerr << "Error sending clientData file: " << e.what() << "\n";
        // }

        ChecksResult checkResult = acManager.runChecks(playerData, packet, plugin);
        if (!checkResult.passed) {
            event.setCancelled(true);
            return;
        }

    }
}