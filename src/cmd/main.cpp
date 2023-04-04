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
        std::fprintf(stderr,
                     "usage: %s [options] <data file>\n"
                     "\tSee `%s --help` for more info.\n",
                     argv[0], argv[0]);
        return 0;
    }

    constexpr struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {0, 0, 0, 0},
    };

    std::string filepath;
    int option_index = 0;
    while (optind < argc) {
        const int arg = getopt_long(argc, argv, "h", longopts, &option_index);
        if (arg == -1) {
            filepath = argv[optind];
            optind++;
        } else {
            switch (arg) {
            case 'f': {
                const std::string str_arg = optarg;
                filepath = str_arg;
            } break;
            case 'h':
                std::fprintf(stderr, "\n");
                return 0;
            default:
                break;
            }
        }
    }

    std::fstream file_stream(filepath, std::ios_base::binary | std::ios_base::in);
    if (!file_stream.is_open()) {
        std::fprintf(stderr, "error: Could not open file %s\n", filepath.c_str());
        return 0;
    }

    std::unique_ptr<u_int8_t[]> file_data = std::make_unique<u_int8_t[]>(buffer_size);

    file_stream.read(reinterpret_cast<char*>(file_data.get()), buffer_size);
    file_stream.close();

    //~ std::printf("%08x\n", *reinterpret_cast<u_int32_t*>(file_data.get() + 0xa93a0));

    std::string json_text;

    scan_data(file_data.get(), buffer_size, json_text);

    std::printf("%s\n", json_text.c_str());

    return 0;
}
