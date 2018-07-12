/**
 * SnapshotManager.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_SNAPSHOT_MANAGER_H__
#define __OVLSNAP_SNAPSHOT_MANAGER_H__

#include <string>

#include <parson/parson.h>

#include "Snapshot/Commit.h"
#include "Snapshot/CommitTree.h"
#include "Snapshot/Snapshot.h"


typedef int (*TRAVERSE_CALLBACK)(
    const char* lower_path, const char* upper_path,
    const struct stat* lower_status, const struct stat* upper_status,
    int* fts_instr
);


class SnapshotManager {

    public:  // Methods.
        SnapshotManager(JSON_Object* statusLoadedData);
        ~SnapshotManager();

        bool create(std::string snapshotName);
        bool drop(int topMostSnapshotManagers);
        bool merge(int topMostSnapshotManagers);
        bool newBranch(std::string branchName, bool checkout);
        bool deleteBranch(std::string branchName);
        void status();

        std::string getLowerDir();
        std::string getUpperDir();
        std::string getWorkDir();
        std::string getMergedDir();
        std::string getRealDir();

    private:  // Members.
        Snapshot* snapshot = NULL;
        CommitTree commitTree;
        Commit commit;
        std::string lowerDir;
        std::string upperDir;

    private:  // Methods.
        int mergeHelper(std::string lowerPath, std::string upperPath);
        void computePaths();
        void appendLowerDir(std::string path);
        bool cleanupSnapshots(JSON_Object* deletedCommits);

        static int traverse(
            const char* lower_root, const char* upper_root,
            TRAVERSE_CALLBACK callback_d, TRAVERSE_CALLBACK callback_dp,
            TRAVERSE_CALLBACK callback_f, TRAVERSE_CALLBACK callback_sl,
            TRAVERSE_CALLBACK callback_whiteout
        );
        static int merge_d(
            const char* lower_path, const char* upper_path,
            const struct stat* lower_status, const struct stat* upper_status,
            int* fts_instr
        );
        static int merge_dp(
            const char* lower_path, const char* upper_path,
            const struct stat* lower_status, const struct stat* upper_status,
            int* fts_instr
        );
        static int merge_f_sl(
            const char *lower_path, const char* upper_path,
            const struct stat* lower_status, const struct stat* upper_status,
            int* fts_instr
        );
        static int merge_whiteout(
            const char* lower_path, const char* upper_path,
            const struct stat* lower_status, const struct stat* upper_status,
            int* fts_instr
        );

    private:  // Constants.
        static constexpr const char* OVERLAY_ROOT_DIR = "/";
        static constexpr const char* BASE_OVERLAYS_PATH = "/overlays";
        static constexpr const char* OVERLAY_WORK_DIR = "workdir";
        static constexpr const char* OVERLAY_REAL_DIR = "real";
        static constexpr const char* OVERLAY_MERGED_DIR = "merged";
};

#endif  // __OVLSNAP_SNAPSHOT_MANAGER_H__
