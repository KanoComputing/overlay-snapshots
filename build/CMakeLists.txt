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

# Run cmake with `-D BUILD_TYPE=Release` to build the release version.
if ("${BUILD_TYPE}" STREQUAL "Release")
    set(OVLSNAP_INIT ovlsnap-init)
    set(OVLSNAP ovlsnap)
# Run cmake with `-D BUILD_TYPE=Debug` to build the dbg version.
else ("${BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    set(OVLSNAP_INIT ovlsnap-init-dbg)
    set(OVLSNAP ovlsnap-dbg)
endif ()

# TODO: compile this lib to be static not dynamic.
find_library(LIBPARSON_STATIC parson)

add_executable(${OVLSNAP_INIT} "${SRC}/ovlsnap-init.cpp")
target_sources(${OVLSNAP_INIT} PUBLIC
    "${SRC}/AppState.cpp"
    "${SRC}/Snapshot.cpp"
    "${SRC}/Status.cpp"
    "${SRC}/Utils.cpp")
target_link_libraries(${OVLSNAP_INIT} ${LIBPARSON_STATIC})

add_executable(${OVLSNAP} "${SRC}/ovlsnap.cpp")
target_sources(${OVLSNAP} PUBLIC
    "${SRC}/AppState.cpp"
    "${SRC}/Snapshot.cpp"
    "${SRC}/Status.cpp"
    "${SRC}/Utils.cpp")
target_link_libraries(${OVLSNAP} ${LIBPARSON_STATIC})
