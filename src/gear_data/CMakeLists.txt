add_library(gear_data
    gear.h
    scan.h
    scan.cpp
)

target_link_libraries(gear_data PUBLIC nlohmann_json::nlohmann_json)
