/**
 * CommandFactory.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_COMMAND_FACTORY_H__
#define __OVLSNAP_COMMAND_FACTORY_H__

#include <parson/parson.h>

#include "Commands/ICommand.h"
#include "Commands/CreateCommand.h"
#include "Commands/DropCommand.h"
#include "Commands/MergeCommand.h"
#include "Commands/BranchCommand.h"


class CommandFactory {

    public:  // Enums.
        // WARNING: Do not change the order of the values below! The status
        // file may contain commands with their types already defined.
        enum CommandType {
            /* 0 */ CREATE,
            /* 1 */ DROP,
            /* 2 */ MERGE,
            /* 3 */ BRANCH,
        };

    public:  // Methods.
        CommandFactory();
        ~CommandFactory();

        ICommand* getCommandByData(JSON_Value* serialisedData);
        ICommand* getCommandById(int id);

    private:  // Members.
        CreateCommand* createCommand = NULL;
        DropCommand* dropCommand = NULL;
        MergeCommand* mergeCommand = NULL;
        BranchCommand* branchCommand = NULL;
};

#endif  // __OVLSNAP_COMMAND_FACTORY_H__
