add_library(gear_data
    gear.h
    leanny.cpp
    leanny.h
    scan.h
    scan.cpp
    json.h.in
)

set(JSON_FILES
    leanny/GearInfoHead.json
    leanny/GearInfoClothes.json
    leanny/GearInfoShoes.json
)

set(SPL_LOCALIZATIONS
    CNzh
    EUde
    EUen
    EUfr
    EUnl
    EUru
    JPja
    KRko
    USen
    USes
    USfr
)

function(download url target)
    if (EXISTS "${target}")
        return()
    endif()
    message(STATUS "Downloading ${url}")
    file(DOWNLOAD "${url}" "${target}")
endfunction()

set(LEAN_DB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/leanny")
cmake_path(RELATIVE_PATH LEAN_DB_PATH BASE_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(LEAN_DB_PATH "${PROJECT_BINARY_DIR}/${LEAN_DB_PATH}")
set(LEAN_DB_URL "https://raw.githubusercontent.com/Leanny/leanny.github.io/master/splat3/data")

foreach(SPL_LOCALIZATION ${SPL_LOCALIZATIONS})
    download("${LEAN_DB_URL}/language/${SPL_LOCALIZATION}.json" "${LEAN_DB_PATH}/${SPL_LOCALIZATION}.json")
    list(APPEND JSON_FILES "leanny/${SPL_LOCALIZATION}.json")
endforeach()
download("${LEAN_DB_URL}/mush/${GAME_VERSION}/GearInfoHead.json" "${LEAN_DB_PATH}/GearInfoHead.json")
download("${LEAN_DB_URL}/mush/${GAME_VERSION}/GearInfoClothes.json" "${LEAN_DB_PATH}/GearInfoClothes.json")
download("${LEAN_DB_URL}/mush/${GAME_VERSION}/GearInfoShoes.json" "${LEAN_DB_PATH}/GearInfoShoes.json")

set(JSON_HEADER_GENERATOR ${PROJECT_SOURCE_DIR}/CMakeModules/GenerateHeaderFromText.cmake)
set(JSON_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/json.h.in)

foreach(JSON_FILE ${JSON_FILES})
    get_filename_component(VAR_NAME ${JSON_FILE} NAME_WE)
    set(VAR_NAME ${VAR_NAME}_json)
    set(JSON_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${JSON_FILE})
    set(JSON_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${JSON_FILE}.h)
    add_custom_command(
        OUTPUT
            ${JSON_HEADER}
        COMMAND
            ${CMAKE_COMMAND} -P ${JSON_HEADER_GENERATOR} ${JSON_SOURCE} ${JSON_HEADER} ${VAR_NAME} ${JSON_TEMPLATE}
        MAIN_DEPENDENCY
            ${JSON_SOURCE}
        DEPENDS
            ${JSON_HEADER_GENERATOR}
            ${JSON_TEMPLATE}
    )
    list(APPEND JSON_HEADERS ${JSON_HEADER})
endforeach()

add_custom_target(json_headers
DEPENDS
    ${JSON_HEADERS}
SOURCES
    ${JSON_FILES}
    json.h.in
)
unset(JSON_HEADERS)

add_dependencies(gear_data json_headers)

target_include_directories(gear_data PRIVATE ${PROJECT_BINARY_DIR}/src)
target_link_libraries(gear_data PUBLIC nlohmann_json::nlohmann_json)
