#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <nlohmann/json.hpp>

#include "gear.h"
#include "scan.h"

// An apparently repeating int for each gear item
// Not sure where all it appears or if it's exclusive to gear
// Lands us immediately after the first gear item id (+0x10)
constexpr u_int32_t gear_marker = 0x4ccaf9b0;

constexpr u_int32_t gear_region_delta = 0x2c020;

constexpr u_int32_t no_result = 0xFFFFFFFF;

u_int32_t find(const u_int8_t* data, const std::size_t length, const u_int32_t interval,
               const u_int32_t target) {
    for (u_int32_t i = 0; i < length; i += interval) {
        if (*reinterpret_cast<const u_int32_t*>(data + i) == target) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

int load_gear_items(GearItem** gear_items, const u_int32_t address, const u_int8_t* data,
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
            delete old;
        }
    }

    return count;
}

void scan_data(const u_int8_t* data, const std::size_t length, std::string& json_text) {
    const u_int32_t search_result = find(data, length, 0x10, gear_marker);
    const u_int32_t first_headgear = search_result - 0x10;
    const u_int32_t first_clothes = first_headgear + gear_region_delta;
    const u_int32_t first_shoes = first_clothes + gear_region_delta;

    GearItem* headgear = nullptr;
    GearItem* clothes = nullptr;
    GearItem* shoes = nullptr;
    const int headgear_count = load_gear_items(&headgear, first_headgear, data, length);
    const int clothes_count = load_gear_items(&clothes, first_clothes, data, length);
    const int shoes_count = load_gear_items(&shoes, first_shoes, data, length);

    json_text.clear();
}
