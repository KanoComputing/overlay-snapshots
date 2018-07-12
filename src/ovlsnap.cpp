/**
 * ovlsnap.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * OverlayFS Snapshots userspace tool.
 */


#include <time.h>
#include <iostream>
#include <string>
#include <stdexcept>

#include <docopt/docopt.h>
#include <parson/parson.h>

#include "Commands/BranchCommand.h"
#include "Commands/CreateCommand.h"
#include "Commands/DropCommand.h"
#include "Commands/MergeCommand.h"
#include "Snapshot/CommitTree.h"
#include "AppState.h"
#include "CommandScheduler.h"
#include "SnapshotManager.h"
#include "Status.h"
#include "Utils.h"


static const char USAGE[] =
R"(OverlayFS Snapshots userspace tool.

Usage:
    ovlsnap (enable | disable)
    ovlsnap create <snapshot_name>
    ovlsnap drop <top_most_snapshots>
    ovlsnap branch <branch_name> [-c | -D]
    ovlsnap clear
    ovlsnap status
    ovlsnap -h | --help

Commands:
    enable
        Enable the execution of ovlsnap-init at boot to execute any
        scheduled and mounting operations.
    disable
        Disable the execution of ovlsnap-init at boot and make the
        programs passive.
    create
        Schedule the creation a new snapshot on the current branch at boot.
        The new snapshot will be placed at the top of the stack.
    drop
        Schedule the removal of top most snapshots on the current branch at boot.
        This will remove any changes made in the removed snapshots.
    branch
        Schedule the creation or removal of a branch of snapshots
        at boot from the current HEAD.
    clear
        Remove all scheduled operations.
    status
        Show the current state of the system and snapshots.

Arguments:
    <snapshot_name>       (str) The name of a new snapshot.
    <top_most_snapshots>  (int) A number of snapshots from the top of the stack.
    <branch_name>         (str) The name of a branch of snapshots.

Options:
    -c, --checkout  Switch to the new or existing snapshot branch.
    -D, --delete    Delete the specified snapshot branch.
    -h, --help      Show this message.

Examples:
    ovlsnap create 'layer1'
        Create a new snapshot named 'layer1' on the current branch.
    ovlsnap drop 2
        Remove the 2 top most snapshots on the current branch.
    ovlsnap branch 'dev' --checkout
        Create a new snapshot branch named 'dev' and switch to it.
    ovlsnap branch 'master' --delete
        Remove the snapshot branch named 'master' and all snapshots on it.)";


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
    CreateCommand command(snapshotName);
    CommandScheduler scheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int drop(int topMostSnapshots, Status status) {
    DropCommand command(topMostSnapshots);
    CommandScheduler scheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int merge(int topMostSnapshots, Status status) {
    MergeCommand command(topMostSnapshots);
    CommandScheduler scheduler(status.getData());

    // TODO: This might fail, implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int branch(std::string branchName, bool checkout, bool branchDelete, Status status) {
    BranchCommand command(branchName, checkout, branchDelete);
    CommandScheduler scheduler(status.getData());

    // TODO: The above might fail. Implement and handle errors.
    scheduler.schedule(command);

    return 0;
}

int clear(Status status) {
    CommandScheduler scheduler(status.getData());
    scheduler.clear();

    return 0;
}

int status(Status status) {
    AppState().status();
    SnapshotManager(status.getData()).status();
    CommandScheduler(status.getData()).status();
    CommitTree(status.getData()).status();

    // TODO: Reimplement this without a system call or remove entirely.
    std::string cmd = "mount | grep 'overlay'";
    std::string result;

    result = runCmd(cmd.c_str());
    if (!result.empty()) {
        std::cout << "Overlays are mounted:" << std::endl;
        std::cout << result;
    } else {
        std::cout << "No overlays are mounted." << std::endl;
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

std::string verifyBranchName(std::map<std::string, docopt::value> args) {
    std::string branchName;
    try {
        branchName = args["<branch_name>"].asString();
    } catch (...) {
        std::cerr << "Invalid <branch_name> argument!" << std::endl;
        std::exit(-1);
    }
    if (branchName.empty()) {
        std::cerr << "<branch_name> Cannot be empty!" << std::endl;
        std::exit(-1);
    }
    return branchName;
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
    std::string branchName;
    int topMostSnapshots = 1;
    bool checkout = args["--checkout"].asBool();
    bool branchDelete = args["--delete"].asBool();

    Status appStatus;
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
    /* TODO: Enable this once the feature is adapted for CommitTree.
    } else if (args["merge"].asBool()) {
        topMostSnapshots = verifyTopMostSnapshots(args);
        rc = merge(topMostSnapshots, appStatus);
    */
    } else if (args["branch"].asBool()) {
        branchName = verifyBranchName(args);
        rc = branch(branchName, checkout, branchDelete, appStatus);

    } else if (args["clear"].asBool()) {
        rc = clear(appStatus);

    } else if (args["status"].asBool()) {
        rc = status(appStatus);

    }

    appStatus.save();

    return rc;
}
