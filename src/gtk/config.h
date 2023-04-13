#pragma once

#include <filesystem>
#include <set>
#include <vector>

enum class SplLocalization;

class Config {
public:
    Config(const std::filesystem::path& path);
    ~Config();

    void LoadSeeds(const std::vector<u_int32_t>& new_seeds);
    const std::set<u_int32_t>& Seeds() const;

    SplLocalization localization;
    std::filesystem::path last_open_file;
    std::filesystem::path last_backup;
    std::filesystem::path last_export;

private:
    const std::filesystem::path config_path;
    std::set<u_int32_t> seeds;
};
