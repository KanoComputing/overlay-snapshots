/**
 * CommandHandler.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_COMMAND_HANDLER_H__
#define __OVLSNAP_COMMAND_HANDLER_H__

#include <parson/parson.h>

#include "Commands/CommandFactory.h"
#include "Commands/ICommand.h"
#include "SnapshotManager.h"


class CommandHandler {

    public:  // Methods.
        CommandHandler(JSON_Object* statusLoadedData);
        ~CommandHandler();

        void executeAll(SnapshotManager* snapshot);

    private:  // Members.
        JSON_Object* statusData = NULL;
        JSON_Array* scheduledCommands = NULL;
        CommandFactory commandFactory;
};

#endif  // __OVLSNAP_COMMAND_HANDLER_H__
