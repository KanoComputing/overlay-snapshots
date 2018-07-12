/**
 * Branch.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_BRANCH_H__
#define __OVLSNAP_BRANCH_H__

#include <string>

#include <parson/parson.h>

#include "IIdentifiable.h"
#include "ISerialisable.h"


class Branch: public ISerialisable {

    public:
        Branch();
        Branch(std::string name);
        ~Branch();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        void setName(std::string name);
        std::string getName();

        void setHeadId(std::string headId);
        std::string getHeadId();

        std::string toString();

    private:  // Members:
        std::string name;
        std::string headId;
};

#endif  // __OVLSNAP_BRANCH_H__
