#include <iostream>
#include <string>

#include <endstone/event/server/packet_send_event.h>
#include <endstone/player.h>

#include <BinaryStream/BinaryStream.hpp>

#include "../../util/packets/util.h"

using namespace bedrock_protocol;

/**
 * Prevents the server from revealing the location of someone who teleported 
 * 
 */
namespace MovePlayerHandler {
    inline void handle(endstone::PacketSendEvent& event, endstone::Plugin* plugin) {
        ReadOnlyBinaryStream packet(event.getPayload());
        int runtimeId = packet.getVarInt();

        // player location
        float x = packet.getFloat();
        float y = packet.getFloat();
        float z = packet.getFloat();

        // rotation
        float pitch = packet.getFloat();
        float yaw = packet.getFloat();
        float headYaw = packet.getFloat();

        // movement mode
        uint8_t mode = packet.getUnsignedVarInt64();

        if (mode == 2) {
            event.setCancelled(true);
        }
    }
}