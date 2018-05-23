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
#include "Logging.h"


Status::Status() {
}

Status::~Status() {
    // TODO: This causes a double free exception?
    // json_value_free(this->data);
}

void Status::load() {
    this->data = json_parse_file(Status::STATUS_PATH);

    if (this->data == NULL) {
        LOG_WARN("Status: load: Could not load status file, using default one.");
        this->data = json_parse_file(Status::DEFAULT_STATUS_PATH);
    }

    LOG_DEBUG("Status: load: JSON is: "
              << json_serialize_to_string_pretty(this->data));
}

void Status::save() {
    JSON_Status status = json_serialize_to_file_pretty(
        this->data, Status::STATUS_PATH
    );
    // TODO: Validate status.
    LOG_DEBUG("Status: save: JSON is: "
              << json_serialize_to_string_pretty(this->data));
}

JSON_Object* Status::getData() {
    return json_value_get_object(this->data);
}
