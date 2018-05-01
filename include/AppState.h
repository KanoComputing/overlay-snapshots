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
        // NOTE: Do not change the order of the states below! The status file
        // contains the current state which is a state defined below.
        enum State {
            /* 0 */ DISABLED,
            /* 1 */ ENABLED,
            /* 2 */ CREATE,
            /* 3 */ DROP,
            /* 4 */ MERGE,
            /* 5 */ DROP_CREATE,
            /* 6 */ MERGE_CREATE
        };

    public:  // Methods.
        AppState(JSON_Object* statusLoadedData);
        ~AppState();

        void changeState(AppState::State newState);
        AppState::State getState();

    private:  // Members.
        JSON_Object* statusData;
        AppState::State state;
};

#endif
