/**
 * CommitTreeIterator.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <string>

#include <parson/parson.h>

#include "Snapshot/CommitTreeIterator.h"
#include "Logging.h"


CommitTreeIterator::CommitTreeIterator(std::string headId, JSON_Object* commitsData):
    commitId(headId),
    commitsData(commitsData),
    commit(new Commit()) {
    this->commit->initialise(this->getCommitData(this->commitId));
}

CommitTreeIterator::~CommitTreeIterator() {
    if (this->commit != NULL) {
        delete this->commit;
    }
}

// Dereferencable.
JSON_Value* CommitTreeIterator::operator*() {
    return this->getCommitData(this->commitId);
}

// Pre-incrementable: ++iterator.
CommitTreeIterator& CommitTreeIterator::operator++() {
    this->commitId = this->commit->getFrom();
    this->commit->initialise(this->getCommitData(this->commitId));
    return *this;
}

// Post-incrementable: iterator++.
CommitTreeIterator CommitTreeIterator::operator++(int) {
    CommitTreeIterator tmp = *this;
    this->commitId = this->commit->getFrom();
    this->commit->initialise(this->getCommitData(this->commitId));
    return tmp;
}

// Equality: iterator == end().
bool CommitTreeIterator::operator==(const CommitTreeIterator& other) {
    return this->commit->getData() == other.commit->getData();
}

// Equality: iterator != end().
bool CommitTreeIterator::operator!=(const CommitTreeIterator& other) {
    return !(*this == other);
}

// --- Private Methods ---

JSON_Value* CommitTreeIterator::getCommitData(std::string id) {
    if (!json_object_has_value_of_type(this->commitsData, id.c_str(), JSONObject)) {
        LOG_ERROR("CommitTreeIterator: initialiseCommit: Could not find commit"
                  << " with id '" << id << "'");
        return json_value_init_object();
    }
    return json_object_get_value(this->commitsData, id.c_str());
}
