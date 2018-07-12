/**
 * CommitTree.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include "Snapshot/Branch.h"
#include "Snapshot/Commit.h"
#include "Snapshot/CommitTree.h"
#include "Snapshot/CommitTreeIterator.h"
#include "Logging.h"


CommitTree::CommitTree(JSON_Object* statusLoadedData):
    headId(""),
    branchId(""),
    idCounter(0) {

    // TODO: Check if field is not there.
    this->data = json_object_get_object(statusLoadedData, "commitTree");
    this->commitsData = json_object_get_object(this->data, "commits");
    this->branchesData = json_object_get_object(this->data, "branches");
    this->headId = json_object_get_string(this->data, "headId");
    this->branchId = json_object_get_string(this->data, "branchId");
    this->idCounter = json_object_get_number(this->data, "idCounter");

    // Initial setup for the root commit and default branch.
    this->treeInit();
}

CommitTree::~CommitTree() {
    if (this->commit != NULL) {
        delete this->commit;
    }
    if (this->branch != NULL) {
        delete this->branch;
    }
}

bool CommitTree::newCommit(JSON_Value* data) {
    this->initialiseStatus();

    // Generate a new unique commit ID by always increasing a counter.
    this->incrementIdCounter();
    return this->newCommit(std::to_string(this->idCounter), data);
}

bool CommitTree::newCommit(std::string commitId, JSON_Value* data) {
    // Setup the new commit.
    this->commit->setId(commitId);
    this->commit->setData(data);
    this->commit->setFrom(this->headId);

    // Change the tree head and update the current branch.
    this->setHeadId(this->commit->getId());
    this->branch->setHeadId(this->headId);

    // Save the new commit and the updated branch.
    json_object_set_value(
        this->commitsData, this->headId.c_str(), this->commit->serialise()
    );
    json_object_set_value(
        this->branchesData, this->branch->getName().c_str(), this->branch->serialise()
    );
    return true;
}

JSON_Object* CommitTree::deleteCommits(int topMostCommits) {
    if (topMostCommits < 1) {
        return nullptr;
    }
    this->initialiseStatus();

    int index = 0;
    bool found = false;

    // Move the current branch HEAD back topMostCommits times.
    for (const auto& commitData : *this) {
        if (index == topMostCommits) {
            this->commit->initialise(commitData);
            this->branch->setHeadId(this->commit->getId());
            found = true;
            break;
        }
        index++;
    }
    if (!found) {
        this->branch->setHeadId(CommitTree::ROOT_COMMIT_ID);
    }

    // Update the branch data and tree head.
    this->setHeadId(this->branch->getHeadId());
    json_object_set_value(
        this->branchesData, this->branch->getName().c_str(), this->branch->serialise()
    );

    // Prune any commits that are no longer reachable.
    return this->pruneTree();
}

bool CommitTree::newBranch(std::string branchName) {
    this->initialiseStatus();

    // Check if the branch already exists.
    if (json_object_has_value(this->branchesData, branchName.c_str())) {
        LOG_DEBUG("CommitTree: newBranch: '" << branchName << "' already exists");
        return false;
    }

    // Setup the new branch.
    this->branch->setName(branchName);
    this->branch->setHeadId(this->headId);

    // Save the new branch.
    json_object_set_value(
        this->branchesData, branchName.c_str(), this->branch->serialise()
    );

    return true;
}

JSON_Object* CommitTree::deleteBranch(std::string branchName) {
    this->initialiseStatus();

    // Do nothing if the branch doesn't already exist.
    if (!this->hasBranch(branchName)) {
        LOG_DEBUG("CommitTree: deleteBranch: Branch does not exist");
        return nullptr;
    }
    // Cannot delete the current branch, switch first.
    if (this->isCurrentBranch(branchName)) {
        LOG_WARN("CommitTree: deleteBranch: Cannot delete the current branch!");
        return nullptr;
    }

    // Remove the branch and prune the commit tree.
    json_object_remove(this->branchesData, branchName.c_str());
    return this->pruneTree();
}

bool CommitTree::switchBranch(std::string branchName) {
    this->initialiseStatus();

    // Check if the branch exists.
    if (!json_object_has_value(this->branchesData, branchName.c_str())) {
        LOG_DEBUG("CommitTree: switchBranch: Branch does not exist");
        return false;
    }

    // Set the new branch and change the tree head.
    this->setBranchId(branchName);
    this->branch->initialise(
        json_object_get_value(this->branchesData, this->branchId.c_str())
    );
    this->setHeadId(this->branch->getHeadId());
    return true;
}

void CommitTree::status() {
    this->initialiseStatus();

    std::cout << "Commit tree:" << std::endl
              << "  tree HEAD is at " << this->headId << std::endl
              << "  current branch is " << this->branchId << std::endl;

    if (!this->emptyBranch()) {
        std::cout << "Commits on current branch are:" << std::endl;
        for (const auto& commitData : *this) {
            std::cout << "  " << json_serialize_to_string(commitData) << std::endl;
        }
    }

    std::cout << "Available branches are:" << std::endl;
    int numBranches = json_object_get_count(this->branchesData);
    for (int i = 0; i < numBranches; i++) {
        std::cout << "  " << json_serialize_to_string(
            json_object_get_value_at(this->branchesData, i)) << std::endl;
    }
}

CommitTreeIterator CommitTree::begin() const {
    return CommitTreeIterator(this->headId, this->commitsData);
}

CommitTreeIterator CommitTree::end() const {
    return CommitTreeIterator(CommitTree::ROOT_COMMIT_ID, this->commitsData);
}

bool CommitTree::empty() {
    // The root commit is not counted or accessible.
    return json_object_get_count(this->commitsData) <= 1;
}

bool CommitTree::emptyBranch() {
    return this->headId == std::string(CommitTree::ROOT_COMMIT_ID);
}

bool CommitTree::hasBranch(std::string branchName) {
    return json_object_has_value(this->branchesData, branchName.c_str());
}

bool CommitTree::isCurrentBranch(std::string branchName) {
    return this->branchId == branchName;
}

JSON_Value* CommitTree::getCommitData(std::string commitId) {
    if (!json_object_has_value_of_type(this->commitsData, commitId.c_str(), JSONObject)) {
        LOG_ERROR("CommitTree: getCommitData: Could not find commit"
                  << " with commitId '" << commitId << "'");
        return NULL;
    }
    return json_object_get_value(this->commitsData, commitId.c_str());
}


// --- Private Methods --------------------------------------------------------

void CommitTree::treeInit() {
    // Setup the default branch and save it.
    if (json_object_get_count(this->branchesData) == 0) {
        this->newBranch(CommitTree::DEFAULT_BRANCH_ID);
        this->setBranchId(CommitTree::DEFAULT_BRANCH_ID);
    }
    // Setup the root commit and save it.
    if (json_object_get_count(this->commitsData) == 0) {
        this->newCommit(std::string(CommitTree::ROOT_COMMIT_ID), NULL);
    }
}

void CommitTree::initialiseStatus() {
    if (this->commit == NULL) {
        this->commit = new Commit();
    }
    if (this->branch == NULL) {
        this->branch = new Branch();
    }

    this->headId = json_object_get_string(this->data, "headId");
    this->branchId = json_object_get_string(this->data, "branchId");

    this->commit->initialise(
        json_object_get_value(this->commitsData, this->headId.c_str())
    );
    this->branch->initialise(
        json_object_get_value(this->branchesData, this->branchId.c_str())
    );

    LOG_DEBUG("CommitTree: initialiseStatus:"
              << " commit is " << this->commit->toString()
              << " branch is " << this->branch->toString()
              << " tree head is '" << this->headId
              << "' current branch is '" << this->branchId << "'");
}

/**
 * Removes any unreachable commits from the tree.
 *
 * This is achieved by reconstructing the commit tree traversing it from all
 * existing branch heads. Any commit that was not traversed will therefore be
 * removed. The deleted commits are computed by reversing the process of
 * builidng the new commits tree, i.e. removing commits from a duplicate of
 * the original.
 *
 * Returns a JSON_Object* containing the deleted commits.
 */
JSON_Object* CommitTree::pruneTree() {
    std::string currentBranch = this->branchId;
    int numBranches = json_object_get_count(this->branchesData);
    JSON_Value* root = json_value_init_object();
    JSON_Object* newCommitsData = json_value_get_object(root);
    JSON_Object* deletedCommitsData = json_value_get_object(
        json_value_deep_copy(json_object_get_wrapping_value(this->commitsData)
    ));

    // Copy the root commit for the new tree and remove it from the deleted ones.
    json_object_set_value(
        newCommitsData,
        CommitTree::ROOT_COMMIT_ID,
        json_value_deep_copy(
            json_object_get_value(this->commitsData, CommitTree::ROOT_COMMIT_ID)
        )
    );
    json_object_remove(deletedCommitsData, CommitTree::ROOT_COMMIT_ID);

    // Rebuild the tree from all branches.
    for (int i = 0; i < numBranches; i++) {
        this->branch->initialise(json_object_get_value_at(this->branchesData, i));
        this->switchBranch(this->branch->getName());

        // Copy untraversed commits into the new tree. Reaching a traversed
        // commit means it reached an ancestor branch that was already handled.
        // Mirror the operation to compute the list of deleted commits.
        for (const auto& commitData : *this) {
            this->commit->initialise(commitData);
            if (json_object_has_value(newCommitsData, this->commit->getId().c_str())) {
                break;
            }
            json_object_set_value(
                newCommitsData,
                this->commit->getId().c_str(),
                json_value_deep_copy(commitData)
            );
            json_object_remove(
                deletedCommitsData,
                this->commit->getId().c_str()
            );
        }
    }
    // Restore the current branch.
    this->switchBranch(currentBranch);

    LOG_DEBUG("CommitTree: pruneTree: newCommitsData JSON is "
              << json_serialize_to_string_pretty(root));
    LOG_DEBUG("CommitTree: pruneTree: deletedCommitsData JSON is "
              << json_serialize_to_string_pretty(json_object_get_wrapping_value(deletedCommitsData)));

    // Replace the old commit tree with the pruned one.
    json_object_remove(this->data, "commits");
    json_object_set_value(this->data, "commits", root);
    this->commitsData = newCommitsData;

    // Make sure the state is set properly.
    this->initialiseStatus();
    return deletedCommitsData;
}

void CommitTree::setHeadId(std::string headId) {
    this->headId = headId;
    json_object_set_string(this->data, "headId", this->headId.c_str());
    LOG_DEBUG("CommitTree: setHeadId: Tree head is " << headId);
}

void CommitTree::setBranchId(std::string branchName) {
    this->branchId = branchName;
    json_object_set_string(this->data, "branchId", this->branchId.c_str());
    LOG_DEBUG("CommitTree: setBranchId: Current branch is " << branchName);
}

void CommitTree::incrementIdCounter() {
    this->idCounter++;
    json_object_set_number(this->data, "idCounter", this->idCounter);
    LOG_DEBUG("CommitTree: incrementIdCounter: id counter is " << this->idCounter);
}
