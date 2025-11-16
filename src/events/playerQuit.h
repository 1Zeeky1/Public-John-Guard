#include <iostream>

#include <endstone/event/player/player_quit_event.h>

#include "anticheat/manager.h"

namespace OnPlayerQuit {
    void handle(endstone::PlayerQuitEvent& event, AntiCheatManager& acManager) {
        endstone::Player& player = event.getPlayer();

        acManager.clearViolations(player.getUniqueId().str());
    }
}