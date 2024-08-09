add_library(unfTestTestReporter1
    unfTest/testReporter1/reporter.cpp
)

target_compile_definitions(unfTestTestReporter1
    PRIVATE
        UNF_EXPORTS=1
)

target_link_libraries(unfTestTestReporter1
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestTestReporter1
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

file(
    GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugInfo_$<CONFIG>.json"
    INPUT "plugInfo.json"
)
