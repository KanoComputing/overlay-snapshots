/**
 * ovlsnap-init.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * OverlayFS Snapshots init script. This is executed by the kernel to
 * start the userspace.
 */


#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdexcept>

#include "AppState.h"
#include "Snapshot.h"
#include "Status.h"


void mountDevs() {
    system("/bin/busybox mount -t proc p /proc");
    system("/bin/busybox mount -o remount,rw /");
    // FYI: If fstab is corrupted, you're on your own here.
    system("/bin/busybox mount /boot");
}


void unmountProc() {
    system("/bin/busybox umount /proc");
}

void unmountDevs() {
    system("/bin/busybox umount /boot");
    system("/bin/busybox mount -o remount,ro /");
    unmountProc();
}

int mountOverlays(std::string lowerDir, std::string upperDir, std::string mergedDir, std::string workDir) {
    const std::string cmd =
        std::string("/bin/busybox mount -t overlay overlay -o") +
        std::string(" lowerdir=") + lowerDir + std::string(",upperdir=") + upperDir + std::string(",workdir=") + workDir +
        std::string(" ") + mergedDir;
    std::cout << "mountOverlays: cmd is: " << cmd << "\n";
    return system(cmd.c_str());
}

void pivotAndChroot(std::string mergedDir, std::string realDir) {
    std::cout << "pivotAndChroot: Executing..\n";
    chdir(mergedDir.c_str());
    syscall(SYS_pivot_root, ".", realDir.c_str());
    chroot(".");
}

void sysInit() {
    std::cout << "sysInit: Executing..\n";
    char const * const args[] = {"/sbin/init", NULL};
    execv(args[0], (char**)args);
}

int main(int argc, char *argv[]) {
    int rc = 0;

    try {
        rc = std::atexit(sysInit);
        if (rc != 0) {
            sysInit();
            return 1;  // TODO: Set error in Status instead?
        }

        std::cout << "Hello World!\n";

        mountDevs();

        Status status = Status();
        status.load();

        AppState state = AppState(status.getData());
        Snapshot snapshot = Snapshot(status.getData());

        JSON_Array* stateParams = json_object_get_array(
            status.getData(), "state_params"
        );
        std::string snapshotName;
        int topMostCount;

        switch (state.getState()) {
            case AppState::DISABLED:
                unmountDevs();
                return 0;
                break;

            case AppState::ENABLED:
                break;  // Nothing to do here.

            case AppState::CREATE:
                snapshotName = json_array_get_string(stateParams, 0);
                json_array_clear(stateParams);

                if (snapshot.create(snapshotName)) {
                    state.changeState(AppState::ENABLED);
                    status.save();
                } else {
                    unmountDevs();
                    return 1;
                }
                break;

            case AppState::DROP:
                topMostCount = json_array_get_number(stateParams, 0);
                json_array_clear(stateParams);

                if (snapshot.drop(topMostCount)) {
                    if (snapshot.getSnapshotsCount() > 0) {
                        state.changeState(AppState::ENABLED);
                        status.save();
                    } else {
                        state.changeState(AppState::DISABLED);
                        status.save();
                        unmountDevs();
                        return 0;
                    }
                } else {
                    unmountDevs();
                    return 1;
                }
                break;

            case AppState::MERGE:
                topMostCount = json_array_get_number(stateParams, 0);
                json_array_clear(stateParams);

                if (snapshot.merge(topMostCount)) {
                    if (snapshot.getSnapshotsCount() > 0) {
                        state.changeState(AppState::ENABLED);
                        status.save();
                    } else {
                        state.changeState(AppState::DISABLED);
                        status.save();
                        unmountDevs();
                        return 0;
                    }
                } else {
                    unmountDevs();
                    return 1;
                }
                break;

            case AppState::DROP_CREATE:
                // TODO
                break;

            case AppState::MERGE_CREATE:
                // TODO
                break;

            default:
                throw std::invalid_argument(
                    std::string("Unexpected state ") +
                    std::to_string(state.getState()) +
                    std::string(" received!")
                );
        }

        rc = mountOverlays(
            snapshot.getLowerDir(),
            snapshot.getUpperDir(),
            snapshot.getMergedDir(),
            snapshot.getWorkDir()
        );
        if (rc != 0) {
            unmountDevs();
            return 1;
        }
        unmountProc();

        pivotAndChroot(
            snapshot.getMergedDir(),
            snapshot.getMergedDir() + snapshot.getRealDir()
        );

    } catch (...) {
        std::cout << "main: Error occured!\n";
    }

    // The final step sysInit() will be called by atexit here.
    return 0;
}
