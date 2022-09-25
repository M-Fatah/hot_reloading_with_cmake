set(HEADER_FILES
)

set(SOURCE_FILES
    game.cpp
)

set (LIBS
    platform
)

# Build game as a dynamic lib.
add_library(game SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(game ${LIBS})
target_compile_options(game PRIVATE ${GLOBAL_COMPILE_OPTIONS})
RANDOMIZE_PDB_NAME(game)

include(GenerateExportHeader)
generate_export_header(game
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h"
    INCLUDE_GUARD_NAME "GAME_API_H"
    EXPORT_MACRO_NAME "GAME_API"
    NO_EXPORT_MACRO_NAME "GAME_NO_API"
    DEPRECATED_MACRO_NAME "GAME_API_DEPRECATED"
    NO_DEPRECATED_MACRO_NAME "GAME_API_NO_DEPRECATED"
)