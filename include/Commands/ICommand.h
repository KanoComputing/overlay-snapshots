/**
 * ICommand.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO: Description
 */


#ifndef __OVLSNAP_ICOMMAND_H__
#define __OVLSNAP_ICOMMAND_H__

#include <string>

#include <parson/parson.h>

#include "ISerialisable.h"
#include "Snapshot.h"


class ICommand: public ISerialisable {

    public:
        virtual bool execute(Snapshot* snapshot) = 0;
        virtual unsigned int getId() = 0;
        virtual std::string toString() = 0;
};

#endif  // __OVLSNAP_ICOMMAND_H__
