/**
 * AppState.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <algorithm>
#include <iostream>
#include <string>

#include "AppState.h"
#include "Logging.h"
#include "Utils.h"


AppState::AppState() {
}

AppState::~AppState() {
}

bool AppState::enable() {
    std::string cmdlineContents = readFileContents(AppState::CMDLINE_PATH);

    if (this->isEnabled(cmdlineContents)) {
        return true;
    } else if (this->isInitWrapped(cmdlineContents)) {
        LOG_WARN("AppState: enable: Cannot enable, init is already configured!");
        return false;
    }

    // Remove any newlines and append the init wrapping config.
    cmdlineContents.erase(
        std::remove(cmdlineContents.begin(), cmdlineContents.end(), '\n'),
        cmdlineContents.end()
    );
    cmdlineContents.append(
        std::string(" ") +
        std::string(AppState::CMDLINE_INIT) +
        std::string(AppState::CMDLINE_CONFIG)
    );

    return writeFileContents(cmdlineContents, AppState::CMDLINE_PATH);
}

bool AppState::disable() {
    std::string cmdlineContents = readFileContents(AppState::CMDLINE_PATH);

    if (!this->isEnabled(cmdlineContents)) {
        return true;
    }

    std::string config(
        std::string(AppState::CMDLINE_INIT) +
        std::string(AppState::CMDLINE_CONFIG)
    );

    // Remove the init wrapping config from the file contents.
    // TODO: This will leave space characters about every time.
    std::size_t position = cmdlineContents.find(config);
    if (position != std::string::npos) {
        cmdlineContents.erase(position, config.length());
    }

    return writeFileContents(cmdlineContents, AppState::CMDLINE_PATH);
}

bool AppState::isEnabled() {
    return this->isEnabled(readFileContents(std::string(AppState::CMDLINE_PATH)));
}

void AppState::status() {
    if (this->isEnabled()) {
        std::cout << "ovlsnap-init is enabled and will run on the next boot." << std::endl;
    } else {
        std::cout << "ovlsnap-init is disabled and will not run on the next boot." << std::endl;
    }
}

// --- Private Methods ---

bool AppState::isEnabled(std::string cmdlineContents) {
    return (
        cmdlineContents.find(
            std::string(AppState::CMDLINE_INIT) +
            std::string(AppState::CMDLINE_CONFIG)
        ) != std::string::npos
    );
}

bool AppState::isInitWrapped(std::string cmdlineContents) {
    return (
        cmdlineContents.find(AppState::CMDLINE_INIT) !=
        std::string::npos
    );
}
