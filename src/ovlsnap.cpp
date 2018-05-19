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

#include <docopt/docopt.h>
#include <parson/parson.h>

#include "AppState.h"
#include "Snapshot.h"
#include "Status.h"
#include "Utils.h"


static const char USAGE[] = R"(
    Usage:
        ovlsnap (enable | disable) [-f]
        ovlsnap create <snapshot_name> [-f]
        ovlsnap drop <top_most_snapshots> [-f]
        ovlsnap merge <top_most_snapshots> [-f]
        ovlsnap status
        ovlsnap -h | --help

    Commands:
        enable    Enable snapshots to be mounted at boot.
        disable   Disable snapshots from being mounted at boot.
        create    Schedule the creation a new snapshot at boot. The new
                  snapshot will be placed at the top of the stack.
        drop      Schedule the removal of top most snapshots at boot. This
                  will remove any changes made in the removed snapshots.
        merge     Schedule the merger of snapshots at boot. This will merge
                  all changes from the merging snapshots into the level below.
        status    Show the current state of the system and snapshots.

    Arguments:
        <snapshot_name>        (str) The name of a new snapshot.
        <top_most_snapshots>   (int) A number of snapshots from the top of
                               the stack.

    Options:
        -f, --force   Force the given operation (to overwrite any existing).
        -h, --help    Show this message.

    Examples:
        ovlsnap create 'layer1'
        ovlsnap merge 1 --force
)";


int enable(bool force, Status* appStatus, AppState* appState) {
    JSON_Object* statusData = appStatus->getData();
    AppState::State state = appState->getState();
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
    AppState::State state = appState->getState();
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
    AppState::State state = appState->getState();
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
    AppState::State state = appState->getState();
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
    AppState::State state = appState->getState();
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
    AppState::State state = appState->getState();
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

std::string verifySnapshotName(std::map<std::string, docopt::value> args) {
    std::string snapshotName;
    try {
        snapshotName = args["<snapshot_name>"].asString();
    } catch (...) {
        std::cerr << "Invalid <snapshot_name> argument!" << std::endl;
        std::exit(-1);
    }
    if (snapshotName.empty()) {
        std::cerr << "<snapshot_name> Cannot be empty!" << std::endl;
        std::exit(-1);
    }
    return snapshotName;
}

int verifyTopMostSnapshots(std::map<std::string, docopt::value> args) {
    int topMostSnapshots = 1;
    try {
        topMostSnapshots = (int)args["<top_most_snapshots>"].asLong();
    } catch (...) {
        std::cerr << "Invalid <top_most_snapshots> argument!" << std::endl;
        std::exit(-1);
    }
    if (topMostSnapshots < 1) {
        std::cerr << "<top_most_snapshots> Must be larger than 0!" << std::endl;
        std::exit(-1);
    }
    return topMostSnapshots;
}

int main(int argc, char *argv[]) {
    // Parsing command line arguments with docopt.
    std::map<std::string, docopt::value> args = docopt::docopt(
        USAGE, {argv + 1, argv + argc}
    );

    std::string snapshotName;
    int topMostSnapshots = 1;
    bool force = args["--force"].asBool();

    Status appStatus = Status();
    appStatus.load();

    AppState appState = AppState(appStatus.getData());

    if (args["enable"].asBool()) {
        return enable(force, &appStatus, &appState);

    } else if (args["disable"].asBool()) {
        return disable(force, &appStatus, &appState);

    } else if (args["status"].asBool()) {
        return status(&appStatus, &appState);

    } else if (args["create"].asBool()) {
        snapshotName = verifySnapshotName(args);
        return create(snapshotName, force, &appStatus, &appState);

    } else if (args["drop"].asBool()) {
        topMostSnapshots = verifyTopMostSnapshots(args);
        return drop(topMostSnapshots, force, &appStatus, &appState);

    } else if (args["merge"].asBool()) {
        topMostSnapshots = verifyTopMostSnapshots(args);
        return merge(topMostSnapshots, force, &appStatus, &appState);
    }

    return 0;
}
