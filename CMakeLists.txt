# Copyright TBD
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)


#=============================================================================
# Setup project
#=============================================================================
project(Boost.Trie CXX)
cmake_minimum_required(VERSION 3.0)

#=============================================================================
# Setup CMake options
#=============================================================================
include(CMakeDependentOption)
option(BOOST_TRIE_ENABLE_PEDANTIC "Compile with pedantic enabled." ON)
option(BOOST_TRIE_ENABLE_WERROR   "Fail and stop if a warning is triggered." OFF)

#=============================================================================
# Setup compiler flags
#=============================================================================
include(CheckCXXCompilerFlag)
macro(boost_trie_append_flag lst testname flag)
    check_cxx_compiler_flag(${flag} ${testname})
    if (${testname})
        list(APPEND ${lst} ${flag})
    endif()
endmacro()

boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_W_FLAG                         -W)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WALL_FLAG                      -Wall)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WEXTRA_FLAG                    -Wextra)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WNO_LONG_LONG_FLAG             -Wno-long-long)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WNO_UNUSED_LOCAL_TYPEDEFS_FLAG -Wno-unused-local-typedefs)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WNO_UNUSED_PARAMETER_FLAG      -Wno-unused-parameter)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WWRITE_STRINGS_FLAG            -Wwrite-strings)
boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_FTEMPLATE_BACKTRACE_LIMIT_FLAG -ftemplate-backtrace-limit=0)

if (BOOST_TRIE_ENABLE_WERROR)
    boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WERROR_FLAG -Werror)
    boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_WX_FLAG     -WX)
endif()

if (BOOST_TRIE_ENABLE_PEDANTIC)
    boost_trie_append_flag(BOOST_TRIE_CXX_WARNING_FLAGS BOOST_TRIE_HAS_PEDANTIC_FLAG -pedantic)
endif()

add_compile_options(
    ${BOOST_TRIE_CXX_WARNING_FLAGS}
    ${BOOST_TRIE_CXX_FEATURE_FLAGS}
)


#=============================================================================
# Search for packages.
#
# Behavior when the package is found or not is customized at the
# point where the package is required.
#=============================================================================
find_package(Boost)
find_package(Doxygen)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


#=============================================================================
# Caveats
#=============================================================================
if (NOT Boost_FOUND)
    message(FATAL_ERROR "The Boost library headers were not found.")
endif()


#=============================================================================
# Setup subdirectories
#=============================================================================
#   boost_trie_target_name_for(<output variable> <source file>)
#
# Return the target name associated to a source file. If the path of the
# source file relative from the root of trie is `path/to/source/file.cpp`,
# the target name associated to it is `path.to.source.file`.
function(boost_trie_target_name_for out file)
    file(RELATIVE_PATH _relative ${CMAKE_SOURCE_DIR} ${file})
    string(REGEX REPLACE "\\.cpp$" "" _name ${_relative})
    string(REGEX REPLACE "/" "." _name ${_name})
    set(${out} "${_name}" PARENT_SCOPE)
endfunction()

enable_testing()
include_directories(include)
add_subdirectory(benchmark)
add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(test)
