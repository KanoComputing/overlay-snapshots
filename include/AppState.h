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

#include <string>


class AppState {

    public:  // Methods.
        AppState();
        ~AppState();

        bool enable();
        bool disable();
        bool isEnabled();
        void status();

    private:  // Methods.
        bool isEnabled(std::string cmdlineContents);
        bool isInitWrapped(std::string cmdlineContents);

    private:  // Constants.
        static constexpr const char* CMDLINE_PATH = "/boot/cmdline.txt";
        static constexpr const char* CMDLINE_INIT = "init=";
        static constexpr const char* CMDLINE_CONFIG = "/bin/ovlsnap-init";
};

#endif  // __OVLSNAP_APP_STATE_H__
