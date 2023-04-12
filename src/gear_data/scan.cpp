#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <nlohmann/json.hpp>

#include "gear.h"
#include "scan.h"

// Delta between offset for headgear and clothes, or clothes to shoes
constexpr u_int32_t gear_region_delta = 0x2c020;

constexpr u_int32_t no_result = 0xFFFFFFFF;

u_int32_t Find(const u_int8_t* data, const std::size_t length, const u_int32_t interval,
               const u_int32_t target) {
    for (u_int32_t i = 0; i < length; i += interval) {
        if (*reinterpret_cast<const u_int32_t*>(data + i) == target) {
            return i;
        }
    }
    return no_result;
}

int LoadGearItems(GearItem** gear_items, const u_int32_t address, const u_int8_t* data,
                  const std::size_t length) {
    int array_length = 32;
    *gear_items = new GearItem[array_length]();

    int count = 0;
    for (u_int32_t i = address; i < length; i += gear_size) {
        std::memcpy(&(*gear_items)[count], data + i, sizeof(GearItem));
        if ((*gear_items)[count].id == 0) {
            break;
        }
        count++;
        if (count == array_length) {
            GearItem* temp = new GearItem[array_length << 1]();
            std::memcpy(temp, *gear_items, array_length * sizeof(GearItem));
            array_length <<= 1;
            GearItem* old = *gear_items;
            *gear_items = temp;
            delete[] old;
        }
    }

    return count;
}

nlohmann::json ItemsToJson(const GearItem* items, nlohmann::json& json_data) {
    for (int i = 0; items[i].id != 0; i++) {
        char id[32];
        std::snprintf(id, 32, "%d", items[i].id);
        const GearItem& item = items[i];

        json_data[id] = {{"ExSkillArray",
                          {ToLeanAb(item.subs[0]), ToLeanAb(item.subs[1]), ToLeanAb(item.subs[2])}},
                         {"ExDrinksArray", nlohmann::json::array()},
                         {"MainSkill", ToLeanAb(item.main)},
                         {"RandomContext", item.seed}};
    }

    return json_data;
}

void ScanData(const u_int8_t* data, const std::size_t length, nlohmann::json& json_data,
              const std::set<u_int32_t>& seeds, ScanInfo& scan_info) {
    for (u_int32_t seed : seeds) {
        ScanData(data, length, json_data, seed, scan_info);
        if (scan_info.headgear_count + scan_info.clothes_count + scan_info.shoes_count > 0) {
            return;
        }
    }
}

void ScanData(const u_int8_t* data, const std::size_t length, nlohmann::json& json_data,
              u_int32_t seed, ScanInfo& scan_info) {
    const u_int32_t search_result = Find(data, length, 0x10, seed);

    if (search_result == no_result) {
        return;
    }

    const u_int32_t found_item = search_result - 0x80;

    if (found_item > search_result) {
        return;
    }

    // Find the first gear item in the pile
    u_int32_t current = found_item;
    GearItem test;
    std::memcpy(&test, data + found_item, gear_size);
    if (!Validate(test)) {
        return;
    }

    while (test.id != 0) {
        current -= gear_size;
        if (current > length) {
            return;
        }
        if (!Validate(test)) {
            return;
        }
        std::memcpy(&test, data + current, gear_size);
    }
    current += gear_size;

    // Find the first pile of gear items
    u_int32_t pile = current;
    std::memcpy(&test, data + pile, gear_size);
    while (test.seed != 0) {
        pile -= gear_region_delta;
        if (pile > length) {
            return;
        }
        if (!Validate(test)) {
            return;
        }
        std::memcpy(&test, data + pile, gear_size);
    }
    pile += gear_region_delta;

    const u_int32_t first_headgear = pile;
    const u_int32_t first_clothes = first_headgear + gear_region_delta;
    const u_int32_t first_shoes = first_clothes + gear_region_delta;

    GearItem* headgear = nullptr;
    GearItem* clothes = nullptr;
    GearItem* shoes = nullptr;
    scan_info.headgear_count = LoadGearItems(&headgear, first_headgear, data, length);
    scan_info.clothes_count = LoadGearItems(&clothes, first_clothes, data, length);
    scan_info.shoes_count = LoadGearItems(&shoes, first_shoes, data, length);
    scan_info.headgear_address = first_headgear - 0x40;
    scan_info.clothes_address = first_clothes - 0x40;
    scan_info.shoes_address = first_shoes - 0x40;

    if (scan_info.headgear != nullptr) {
        delete[] scan_info.headgear;
        delete[] scan_info.clothes;
        delete[] scan_info.shoes;
    }

    scan_info.headgear = headgear;
    scan_info.clothes = clothes;
    scan_info.shoes = shoes;

    ItemsToJson(headgear, json_data["GearDB"]["HaveGearHeadMap"]);
    ItemsToJson(clothes, json_data["GearDB"]["HaveGearClothesMap"]);
    ItemsToJson(shoes, json_data["GearDB"]["HaveGearShoesMap"]);
}
