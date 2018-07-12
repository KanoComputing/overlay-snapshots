/**
 * CommitTreeIterator.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_COMMIT_TREE_ITERATOR_H__
#define __OVLSNAP_COMMIT_TREE_ITERATOR_H__

#include <string>

#include <parson/parson.h>

#include "Snapshot/Commit.h"


class CommitTreeIterator {

    public:  // Methods.
        CommitTreeIterator(std::string headId, JSON_Object* commitsData);
        ~CommitTreeIterator();

        // Dereferencable.
        JSON_Value* operator*();

        // Pre- and post-incrementable.
        CommitTreeIterator& operator++();
        CommitTreeIterator operator++(int);

        // Equality / inequality.
        bool operator==(const CommitTreeIterator& other);
        bool operator!=(const CommitTreeIterator& other);

    private:  // Members.
        JSON_Object* commitsData = NULL;
        std::string commitId;
        Commit* commit;

    private:  // Methods.
        JSON_Value* getCommitData(std::string id);
};

#endif  // __OVLSNAP_COMMIT_TREE_ITERATOR_H__
