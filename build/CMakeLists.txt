# CMakeLists.txt
#
# Copyright (C) 2018 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
#
# Build script using CMake.


cmake_minimum_required(VERSION 3.1)

project(ovlsnap)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../bin)
set(SRC "${CMAKE_CURRENT_LIST_DIR}/../src")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../include")

set(CMAKE_BUILD_TYPE Debug)

# TODO: compile this lib to be static not dynamic.
find_library(LIBPARSON_STATIC parson)

add_executable(ovlsnap-init "${SRC}/ovlsnap-init.cpp")
target_sources(ovlsnap-init PUBLIC
    "${SRC}/AppState.cpp"
    "${SRC}/Snapshot.cpp"
    "${SRC}/Status.cpp"
    "${SRC}/Utils.cpp")
target_link_libraries(ovlsnap-init ${LIBPARSON_STATIC})

add_executable(ovlsnap "${SRC}/ovlsnap.cpp")
target_sources(ovlsnap PUBLIC
    "${SRC}/AppState.cpp"
    "${SRC}/Snapshot.cpp"
    "${SRC}/Status.cpp"
    "${SRC}/Utils.cpp")
target_link_libraries(ovlsnap ${LIBPARSON_STATIC})
