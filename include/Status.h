/**
 * Status.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_STATUS_H__
#define __OVLSNAP_STATUS_H__

#include <iostream>

#include <parson/parson.h>


class Status {

    public:  // Methods.
        Status();
        ~Status();

        void load();
        void save();

        JSON_Object* getData();

    private:  // Members.
        JSON_Value* status_json;

    private:  // Constants.
        static constexpr const char* STATUS_PATH = "/boot/ovlsnap_status.json";
};

#endif
