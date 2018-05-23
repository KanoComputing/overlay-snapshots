/**
 * CommandFactory.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#include <parson/parson.h>

#include "Commands/CommandFactory.h"
#include "Commands/ICommand.h"
#include "Commands/EnableCommand.h"
#include "Commands/CreateCommand.h"
#include "Commands/DropCommand.h"
#include "Commands/MergeCommand.h"
#include "Logging.h"


CommandFactory::CommandFactory() {
}

CommandFactory::~CommandFactory() {
    if (this->enableCommand != NULL) {
        delete this->enableCommand;
    }
    if (this->createCommand != NULL) {
        delete this->createCommand;
    }
    if (this->dropCommand != NULL) {
        delete this->dropCommand;
    }
    if (this->mergeCommand != NULL) {
        delete this->mergeCommand;
    }
}

ICommand* CommandFactory::getCommandByData(JSON_Value* serialisedData) {
    JSON_Object* jsonObject = json_value_get_object(serialisedData);

    // TODO: Update the Kano libparson to get these:
    // if (!json_object_has_value_of_type(jsonObject, "id", JSONNumber)) {
    //     LOG_ERROR(
    //         "CommandScheduler: status: Scheduled command at index "
    //         << index << " with body " << json_serialize_to_string(serialisedData)
    //         << " does not contain and id field!"
    //     );
    //     return NULL;
    // }

    int id = json_object_get_number(jsonObject, "id");
    return this->getCommandById(id);
}

ICommand* CommandFactory::getCommandById(int id) {
    switch (id) {
        case CommandFactory::ENABLE:
            if (this->enableCommand == NULL) {
                this->enableCommand = new EnableCommand();
            }
            LOG_DEBUG("CommandFactory: getCommandById: command is EnableCommand");
            return this->enableCommand;

        case CommandFactory::CREATE:
            if (this->createCommand == NULL) {
                this->createCommand = new CreateCommand();
            }
            LOG_DEBUG("CommandFactory: getCommandById: command is CreateCommand");
            return this->createCommand;

        case CommandFactory::DROP:
            if (this->dropCommand == NULL) {
                this->dropCommand = new DropCommand();
            }
            LOG_DEBUG("CommandFactory: getCommandById: command is DropCommand");
            return this->dropCommand;

        case CommandFactory::MERGE:
            if (this->mergeCommand == NULL) {
                this->mergeCommand = new MergeCommand();
            }
            LOG_DEBUG("CommandFactory: getCommandById: command is MergeCommand");
            return this->mergeCommand;
    }
    return NULL;
}
