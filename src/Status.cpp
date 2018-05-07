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
    json_value_free(this->data);
}

void Status::load() {
    this->data = json_parse_file(Status::STATUS_PATH);

    if (this->data == NULL) {
        std::cout << "[WARNING] Status: load: Could not load status file,"
                  << " using default one.\n";
        this->data = json_parse_file(Status::DEFAULT_STATUS_PATH);
    }

    std::cout << "Status: load: JSON is: "
              << json_serialize_to_string_pretty(this->data) << "\n";
}

void Status::save() {
    JSON_Status status = json_serialize_to_file_pretty(
        this->data, Status::STATUS_PATH
    );
    // TODO: Validate status.
    std::cout << "Status: save: JSON is: "
              << json_serialize_to_string_pretty(this->data) << "\n";
}

JSON_Object* Status::getData() {
    return json_value_get_object(this->data);
}
