add_executable(s3-seed-finder
    main.cpp
)

target_link_libraries(s3-seed-finder PRIVATE gear_data nlohmann_json::nlohmann_json)
