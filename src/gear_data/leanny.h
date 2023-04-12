#pragma once

#include <array>
#include <map>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>

enum class GearAbility : u_int32_t;

enum class SplLocalization {
    CNzh,
    EUde,
    EUen,
    EUfr,
    EUit,
    EUnl,
    EUru,
    JPja,
    KRko,
    USen,
    USes,
    USfr,
};

enum class Category {
    Headgear = 0,
    Clothes = 1,
    Shoes = 2,
    Brand,
    Ability,
    Invalid,
};

class LeannyDB {
public:
    explicit LeannyDB(SplLocalization localization = SplLocalization::USen);
    ~LeannyDB();

    std::string GetCode(u_int32_t id, Category gear_type);
    std::string GetBrand(u_int32_t id, Category gear_type);
    std::string LocalizedGearName(const std::string& code);
    std::string LocalizedBrand(const std::string& code);
    std::string LocalizedAbility(const std::string& ability);

    void ChangeLocalization(SplLocalization localization);

private:
    static void LoadGear(std::map<u_int32_t, nlohmann::json>& map, const char* json_text);
    std::tuple<Category, std::string> GetShortCode(const std::string& code);

    std::array<std::map<u_int32_t, nlohmann::json>, 3> gear_data{};
    nlohmann::json localization_json;
    SplLocalization current_localization;
};
