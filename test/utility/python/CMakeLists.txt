add_library(pyUnfTest SHARED
    module.cpp
    wrapNotice.cpp
)

# Supress warnings from Boost Python regarding deprecated Bind placeholders.
# https://github.com/boostorg/python/issues/359
target_compile_definitions(
    pyUnfTest PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS
)

target_compile_definitions(
    pyUnfTest PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES
)

set_target_properties(pyUnfTest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/unf_test"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/unf_test"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/unf_test"
)

target_include_directories(pyUnfTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(pyUnfTest
    PUBLIC
        unf
        unfTest
        Boost::python
)

set_target_properties(pyUnfTest
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME _unf_test
        LIBRARY_OUTPUT_DIRECTORY unf_test
)

if(WIN32)
    # Python modules must be suffixed with .pyd on Windows.
    set_target_properties(pyUnfTest
        PROPERTIES
            SUFFIX ".pyd"
    )
endif()

target_compile_definitions(
    pyUnfTest
    PRIVATE
        BOOST_PYTHON_NO_PY_SIGNATURES
        PXR_PYTHON_ENABLED=1
        PXR_PYTHON_MODULES_ENABLED=1
        MFB_PACKAGE_MODULE=unf_test
        MFB_PACKAGE_NAME=unf_test
        MFB_ALT_PACKAGE_NAME=unf_test
	UNF_EXPORTS=1
)

file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:pyUnfTest>/__init__.py"
    INPUT "__init__.py"
)
