/**
 * CreateCommand.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#include <string>
#include <sstream>

#include <parson/parson.h>

#include "Commands/CommandFactory.h"
#include "Commands/CreateCommand.h"
#include "Commands/ICommand.h"
#include "Logging.h"
#include "SnapshotManager.h"


CreateCommand::CreateCommand():
    CreateCommand("") {
}

CreateCommand::CreateCommand(std::string snapshotName):
    id(CommandFactory::CREATE),
    snapshotName(snapshotName) {

    if (this->snapshotName.empty()) {
        // TODO: Explode.
    }

    LOG_DEBUG("CreateCommand: constructor called");
}

CreateCommand::~CreateCommand() {
}

bool CreateCommand::initialise(JSON_Value* serialisedData) {
    JSON_Object* jsonObject = json_value_get_object(serialisedData);

    if (!json_object_has_value_of_type(jsonObject, "id", JSONNumber)) {
        LOG_ERROR("CreateCommand: initialise: serialisedData does not contain a"
                  << " 'id' field with a JSONNumber value!");
        return false;
    }
    if (json_object_get_number(jsonObject, "id") != CommandFactory::CREATE) {
        LOG_ERROR("CreateCommand: initialise: serialisedData is not for a"
                  << " CREATE command!");
        return false;
    }
    if (!json_object_has_value_of_type(jsonObject, "snapshot_name", JSONString)) {
        LOG_ERROR("CreateCommand: initialise: serialisedData does not contain a"
                  << " 'snapshot_name' field with a JSONString value!");
        return false;
    }

    this->snapshotName = json_object_get_string(jsonObject, "snapshot_name");
}

JSON_Value* CreateCommand::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_number(data, "id", this->id);
    json_object_set_string(data, "snapshot_name", this->snapshotName.c_str());

    return root;
}

bool CreateCommand::execute(SnapshotManager* snapshotManager) {
    LOG_DEBUG("CreateCommand: execute: Called");

    snapshotManager->create(this->snapshotName);
}

unsigned int CreateCommand::getId() {
    return this->id;
}

std::string CreateCommand::toString() {
    std::ostringstream result;
    result << "CreateCommand: Snapshot '" << this->snapshotName
           << "' will be created";
    return result.str();
}
