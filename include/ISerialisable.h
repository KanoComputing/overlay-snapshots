/**
 * ISerialisable.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_ISERIALISABLE_H__
#define __OVLSNAP_ISERIALISABLE_H__

#include <string>

#include <parson/parson.h>


class ISerialisable {

    public:
        virtual bool initialise(JSON_Value* serialisedData) = 0;
        virtual JSON_Value* serialise() = 0;
};

#endif  // __OVLSNAP_ISERIALISABLE_H__
