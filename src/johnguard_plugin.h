#pragma once

#include <iostream>

#include <endstone/plugin/plugin.h>

#include <endstone/event/player/player_command_event.h>

#include "util/config.h"

#include "anticheat/modules/register.h"
#include "events/packetReceive.h"
#include "events/packetSend.h"
#include "events/playerQuit.h"

class JohnGuardPlugin : public endstone::Plugin {
    AntiCheatManager checkManager;

    void onEnable() override {
        ConfigManager::load(getServer());
        registerAntiCheatChecks(checkManager);

        registerEvent(&JohnGuardPlugin::onPacketReceiveEvent, *this);
        registerEvent(&JohnGuardPlugin::onPacketSendEvent, *this);
    };

    void onPacketReceiveEvent(endstone::PacketReceiveEvent& event) {
        OnPacketReceive::handle(event, this, checkManager);
    };

    void onPacketSendEvent(endstone::PacketSendEvent& event) {
        OnPacketSend::handle(event, this);
    };

    void onPlayerCommandEvent(endstone::PlayerCommandEvent& event) {
        endstone::Player& player = event.getPlayer();
        std::string command = event.getCommand();
        
        const std::ptrdiff_t count = std::count(command.begin(), command.end(), '@');
        if (count >= 1 && !player.isOp()) {
            player.sendErrorMessage("@ symbols aren't allowed");
            event.setCancelled(true);
        }
    };

    void onPlayerQuitEvent(endstone::PlayerQuitEvent& event) {
        OnPlayerQuit::handle(event, checkManager);
    };
};