/**
 * Snapshot.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <string>

#include <parson/parson.h>

#include "Snapshot/Snapshot.h"
#include "Logging.h"


Snapshot::Snapshot():
    Snapshot("", "") {
}

Snapshot::Snapshot(std::string name, std::string basePath):
    name(name),
    basePath(basePath) {
}

Snapshot::~Snapshot() {
}

bool Snapshot::initialise(JSON_Value* serialisedData) {
    JSON_Object* data = json_value_get_object(serialisedData);

    if (!json_object_has_value_of_type(data, "name", JSONString)) {
        LOG_ERROR("Snapshot: initialise: serialisedData does not contain a 'name'"
                  << " field with a JSONString value!");
        return false;
    }
    if (!json_object_has_value_of_type(data, "basePath", JSONString)) {
        LOG_ERROR("Snapshot: initialise: serialisedData does not contain a 'basePath'"
                  << " field with a JSONString value!");
        return false;
    }

    this->name = json_object_get_string(data, "name");
    this->basePath = json_object_get_string(data, "basePath");
}

JSON_Value* Snapshot::serialise() {
    JSON_Value* root = json_value_init_object();
    JSON_Object* data = json_value_get_object(root);

    json_object_set_string(data, "name", this->name.c_str());
    json_object_set_string(data, "basePath", this->basePath.c_str());

    return root;
}

void Snapshot::setName(std::string name) {
    this->name = name;
}

std::string Snapshot::getName() {
    return this->name;
}

void Snapshot::setBasePath(std::string path) {
    this->basePath = path;
}

std::string Snapshot::getBasePath() {
    return this->basePath;
}

std::string Snapshot::getPath() {
    return this->basePath + "/" + this->name;
}

std::string Snapshot::toString() {
    return std::string("TODO");
}
