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
#include "CommandHandler.h"
#include "Logging.h"
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
    LOG_DEBUG("ovlsnap-init: mountOverlays: cmd is: " << cmd);
    return system(cmd.c_str());
}

void pivotAndChroot(std::string mergedDir, std::string realDir) {
    LOG_DEBUG("ovlsnap-init: pivotAndChroot: Executing..");
    chdir(mergedDir.c_str());
    syscall(SYS_pivot_root, ".", realDir.c_str());
    chroot(".");
}

void sysInit() {
    LOG_DEBUG("ovlsnap-init: sysInit: Executing..");
    char const * const args[] = {"/sbin/init", NULL};
    execv(args[0], (char**)args);
}

int main(int argc, char *argv[]) {
    LOG_DEBUG("ovlsnap-init: Hello World!");

    int rc = 0;

    try {
        rc = std::atexit(sysInit);
        if (rc != 0) {
            sysInit();
            rc = 1;  // TODO: Set error in Status instead.
            goto end;
        }

        mountDevs();

        Status status = Status();
        status.load();

        AppState state = AppState(status.getData());
        Snapshot snapshot = Snapshot(status.getData());

        CommandHandler(status.getData()).executeAll(&state, &snapshot);
        status.save();

        switch (state.getState()) {
            case AppState::DISABLED:
                unmountDevs();
                goto end;

            case AppState::ENABLED:
                rc = mountOverlays(
                    snapshot.getLowerDir(),
                    snapshot.getUpperDir(),
                    snapshot.getMergedDir(),
                    snapshot.getWorkDir()
                );
                if (rc != 0) {
                    unmountDevs();
                    rc = 2;  // TODO: Set error in Status instead.
                    goto end;
                }
                unmountProc();

                pivotAndChroot(
                    snapshot.getMergedDir(),
                    snapshot.getMergedDir() + snapshot.getRealDir()
                );

            default:
                throw std::invalid_argument(
                    std::string("Unexpected state ") +
                    std::to_string(state.getState()) +
                    std::string(" received!")
                );
        }
    } catch (...) {
        LOG_ERROR("ovlsnap-init: Error occured!");
    }

  end:
    // The final step sysInit() will be called by atexit here.
    return rc;
}
