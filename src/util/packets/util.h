#pragma once

#include <string>

#include <BinaryStream/ReadOnlyBinaryStream.hpp>
#include <binaryStream/BinaryStream.hpp>

using namespace bedrock_protocol;

class MinecraftPackets {
public:
    enum Id : uint32_t {
        Login                   = 0x01,
        Disconnect              = 0x5,
        AddPlayer               = 0xc,
        AddEntity               = 0xd,
        RemoveEntity            = 0xe,
        MovePlayer              = 0x13,
        InventoryTransaction    = 0x1e,
        BlockEvent              = 0x1a,
        EntityEvent             = 0x1b,
        Interact                = 0x21,
        PlayerAction            = 0x24,
        ContainerOpen           = 0x2e,
        ContainerClose          = 0x2f,
        LevelChunk              = 0x3a,
        PlayerList              = 0x3f,
        SpawnExperienceOrb      = 0x42,
        SetTitle                = 0x58, 
        UpdateBlock             = 0x4e,
        PlayerSkin              = 0x5d,
        PlayerAuthInput         = 0x90,
        Subchunk                = 0xae,
        SubchunkRequest         = 0xaf,
        RequestPermissions      = 0xb9
    };

    static std::string getName(int id);
};

class Packet {
public:
    ReadOnlyBinaryStream stream;
    BinaryStream out;

    explicit Packet(std::string_view payload);
    virtual ~Packet() = default;

    [[nodiscard]] virtual uint64_t getPacketId() const = 0;
    virtual void deserialize() = 0;
    virtual void serialize() {}

    // ZigZag
    virtual int32_t  getZigZag32();
    virtual int64_t  getZigZag64();
    virtual void     writeZigZag32(int32_t value);
    virtual void     writeZigZag64(int64_t value);

    virtual std::string getUUID();
    virtual void writeUUID(const std::string& uuid);

    struct MetadataEntry {
        uint8_t key;
        uint8_t type;

        std::byte byteValue{};
        int16_t shortValue{};
        int32_t intValue{};
        int64_t longValue{};
        float floatValue{};
        std::string stringValue;

        struct { int32_t x, y, z; } blockPos{};
        struct { float x, y, z; } vec3f{};
    };

    virtual std::vector<MetadataEntry> getMetadata();

    [[nodiscard]] std::string_view getOutputData() const { return out.data(); }
};

class BidirectionalPacket : public Packet {
public:
    using Packet::Packet;
    virtual ~BidirectionalPacket() = default;
    
    virtual void serialize() override {};
    int64_t getZigZag64() override { return Packet::getZigZag64(); }
    int32_t getZigZag32() override { return Packet::getZigZag32(); }
    std::string getUUID() override { return Packet::getUUID(); }

    void writeZigZag64(int64_t value) override { Packet::writeZigZag64(value); }
    void writeZigZag32(int32_t value) override { Packet::writeZigZag32(value); }
    void writeUUID(const std::string& uuid) override { Packet::writeUUID(uuid); }
};

class ServerboundPacket : public Packet {
public:
    using Packet::Packet;
    virtual ~ServerboundPacket() = default;

    // Unable to serialize serverbound packets
    void serialize() override {}
    std::vector<MetadataEntry> getMetadata() override { return Packet::getMetadata(); };
    int64_t getZigZag64() override { return Packet::getZigZag64(); }
    int32_t getZigZag32() override { return Packet::getZigZag32(); }
    std::string getUUID() override { return Packet::getUUID(); }


    // Unable to write to serverbound packets
    void writeZigZag64(int64_t value) override {}

    // Unable to write to serverbound packets
    void writeZigZag32(int32_t value) override {}

    // Unable to write to serverbound packets
    void writeUUID(const std::string& uuid) override {}
};

class ClientboundPacket : public Packet {
public:
    using Packet::Packet;
    virtual ~ClientboundPacket() = default;

    virtual void serialize() override {};
    int64_t getZigZag64() override { return Packet::getZigZag64(); }
    int32_t getZigZag32() override { return Packet::getZigZag32(); }
    std::string getUUID() override { return Packet::getUUID(); }

    void writeZigZag64(int64_t value) override { Packet::writeZigZag64(value); }
    void writeZigZag32(int32_t value) override { Packet::writeZigZag32(value); }
    void writeUUID(const std::string& uuid) override { Packet::writeUUID(uuid); }
};

enum class PacketStatus {
    OK,
    Unknown,
    DeserializeError,
    JsonParseError,
    serializeError,
    JwtParseFailure
};

struct PacketResult {
    PacketStatus status;
    int code;
    std::string message;
};

struct PacketParseResult {
    bool success;
    std::string message;
};

class DeviceOS {
public:
    enum Id : int {
        Undefined       = 0x0,
        Android         = 0x01,
        IOS             = 0x02,
        MacOS           = 0x03,
        AmazonGear      = 0x04,
        GearVR          = 0x05,
        HoloLens        = 0x06,
        Windows         = 0x07,
        Windows32Bit    = 0x08,
        Dedicated       = 0x09,
        TvOS            = 0x0a,
        PlayStation     = 0x0b,
        NintendoSwitch  = 0x0c,
        Xbox            = 0x0d,
        WindowsPhone    = 0x0e,
        Linux           = 0x0f,
        ChromeOS        = 0x10
    };

    static std::string getName(int id);
    static std::string getName(uint32_t  titleId);
    static int getOSId(int titleId);
    static int getDefaultInput(int deviceOS);
};

class InputModes {
public:
    enum inputModes : int {
        InputModeMouse      = 1,
        InputModeTouch      = 2,
        InputModeGamePad    = 3
    };

    enum deviceInputs : int {
        Android         = InputModeTouch,
        IOS             = InputModeTouch,
        AmazonGear      = InputModeTouch,
        Windows         = InputModeMouse,
        Windows32Bit    = InputModeMouse,
        PlayStation     = InputModeGamePad,
        NintendoSwitch  = InputModeGamePad,
        Xbox            = InputModeGamePad,
        // ChromeOS        = 0x10
    };
};

class WindowType {
public:
    enum Id : int8_t  {
        None                = -9,
        Inventory           = -1,
        Container           = 0,
        Workbench           = 1,
        Furnace             = 2,
        Enchantment         = 3,
        BrewingStand        = 4,
        Anvil               = 5,
        Dispenser           = 6,
        Dropper             = 7,
        Hopper              = 8,
        Cauldron            = 9,
        MinecartChest       = 10,
        MinecartHopper      = 11,
        Horse               = 12,
        Beacon              = 13,
        StructureEditor     = 14,
        Trading             = 15,
        CommandBlock        = 16,
        Jukebox             = 17,
        Armor               = 18,
        Hand                = 19,
        CompoundCreator     = 20,
        ElementConstructor  = 21,
        MaterialReducer     = 22,
        LabTable            = 23,
        Loom                = 24,
        Lectern             = 25,
        Grindstone          = 26,
        BlastFurnace        = 27,
        Smoker              = 28,
        Stonecutter         = 29,
        Cartography         = 30,
        HUD                 = 31,
        JigsawEditor        = 32,
        SmithingTable       = 33,
        ChestBoat           = 34,
        DecoratedPot        = 35,
        Crafter             = 36
    };

    // static std::string getName(int id);
};