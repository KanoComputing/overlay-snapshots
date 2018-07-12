/**
 * Commit.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <string>

#include <parson/parson.h>

#include "Snapshot/Commit.h"
#include "Logging.h"


Commit::Commit():
    Commit(NULL) {
}

Commit::Commit(JSON_Value* data):
    id(""),
    from("") {
    this->setData(data);
}

Commit::~Commit() {
}

bool Commit::initialise(JSON_Value* serialisedData) {
    JSON_Object* data = json_value_get_object(serialisedData);

    if (!json_object_has_value_of_type(data, "id", JSONString)) {
        LOG_ERROR("Commit: initialise: serialisedData does not contain a 'id'"
                  << " field with a JSONString value!");
        return false;
    }
    if (!json_object_has_value_of_type(data, "from", JSONString)) {
        LOG_ERROR("Commit: initialise: serialisedData does not contain a 'from'"
                  << " field with a JSONString value!");
        return false;
    }
    if (!json_object_has_value_of_type(data, "data", JSONObject)) {
        LOG_ERROR("Commit: initialise: serialisedData does not contain a 'from'"
                  << " field with a JSONValue value!");
        return false;
    }

    this->id = json_object_get_string(data, "id");
    this->from = json_object_get_string(data, "from");
    this->data = json_object_get_value(data, "data");
    return true;
}

JSON_Value* Commit::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_string(data, "id", this->id.c_str());
    json_object_set_string(data, "from", this->from.c_str());
    json_object_set_value(data, "data", this->data);

    return root;
}

void Commit::setId(std::string id) {
    this->id = id;
}

std::string Commit::getId() {
    return this->id;
}

void Commit::setFrom(std::string commitId) {
    this->from = commitId;
}

std::string Commit::getFrom() {
    return this->from;
}

void Commit::setData(JSON_Value* data) {
    if (data == NULL) {
        data = json_value_init_object();
    }
    this->data = data;
}

JSON_Value* Commit::getData() {
    return this->data;
}

std::string Commit::toString() {
    return std::string(json_serialize_to_string(this->serialise()));
}
