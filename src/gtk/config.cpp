#include <cstdio>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "gear_data/leanny.h"
#include "gtk/config.h"

#ifdef __linux__
#include <sys/stat.h>
#endif

const char* json_localization_key = "localization";
const char* json_seeds_key = "seeds";
const SplLocalization localization_default = SplLocalization::USen;

Config::Config(const std::filesystem::path& path) : config_path{path} {
    // JSON data is read from config path
    std::fstream config_stream(path, std::ios_base::in);
    nlohmann::json config_json;

    // Load JSON data
    if (config_stream.is_open()) {
        config_json = nlohmann::json::parse(config_stream);
        config_stream.close();
    }

    // Load config (or default if none exists)
    const auto& json_localization = config_json[json_localization_key];
    localization = json_localization.is_null()
                       ? localization_default
                       : static_cast<SplLocalization>(json_localization.get<int>());

    const auto& json_seeds = config_json[json_seeds_key];
    if (json_seeds.is_array()) {
        const std::vector<u_int32_t>& loaded_seeds = json_seeds.get<std::vector<u_int32_t>>();
        for (u_int32_t x : loaded_seeds) {
            seeds.insert(x);
        }
    }
}

Config::~Config() {
    // Serialize config to JSON as string
    nlohmann::json json_data;
    json_data[json_localization_key] = localization;
    json_data[json_seeds_key] = seeds;
    const std::string json_text = json_data.dump();

    // Make sure the config base directory exists
    const std::filesystem::path parent_path = config_path.parent_path();
    if (!std::filesystem::exists(parent_path)) {
        std::filesystem::create_directory(parent_path);
    }

    // Write JSON data to config
    std::fstream config_stream{config_path, std::ios_base::out | std::ios_base::trunc};
    if (!config_stream.is_open()) {
        return;
    }
    config_stream << json_text;
    config_stream.close();
}

void Config::LoadSeeds(const std::vector<u_int32_t>& new_seeds) {
    for (u_int32_t seed : new_seeds) {
        seeds.insert(seed);
    }
}

const std::set<u_int32_t>& Config::Seeds() const {
    return seeds;
}
