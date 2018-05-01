/**
 * Snapshot.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fts.h>
#include <string.h>  // strcpy, strlen
#include <iostream>

#include <parson/parson.h>

#include "Snapshot.h"
#include "Utils.h"


Snapshot::Snapshot(JSON_Object* statusLoadedData) {
    this->statusData = statusLoadedData;

    this->snapshots = json_object_get_array(this->statusData, "snapshots");
    computePaths();
}

Snapshot::~Snapshot() {
}

bool Snapshot::create(std::string snapshotName) {
    if (jsonArrayContainsString(this->snapshots, snapshotName)) {
        std::cout << "[Error] Snapshot: create: Snapshots already contains "
                  << snapshotName << "\n";
        // TODO: Set error code.
        return false;
    }

    // TODO?: Edge case: the dir is already created.
    std::string snapshotPath = std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" + snapshotName;
    if (mkdir(snapshotPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
        std::cout << "[Error] Snapshot: create: Could not create "
                  << snapshotPath << "\n";
        // TODO: Set error code.
        return false;
    }

    json_array_append_string(this->snapshots, snapshotName.c_str());
    computePaths();
    return true;
}

bool Snapshot::drop(int topMostSnapshots) {
    if (topMostSnapshots < 0) {
        // TODO: Set error code.
        return false;
    }

    int snapshotCount = json_array_get_count(this->snapshots);
    topMostSnapshots = std::min((int)topMostSnapshots, snapshotCount);
    std::string snapshotPath, cmd;

    for (int i = snapshotCount - 1; i >= snapshotCount - topMostSnapshots; --i) {
        snapshotPath =
            std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
            std::string(json_array_get_string(this->snapshots, i));
        cmd = std::string("/bin/busybox rm -rf ") + snapshotPath;

        std::cout << "Snapshot: drop: Removing snapshot " << snapshotPath << "\n";
        if (system(cmd.c_str()) != 0) {
            std::cout << "[Error] Snapshot: drop: Could not remove " << snapshotPath << "\n";
            // TODO: Set error code.
            return false;
        }

        json_array_remove(this->snapshots, i);
    }

    computePaths();
    return true;
}

bool Snapshot::merge(int topMostSnapshots) {
    if (topMostSnapshots < 0) {
        // TODO: Set error code.
        return false;
    }

    int snapshotCount = json_array_get_count(this->snapshots);
    topMostSnapshots = std::min((int)topMostSnapshots, snapshotCount);
    std::string lowerPath, upperPath;
    int rc, tries;
    std::string cmd;

    for (int i = snapshotCount - 1; i >= snapshotCount - topMostSnapshots; --i) {
        upperPath =
            std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
            std::string(json_array_get_string(this->snapshots, i));
        if (i > 0) {
            lowerPath =
                std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
                std::string(json_array_get_string(this->snapshots, i - 1));
        } else {
            lowerPath = std::string(Snapshot::OVERLAY_ROOT_DIR);
        }

        // TODO: /tmp should be discarded anyway. Safe?
        cmd = std::string("/bin/busybox rm -r ") + upperPath + std::string("/tmp");
        std::cout << "Snapshot: merge: Silently removing /tmp from "
                  << upperPath << "\n";
        system(cmd.c_str());

        // TODO: Is it safe to attempt this multiple times?
        tries = 20;
        do {
            rc = traverse(
                lowerPath.c_str(), upperPath.c_str(),
                Snapshot::merge_d, Snapshot::merge_dp,
                Snapshot::merge_f_sl, Snapshot::merge_f_sl,
                Snapshot::merge_whiteout
            );
            tries--;
        } while (rc != 0 || tries == 0);

        if (rc != 0) {
            std::cout << "[ERROR] Snapshot: merge: Merging " << upperPath
                      << " to " << lowerPath << " failed with rc = " << rc << "\n";
            goto end;
        }

        json_array_remove(this->snapshots, i);
    }

  end:
    computePaths();
    return rc == 0;
}

void Snapshot::status() {
    int snapshotCount = json_array_get_count(this->snapshots);
    std::string snapshotPath;

    std::cout << "Snapshot stack is:\n";

    for (int i = snapshotCount - 1; i >= 0; --i) {
        snapshotPath =
            std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
            std::string(json_array_get_string(this->snapshots, i));
        std::cout << "  " << snapshotPath << "\n";
    }
    std::cout << "  " << std::string(Snapshot::OVERLAY_ROOT_DIR) << "\n";
}

void Snapshot::computePaths() {
    this->lowerDir = std::string(Snapshot::OVERLAY_ROOT_DIR);
    this->upperDir = "";

    std::string snapshotName;
    int snapshotCount = json_array_get_count(this->snapshots);

    for (int i = 0; i < snapshotCount - 1; i++) {
        snapshotName = std::string(json_array_get_string(this->snapshots, i));
        this->lowerDir =
            std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" + snapshotName + ":" +
            this->lowerDir;
    }

    if (snapshotCount > 0) {
        this->upperDir =
            std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
            std::string(json_array_get_string(this->snapshots, snapshotCount - 1));
    }

    std::cout << "Snapshot: computePaths:\n"
              << "\tlowerDir is: " << getLowerDir() << "\n"
              << "\tupperDir is: " << getUpperDir() << "\n"
              << "\tworkDir is: " << getWorkDir() << "\n"
              << "\tmergedDir is: " << getMergedDir() << "\n"
              << "\trealDir is: " << getMergedDir() + getRealDir() << "\n";
}

int Snapshot::getSnapshotsCount() {
    return json_array_get_count(this->snapshots);
}

std::string Snapshot::getLowerDir() {
    return this->lowerDir;
}

std::string Snapshot::getUpperDir() {
    return this->upperDir;
}

std::string Snapshot::getWorkDir() {
    return  // TODO: Make these results constants.
        std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
        std::string(Snapshot::OVERLAY_WORK_DIR);
}

std::string Snapshot::getMergedDir() {
    return  // TODO: Make these results constants.
        std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
        std::string(Snapshot::OVERLAY_MERGED_DIR);
}

std::string Snapshot::getRealDir() {
    return  // TODO: Make these results constants.
        std::string(Snapshot::BASE_OVERLAYS_PATH) + "/" +
        std::string(Snapshot::OVERLAY_REAL_DIR);
}

int Snapshot::traverse(
    const char *lower_root, const char *upper_root,
    TRAVERSE_CALLBACK callback_d, TRAVERSE_CALLBACK callback_dp,
    TRAVERSE_CALLBACK callback_f, TRAVERSE_CALLBACK callback_sl,
    TRAVERSE_CALLBACK callback_whiteout
) {

    FTSENT *cur;
    char *paths[2] = {(char *) upper_root, NULL };
    char lower_path[PATH_MAX];
    strcpy(lower_path, lower_root);
    size_t upper_root_len = strlen(upper_root);
    size_t lower_root_len = strlen(lower_root);
    FTS *ftsp = fts_open(paths, FTS_NOCHDIR | FTS_PHYSICAL, NULL);
    if (ftsp == NULL) { return -1; }
    int return_val = 0;
    std::string cmd;

    while (((cur = fts_read(ftsp)) != NULL) && (return_val == 0)) {
        TRAVERSE_CALLBACK callback = NULL;
        switch (cur->fts_info) {
            case FTS_D:
                callback = callback_d;
                break;
            case FTS_DP:
                callback = callback_dp;
                break;
            case FTS_F:
                callback = callback_f;
                break;
            case FTS_SL:
                callback = callback_sl;
                break;
            case FTS_DEFAULT:
                if (is_whiteout(cur->fts_statp)) {
                    callback = callback_whiteout;
                } else {
                    // TODO: Is skipping & removing unsupported files safe?
                    // return_val = -1;
                    std::cout << "[WARNING] File " << std::string(cur->fts_path)
                              << " is a special file (device or pipe) which"
                              << " cannot handled. Removing file!\n";
                }
                break;
            default:
                return_val = -1;
                std::cout << "[ERROR] Snapshot: traverse: When opening"
                          << std::string(cur->fts_path) << "\n";
        }
        if (callback != NULL) {
            int fts_instr = 0;
            struct stat lower_status;
            bool lower_exist = true;
            strcpy(&lower_path[lower_root_len], &(cur->fts_path[upper_root_len]));
            if (lstat(lower_path, &lower_status) != 0) {
                if (errno == ENOENT || errno == ENOTDIR) { // the corresponding lower file (or its ancestor) does not exist at all
                    lower_exist = false;
                } else { // stat failed for some unknown reason
                    std::cout << "[ERROR] Snapshot: traverse: Failed to stat "
                              << std::string(lower_path) << "\n";
                    return_val = -1;
                    break; // do not call callback in this case
                }
            }
            return_val = callback(lower_path, cur->fts_path, lower_exist ? &lower_status : NULL, cur->fts_statp, &fts_instr); // return_val must previously be 0
            if (fts_instr) {
                fts_set(ftsp, cur, fts_instr);
            }
        }
    }
    // If no error happened, fts_read will "sets the external variable errno
    // to 0" according to the documentation.
    // TODO: Figure out why this is causing problems!
    // if (errno) {
    //     std::cout << "[ERROR] Snapshot: traverse: errno was set by something\n";
    //     return_val = -1;
    // }
    return fts_close(ftsp) || return_val;
}

int Snapshot::merge_d(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string cmd;
    if (lower_status != NULL) {
        if (file_type(lower_status) == S_IFDIR) {
            bool opaque = false;
            if (is_opaquedir(upper_path, &opaque) < 0) {
                std::cout << "[ERROR] Snapshot: merge_d: is_opaquedir failed on "
                          << std::string(upper_path) << "\n";
                return -1;
            }
            if (opaque) {
                cmd = std::string("/bin/busybox rm -r ") +
                      std::string(lower_path);
                std::cout << "Snapshot: merge_d: " << cmd << "\n";
                if (system(cmd.c_str()) < 0) {
                    std::cout << "[ERROR] Snapshot: merge_d: Could not remove "
                              << std::string(lower_path) << "\n";
                    return -1;
                };
            } else {
                if (!permission_identical(lower_status, upper_status)) {
                    cmd = std::string("/bin/busybox chmod --reference ") +
                          std::string(upper_path) +
                          std::string(lower_path);
                    std::cout << "Snapshot: merge_d: " << cmd << "\n";
                    system(cmd.c_str());
                }
                return 0; // children must be recursed, and directory itself does not need to be printed
            }
        } else {
            cmd = std::string("/bin/busybox rm %'") + std::string(lower_path);
            std::cout << "Snapshot: merge_d: " << cmd << "\n";
            system(cmd.c_str());
        }
    }
    *fts_instr = FTS_SKIP;
    cmd = std::string("/bin/busybox mv -f ") +
          std::string(upper_path) + std::string(" ") + std::string(lower_path);
    std::cout << "Snapshot: merge_d: " << cmd << "\n";
    return system(cmd.c_str());
}

int Snapshot::merge_dp(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string cmd;
    if (lower_status != NULL) {
        if (file_type(lower_status) == S_IFDIR) {
            bool opaque = false;
            if (is_opaquedir(upper_path, &opaque) < 0) { return -1; }
            if (!opaque) { // delete the directory: it should be empty already
                // TODO: If files were skipped, rmdir won't work here.
                cmd = std::string("/bin/busybox rm -r ") + std::string(upper_path);
                std::cout << "Snapshot: merge_dp: " << cmd << "\n";
                return system(cmd.c_str());
            }
        }
    }
    return 0;
}

int Snapshot::merge_f_sl(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string rm_cmd =
        std::string("/bin/busybox rm -rf ") + std::string(lower_path);
    std::string mv_cmd =
        std::string("/bin/busybox mv -f ") +
        std::string(upper_path) + std::string(" ") + std::string(lower_path);

    std::cout << "Snapshot: merge_f_sl: " << rm_cmd << "\n";
    std::cout << "Snapshot: merge_f_sl: " << mv_cmd << "\n";
    return system(rm_cmd.c_str()) || system(mv_cmd.c_str());
}

int Snapshot::merge_whiteout(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string rml_cmd =
        std::string("/bin/busybox rm -r ") + std::string(lower_path);
    std::string rmu_cmd =
        std::string("/bin/busybox rm ") + std::string(upper_path);

    std::cout << "Snapshot: merge_whiteout: " << rml_cmd << "\n";
    std::cout << "Snapshot: merge_whiteout: " << rmu_cmd << "\n";
    return system(rml_cmd.c_str()) || system(rmu_cmd.c_str());
}
