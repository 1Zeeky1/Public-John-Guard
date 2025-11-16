#pragma once

#include "util.h"

using namespace bedrock_protocol;

class EntityEventPacket final : public BidirectionalPacket {
public:
    PacketResult result = { PacketStatus::Unknown, EntityEventPacket::Unknown, "Unknown error" };
    uint64_t runtimeEntityId{};
    uint8_t eventId{};
    int32_t  data{};

    enum EntityEventCodes : int {
        Success                 = 0,
        Unknown                 = 1,
    };

    enum EventId : uint8_t {
        Jump                            = 0x01,
        HurtAnimation                   = 0x02,
        DeathAnimation                  = 0x03,
        ArmSwing                        = 0x04,
        StopAttack                      = 0x05,
        TameFail                        = 0x06,
        TameSuccess                     = 0x07,
        ShakeWet                        = 0x08,
        UseItem                         = 0x09,
        EatGrassAnimation               = 0x0A,
        FishHookBubble                  = 0x0B,
        FishHookPosition                = 0x0C,
        FishHookHook                    = 0x0D,
        FishHookTease                   = 0x0E,
        SquidInkCloud                   = 0x0F,
        ZombieVillagerCure              = 0x10,
        Respawn                         = 0x12,
        IronGolemOfferFlower            = 0x13,
        IronGolemWithdrawFlower         = 0x14,
        LoveParticles                   = 0x15,
        VillagerAngry                   = 0x16,
        VillagerHappy                   = 0x17,
        WitchSpellParticles             = 0x18,
        FireworkParticles               = 0x19,
        InLoveParticles                 = 0x1A,
        SilverfishSpawnAnimation        = 0x1B,
        GuardianAttack                  = 0x1C,
        WitchDrinkPotion                = 0x1D,
        WitchThrowPotion                = 0x1E,
        MinecartTntPrimeFuse            = 0x1F,
        CreeperPrimeFuse                = 0x20,
        AirSupplyExpired                = 0x21,
        PlayerAddXpLevels               = 0x22,
        ElderGuardianCurse              = 0x23,
        AgentArmSwing                   = 0x24,
        EnderDragonDeath                = 0x25,
        DustParticles                   = 0x26,
        ArrowShake                      = 0x27,
        EatingItem                      = 0x39,
        BabyAnimalFeed                  = 0x3C,
        DeathSmokeCloud                 = 0x3D,
        CompleteTrade                   = 0x3E,
        RemoveLeash                     = 0x3F,
        Caravan                         = 0x40,
        ConsumeTotem                    = 0x41,
        PlayerCheckTreasureHunter       = 0x42,
        EntitySpawn                     = 0x43,
        DragonPuke                      = 0x44,
        ItemEntityMerge                 = 0x45,
        StartSwim                       = 0x46,
        BalloonPop                      = 0x47,
        TreasureHunt                    = 0x48,
        AgentSummon                     = 0x49,
        ChargedItem                     = 0x4A,
        Fall                            = 0x4B,
        GrowUp                          = 0x4C,
        VibrationDetected               = 0x4D,
        DrinkMilk                       = 0x4E
    };

    explicit EntityEventPacket(std::string_view payload);
    
    EntityEventPacket(uint64_t runtimeEntityId, uint8_t eventId, int32_t data);

    [[nodiscard]] uint64_t getPacketId() const override { return MinecraftPackets::EntityEvent; };
    
    /**
     * Returns the event name of the current payload
     */
    std::string getEventName();

private:
    void serialize();
    void deserialize();

};