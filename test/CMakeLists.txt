# Copyright TBD
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#=============================================================================
# Setup unit tests
#=============================================================================

# Target to build and run all the unit tests
add_custom_target(tests ALL
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "test.+"
    COMMENT "Build and run all the unit tests.")

# A list of all the test files
file(GLOB_RECURSE BOOST_TRIE_TEST_SOURCES "*.cpp")

if (NOT EXISTS $ENV{BOOST_ROOT})
    message(FATAL_ERROR "The BOOST_ROOT environment variable is not properly "
    "set")
endif()

# Add all the unit tests
add_library(boost_test_framework STATIC IMPORTED)
set_property(
    TARGET boost_test_framework
    PROPERTY
    IMPORTED_LOCATION $ENV{BOOST_ROOT}/stage/lib/libboost_unit_test_framework.a
)

foreach(file IN LISTS BOOST_TRIE_TEST_SOURCES)
    boost_trie_target_name_for(_target "${file}")
    add_executable(${_target} EXCLUDE_FROM_ALL "${file}")
    add_test(NAME ${_target} COMMAND ${_target})
    add_dependencies(tests ${_target})
    target_link_libraries(${_target} boost_test_framework)
endforeach()
