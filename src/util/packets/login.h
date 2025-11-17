#pragma once

#include "json.hpp"
#include "util.h"

using json = nlohmann::json;

class LoginPacket final : public ServerboundPacket {
public:
    PacketResult result = { PacketStatus::Unknown, LoginPacket::Unknown, "Unknown error" };
    uint32_t protocolVersion{};
    uint32_t packetlength{};
    uint32_t tokensDataLength{};
    std::string rawTokensData{};
    uint32_t clientDataLength{};
    std::string rawClientData{};
    json identityData{};
    json token{};
    json clientData{};

    enum LoginCodes : int {
        Success                 = 0,
        Unknown                 = 1,
        Overflow                = 2,
        NoCertificate           = 3,
        NotAuthenticated        = 4,
        MalformedChainData      = 5,
        NoExtraData             = 6,
        NoToken                 = 7,
        MalformedToken          = 8,
        ChainDataParseError     = 9,
        ClientDataParseError    = 10,
        MalformedAuthType       = 11,
    };

    explicit LoginPacket(std::string_view payload);

    [[nodiscard]] uint64_t getPacketId() const override {
        return MinecraftPackets::Login;
    }

private:
    void deserialize();
    bool parseIdentityData();
    bool parseClientData();
};