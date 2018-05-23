/**
 * AppState.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_APP_STATE_H__
#define __OVLSNAP_APP_STATE_H__

#include <parson/parson.h>


class AppState {

    public:  // Enums.
        // WARNING: Do not change the order of the states below! The status
        // file contains the current state which is a state defined below.
        enum State {
            /* 0 */ DISABLED,
            /* 1 */ ENABLED
        };

    public:  // Methods.
        AppState(JSON_Object* statusLoadedData);
        ~AppState();

        void changeState(AppState::State newState);
        AppState::State getState();

    private:  // Members.
        JSON_Object* statusData = NULL;
        AppState::State state;
};

#endif  // __OVLSNAP_APP_STATE_H__
