/**
 * CreateCommand.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_CREATE_COMMAND_H__
#define __OVLSNAP_CREATE_COMMAND_H__

#include <string>

#include <parson/parson.h>

#include "Commands/ICommand.h"
#include "Snapshot.h"


class CreateCommand: public ICommand {

    public:  // Methods.
        CreateCommand();
        CreateCommand(std::string snapshotName);
        ~CreateCommand();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        // Implement ICommand:
        bool execute(Snapshot* snapshot);
        unsigned int getId();
        std::string toString();

    private:  // Members.
        unsigned int id;
        std::string snapshotName;
};

#endif  // __OVLSNAP_CREATE_COMMAND_H__
