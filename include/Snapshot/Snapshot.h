/**
 * Snapshot.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_SNAPSHOT_H__
#define __OVLSNAP_SNAPSHOT_H__

#include <string>

#include <parson/parson.h>

#include "ISerialisable.h"


class Snapshot: public ISerialisable {

    public:
        Snapshot();
        Snapshot(std::string name, std::string basePath);
        ~Snapshot();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        void setName(std::string name);
        std::string getName();

        void setBasePath(std::string path);
        std::string getBasePath();

        std::string getPath();

        std::string toString();

    private:  // Members:
        std::string name;
        std::string basePath;
};

#endif  // __OVLSNAP_SNAPSHOT_H__
