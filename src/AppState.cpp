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


AppState::AppState(JSON_Object* statusLoadedData) {
    this->statusData = statusLoadedData;

    this->state = (AppState::State)(int)json_object_get_number(
        this->statusData, "state"
    );
    std::cout << "AppState: state is: " << std::to_string(this->state) << "\n";
}

AppState::~AppState() {
}

void AppState::changeState(AppState::State newState) {
    this->state = newState;
    json_object_set_number(this->statusData, "state", newState);
    std::cout << "AppState: changeState: new state is: "
              << std::to_string(this->state) << "\n";
}

AppState::State AppState::getState() {
    return this->state;
}
