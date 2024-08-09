add_executable(testUnitBroker testBroker.cpp)
target_link_libraries(testUnitBroker
    PRIVATE
        unf
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBroker)

add_executable(testUnitBrokerFlow testBrokerFlow.cpp)
target_link_libraries(testUnitBrokerFlow
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBrokerFlow)

add_executable(testUnitDispatcher testDispatcher.cpp)
target_link_libraries(testUnitDispatcher
    PRIVATE
        unf
        unfTest
        unfTestNewStageDispatcher
        unfTestNewDispatcher
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitDispatcher)

add_executable(testUnitDispatcherPlugin testDispatcherPlugin.cpp)
target_link_libraries(testUnitDispatcherPlugin
    PRIVATE
        unf
        unfTest
        unfTestNewStageDispatcher
        unfTestNewDispatcher
        GTest::gtest
        GTest::gtest_main
)
set(_path "$<TARGET_FILE_DIR:unfTest>/plugins/*Dispatcher*/plugInfo_$<CONFIG>.json")
gtest_discover_tests(
    testUnitDispatcherPlugin
    PROPERTIES ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${_path}$<IF:$<BOOL:${WIN32}>,;,:>$ENV{PXR_PLUGINPATH_NAME}"
)

add_executable(testUnitTransaction testTransaction.cpp)
target_link_libraries(testUnitTransaction
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitTransaction)

add_executable(testUnitObjectsChanged testObjectsChanged.cpp)
target_link_libraries(testUnitObjectsChanged
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitObjectsChanged)

add_executable(testUnitReporterManager testReporterManager.cpp)
target_link_libraries(testUnitReporterManager
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitReporterManager)

add_executable(testUnitReporter testReporter.cpp)
target_link_libraries(testUnitReporter
    PRIVATE
        unf
        unfTest
        unfTestTestReporter1
        unfTestTestReporter2
        unfTestChildReporter
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitReporter)

add_executable(testUnitReporterPlugin testReporterPlugin.cpp)
target_link_libraries(testUnitReporterPlugin
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
set(_path "$<TARGET_FILE_DIR:unfTest>/plugins/*Reporter*/plugInfo_$<CONFIG>.json")
gtest_discover_tests(
    testUnitReporterPlugin
    PROPERTIES ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${_path}$<IF:$<BOOL:${WIN32}>,;,:>$ENV{PXR_PLUGINPATH_NAME}"
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
