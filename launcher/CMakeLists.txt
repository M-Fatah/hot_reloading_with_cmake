set(HEADER_FILES
)

set(SOURCE_FILES
    launcher.cpp
)

set (LIBS
    platform
)

# Build launcher as an executable. This is the entry point.
add_executable(launcher ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(launcher PUBLIC ${LIBS})
target_compile_options(launcher PRIVATE ${GLOBAL_COMPILE_OPTIONS})