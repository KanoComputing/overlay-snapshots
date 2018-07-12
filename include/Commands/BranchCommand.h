/**
 * BranchCommand.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_BRANCH_COMMAND_H__
#define __OVLSNAP_BRANCH_COMMAND_H__

#include <string>

#include <parson/parson.h>

#include "Commands/ICommand.h"
#include "SnapshotManager.h"


class BranchCommand: public ICommand {

    public:  // Methods.
        BranchCommand();
        BranchCommand(std::string branchName, bool checkout, bool deleteBranch);
        ~BranchCommand();

        // Implement ISerialisable:
        bool initialise(JSON_Value* serialisedData);
        JSON_Value* serialise();

        // Implement ICommand:
        bool execute(SnapshotManager* snapshotManager);
        unsigned int getId();
        std::string toString();

    private:  // Members.
        unsigned int id;
        std::string branchName;
        bool checkout;
        bool deleteBranch;
};

#endif  // __OVLSNAP_BRANCH_COMMAND_H__
