#include <array>
#include <cstdio>
#include <fstream>
#include <memory>
#include <getopt.h>
#include <nlohmann/json.hpp>
#include <sys/stat.h>

#include "gear_data/gear.h"
#include "gear_data/scan.h"

int main(const int argc, char* const* argv) {
    if (argc == 1) {
        std::fprintf(stderr, "See `%s --help` for more info.\n", argv[0]);
        return 0;
    }

    constexpr struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"seed", required_argument, 0, 's'},
        {"update", required_argument, 0, 'u'},
        {0, 0, 0, 0},
    };

    std::string seed_str{};
    std::string filepath{};
    std::string backup_path{};
    int option_index = 0;
    while (optind < argc) {
        const int arg = getopt_long(argc, argv, "u:f:s:h", longopts, &option_index);
        if (arg == -1) {
            filepath = argv[optind];
            optind++;
        } else {
            switch (arg) {
            case 'u': {
                const std::string str_arg = optarg;
                backup_path = str_arg;
            } break;
            case 'f': {
                const std::string str_arg = optarg;
                filepath = str_arg;
            } break;
            case 's': {
                const std::string str_arg = optarg;
                seed_str = str_arg;
            } break;
            case 'h':
                std::fprintf(stderr, "\n");
                return 0;
            default:
                break;
            }
        }
    }

    if (seed_str.empty()) {
        std::fprintf(stderr, "error: A seed must be specified with `--seed`, but none was given\n");
        return 0;
    }

    const u_int32_t search_seed = [&]() {
        u_int32_t seed;
        std::sscanf(seed_str.c_str(), "%x", &seed);
        return seed;
    }();

    struct stat file_stat {};
    stat(filepath.c_str(), &file_stat);
    const std::size_t file_size = file_stat.st_size;

    std::fstream file_stream(filepath, std::ios_base::binary | std::ios_base::in);
    if (!file_stream.is_open()) {
        std::fprintf(stderr, "error: Could not open file %s\n", filepath.c_str());
        return 0;
    }
    std::unique_ptr<u_int8_t[]> file_data = std::make_unique<u_int8_t[]>(file_size);

    file_stream.read(reinterpret_cast<char*>(file_data.get()), file_size);
    file_stream.close();

    nlohmann::json json_data;

    if (!backup_path.empty()) {
        std::fstream backup_stream(backup_path, std::ios_base::in);
        if (!backup_stream.is_open()) {
            std::fprintf(stderr, "error: Could not open file %s\n", backup_path.c_str());
            return 0;
        }

        json_data = nlohmann::json::parse(backup_stream);

        backup_stream.close();
    }

    ScanInfo scan_info{};

    scan_data(file_data.get(), file_size, json_data, search_seed, scan_info);

    if (scan_info.headgear_count == 0 && scan_info.clothes_count == 0 &&
        scan_info.shoes_count == 0) {
        std::fprintf(stderr, "No gear seeds were found\n");
    } else {
        std::fprintf(
            stderr,
            "Found seeds:\nHeadgear: %d @ 0x%08x\nClothes: %d @ 0x%08x\nShoes: %d @ 0x%08x\n",
            scan_info.headgear_count, scan_info.headgear_address, scan_info.clothes_count,
            scan_info.clothes_address, scan_info.shoes_count, scan_info.shoes_address);
        std::printf("%s\n", json_data.dump().c_str());
    }

    return 0;
}
