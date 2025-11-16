#pragma once

#include "../handlers/packetReceive/loginPacket.h"
#include "../anticheat/manager.h"
#include "../util/packets/util.h"

namespace OnPacketReceive {
    void handle(endstone::PacketReceiveEvent& event, endstone::Plugin* plugin, AntiCheatManager& acManager) {
        int packetId = event.getPacketId();

        switch (packetId) {
            case MinecraftPackets::Login:
                LoginHandler::handle(event, plugin, acManager);
                break;

            default:
                break;
        }
    }
};