/**
 * CommandScheduler.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#include <parson/parson.h>

#include "CommandScheduler.h"
#include "Commands/ICommand.h"
#include "Commands/CommandFactory.h"
#include "Logging.h"


CommandScheduler::CommandScheduler(JSON_Object* statusLoadedData):
    statusData(statusLoadedData) {

    // TODO: Check if field is not there.
    this->scheduledCommands = json_object_get_array(
        this->statusData, "scheduledCommands"
    );
}

CommandScheduler::~CommandScheduler() {
}

void CommandScheduler::schedule(ICommand& command) {
    // TODO: Handle edge-cases for various types of commands:
    switch(command.getId()) {
        case CommandFactory::CREATE:
            //  * CreateCommand: check if it already exists
            break;

        case CommandFactory::DROP:
            //  * DropCommand: resize to max num of snapshots, merge consecutives
            break;

        case CommandFactory::MERGE:
            //  * MergeCommnad: resize to max num of snapshots, merge consecutives
            break;

        case CommandFactory::BRANCH:
            break;
    }

    json_array_append_value(this->scheduledCommands, command.serialise());
}

void CommandScheduler::clear() {
    json_array_clear(this->scheduledCommands);
}

void CommandScheduler::status() {
    int commandsCount = json_array_get_count(this->scheduledCommands);
    if (commandsCount == 0) {
        return;
    }

    std::cout << "Scheduled commands after reboot are:" << std::endl;

    ICommand* command;
    JSON_Value* commandData;

    for (int index = 0; index < commandsCount; index++) {
        commandData = json_array_get_value(this->scheduledCommands, index);
        command = this->commandFactory.getCommandByData(commandData);

        if (command == NULL) {
            LOG_ERROR(
                "CommandScheduler: status: Scheduled command at index "
                << index << " with body " << json_serialize_to_string(commandData)
                << " does not contain a known id field!"
            );
            continue;
        }

        command->initialise(commandData);
        std::cout << "  " << command->toString() << std::endl;
    }
}
