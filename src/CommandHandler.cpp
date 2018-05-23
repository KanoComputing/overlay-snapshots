/**
 * CommandHandler.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#include <parson/parson.h>

#include "AppState.h"
#include "CommandHandler.h"
#include "Commands/ICommand.h"
#include "Commands/CommandFactory.h"
#include "Logging.h"
#include "Snapshot.h"


CommandHandler::CommandHandler(JSON_Object* statusLoadedData):
    statusData(statusLoadedData) {

    // TODO: Check if field is not there.
    this->scheduledCommands = json_object_get_array(
        this->statusData, "scheduled_commands"
    );
}

CommandHandler::~CommandHandler() {
}

void CommandHandler::executeAll(AppState* state, Snapshot* snapshot) {
    int commandsCount = json_array_get_count(this->scheduledCommands);

    ICommand* command;
    JSON_Value* commandData;

    for (int index = 0; index < commandsCount; index++) {
        commandData = json_array_get_value(this->scheduledCommands, index);
        command = this->commandFactory.getCommandByData(commandData);

        if (command == NULL) {
            LOG_ERROR(
                "CommandHandler: executeAll: Scheduled command at index "
                << index << " with body " << json_serialize_to_string(commandData)
                << " does not contain a known id field!"
            );
            continue;
        }

        command->initialise(commandData);
        command->execute(state, snapshot);
    }

    json_array_clear(this->scheduledCommands);
}
