#include <array>
#include <cstdlib>

enum class Ability : u_int32_t {
    Scrubbed = 0x0,
    InkSaverMain = 0x1,
    InkSaverSub = 0x2,
    InkRecoveryUp = 0x3,
    RunSpeedUp = 0x4,
    SwimSpeedUp = 0x5,
    SpecialChargeUp = 0x6,
    SpecialSaver = 0x7,
    SpecialPowerUp = 0x8,
    QuickRespawn = 0x9,
    QuickSuperJump = 0xa,
    SubPowerUp = 0xb,
    InkResistanceUp = 0xc,
    SubResistanceUp = 0xd,
    IntensifyAction = 0xe,
    OpeningGambit = 0xf,
    LastDitchEffort = 0x10,
    Tenacity = 0x11,
    Comeback = 0x12,
    NinjaSquid = 0x13,
    Haunt = 0x14,
    ThermalInk = 0x15,
    RespawnPunisher = 0x16,
    AbilityDoubler = 0x17,
    StealthJump = 0x18,
    ObjectShredder = 0x19,
    DropRoller = 0x1a,
    Max = 0x1b,
};

constexpr int lean_ab_map[27] = {-1, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11, 12,
                                 13, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111};

constexpr int to_lean_ab(Ability ab) {
    return lean_ab_map[static_cast<u_int32_t>(ab)];
}

constexpr u_int32_t gear_size = 0xB0;

typedef struct __attribute__((packed)) {
    u_int32_t id;
    std::array<u_int8_t, 0x50> unknown_a;
    Ability main;
    std::array<u_int8_t, 0x14> unknown_b;
    std::array<Ability, 3> subs;
    std::array<u_int8_t, 0x08> unknown_c;
    u_int32_t seed;
    std::array<u_int8_t, 0x2c> unknown_d;
} GearItem;

static_assert(sizeof(GearItem) == gear_size, "Gear item structure size is wrong");

constexpr bool validate(GearItem& item) {
    if (item.id == 0) {
        return false;
    }

    if (item.main >= Ability::Max || item.main == Ability::Scrubbed) {
        return false;
    }

    for (int i = 0; i < 3; i++) {
        if (item.subs[i] >= Ability::Max) {
            return false;
        }
    }

    if (item.seed == 0) {
        return false;
    }

    return true;
}
