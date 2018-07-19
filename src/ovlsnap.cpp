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
#include "CommandScheduler.h"
#include "Commands/CreateCommand.h"
#include "Commands/DropCommand.h"
#include "Commands/MergeCommand.h"
#include "Snapshot.h"
#include "Status.h"
#include "Utils.h"


static const char USAGE[] =
R"(OverlayFS Snapshots userspace tool.

Usage:
    ovlsnap (enable | disable)
    ovlsnap create <snapshot_name>
    ovlsnap drop <top_most_snapshots>
    ovlsnap merge <top_most_snapshots> [-f]
    ovlsnap clear
    ovlsnap status
    ovlsnap -h | --help

Commands:
    enable    Enable the execution of ovlsnap-init at boot to execute any
              scheduled and mounting operations.
    disable   Disable the execution of ovlsnap-init at boot and make the
              programs passive.
    create    Schedule the creation a new snapshot at boot. The new snapshot
              will be placed at the top of the stack.
    drop      Schedule the removal of top most snapshots at boot. This will
              remove any changes made in the removed snapshots.
    merge     Schedule the merger of snapshots at boot. This will merge all
              changes from the merging snapshots into the level below.
    clear     Remove all scheduled operations.
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
    ovlsnap merge 1 --force)";


int enable(bool isEnabled) {

    // TODO: This might fail, implement and handle errors.
    if (isEnabled) {
        AppState().enable();
    } else {
        AppState().disable();
    }

    return 0;
}

int create(std::string snapshotName, Status status) {
    CreateCommand command = CreateCommand(snapshotName);
    CommandScheduler scheduler = CommandScheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int drop(int topMostSnapshots, Status status) {
    DropCommand command = DropCommand(topMostSnapshots);
    CommandScheduler scheduler = CommandScheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int merge(int topMostSnapshots, bool force, Status status) {
    // TODO: Remove this once it's been deemed safe.
    if (!force) {
        std::cout << "WARNING: This feature is unstable and may break your system!\n";
        std::cout << "Run with --force to use it anyway.\n";
        return -2;
    }

    MergeCommand command = MergeCommand(topMostSnapshots);
    CommandScheduler scheduler = CommandScheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int clear(Status status) {
    CommandScheduler scheduler = CommandScheduler(status.getData());
    scheduler.clear();

    return 0;
}

int status(Status status) {
    AppState().status();
    Snapshot(status.getData()).status();
    CommandScheduler(status.getData()).status();

    std::string cmd = "mount | grep 'overlay'";
    std::string result;

    result = runCmd(cmd.c_str());
    if (!result.empty()) {
        std::cout << "Overlays are mounted:\n";
        std::cout << result;
    } else {
        std::cout << "No overlays are mounted.\n";
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

    // TODO: Check if running with root permissions.
    // TODO: Check for single program instance.

    int rc = 0;
    std::string snapshotName;
    int topMostSnapshots = 1;
    bool force = args["--force"].asBool();

    Status appStatus = Status();
    appStatus.load();

    if (args["enable"].asBool()) {
        rc = enable(true);

    } else if (args["disable"].asBool()) {
        rc = enable(false);

    } else if (args["create"].asBool()) {
        snapshotName = verifySnapshotName(args);
        rc = create(snapshotName, appStatus);

    } else if (args["drop"].asBool()) {
        topMostSnapshots = verifyTopMostSnapshots(args);
        rc = drop(topMostSnapshots, appStatus);

    } else if (args["merge"].asBool()) {
        topMostSnapshots = verifyTopMostSnapshots(args);
        rc = merge(topMostSnapshots, force, appStatus);

    } else if (args["clear"].asBool()) {
        rc = clear(appStatus);

    } else if (args["status"].asBool()) {
        rc = status(appStatus);

    }

    appStatus.save();

    return rc;
}
