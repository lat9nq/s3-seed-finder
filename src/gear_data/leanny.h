#pragma once

#include <map>
#include <string>
#include <nlohmann/json.hpp>

enum class GearAbility : u_int32_t;

enum class SplatoonLanguage {
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
    USfr
};

enum class Category {
    Invalid,
    Headgear,
    Clothes,
    Shoes,
    Brand,
    Ability,
};

class LeannyDB {
public:
    explicit LeannyDB();
    ~LeannyDB();

    std::string GetCode(u_int32_t id, Category gear_type);
    std::string GetBrand(u_int32_t id);
    std::string LocalizedGearName(const std::string& code);
    std::string LocalizedBrand(const std::string& code);
    std::string LocalizedAbility(const std::string& ability);

    void ChangeLanguage(SplatoonLanguage language);

private:
    static void LoadGear(std::map<u_int32_t, nlohmann::json>& map, const char* json_text);

    std::map<u_int32_t, nlohmann::json> headgear_data;
    std::map<u_int32_t, nlohmann::json> clothes_data;
    std::map<u_int32_t, nlohmann::json> shoes_data;
    nlohmann::json localization_json;
    SplatoonLanguage current_localization{SplatoonLanguage::USen};
};
