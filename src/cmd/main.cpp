#include <array>
#include <cstdio>
#include <fstream>
#include <memory>
#include <getopt.h>

#include "gear_data/gear.h"
#include "gear_data/scan.h"

constexpr u_int32_t buffer_size = 0x10000000;

int main(const int argc, char* const* argv) {
    if (argc == 1) {
        std::fprintf(stderr, "See `%s --help` for more info.\n", argv[0]);
        return 0;
    }

    constexpr struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {"seed", required_argument, 0, 's'},
        {0, 0, 0, 0},
    };

    std::string seed_str{};
    std::string filepath{};
    int option_index = 0;
    while (optind < argc) {
        const int arg = getopt_long(argc, argv, "f:s:h", longopts, &option_index);
        if (arg == -1) {
            filepath = argv[optind];
            optind++;
        } else {
            switch (arg) {
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

    std::fstream file_stream(filepath, std::ios_base::binary | std::ios_base::in);
    if (!file_stream.is_open()) {
        std::fprintf(stderr, "error: Could not open file %s\n", filepath.c_str());
        return 0;
    }

    std::unique_ptr<u_int8_t[]> file_data = std::make_unique<u_int8_t[]>(buffer_size);

    file_stream.read(reinterpret_cast<char*>(file_data.get()), buffer_size);
    file_stream.close();

    ScanInfo scan_info;
    std::string json_text;

    scan_data(file_data.get(), buffer_size, json_text, search_seed, scan_info);

    std::fprintf(stderr,
                 "Found seeds:\nHeadgear: %d @ 0x%08x\nClothes: %d @ 0x%08x\nShoes: %d @ 0x%08x\n",
                 scan_info.headgear_count, scan_info.headgear_address, scan_info.clothes_count,
                 scan_info.clothes_address, scan_info.shoes_count, scan_info.shoes_address);
    std::printf("%s\n", json_text.c_str());

    return 0;
}
