# CMakeLists.txt
#
# Copyright (C) 2018 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
#
# Build script using CMake.


cmake_minimum_required(VERSION 3.1)

project(ovlsnap)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin")
set(SRC "${CMAKE_CURRENT_LIST_DIR}/../src")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../include")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

if("${BUILD_TYPE}" STREQUAL "Development")
    message("++ Setting development flags")
    set(CMAKE_CXX_FLAGS "-D_DEBUG_")
endif()

find_library(LIBDOCOPT_STATIC libdocopt.a)
# TODO: compile this lib to be static not dynamic.
find_library(LIBPARSON_STATIC parson)

# Create a list of all source files.
set(OVLSNAP_SRC
    "${SRC}/Commands/CommandFactory.cpp"
    "${SRC}/Commands/CreateCommand.cpp"
    "${SRC}/Commands/DropCommand.cpp"
    "${SRC}/Commands/MergeCommand.cpp"
    "${SRC}/AppState.cpp"
    "${SRC}/CommandHandler.cpp"
    "${SRC}/CommandScheduler.cpp"
    "${SRC}/Snapshot.cpp"
    "${SRC}/Status.cpp"
    "${SRC}/Utils.cpp"
)

# Create a library from all sources files.
set(LIBOVLSNAP_STATIC ovlsnap-static)
add_library(${LIBOVLSNAP_STATIC} STATIC "${OVLSNAP_SRC}")

# Create the init wrapper executable.
add_executable(ovlsnap-init "${SRC}/ovlsnap-init.cpp")
target_link_libraries(ovlsnap-init
    ${LIBOVLSNAP_STATIC}
    ${LIBPARSON_STATIC}
)

# Create the userspace executable.
add_executable(ovlsnap "${SRC}/ovlsnap.cpp")
target_link_libraries(ovlsnap
    ${LIBOVLSNAP_STATIC}
    ${LIBDOCOPT_STATIC}
    ${LIBPARSON_STATIC}
)
