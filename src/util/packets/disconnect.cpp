#include "disconnect.h"

DisconnectPacket::DisconnectPacket(std::string_view payload) : ClientboundPacket(payload) {
        deserialize();
}

DisconnectPacket::DisconnectPacket(int32_t disconnectFailReason, int hideDisconnectionReason,
        std::string message, std::string filteredMessage) :
    ClientboundPacket(""),  disconnectFailReason(disconnectFailReason), 
    hideDisconnectionReason(hideDisconnectionReason),
    message(message), filteredMessage(filteredMessage) {
        serialize();
}

void DisconnectPacket::serialize() {
    ClientboundPacket::writeZigZag32(disconnectFailReason);
    out.writeVarInt(hideDisconnectionReason);
    out.writeString(message);
    out.writeString(filteredMessage);
    result = { PacketStatus::OK, DisconnectPacket::Success, "success" };
}

void DisconnectPacket::deserialize() {
    disconnectFailReason = ClientboundPacket::getZigZag32();
    hideDisconnectionReason = stream.getVarInt();
    stream.getString(message);
    stream.getString(filteredMessage);
    result = { PacketStatus::OK, DisconnectPacket::Success, "success" };
}