/**
 * Status.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <parson/parson.h>

#include "Status.h"


Status::Status() {
}

Status::~Status() {
    json_value_free(this->status_json);
}

void Status::load() {
    this->status_json = json_parse_file(Status::STATUS_PATH);
    // TODO: Validate JSON result.
    std::cout << "Status: load: JSON is: "
              << json_serialize_to_string_pretty(this->status_json) << "\n";
}

void Status::save() {
    JSON_Status status = json_serialize_to_file_pretty(
        this->status_json, Status::STATUS_PATH
    );
    // TODO: Validate status.
    std::cout << "Status: save: JSON is: "
              << json_serialize_to_string_pretty(this->status_json) << "\n";
}

JSON_Object* Status::getData() {
    return json_value_get_object(this->status_json);
}
