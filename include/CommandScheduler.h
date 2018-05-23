/**
 * CommandScheduler.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_COMMAND_SCHEDULER_H__
#define __OVLSNAP_COMMAND_SCHEDULER_H__

#include <parson/parson.h>

#include "Commands/ICommand.h"
#include "Commands/CommandFactory.h"


class CommandScheduler {

    public:  // Methods.
        CommandScheduler(JSON_Object* statusLoadedData);
        ~CommandScheduler();

        void schedule(ICommand& command);
        void clear();
        void status();

    private:  // Members.
        JSON_Object* statusData = NULL;
        JSON_Array* scheduledCommands = NULL;
        CommandFactory commandFactory;
};

#endif  // __OVLSNAP_COMMAND_SCHEDULER_H__
