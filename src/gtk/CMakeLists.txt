add_executable(s3-seed-finder-gtk
    main.cpp
    main.glade
    main.h
)

set(GLADE_FILES
    main.glade
)

set(GLADE_HEADER_GENERATOR ${PROJECT_SOURCE_DIR}/CMakeModules/GenerateHeaderFromText.cmake)
set(GLADE_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/glade.h.in)

foreach(GLADE_FILE ${GLADE_FILES})
    get_filename_component(VAR_NAME ${GLADE_FILE} NAME_WE)
    set(VAR_NAME ${VAR_NAME}_glade)
    set(GLADE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${GLADE_FILE})
    set(GLADE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${GLADE_FILE}.h)
    add_custom_command(
        OUTPUT
            ${GLADE_HEADER}
        COMMAND
            ${CMAKE_COMMAND} -P ${GLADE_HEADER_GENERATOR} ${GLADE_SOURCE} ${GLADE_HEADER} ${VAR_NAME} ${GLADE_TEMPLATE}
        MAIN_DEPENDENCY
            ${GLADE_SOURCE}
        DEPENDS
            ${GLADE_HEADER_GENERATOR}
            ${GLADE_TEMPLATE}
    )
    list(APPEND GLADE_HEADERS ${GLADE_HEADER})
endforeach()

add_custom_target(glade_headers
DEPENDS
    ${GLADE_HEADERS}
SOURCES
    ${GLADE_FILES}
    glade.h.in
)
unset(GLADE_HEADERS)

add_dependencies(s3-seed-finder-gtk glade_headers)

if (NOT WIN32)
    target_link_options(s3-seed-finder-gtk PRIVATE "LINKER:-export-dynamic")
endif()

target_include_directories(s3-seed-finder-gtk PRIVATE ${PROJECT_BINARY_DIR}/src)
target_include_directories(s3-seed-finder-gtk PRIVATE ${GTK3_INCLUDE_DIRS} ${GMODULE_EXPORT_INCLUDE_DIRS})
target_link_libraries(s3-seed-finder-gtk PRIVATE ${GTK3_LIBRARIES} ${GMODULE_EXPORT_LIBRARIES})
target_link_libraries(s3-seed-finder-gtk PRIVATE gear_data)
