/**
 * SnapshotManager.cpp
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

#include "Snapshot/Commit.h"
#include "Snapshot/CommitTree.h"
#include "SnapshotManager.h"
#include "Utils.h"
#include "Logging.h"


SnapshotManager::SnapshotManager(JSON_Object* statusLoadedData):
    commitTree(statusLoadedData) {
    this->snapshot = new Snapshot();

    if (!createDir(SnapshotManager::BASE_OVERLAYS_PATH)) {
        LOG_ERROR("SnapshotManager constructor: Could not create base overlays dir!");
    }
    if (!createDir(getWorkDir().c_str())) {
        LOG_ERROR("SnapshotManager constructor: Could not create workdir!");
    }
    if (!createDir(getMergedDir().c_str())) {
        LOG_ERROR("SnapshotManager constructor: Could not create mergeddir!");
    }
    if (!createDir(getRealDir().c_str())) {
        LOG_ERROR("SnapshotManager constructor: Could not create realdir!");
    }
    computePaths();
}

SnapshotManager::~SnapshotManager() {
    if (this->snapshot != NULL) {
        delete this->snapshot;
    }
}

bool SnapshotManager::create(std::string snapshotName) {
    this->snapshot->setBasePath(std::string(SnapshotManager::BASE_OVERLAYS_PATH));
    this->snapshot->setName(snapshotName);

    if (dirExists(this->snapshot->getPath().c_str())) {
        LOG_ERROR("SnapshotManager: create: Snapshots already contains " << snapshotName);
        // TODO: Set error code.
        return false;
    }

    // TODO?: Edge case: the dir is already created.
    if (!createDir(this->snapshot->getPath().c_str())) {
        LOG_ERROR("SnapshotManager: create: Could not create " << this->snapshot->getPath());
        // TODO: Set error code.
        return false;
    }

    this->commitTree.newCommit(this->snapshot->serialise());

    computePaths();
    return true;
}

bool SnapshotManager::drop(int topMostSnapshots) {
    if (topMostSnapshots < 0) {
        // TODO: Set error code.
        return false;
    }

    JSON_Object* deletedCommits = this->commitTree.deleteCommits(topMostSnapshots);
    bool successful = this->cleanupSnapshots(deletedCommits);

    computePaths();
    return successful;
}

bool SnapshotManager::merge(int topMostSnapshots) {
    if (topMostSnapshots < 0) {
        // TODO: Set error code.
        return false;
    }

    int snapshotsRemoved = 0;
    int rc;
    std::string lowerPath = "";
    std::string upperPath = "";
    JSON_Value* tempCommitData = NULL;

    for (const auto& commitData : this->commitTree) {
        if (snapshotsRemoved >= topMostSnapshots) {
            break;
        }

        upperPath = lowerPath;

        this->commit.initialise(commitData);
        this->snapshot->initialise(this->commit.getData());
        lowerPath = this->snapshot->getPath();

        if (upperPath.empty()) {
            continue;
        }

        LOG_DEBUG("SnapshotManager: merge: lowerPath " << lowerPath
                  << " upperPath " << upperPath);

        if (this->mergeHelper(lowerPath, upperPath) != 0) {
            LOG_ERROR("SnapshotManager: merge: Merging " << upperPath << " to "
                      << lowerPath << " failed with rc = " << rc);
            goto end;
        }

        snapshotsRemoved++;
    }

  end:
    this->commitTree.deleteCommits(snapshotsRemoved);

    computePaths();
    return rc == 0;
}

int SnapshotManager::mergeHelper(std::string lowerPath, std::string upperPath) {
    // TODO: This should basically just call the traverse function once and
    // the method won't be needed anymore.

    int rc = 0;
    int tries = 20;
    std::string cmd;

    // TODO: /tmp should be discarded anyway. Safe?
    cmd = std::string("/bin/busybox rm -r ") + upperPath + std::string("/tmp");
    LOG_INFO("SnapshotManager: merge: Silently removing /tmp from " << upperPath);
    system(cmd.c_str());

    // TODO: Is it safe to attempt this multiple times? Why is this needed?
    do {
        rc = traverse(
            lowerPath.c_str(), upperPath.c_str(),
            SnapshotManager::merge_d, SnapshotManager::merge_dp,
            SnapshotManager::merge_f_sl, SnapshotManager::merge_f_sl,
            SnapshotManager::merge_whiteout
        );
        tries--;
    } while (rc != 0 || tries == 0);

    return rc;
}

bool SnapshotManager::newBranch(std::string branchName, bool checkout) {
    this->commitTree.newBranch(branchName);
    return checkout && this->commitTree.switchBranch(branchName);
}

bool SnapshotManager::deleteBranch(std::string branchName) {
    JSON_Object* deletedCommits = this->commitTree.deleteBranch(branchName);
    return this->cleanupSnapshots(deletedCommits);
}

void SnapshotManager::status() {
    std::cout << "SnapshotManager stack is:" << std::endl;

    for (const auto& commitData : this->commitTree) {
        this->commit.initialise(commitData);
        this->snapshot->initialise(this->commit.getData());
        std::cout << "  " << this->snapshot->getPath() << std::endl;
    }
    std::cout << "  " << SnapshotManager::OVERLAY_ROOT_DIR << std::endl;
}

void SnapshotManager::computePaths() {
    this->lowerDir = this->upperDir = "";

    for (const auto& commitData : this->commitTree) {
        this->commit.initialise(commitData);
        this->snapshot->initialise(this->commit.getData());

        if (this->upperDir.empty()) {
            this->upperDir = this->snapshot->getPath();
        } else {
            this->appendLowerDir(this->snapshot->getPath());
        }
    }
    this->appendLowerDir(std::string(SnapshotManager::OVERLAY_ROOT_DIR));

    LOG_DEBUG("SnapshotManager: computePaths:" << std::endl
              << "  upperDir is: " << getUpperDir() << std::endl
              << "  lowerDir is: " << getLowerDir() << std::endl
              << "  workDir is: " << getWorkDir() << std::endl
              << "  mergedDir is: " << getMergedDir() << std::endl
              << "  realDir is: " << getMergedDir() + getRealDir()
    );
}

void SnapshotManager::appendLowerDir(std::string path) {
    if (this->lowerDir.empty()) {
        this->lowerDir = path;
    } else {
        this->lowerDir = this->lowerDir + ":" + path;
    }
}

std::string SnapshotManager::getLowerDir() {
    return this->lowerDir;
}

std::string SnapshotManager::getUpperDir() {
    return this->upperDir;
}

std::string SnapshotManager::getWorkDir() {
    return  // TODO: Make these results constants.
        std::string(SnapshotManager::BASE_OVERLAYS_PATH) + "/" +
        std::string(SnapshotManager::OVERLAY_WORK_DIR);
}

std::string SnapshotManager::getMergedDir() {
    return  // TODO: Make these results constants.
        std::string(SnapshotManager::BASE_OVERLAYS_PATH) + "/" +
        std::string(SnapshotManager::OVERLAY_MERGED_DIR);
}

std::string SnapshotManager::getRealDir() {
    return  // TODO: Make these results constants.
        std::string(SnapshotManager::BASE_OVERLAYS_PATH) + "/" +
        std::string(SnapshotManager::OVERLAY_REAL_DIR);
}

bool SnapshotManager::cleanupSnapshots(JSON_Object* deletedCommits) {
    if (deletedCommits == nullptr) {
        return false;
    }

    bool successful = true;
    std::string cmd;

    for (int index = 0; index < json_object_get_count(deletedCommits); index++) {
        this->commit.initialise(json_object_get_value_at(deletedCommits, index));
        this->snapshot->initialise(this->commit.getData());

        LOG_INFO("SnapshotManager: cleanupSnapshots: Removing snapshot "
                 << this->snapshot->getPath());

        cmd = std::string("/bin/busybox rm -rf ") + this->snapshot->getPath();
        if (system(cmd.c_str()) != 0) {
            LOG_ERROR("SnapshotManager: cleanupSnapshots: Could not remove "
                      << this->snapshot->getPath());
            // TODO: Set error code.
            successful = false;
        }
    }
    json_value_free(json_object_get_wrapping_value(deletedCommits));

    return successful;
}

int SnapshotManager::traverse(
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
                    LOG_WARN("File " << cur->fts_path << " is a special file"
                             << " (device or pipe) which cannot be handled."
                             << " Removing file!");
                }
                break;
            default:
                return_val = -1;
                LOG_ERROR("SnapshotManager: traverse: When opening" << cur->fts_path);
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
                    LOG_ERROR("SnapshotManager: traverse: Failed to stat " << lower_path);
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
    //     LOG_ERROR("SnapshotManager: traverse: errno was set by something");
    //     return_val = -1;
    // }
    return fts_close(ftsp) || return_val;
}

int SnapshotManager::merge_d(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string cmd;
    if (lower_status != NULL) {
        if (file_type(lower_status) == S_IFDIR) {
            bool opaque = false;
            if (is_opaquedir(upper_path, &opaque) < 0) {
                LOG_ERROR("SnapshotManager: merge_d: is_opaquedir failed on " << upper_path);
                return -1;
            }
            if (opaque) {
                cmd = std::string("/bin/busybox rm -r ") +
                      std::string(lower_path);
                LOG_INFO("SnapshotManager: merge_d: " << cmd);
                if (system(cmd.c_str()) < 0) {
                    LOG_ERROR("SnapshotManager: merge_d: Could not remove " << lower_path);
                    return -1;
                };
            } else {
                if (!permission_identical(lower_status, upper_status)) {
                    cmd = std::string("/bin/busybox chmod --reference ") +
                          std::string(upper_path) +
                          std::string(lower_path);
                    LOG_INFO("SnapshotManager: merge_d: " << cmd);
                    system(cmd.c_str());
                }
                return 0; // children must be recursed, and directory itself does not need to be printed
            }
        } else {
            cmd = std::string("/bin/busybox rm %'") + std::string(lower_path);
            LOG_INFO("SnapshotManager: merge_d: " << cmd);
            system(cmd.c_str());
        }
    }
    *fts_instr = FTS_SKIP;
    cmd = std::string("/bin/busybox mv -f ") +
          std::string(upper_path) + std::string(" ") + std::string(lower_path);
    LOG_INFO("SnapshotManager: merge_d: " << cmd);
    return system(cmd.c_str());
}

int SnapshotManager::merge_dp(
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
                LOG_INFO("SnapshotManager: merge_dp: " << cmd);
                return system(cmd.c_str());
            }
        }
    }
    return 0;
}

int SnapshotManager::merge_f_sl(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string rm_cmd =
        std::string("/bin/busybox rm -rf ") + std::string(lower_path);
    std::string mv_cmd =
        std::string("/bin/busybox mv -f ") +
        std::string(upper_path) + std::string(" ") + std::string(lower_path);

    LOG_INFO("SnapshotManager: merge_f_sl: " << rm_cmd);
    LOG_INFO("SnapshotManager: merge_f_sl: " << mv_cmd);
    return system(rm_cmd.c_str()) || system(mv_cmd.c_str());
}

int SnapshotManager::merge_whiteout(
    const char *lower_path, const char* upper_path,
    const struct stat *lower_status, const struct stat *upper_status,
    int *fts_instr
) {
    std::string rml_cmd =
        std::string("/bin/busybox rm -r ") + std::string(lower_path);
    std::string rmu_cmd =
        std::string("/bin/busybox rm ") + std::string(upper_path);

    LOG_INFO("SnapshotManager: merge_whiteout: " << rml_cmd);
    LOG_INFO("SnapshotManager: merge_whiteout: " << rmu_cmd);
    return system(rml_cmd.c_str()) || system(rmu_cmd.c_str());
}
