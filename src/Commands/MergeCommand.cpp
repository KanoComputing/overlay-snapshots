/**
 * MergeCommand.cpp
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
#include "Commands/MergeCommand.h"
#include "Logging.h"
#include "Snapshot.h"


MergeCommand::MergeCommand():
    MergeCommand(1) {
}

MergeCommand::MergeCommand(int topMostSnapshots):
    id(CommandFactory::MERGE),
    topMostSnapshots(topMostSnapshots) {

    if (this->topMostSnapshots < 1) {
        // TODO: Explode.
    }
    LOG_DEBUG("MergeCommand: constructor called");
}

MergeCommand::~MergeCommand() {
}

bool MergeCommand::initialise(JSON_Value* serialisedData) {
    JSON_Object* jsonObject = json_value_get_object(serialisedData);

    // TODO: Update the Kano libparson to get these:
    // if (!json_object_has_value_of_type(jsonObject, "id", JSONNumber)) {
    //     return false;
    // }
    // if (json_object_get_number(jsonObject, "id") != ICommand::MERGE) {
    //     return false;
    // }
    // if (!json_object_has_value_of_type(jsonObject, "top_most_snapshots", JSONNumber)) {
    //     return false;
    // }

    this->topMostSnapshots = json_object_get_number(jsonObject, "top_most_snapshots");
}

JSON_Value* MergeCommand::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_number(data, "id", this->id);
    json_object_set_number(data, "top_most_snapshots", this->topMostSnapshots);

    return root;
}

bool MergeCommand::execute(Snapshot* snapshot) {
    LOG_DEBUG("MergeCommand: execute: Called");

    snapshot->merge(this->topMostSnapshots);
}

unsigned int MergeCommand::getId() {
    return this->id;
}

std::string MergeCommand::toString() {
    std::ostringstream result;
    result << "MergeCommand: Top most " << this->topMostSnapshots
           << " snapshots will be merged down";
    return result.str();
}
