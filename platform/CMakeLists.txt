set(HEADER_FILES
    application.h
    platform.h
)

set(SOURCE_FILES
)

set(LIBS
)

if(WIN32)
    list(APPEND SOURCE_FILES
        platform_win32.cpp
    )
    list(APPEND LIBS
        winmm.lib
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND SOURCE_FILES
        platform_linux.cpp
    )
    list(APPEND LIBS
        dl
    )
endif()

add_library(platform SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(platform PUBLIC ${LIBS})
target_compile_options(platform PRIVATE ${GLOBAL_COMPILE_OPTIONS})

include(GenerateExportHeader)
generate_export_header(platform
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h"
    INCLUDE_GUARD_NAME "PLATFORM_API_H"
    EXPORT_MACRO_NAME "PLATFORM_API"
    NO_EXPORT_MACRO_NAME "PLATFORM_NO_API"
    DEPRECATED_MACRO_NAME "PLATFORM_API_DEPRECATED"
    NO_DEPRECATED_MACRO_NAME "PLATFORM_API_NO_DEPRECATED"
)