# Enables modules to include other modules
include_directories(.)

add_compile_options(
    -Wall
    -Werror=array-bounds
    -Werror=conversion
    -Werror=ignored-qualifiers
    -Werror=implicit-fallthrough
    -Werror=missing-declarations
    -Werror=missing-field-initializers
    -Werror=reorder
    -Werror=shadow
    -Werror=sign-compare
    -Werror=switch
    -Werror=uninitialized
    -Werror=unused-function
    -Werror=unused-result
    -Werror=unused-variable
    -Wextra
    -Wmissing-declarations
    -Wno-attributes
    -Wno-invalid-offsetof
    -Wno-sign-conversion
    -Wno-unused-parameter

    $<$<CXX_COMPILER_ID:GNU>:-Werror=class-memaccess>
    $<$<CXX_COMPILER_ID:GNU>:-Werror=unused-but-set-parameter>
    $<$<CXX_COMPILER_ID:GNU>:-Werror=unused-but-set-variable>

    $<$<CXX_COMPILER_ID:Clang>:-fsized-deallocation>
)

add_subdirectory(gear_data)
add_subdirectory(cmd)
add_subdirectory(gtk)
