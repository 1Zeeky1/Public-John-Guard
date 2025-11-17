#include <iostream>

#include "util/decoder/util.h"
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

        auto authenticationType = verifyJsonKey<int>(tokensData, "AuthenticationType");
        if (!authenticationType.success) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::MalformedAuthType,
                "Malformed auth type"
            };
            return false;
        }

        if (authenticationType.value != 0) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::NotAuthenticated,
                "Offline mode"
            };
            return false;
        }

        auto rawCertificate = verifyJsonKey<std::string>(tokensData, "Certificate");
        if (!rawCertificate.success) {
            result = { 
                PacketStatus::JsonParseError, 
                LoginPacket::NoCertificate,
                "Client authentication error"
            };
            return false;
        }

        json certificate = json::parse(rawCertificate.value);

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
        auto rawToken = verifyJsonKey<std::string>(tokensData, "Token");
        if (!rawToken.success || rawToken.value.empty()) {
            result = {
                PacketStatus::JsonParseError,
                LoginPacket::NoToken,
                "No token"
            };
            return false;    
        }
            
        try {
            token = parseJwtPayload(rawToken.value);
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