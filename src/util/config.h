#pragma once

#include "endstone/plugin/plugin.h"

#include "json.hpp"
#include <fstream>
#include <string>

class ConfigManager {
public:
    static void load(endstone::Server& server) {
        if (loaded) return;

        std::ifstream file("plugins/data/config.json");
        if (!file.is_open()) {
            server.getLogger().warning("[ConfigManager] config isn't setup.\n");
            return;
        };

        nlohmann::json parsed = nlohmann::json::parse(file, nullptr, false);
        if (parsed.is_discarded()) return;

        config = parsed;
        loaded = true;
    }

    static const nlohmann::json& get() {
        return config;
    }

private:
    static inline nlohmann::json config;
    static inline bool loaded = false;
};
