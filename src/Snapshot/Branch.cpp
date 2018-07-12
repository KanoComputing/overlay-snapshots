/**
 * Branch.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <string>

#include <parson/parson.h>

#include "Snapshot/Branch.h"
#include "Logging.h"


Branch::Branch():
    Branch("") {
}

Branch::Branch(std::string name):
    name(name),
    headId("") {
}

Branch::~Branch() {
}

bool Branch::initialise(JSON_Value* serialisedData) {
    JSON_Object* data = json_value_get_object(serialisedData);

    if (!json_object_has_value_of_type(data, "name", JSONString)) {
        LOG_ERROR("Branch: initialise: serialisedData does not contain a 'name'"
                  << " field with a JSONString value!");
        return false;
    }
    if (!json_object_has_value_of_type(data, "headId", JSONString)) {
        LOG_ERROR("Branch: initialise: serialisedData does not contain a 'headId'"
                  << " field with a JSONString value!");
        return false;
    }

    this->name = json_object_get_string(data, "name");
    this->headId = json_object_get_string(data, "headId");
}

JSON_Value* Branch::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_string(data, "name", this->name.c_str());
    json_object_set_string(data, "headId", this->headId.c_str());

    return root;
}

void Branch::setName(std::string name) {
    this->name = name;
}

std::string Branch::getName() {
    return this->name;
}

void Branch::setHeadId(std::string headId) {
    this->headId = headId;
}

std::string Branch::getHeadId() {
    return this->headId;
}

std::string Branch::toString() {
    return std::string(json_serialize_to_string(this->serialise()));
}
