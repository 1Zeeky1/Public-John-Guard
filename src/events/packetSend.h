#pragma once

#include "../handlers/packetSend/blockEventPacket.h"
#include "../handlers/packetSend/movePlayerPacket.h"
#include "../util/packets/util.h"

namespace OnPacketSend {
    void handle(endstone::PacketSendEvent& event, endstone::Plugin* plugin) {
        int packetId = event.getPacketId();

        switch (packetId) {
            case MinecraftPackets::BlockEvent:
                BlockEventHandler::handle(event, plugin);
                break;

            case MinecraftPackets::MovePlayer:
                MovePlayerHandler::handle(event, plugin);
                break;

            default:
                break;
        }
    }
};