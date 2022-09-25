cmake_minimum_required(VERSION 3.20)

project(hot_reloading_with_cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin/"         )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

# Global compile definitions.
add_compile_definitions(
    $<$<PLATFORM_ID:Linux>:PLATFORM_LINUX=1>
    $<$<PLATFORM_ID:Windows>:PLATFORM_WIN32=1>
)

# Global compile options.
if(MSVC)
    set(GLOBAL_COMPILE_OPTIONS -W4 -WX -std:c++17)
else()
    set(GLOBAL_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror -std=c++17)
endif()

# This custom target's job is to configure cmake each time we compile, this is only useful for hot-reloading feature.
message(STATUS "Hot reloading is enabled.")
add_custom_target(hot_reload ALL
    COMMAND cd .. && cmake -B build
)

# Macro used to append a random string to the target's pdb name.
macro(RANDOMIZE_PDB_NAME target)
    string(RANDOM random_string)
    set(pdbname "${target}_")
    string(APPEND pdbname ${random_string})
    set_target_properties(${target}
        PROPERTIES
        COMPILE_PDB_NAME ${pdbname}
        PDB_NAME ${pdbname}
    )
endmacro()

add_subdirectory(platform)
add_subdirectory(launcher)
add_subdirectory(game)