#include <cstdio>
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "gear_data/leanny.h"
#include "gear_data/leanny/GearInfoClothes.json.h"
#include "gear_data/leanny/GearInfoHead.json.h"
#include "gear_data/leanny/GearInfoShoes.json.h"
#include "gear_data/leanny/USen.json.h"

const char* localization_path[6] = {"",
                                    "CommonMsg/Gear/GearName_Head",
                                    "CommonMsg/Gear/GearName_Clothes",
                                    "CommonMsg/Gear/GearName_Shoes",
                                    "CommonMsg/Gear/GearBrandName",
                                    "CommonMsg/Gear/GearPowerName"};

LeannyDB::LeannyDB() {
    localization_json = nlohmann::json::parse(USen_json);

    LoadGear(headgear_data, GearInfoHead_json);
    LoadGear(clothes_data, GearInfoClothes_json);
    LoadGear(shoes_data, GearInfoShoes_json);
}
LeannyDB::~LeannyDB() = default;

std::string LeannyDB::GetCode(u_int32_t code, Category type) {
    std::map<u_int32_t, nlohmann::json>* map = [&]() {
        switch (type) {
        case Category::Headgear: {
            return &headgear_data;
        } break;
        case Category::Clothes: {
            return &clothes_data;
        } break;
        case Category::Shoes: {
            return &shoes_data;
        } break;
        default:
            return static_cast<std::map<u_int32_t, nlohmann::json>*>(nullptr);
            break;
        }
    }();
    if ((*map)[code] == nullptr) {
        return {};
    }
    return (*map)[code]["__RowId"].get<std::string>();
}

std::string LeannyDB::GetBrand(u_int32_t id) {
    return std::string{};
}

std::string LeannyDB::LocalizedGearName(const std::string& code) {
    const Category gear_type = [&]() {
        if (code.find("Hed_") == 0) {
            return Category::Headgear;
        } else if (code.find("Clt_") == 0) {
            return Category::Clothes;
        } else if (code.find("Shs_") == 0) {
            return Category::Shoes;
        }
        return Category::Invalid;
    }();

    if (gear_type == Category::Invalid) {
        return {};
    }
    const std::string short_code = code.substr(4);

    nlohmann::json& names = localization_json[localization_path[static_cast<u_int32_t>(gear_type)]];
    nlohmann::json& name = names[short_code];
    if (name.is_null()) {
        return {};
    }

    return name.get<std::string>();
}

std::string LeannyDB::LocalizedBrand(const std::string& code) {
    return std::string{};
}

std::string LeannyDB::LocalizedAbility(const std::string& ability) {
    return std::string{};
}

void LeannyDB::ChangeLanguage(SplatoonLanguage language) {}

void LeannyDB::LoadGear(std::map<u_int32_t, nlohmann::json>& map, const char* json_text) {
    nlohmann::json json_data = nlohmann::json::parse(json_text);

    for (nlohmann::json::iterator it = json_data.begin(); it != json_data.end(); ++it) {
        const u_int32_t id = (*it)["Id"];
        map[id] = (*it);
    }
}
