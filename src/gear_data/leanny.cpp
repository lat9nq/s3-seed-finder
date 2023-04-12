#include <array>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>
#include "gear_data/leanny.h"
#include "gear_data/leanny/CNzh.json.h"
#include "gear_data/leanny/EUde.json.h"
#include "gear_data/leanny/EUen.json.h"
#include "gear_data/leanny/EUfr.json.h"
#include "gear_data/leanny/EUnl.json.h"
#include "gear_data/leanny/EUru.json.h"
#include "gear_data/leanny/GearInfoClothes.json.h"
#include "gear_data/leanny/GearInfoHead.json.h"
#include "gear_data/leanny/GearInfoShoes.json.h"
#include "gear_data/leanny/JPja.json.h"
#include "gear_data/leanny/KRko.json.h"
#include "gear_data/leanny/USen.json.h"
#include "gear_data/leanny/USes.json.h"
#include "gear_data/leanny/USfr.json.h"

const char* localization_path[6] = {
    "CommonMsg/Gear/GearName_Head",  "CommonMsg/Gear/GearName_Clothes",
    "CommonMsg/Gear/GearName_Shoes", "CommonMsg/Gear/GearBrandName",
    "CommonMsg/Gear/GearPowerName",  "",
};

const std::map<SplLocalization, const char*> localization_json_text = {
    {SplLocalization::CNzh, CNzh_json}, {SplLocalization::EUde, EUde_json},
    {SplLocalization::EUen, EUen_json}, {SplLocalization::EUfr, EUfr_json},
    {SplLocalization::EUnl, EUnl_json}, {SplLocalization::EUru, EUru_json},
    {SplLocalization::JPja, JPja_json}, {SplLocalization::KRko, KRko_json},
    {SplLocalization::USen, USen_json}, {SplLocalization::USes, USes_json},
    {SplLocalization::USfr, USfr_json},
};

LeannyDB::LeannyDB(SplLocalization localization) : current_localization{localization} {
    localization_json = nlohmann::json::parse(localization_json_text.at(localization));

    LoadGear(gear_data[static_cast<u_int32_t>(Category::Headgear)], GearInfoHead_json);
    LoadGear(gear_data[static_cast<u_int32_t>(Category::Clothes)], GearInfoClothes_json);
    LoadGear(gear_data[static_cast<u_int32_t>(Category::Shoes)], GearInfoShoes_json);
}
LeannyDB::~LeannyDB() = default;

std::string LeannyDB::GetCode(u_int32_t id, Category type) {
    if (type > Category::Shoes) {
        return {};
    }
    auto& map = gear_data[static_cast<u_int32_t>(type)];
    auto& gear_item = map[id];
    if (gear_item.is_null()) {
        return std::to_string(id);
    }
    return map[id]["__RowId"].get<std::string>();
}

std::string LeannyDB::GetBrand(u_int32_t id, Category type) {
    if (type > Category::Shoes) {
        return {};
    }
    auto& map = gear_data[static_cast<u_int32_t>(type)];
    auto& gear_item = map[id];
    if (gear_item.is_null()) {
        return "???";
    }
    return map[id]["Brand"].get<std::string>();
}

std::string LeannyDB::LocalizedGearName(const std::string& code) {
    const auto info = GetShortCode(code);
    const Category gear_type = std::get<0>(info);
    const std::string& short_code = std::get<1>(info);

    nlohmann::json& names = localization_json[localization_path[static_cast<u_int32_t>(gear_type)]];
    nlohmann::json& name = names[short_code];
    if (name.is_null()) {
        return code;
    }

    return name.get<std::string>();
}

std::string LeannyDB::LocalizedBrand(const std::string& code) {
    nlohmann::json& names =
        localization_json[localization_path[static_cast<u_int32_t>(Category::Brand)]];
    nlohmann::json& name = names[code];
    if (name.is_null()) {
        return code;
    }

    return name.get<std::string>();
}

std::string LeannyDB::LocalizedAbility(const std::string& ability) {
    nlohmann::json& names =
        localization_json[localization_path[static_cast<u_int32_t>(Category::Ability)]];
    nlohmann::json& name = names[ability];
    if (name.is_null()) {
        return ability;
    }

    return name.get<std::string>();
}

void LeannyDB::ChangeLocalization(SplLocalization localization) {
    current_localization = localization;
    localization_json = nlohmann::json::parse(localization_json_text.at(localization));
}

void LeannyDB::LoadGear(std::map<u_int32_t, nlohmann::json>& map, const char* json_text) {
    nlohmann::json json_data = nlohmann::json::parse(json_text);

    for (nlohmann::json::iterator it = json_data.begin(); it != json_data.end(); ++it) {
        const u_int32_t id = (*it)["Id"];
        map[id] = (*it);
    }
}

std::tuple<Category, std::string> LeannyDB::GetShortCode(const std::string& code) const {
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

    return std::make_tuple(gear_type, short_code);
}
