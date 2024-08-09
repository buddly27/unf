add_library(unfTestTestReporter2
    unfTest/testReporter2/reporter.cpp
)

target_compile_definitions(unfTestTestReporter2
    PRIVATE
        UNF_EXPORTS=1
)

target_link_libraries(unfTestTestReporter2
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestTestReporter2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

file(
    GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugInfo_$<CONFIG>.json"
    INPUT "plugInfo.json"
)
