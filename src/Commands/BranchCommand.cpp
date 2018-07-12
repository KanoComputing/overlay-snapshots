/**
 * BranchCommand.cpp
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
#include "Commands/ICommand.h"
#include "Commands/BranchCommand.h"
#include "Logging.h"
#include "SnapshotManager.h"


BranchCommand::BranchCommand():
    BranchCommand("", false, false) {
}

BranchCommand::BranchCommand(std::string branchName, bool checkout, bool deleteBranch):
    id(CommandFactory::BRANCH),
    branchName(branchName),
    checkout(checkout),
    deleteBranch(deleteBranch) {

    if (this->branchName.empty()) {
        // TODO: Explode.
    }

    LOG_DEBUG("BranchCommand: constructor called");
}

BranchCommand::~BranchCommand() {
}

bool BranchCommand::initialise(JSON_Value* serialisedData) {
    JSON_Object* jsonObject = json_value_get_object(serialisedData);

    if (!json_object_has_value_of_type(jsonObject, "id", JSONNumber)) {
        LOG_ERROR("BranchCommand: initialise: serialisedData does not contain a"
                  << " 'id' field with a JSONNumber value!");
        return false;
    }
    if (json_object_get_number(jsonObject, "id") != CommandFactory::BRANCH) {
        LOG_ERROR("BranchCommand: initialise: serialisedData is not for a"
                  << " BRANCH command!");
        return false;
    }
    if (!json_object_has_value_of_type(jsonObject, "branchName", JSONString)) {
        LOG_ERROR("BranchCommand: initialise: serialisedData does not contain a"
                  << " 'branchName' field with a JSONString value!");
        return false;
    }
    if (!json_object_has_value_of_type(jsonObject, "checkout", JSONBoolean)) {
        LOG_ERROR("BranchCommand: initialise: serialisedData does not contain a"
                  << " 'checkout' field with a JSONBoolean value!");
        return false;
    }
    if (!json_object_has_value_of_type(jsonObject, "deleteBranch", JSONBoolean)) {
        LOG_ERROR("BranchCommand: initialise: serialisedData does not contain a"
                  << " 'deleteBranch' field with a JSONBoolean value!");
        return false;
    }

    this->branchName = json_object_get_string(jsonObject, "branchName");
    this->checkout = json_object_get_boolean(jsonObject, "checkout");
    this->deleteBranch = json_object_get_boolean(jsonObject, "deleteBranch");
}

JSON_Value* BranchCommand::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_number(data, "id", this->id);
    json_object_set_string(data, "branchName", this->branchName.c_str());
    json_object_set_boolean(data, "checkout", this->checkout);
    json_object_set_boolean(data, "deleteBranch", this->deleteBranch);

    return root;
}

bool BranchCommand::execute(SnapshotManager* snapshotManager) {
    LOG_DEBUG("BranchCommand: execute: Called");

    if (this->deleteBranch) {
        snapshotManager->deleteBranch(this->branchName);
    } else {
        snapshotManager->newBranch(this->branchName, this->checkout);
    }
}

unsigned int BranchCommand::getId() {
    return this->id;
}

std::string BranchCommand::toString() {
    std::ostringstream result;

    if (this->deleteBranch) {
        result << "BranchCommand: Snapshot branch '"
               << this->branchName << "' will be deleted";
    } else if (this->checkout) {
        result << "BranchCommand: Ensuring snapshot branch '"
               << this->branchName << "' and switching to it";
    } else {
        result << "BranchCommand: Creating snapshot branch '"
               << this->branchName << "'";
    }
    return result.str();
}
