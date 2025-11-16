#include <iostream>
#include <string>

#include "entityEvent.h"

EntityEventPacket::EntityEventPacket(std::string_view payload) : 
    BidirectionalPacket(payload) {
        deserialize();
}

EntityEventPacket::EntityEventPacket(uint64_t runtimeEntityId, uint8_t eventId, int32_t data) :
    BidirectionalPacket(""), runtimeEntityId(runtimeEntityId), eventId(eventId), data(data) {
        serialize();
}

void EntityEventPacket::serialize() {
    out.writeUnsignedVarInt64(runtimeEntityId);
    out.writeUnsignedVarInt64(eventId);
    BidirectionalPacket::writeZigZag32(data);
    result = { PacketStatus::OK, EntityEventPacket::Success, "success" };
}

void EntityEventPacket::deserialize() {
    runtimeEntityId = stream.getUnsignedVarInt64();
    eventId = stream.getUnsignedVarInt64();
    data = BidirectionalPacket::getZigZag32();
    result = { PacketStatus::OK, EntityEventPacket::Success, "success" };
}

std::string EntityEventPacket::getEventName() {
    switch (eventId) {
        case Jump:                          return "Jump";
        case HurtAnimation:                 return "Hurt Animation";
        case DeathAnimation:                return "Death Animation";
        case ArmSwing:                      return "Arm Swing";
        case StopAttack:                    return "Stop Attack";
        case TameFail:                      return "Tame Fail";
        case TameSuccess:                   return "Tame Success";
        case ShakeWet:                      return "Shake Wet";
        case UseItem:                       return "Use Item";
        case EatGrassAnimation:             return "Eat Grass Animation";
        case FishHookBubble:                return "Fish Hook Bubble";
        case FishHookPosition:              return "Fish Hook Position";
        case FishHookHook:                  return "Fish Hook Hook";
        case FishHookTease:                 return "Fish Hook Tease";
        case SquidInkCloud:                 return "Squid Ink Cloud";
        case ZombieVillagerCure:            return "Zombie Villager Cure";
        case Respawn:                       return "Respawn";
        case IronGolemOfferFlower:          return "Iron Golem Offer Flower";
        case IronGolemWithdrawFlower:       return "Iron Golem Withdraw Flower";
        case LoveParticles:                 return "Love Particles";
        case VillagerAngry:                 return "Villager Angry";
        case VillagerHappy:                 return "Villager Happy";
        case WitchSpellParticles:           return "Witch Spell Particles";
        case FireworkParticles:             return "Firework Particles";
        case InLoveParticles:               return "In Love Particles";
        case SilverfishSpawnAnimation:      return "Silverfish Spawn Animation";
        case GuardianAttack:                return "Guardian Attack";
        case WitchDrinkPotion:              return "Witch Drink Potion";
        case WitchThrowPotion:              return "Witch Throw Potion";
        case MinecartTntPrimeFuse:          return "Minecart TNT Prime Fuse";
        case CreeperPrimeFuse:              return "Creeper Prime Fuse";
        case AirSupplyExpired:              return "Air Supply Expired";
        case PlayerAddXpLevels:             return "Player Add XP Levels";
        case ElderGuardianCurse:            return "Elder Guardian Curse";
        case AgentArmSwing:                 return "Agent Arm Swing";
        case EnderDragonDeath:              return "Ender Dragon Death";
        case DustParticles:                 return "Dust Particles";
        case ArrowShake:                    return "Arrow Shake";
        case EatingItem:                    return "Eating Item";
        case BabyAnimalFeed:                return "Baby Animal Feed";
        case DeathSmokeCloud:               return "Death Smoke Cloud";
        case CompleteTrade:                 return "Complete Trade";
        case RemoveLeash:                   return "Remove Leash";
        case Caravan:                       return "Caravan";
        case ConsumeTotem:                  return "Consume Totem";
        case PlayerCheckTreasureHunter:     return "Player Check Treasure Hunter Achievement";
        case EntitySpawn:                   return "Entity Spawn";
        case DragonPuke:                    return "Dragon Puke";
        case ItemEntityMerge:               return "Item Entity Merge";
        case StartSwim:                     return "Start Swim";
        case BalloonPop:                    return "Balloon Pop";
        case TreasureHunt:                  return "Treasure Hunt";
        case AgentSummon:                   return "Agent Summon";
        case ChargedItem:                   return "Charged Item";
        case Fall:                          return "Fall";
        case GrowUp:                        return "Grow Up";
        case VibrationDetected:             return "Vibration Detected";
        case DrinkMilk:                     return "Drink Milk";
        default:                            return "Unknown Event";
    }
}