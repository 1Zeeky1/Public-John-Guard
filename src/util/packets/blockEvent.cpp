#include "blockEvent.h"

BlockEventPacket::BlockEventPacket(std::string_view payload) : ClientboundPacket(payload) {
    deserialize();
}

void BlockEventPacket::deserialize() {
    x = stream.getVarInt();
    y = stream.getUnsignedVarInt();
    z = stream.getVarInt();
    type = stream.getVarInt();
    data = stream.getVarInt();
    result = { PacketStatus::OK, BlockEventPacket::Success, "Success" };
}