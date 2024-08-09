set(_path "${CMAKE_BINARY_DIR}/test/utility/plugins/*Reporter*/plugInfo_$<CONFIG>.json")

pytest_discover_tests(
    PythonUnitTests
    LIBRARY_PATH_PREPEND
        $<TARGET_FILE_DIR:unf>
        $<TARGET_FILE_DIR:unfTest>
        $<TARGET_FILE_DIR:pyUnf>
        $<TARGET_FILE_DIR:pyUnfTest>
    PYTHON_PATH_PREPEND
        "$<TARGET_FILE_DIR:pyUnf>/.."
        "$<TARGET_FILE_DIR:pyUnfTest>/.."
    TRIM_FROM_NAME "^test_"
    BUNDLE_TESTS ${BUNDLE_PYTHON_TESTS}
    DEPENDS unf pyUnf pyUnfTest
    ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${_path}$<IF:$<BOOL:${WIN32}>,;,:>$ENV{PXR_PLUGINPATH_NAME}"
)
