#pragma once

#include <deque>
#include <chrono>
#include <format> 
#include <memory>
#include <string>
#include <chrono>
#include <cstdint>
#include <unordered_map>

#include <endstone/plugin/plugin.h>
#include <endstone/event/player/player_login_event.h>
#include <endstone/event/server/packet_receive_event.h>

#include "util/discord.h"
#include "util/config.h"
#include "../check.h"

namespace AntiCheat {
    inline const auto& getDiscord() {
        static const auto& discord = ConfigManager::get()["discord"];
        return discord;
    }

    inline void logAlert(PlayerData player, const std::string& flagType, const std::string& info, const std::string& action = "flagged") {
        WebhookPayload payload;
        payload.username = "John Guard";
        payload.avatar_url = getDiscord()["icons"]["alert"];

        std::cout << "[Flagger] " << player.gamertag << "(" << player.xuid << ") - " << flagType << "\n";

        Embed embed;
        embed.title = "**" + player.gamertag + " was " + action + " for " + flagType + "**";
        embed.color = 0xED4245;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::floor<std::chrono::seconds>(now);
        embed.timestamp = std::format("{:%FT%TZ}", time);

        uint32_t titleIdInt = 0;
        try {
            if (!player.titleId.empty())
                titleIdInt = static_cast<uint32_t>(std::stoul(player.titleId));
        } catch (const std::exception&) {
            titleIdInt = 0;
        }

        embed.description =
			"**Description:** `" + info +
            "`\n**XUID:** `" + player.xuid + 
            "`\n**IP:** `" + player.ip + 
            "`\n**Device:** `" + DeviceOS::getName(titleIdInt) + 
            "`\n**HWID:** `" + player.deviceId + "`\n";

        payload.embeds.push_back(embed);
        std::string json = payload.toJson();

        sendWebhook(getDiscord()["webhooks"]["ac"], json);
    }

    inline void flagPlayerOnLogin(
        endstone::Plugin* plugin, const std::string& flagType, 
        PlayerData playerData, 
        endstone::PacketReceiveEvent* event, const std::string& extraInfo = ""
    ) {
        endstone::Server& server = plugin->getServer();
        std::string formattedInfo = extraInfo.empty() ? "" : "(" + extraInfo + ")";

        server.broadcast(
            "§l§8[§r§cJohn Guard§l§8]§r " + 
            playerData.gamertag +
            "(" + playerData.xuid + ")" + 
            " §6was kicked for §4" +
            flagType +
            "§7 " + formattedInfo,
            "minecraft.command.kill"
        );

        logAlert(playerData, flagType, formattedInfo, "kicked");
    }
}