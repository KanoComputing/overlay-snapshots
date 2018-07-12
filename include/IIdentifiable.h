/**
 * IIdentifiable.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_IIDENFIABLE_H__
#define __OVLSNAP_IIDENFIABLE_H__

#include <string>


class IIdentifiable {

    public:
        virtual std::string getId() = 0;
};

#endif  // __OVLSNAP_IIDENFIABLE_H__
