#pragma once

#include <iostream>
#include "util.h"

using namespace bedrock_protocol;

class DisconnectPacket final : public ClientboundPacket {
public:
    PacketResult result = { PacketStatus::Unknown, DisconnectPacket::Unknown, "Unknown error" };
    int32_t disconnectFailReason{};
    int hideDisconnectionReason{};
    std::string message{};
    std::string filteredMessage{};

    enum DisconnectCodes : int {
        Success                 = 0,
        Unknown                 = 1,
    };

    explicit DisconnectPacket(std::string_view payload);

    DisconnectPacket(int32_t disconnectFailReason, int hideDisconnectionReason,
        std::string message = "", std::string filteredMessage = "");

    [[nodiscard]] uint64_t getPacketId() const override {
        return MinecraftPackets::Disconnect;
    }

private:
    void serialize();
    void deserialize();
};