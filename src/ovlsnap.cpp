/**
 * ovlsnap.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * OverlayFS Snapshots userspace tool.
 */


#include <iostream>
#include <string>
#include <stdexcept>

#include <parson/parson.h>

#include "AppState.h"
#include "Snapshot.h"
#include "Status.h"
#include "Utils.h"


const std::string usage =
    "Usage:\n"
    "    ovlsnap (enable | disable) [options]\n"
    "    ovlsnap create <snapshot_name> [options]\n"
    "    ovlsnap drop <top_most_snapshots> [options]\n"
    "    ovlsnap merge <top_most_snapshots> [options]\n"
    "    ovlsnap status\n"
    "    ovlsnap -h | --help\n"
    "\n"
    "Commands:\n"
    "    enable\n"
    "    disable\n"
    "    create\n"
    "    drop\n"
    "    merge\n"
    "    status\n"
    "\n"
    "Arguments:\n"
    "    <snapshot_name>\n"
    "    <top_most_snapshots>\n"
    "\n"
    "Options:\n"
    "    -f, --force\n"
    "    -h, --help\n"
    "\n"
    "Examples:\n"
    "    TODO\n";


void printUsage() {
    std::cout << usage;
}

int enable(bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    if (not force && (state == AppState::DROP || state == AppState::MERGE ||
        state == AppState::CREATE)) {

        std::cout << "Another operation was already scheduled. Use --force"
                  << " to override it.\n";
        return 1;
    }

    appState->changeState(AppState::ENABLED);
    json_array_clear(stateParams);
    appStatus->save();
    return 0;
}

int disable(bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    if (not force && (state == AppState::DROP || state == AppState::MERGE ||
        state == AppState::CREATE)) {

        std::cout << "Another operation was already scheduled. Use --force"
                  << " to override it.\n";
        return 1;
    }

    appState->changeState(AppState::DISABLED);
    json_array_clear(stateParams);
    appStatus->save();
    return 0;
}

int create(std::string snapshotName, bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* snapshots = json_object_get_array(statusData, "snapshots");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    if (jsonArrayContainsString(snapshots, snapshotName)) {
        std::cout << "Snapshot already exists!\n";
        return 1;
    }

    if (not force && (state == AppState::DROP || state == AppState::MERGE ||
        state == AppState::CREATE)) {

        std::cout << "Another operation was already scheduled. Use --force"
                  << " to override it.\n";
        return 1;
    }

    appState->changeState(AppState::CREATE);
    json_array_clear(stateParams);
    json_array_append_string(stateParams, snapshotName.c_str());

    appStatus->save();
    return 0;
}

int drop(int topMostSnapshots, bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    if (not force && (state == AppState::DROP || state == AppState::MERGE ||
        state == AppState::CREATE)) {

        std::cout << "Another operation was already scheduled. Use --force"
                  << " to override it.\n";
        return 1;
    }

    appState->changeState(AppState::DROP);
    json_array_clear(stateParams);
    json_array_append_number(stateParams, topMostSnapshots);
    appStatus->save();
    return 0;
}

int merge(int topMostSnapshots, bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    if (not force && (state == AppState::DROP || state == AppState::MERGE ||
        state == AppState::CREATE)) {

        std::cout << "Another operation was already scheduled. Use --force"
                  << " to override it.\n";
        return 1;
    }

    appState->changeState(AppState::MERGE);
    json_array_clear(stateParams);
    json_array_append_number(stateParams, topMostSnapshots);
    appStatus->save();
    return 0;
}

int status(Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = (AppState::State)(int)json_object_get_number(statusData, "state");
    JSON_Array* stateParams = json_object_get_array(statusData, "state_params");

    std::string cmd = "mount | grep 'overlay'";
    std::string result;

    result = runCmd(cmd.c_str());
    if (!result.empty()) {
        std::cout << "Overlays are mounted:\n";
        std::cout << result;
    } else {
        std::cout << "No overlays are mounted.\n";
    }

    Snapshot(statusData).status();

    if (state == AppState::CREATE) {
        std::string snapshotName = json_array_get_string(stateParams, 0);
        std::cout << "Create snapshot '" << snapshotName << "' operation scheduled.\n"
                  << "Reboot is required for changes to take effect.\n";

    } else if (state == AppState::DROP) {
        int topMostSnapshots = json_array_get_number(stateParams, 0);
        std::cout << "Drop top most " << topMostSnapshots << " snapshots"
                  << " operation scheduled.\nReboot is required for changes to"
                  << " take effect.\n";

    } else if (state == AppState::MERGE) {
        int topMostSnapshots = json_array_get_number(stateParams, 0);
        std::cout << "Merge top most " << topMostSnapshots << " snapshots"
                  << " operation scheduled.\nReboot is required for changes to"
                  << " take effect.\n";
    }
    return 0;
}

int main(int argc, char *argv[]) {
    std::string snapshotName;
    int topMostSnapshots = 0;
    bool force = false;

    Status appStatus = Status();
    appStatus.load();

    AppState appState = AppState(appStatus.getData());

    // This is implemented with a chain of "if else if" because getopt_long()
    // creates disgraceful user interfaces to a program.
    // TODO: clone, compile, and package, the C++ docopt port:
    // https://github.com/docopt/docopt.cpp

    std::string command = std::string(argv[1]);
    std::string option;

    // Parsing options.

    for (int i = 2; i < argc; i++) {
        option = std::string(argv[i]);

        if (option.compare("-f") == 0 || option.compare("--force") == 0) {
            force = true;
        }
    }

    // Parsing commands.

    if (command.compare("enable") == 0) {
        return enable(force, &appStatus, &appState);

    } else if (command.compare("disable") == 0) {
        return disable(force, &appStatus, &appState);

    } else if (command.compare("status") == 0) {
        return status(&appStatus, &appState);

    } else if (command.compare("create") == 0) {
        try {
            snapshotName = std::string(argv[2]);
            if (snapshotName.empty())
                throw std::invalid_argument("Cannot be empty!");
        } catch (...) {
            printUsage();
            return 1;
        }
        return create(snapshotName, force, &appStatus, &appState);

    } else if (command.compare("drop") == 0) {
        try {
            topMostSnapshots = std::stoi(argv[2]);
            if (topMostSnapshots < 1)
                throw std::invalid_argument("Must be larger than 0!");
        } catch (...) {
            printUsage();
            return 1;
        }
        return drop(topMostSnapshots, force, &appStatus, &appState);

    } else if (command.compare("merge") == 0) {
        try {
            topMostSnapshots = std::stoi(argv[2]);
            if (topMostSnapshots < 1)
                throw std::invalid_argument("Must be larger than 0!");
        } catch (...) {
            printUsage();
            return 1;
        }
        return merge(topMostSnapshots, force, &appStatus, &appState);

    } else if (command.compare("-h") == 0 || command.compare("--help") == 0) {
        printUsage();
    }

    return 0;
}
