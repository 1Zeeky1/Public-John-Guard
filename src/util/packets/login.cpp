#include <iostream>

#include "../decoder/util.h"
#include "login.h"

using namespace bedrock_protocol;

LoginPacket::LoginPacket(std::string_view payload) : ServerboundPacket(payload) {
    deserialize();
}

void LoginPacket::deserialize() {
    // Read protocol version
    protocolVersion = stream.getSignedBigEndianInt();
    packetlength = stream.getUnsignedVarInt();

    // Read login tokens length
    tokensDataLength = stream.getUnsignedInt();
    
    // Safety gaurd 
    if (tokensDataLength > stream.size() - stream.getPosition()) {
        result = { PacketStatus::DeserializeError, LoginPacket::Overflow, "ChainData length (overflow)"};
        return;
    }

    // Read login tokens
    rawTokensData = stream.getRawBytes(tokensDataLength);

    // Deserialize the identity data in the login tokens
    bool identityDataParseResult = parseIdentityData();
    if (!identityDataParseResult) return;

    // Read client data
    clientDataLength = stream.getUnsignedInt();
    rawClientData = stream.getRawBytes(clientDataLength);

    bool clientDataParseResult = parseClientData();
    if (!clientDataParseResult) return;

    result = { PacketStatus::OK, LoginPacket::Success, "Success" };
}

bool LoginPacket::parseIdentityData() {
    try {
        // Json object containing the "Certificate", "AuthenticationType", and "Token".
        json tokensData = json::parse(rawTokensData);

        if (!tokensData.contains("Certificate") || 
            !tokensData["Certificate"].is_string()) {
                result = { 
                    PacketStatus::JsonParseError, 
                    LoginPacket::NoCertificate,
                    "Client authentication error"
                };
                return false;
        }

        if (
            !tokensData.contains("AuthenticationType") || 
            !tokensData["AuthenticationType"].is_number_integer()
        ) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::MalformedAuthType,
                "Malformed auth type"
            };
            return false;
        }

        int authType = tokensData["AuthenticationType"].get<int>();
        if (authType != 0) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::OfflineClient,
                "Offline mode"
            };
            return false;
        }

        std::string rawCertificate = tokensData["Certificate"];
        json certificate = json::parse(rawCertificate);

        // Prevents a one packet crash with the login packet
        if (
            !certificate.contains("chain") || 
            certificate["chain"].size() != 3
        ) {
            result = {
                PacketStatus::DeserializeError,
                LoginPacket::MalformedChainData,
                "Malformed chain data", 
            };
            return false;
        }

        json rawIdentityData = certificate["chain"][2];
        json fullIdentityData;
        try {
            fullIdentityData = parseJwtPayload(rawIdentityData);
        } catch (const std::exception& e) {
            result = {
                PacketStatus::JwtParseFailure,
                LoginPacket::MalformedChainData,
                "Invalid chain data"
            };
            return false;
        }

        identityData = fullIdentityData["extraData"];

        if (identityData.is_null()) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::NoExtraData,
                "No identity data", 
            };
            return false;
        }

        // Once analyzed more I'll add additional checks
        if (
            !tokensData.contains("Token") || 
            !tokensData["Token"].is_string() ||
            tokensData["Token"].get<std::string>().empty()
        ) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::NoToken,
                "No token"
            };
            return false;
        }

        std::string rawToken = tokensData["Token"];
        try {
            token = parseJwtPayload(rawToken);
        } catch (const std::exception& e) {
            result = {
                PacketStatus::JwtParseFailure,
                LoginPacket::MalformedToken,
                "Malformed token"
            };
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        result = {
            PacketStatus::JsonParseError,
            LoginPacket::ChainDataParseError,
            e.what()
        };  
        return false;
    }
}

bool LoginPacket::parseClientData() {
    try {
        clientData = parseJwtPayload(rawClientData);

        result = {
            PacketStatus::OK,
            LoginPacket::Success,
            "Success"
        };

        return true;
    } catch (const std::exception& e) {
        result = {
            PacketStatus::JwtParseFailure,
            LoginPacket::ClientDataParseError,
            e.what()
        };

        return true;
    }
}