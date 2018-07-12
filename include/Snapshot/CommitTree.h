/**
 * CommitTree.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_COMMIT_TREE_H__
#define __OVLSNAP_COMMIT_TREE_H__

#include <string>

#include <parson/parson.h>

#include "Snapshot/Branch.h"
#include "Snapshot/Commit.h"
#include "Snapshot/CommitTreeIterator.h"


class CommitTree {

    public:  // Methods.
        CommitTree(JSON_Object* statusLoadedData);
        ~CommitTree();

        bool newCommit(JSON_Value* data);
        JSON_Object* deleteCommits(int topMostCommits);

        bool newBranch(std::string branchName);
        JSON_Object* deleteBranch(std::string branchName);
        bool switchBranch(std::string branchName);

        void status();

        CommitTreeIterator begin() const;
        CommitTreeIterator end() const;

        // ---

        bool empty();
        bool emptyBranch();
        bool hasBranch(std::string branchName);
        bool isCurrentBranch(std::string branchName);
        JSON_Value* getCommitData(std::string commitId);

    public:  // Copy and move operations are not supported. (MVP of iterator)
        CommitTree(const CommitTree&) = delete;
        CommitTree(CommitTree&&) = delete;
        CommitTree& operator=(const CommitTree&) = delete;
        CommitTree& operator=(CommitTree&&) = delete;

    private:  // Methods.
        void treeInit();
        void initialiseStatus();
        bool newCommit(std::string commitId, JSON_Value* data);
        JSON_Object* pruneTree();

        void setHeadId(std::string commitId);
        void setBranchId(std::string branchName);
        void incrementIdCounter();

    private:  // Members.
        JSON_Object* data = NULL;
        JSON_Object* commitsData = NULL;
        JSON_Object* branchesData = NULL;
        std::string headId;
        std::string branchId;
        Commit* commit = NULL;
        Branch* branch = NULL;
        long int idCounter;

    private:  // Constants.
        static constexpr const char* ROOT_COMMIT_ID = "root";
        static constexpr const char* DEFAULT_BRANCH_ID = "master";
};

#endif  // __OVLSNAP_COMMIT_TREE_H__
