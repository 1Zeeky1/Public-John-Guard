#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "util.h"

/**
 * ================================================================================
 *                              Packet class
 * ================================================================================
 */

Packet::Packet(std::string_view payload) : stream(payload) {};


int32_t Packet::getZigZag32() {
    uint32_t value = stream.getUnsignedVarInt();
    return (value >> 1) ^ -(value & 1);
}

int64_t Packet::getZigZag64() {
    uint64_t value = stream.getUnsignedVarInt64();
    return (value >> 1) ^ -(value & 1);
}

std::vector<Packet::MetadataEntry> Packet::getMetadata() {
    std::vector<MetadataEntry> list;

    MetadataEntry m{};

    while (true) {
        m.key = stream.getUnsignedChar();   // 0–255
        if (m.key == 0xFF)                  // end marker
            break;

        m.type = stream.getUnsignedChar();

        MetadataEntry entry{};
        entry.key = m.key;
        entry.type = m.type;

        switch (m.type) {
            case 0: entry.byteValue = stream.getByte(); break;
            case 1: entry.shortValue = stream.getSignedShort(); break;
            case 2: entry.intValue = stream.getVarInt(); break;
            case 3: entry.floatValue = stream.getFloat(); break;
            case 4: entry.stringValue = stream.getString(); break;
            case 5: entry.stringValue = stream.getString(); break; // NBT
            case 6:
                entry.blockPos.x = stream.getVarInt();
                entry.blockPos.y = stream.getVarInt();
                entry.blockPos.z = stream.getVarInt();
                break;
            case 7:  // Vec3f
                entry.vec3f.x = stream.getFloat();
                entry.vec3f.y = stream.getFloat();
                entry.vec3f.z = stream.getFloat();
                break;
            case 8: entry.longValue = stream.getVarInt(); break;

            default:
                // UNKNOWN TYPE: skip ONE varint, do not desync
                stream.getUnsignedVarInt();
                continue;
        }

        list.push_back(entry);
    }

    return list;


    return list;
}

std::string Packet::getUUID() {
    if (stream.size() - stream.getPosition() < 16)
        throw std::runtime_error("UUID too short");

    unsigned char buf[16];

    if (!stream.getBytes(buf, 16))
        throw std::runtime_error("Failed to read UUID bytes");

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            ss << "-";
        ss << std::setw(2) << static_cast<unsigned int>(buf[i]);
    }

    return ss.str();
}

void Packet::writeZigZag32(int32_t value) {
    uint32_t encoded = (value << 1) ^ (value >> 31);
    out.writeUnsignedVarInt(encoded);
}

void Packet::writeZigZag64(int64_t value) {
    uint64_t encoded = (value << 1) ^ (value >> 63);
    out.writeUnsignedVarInt64(encoded);
}

void Packet::writeUUID(const std::string& uuid) {
    std::string hex;
    hex.reserve(32);
    for (char c : uuid) {
        if (c != '-')
            hex.push_back(c);
    }

    if (hex.size() != 32)
        throw std::runtime_error("Invalid UUID format");

    unsigned char buf[16];

    for (int i = 0; i < 16; ++i) {
        std::string byteStr = hex.substr(i * 2, 2);
        buf[i] = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
    }

    out.writeBytes(buf, 16);
}

/**
 * ================================================================================
 *                              MinecraftPackets class
 * ================================================================================
 */

std::string MinecraftPackets::getName(int id) {
    switch (id) {
        case Login:                 return "Login";
        case Disconnect:            return "Disconnect";
        case AddPlayer:             return "AddPlayer";
        case AddEntity:             return "AddEntity";
        case RemoveEntity:          return "RemoveEntity";
        case UpdateBlock:           return "UpdateBlock";
        case BlockEvent:            return "BlockEvent";
        case EntityEvent:           return "EntityEvent";
        case Interact:              return "Interact";
        case ContainerOpen:         return "ContainerOpen";
        case ContainerClose:        return "ContainerClose";
        case LevelChunk:            return "LevelChunk";
        case PlayerList:            return "PlayerList";
        case SpawnExperienceOrb:    return "SpawnExperienceOrb";
        case SetTitle:              return "SetTitle";
        case PlayerSkin:            return "PlayerSkin";
        case Subchunk:              return "Subchunk";
        case SubchunkRequest:       return "SubchunkRequest";
        case RequestPermissions:    return "RequestPermissions";
        default:                    return "UnknownPacket";
    }
};

/**
 * ================================================================================
 *                              DeviceOS Class
 * ================================================================================
 */

std::string DeviceOS::getName(int id) {
    switch (id) {               
        case Undefined:             return "Undefined";
        case Android:               return "Android";
        case IOS:                   return "IOS";
        case MacOS:                 return "MacOS";
        case AmazonGear:            return "Amazon Fire";
        case GearVR:                return "GearVR";
        case HoloLens:              return "HoloLens";
        case Windows:               return "Windows";
        case Windows32Bit:          return "Windows 32-bit";
        case Dedicated:             return "Dedicated";
        case TvOS:                  return "TvOS";
        case PlayStation:           return "Playstation";
        case NintendoSwitch:        return "Nintendo Switch";
        case Xbox:                  return "Xbox";
        case WindowsPhone:          return "Windows Phone";
        case Linux:                 return "Linux";
        case ChromeOS:              return "ChromeOS";
        default:                    return "Spoofed";
    }
};

std::string DeviceOS::getName(uint32_t titleId) {
    switch (titleId) {               
        case 1739947436:            return "Android";
        case 1810924247:            return "IOS";
        case 1944307183:            return "Amazon Fire";
        case 896928775:             return "Windows";
        case 2044456598:            return "Playstation";
        case 2047319603:            return "Nintendo Switch";
        case 1828326430:            return "Xbox";
        default:                    return "Unknown";
    }
};

int DeviceOS::getOSId(int titleId) {
    switch (titleId) {               
        case 1739947436:            return DeviceOS::Android;
        case 1810924247:            return DeviceOS::IOS;
        case 1944307183:            return DeviceOS::AmazonGear;
        case 896928775:             return DeviceOS::Windows32Bit;
        case 2044456598:            return DeviceOS::PlayStation;
        case 2047319603:            return DeviceOS::NintendoSwitch;
        case 1828326430:            return DeviceOS::Xbox;
        default:                    return DeviceOS::Undefined;
    }
};

int DeviceOS::getDefaultInput(int deviceOS) {
    switch (deviceOS) {
        case DeviceOS::Android:
            return InputModes::Android;

        case DeviceOS::IOS:
            return InputModes::IOS;

        case DeviceOS::AmazonGear:
            return InputModes::AmazonGear;

        case DeviceOS::Windows:
            return InputModes::Windows;

        case DeviceOS::Windows32Bit:
            return InputModes::Windows32Bit;

        case DeviceOS::PlayStation:
            return InputModes::PlayStation;

        case DeviceOS::NintendoSwitch:
            return InputModes::NintendoSwitch;

        case DeviceOS::Xbox:
            return InputModes::Xbox;

        default:
            return -1;
    }
}

// switch (deviceOS) {
//     case DeviceOS::Android:
//         return InputModes::Android;

//     case DeviceOS::IOS:
//         return InputModes::IOS;

//     case DeviceOS::AmazonGear:
//         return InputModes::AmazonGear;

//     case DeviceOS::Windows:
//         return InputModes::Windows;

//     case DeviceOS::PlayStation:
//         return InputModes::PlayStation;

//     case DeviceOS::NintendoSwitch:
//         return InputModes::NintendoSwitch;

//     case DeviceOS::Xbox:
//         return InputModes::Xbox;

//     default:
//         return -1;
// }

/**
 * ================================================================================
 *                               ""
 * ================================================================================
 */