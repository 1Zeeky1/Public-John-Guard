#pragma once

#include <iostream>
#include "util.h"

using namespace bedrock_protocol;

class BlockEventPacket final : public ClientboundPacket {
public:
    PacketResult result = { PacketStatus::Unknown, BlockEventPacket::Unknown, "Unknown error" };
    int32_t x {};
    uint32_t y {};
    int32_t z {};
    int32_t type {};
    int32_t data {};

    enum BlockEventCodes : int {
        Success                 = 0,
        Unknown                 = 1,
    };

    explicit BlockEventPacket(std::string_view payload);

    [[nodiscard]] uint64_t getPacketId() const override {
        return MinecraftPackets::BlockEvent;
    }

private:
    // void serialize();
    void deserialize();
};