/**
 * EnableCommand.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#include <string>

#include <parson/parson.h>

#include "AppState.h"
#include "Commands/CommandFactory.h"
#include "Commands/EnableCommand.h"
#include "Commands/ICommand.h"
#include "Logging.h"
#include "Snapshot.h"


EnableCommand::EnableCommand():
    EnableCommand(true) {
}

EnableCommand::EnableCommand(bool isEnabled):
    id(CommandFactory::ENABLE),
    isEnabled(isEnabled) {

    LOG_DEBUG("EnableCommand: constructor called");
}

EnableCommand::~EnableCommand() {
}

bool EnableCommand::initialise(JSON_Value* serialisedData) {
    JSON_Object* jsonObject = json_value_get_object(serialisedData);

    // TODO: Update the Kano libparson to get these:
    // if (!json_object_has_value_of_type(jsonObject, "id", JSONNumber)) {
    //     return false;
    // }
    // if (json_object_get_number(jsonObject, "id") != ICommand::ENABLE) {
    //     return false;
    // }
    // if (!json_object_has_value_of_type(jsonObject, "is_enabled", JSONBoolean)) {
    //     return false;
    // }

    this->isEnabled = json_object_get_boolean(jsonObject, "is_enabled");
}

JSON_Value* EnableCommand::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_number(data, "id", this->id);
    json_object_set_boolean(data, "is_enabled", this->isEnabled);

    return root;
}

bool EnableCommand::execute(AppState* state, Snapshot* snapshot) {
    LOG_DEBUG("EnableCommand: execute: Called");

    if (this->isEnabled) {
        state->changeState(AppState::ENABLED);
    } else {
        state->changeState(AppState::DISABLED);
    }
}

unsigned int EnableCommand::getId() {
    return this->id;
}

std::string EnableCommand::toString() {
    std::string result;

    if (this->isEnabled) {
        result = "EnableCommand: Overlays will be mounted on boot";
    } else {
        result = "EnableCommand: Overlays mounting will be disabled";
    }
    return result;
}
