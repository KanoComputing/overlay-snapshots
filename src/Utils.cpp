/**
 * Utils.cpp
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <array>
#include <memory>

#include <parson/parson.h>
#include <attr/xattr.h>

#include "Utils.h"
#include "Logging.h"


// exactly the same as in linux/fs.h
#define WHITEOUT_DEV 0
// exact the same as in fs/overlayfs/super.c
const char *ovl_opaque_xattr = "trusted.overlay.opaque";


bool jsonArrayContainsString(JSON_Array* jsonArray, std::string someString) {
    std::string currentString;

    for (int i = 0; i < json_array_get_count(jsonArray); i++) {
        currentString = std::string(json_array_get_string(jsonArray, i));
        if (currentString.compare(someString) == 0) {
            return true;
        }
    }
    return false;
}

std::string runCmd(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
        LOG_ERROR("runCmd: popen() failed!");
        goto end;
    }

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
  end:
    return result;
}

bool createDir(const char* path) {
    int rc = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (rc != 0 && errno != EEXIST) {
        return false;
    }
    return true;
}

mode_t file_type(const struct stat *status) {
    return status->st_mode & S_IFMT;
}

mode_t permission_bits(const struct stat *status) {
    return status->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISVTX);
}

bool is_whiteout(const struct stat *status) {
    return (file_type(status) == S_IFCHR) && (status->st_rdev == WHITEOUT_DEV);
}

int is_opaquedir(const char *path, bool *output) {
    char val;
    ssize_t res = getxattr(path, ovl_opaque_xattr, &val, 1);
    if ((res < 0) && (errno != ENOATTR)) {
        return -1;
    }
    *output = (res == 1 && val == 'y');
    return 0;
}

bool permission_identical(
    const struct stat *lower_status, const struct stat *upper_status
) {
    return (
        (permission_bits(lower_status) == permission_bits(upper_status)) &&
        (lower_status->st_uid == upper_status->st_uid) &&
        (lower_status->st_gid == upper_status->st_gid)
    );
}
