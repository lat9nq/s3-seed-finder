add_library(gear_data
    gear.h
    scan.h
    scan.cpp
)

target_link_libraries(gear_data PRIVATE nlohmann_json::nlohmann_json)
