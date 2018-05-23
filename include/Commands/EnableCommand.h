/**
 * EnableCommand.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_ENABLE_COMMAND_H__
#define __OVLSNAP_ENABLE_COMMAND_H__

#include <string>

#include <parson/parson.h>

#include "AppState.h"
#include "Commands/ICommand.h"
#include "Snapshot.h"


class EnableCommand: public ICommand {

    public:  // Methods.
        EnableCommand();
        EnableCommand(bool isEnabled);
        ~EnableCommand();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        // Implement ICommand:
        bool execute(AppState* state, Snapshot* snapshot);
        unsigned int getId();
        std::string toString();

    private:  // Members.
        unsigned int id;
        bool isEnabled;
};

#endif  // __OVLSNAP_ENABLE_COMMAND_H__
