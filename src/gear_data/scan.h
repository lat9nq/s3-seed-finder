#pragma once

#include <cstdlib>
#include <string>
#include <nlohmann/json.hpp>
#include "gear_data/gear.h"

typedef struct {
    int headgear_count;
    int clothes_count;
    int shoes_count;
    u_int32_t headgear_address;
    u_int32_t clothes_address;
    u_int32_t shoes_address;
    GearItem* headgear;
    GearItem* clothes;
    GearItem* shoes;
} ScanInfo;

u_int32_t find(const u_int8_t* data, const std::size_t length, const u_int32_t interval,
               const u_int32_t target);
int load_gear_items(GearItem** gear_items, const u_int32_t address, const u_int8_t* data,
                    const std::size_t length);
nlohmann::json items_to_json(const GearItem* items, nlohmann::json& json_data);
void scan_data(const u_int8_t* data, const std::size_t length, nlohmann::json& json_data,
               u_int32_t seed, ScanInfo& scan_info);
constexpr bool validate(GearItem& item);
