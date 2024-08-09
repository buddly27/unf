add_library(unfTestChildReporter
    unfTest/childReporter/reporter.cpp
)

target_compile_definitions(unfTestChildReporter
    PRIVATE
        UNF_EXPORTS=1
)

target_link_libraries(unfTestChildReporter
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestChildReporter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

file(
    GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugInfo_$<CONFIG>.json"
    INPUT "plugInfo.json"
)
