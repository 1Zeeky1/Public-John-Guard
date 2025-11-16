#include <iostream>
#include <string>

#include <endstone/event/server/packet_receive_event.h>

#include <BinaryStream/BinaryStream.hpp>

#include "../../util/packets/entityEvent.h"
#include "../../util/packets/disconnect.h"
#include "../../anticheat/manager.h"

namespace EntityEventHandler {
    inline void handle(endstone::PacketReceiveEvent& event, endstone::Plugin* plugin, AntiCheatManager& acManager) {
        // EntityEventPacket packet(event.getPayload());

        // endstone::Player* player = event.getPlayer();
        
        // if (player == nullptr) return;

        // PlayerData playerData = {
        //     player->getUniqueId().str(),
        //     player->getXuid(),
        //     player->getName(),
        //     player->getAddress().getHostname(),
        //     player->getDeviceId(),
        //     player->getDeviceOS()
        // };

        // ChecksResult checkResult = acManager.runChecks(playerData, packet, plugin);
        // if (!checkResult.passed) {
        //     event.setCancelled(true);

        //     if (checkResult.kick) {
        //         DisconnectPacket dcPacket = DisconnectPacket(0, 0, checkResult.message, checkResult.message);
        //         player->sendPacket(dcPacket.getPacketId(), dcPacket.getOutputData());
        //         return;
        //     }
        // }
    }
}