/**
 * AppState.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <iostream>
#include <string>

#include <parson/parson.h>

#include "AppState.h"
#include "Logging.h"


AppState::AppState(JSON_Object* statusLoadedData) {
    this->statusData = statusLoadedData;

    this->state = (AppState::State)(int)json_object_get_number(
        this->statusData, "state"
    );
    LOG_DEBUG("AppState: state is: " << std::to_string(this->state));
}

AppState::~AppState() {
}

void AppState::changeState(AppState::State newState) {
    this->state = newState;
    json_object_set_number(this->statusData, "state", newState);
    LOG_DEBUG("AppState: changeState: new state is: " << std::to_string(this->state));
}

AppState::State AppState::getState() {
    return this->state;
}
