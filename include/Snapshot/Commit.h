/**
 * Commit.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_COMMIT_H__
#define __OVLSNAP_COMMIT_H__

#include <string>

#include <parson/parson.h>

#include "IIdentifiable.h"
#include "ISerialisable.h"


class Commit: public ISerialisable {

    public:
        Commit();
        Commit(JSON_Value* data);
        ~Commit();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        void setId(std::string id);
        std::string getId();

        void setFrom(std::string commitId);
        std::string getFrom();

        void setData(JSON_Value* data);
        JSON_Value* getData();

        std::string toString();

    private:  // Members:
        std::string id;
        std::string from;
        JSON_Value* data = NULL;
};

#endif  // __OVLSNAP_COMMIT_H__
