#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <endstone/event/server/packet_send_event.h>
#include <BinaryStream/BinaryStream.hpp>

#include "../../util/packets/blockEvent.h"
#include "../../util/packets/util.h"

/**
 * When ever a chest, shulker, etc is opened BDS sends the packet to everyone
 * Revealing the players location.
 * This code prevents players from over 50 blocks away from getting the packet
 */
namespace BlockEventHandler {

    static std::unordered_map<std::string, std::unordered_set<std::string>> receivedOpen;

    inline std::string makeKey(int x, int y, int z) {
        return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
    }

    inline void handle(endstone::PacketSendEvent& event, endstone::Plugin* plugin) {
        endstone::Player* player = event.getPlayer();
        if (!player) return;

        BlockEventPacket packet(event.getPayload());
        std::string uuid = player->getUniqueId().str();
        if (packet.type != 0 && packet.type != 1) return;

        std::string key = makeKey(packet.x, packet.y, packet.z);

        if (packet.type == 1) {
            auto loc = player->getLocation();
            double dx = loc.getX() - packet.x;
            double dy = loc.getY() - packet.y;
            double dz = loc.getZ() - packet.z;
            double dist = std::sqrt(dx*dx + dy*dy + dz*dz);

            if (dist > 50.0) {
                event.setCancelled(true);
                return;
            }

            receivedOpen[key].insert(uuid);
            return;
        }

        if (packet.type == 0) {
            auto it = receivedOpen.find(key);
            if (it == receivedOpen.end()) {
                event.setCancelled(true);
                return;
            }

            if (it->second.count(uuid) == 0) {
                event.setCancelled(true);
                return;
            }

            receivedOpen.erase(key);
            return;
        }
    }
}