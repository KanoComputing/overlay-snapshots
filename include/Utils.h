/**
 * Utils.h
 *
 * Copyright (C) 2018 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * TODO
 */


#ifndef __OVLSNAP_UTILS_H__
#define __OVLSNAP_UTILS_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include <attr/xattr.h>
#include <parson/parson.h>


// JSON Parson additions.
bool jsonArrayContainsString(JSON_Array* jsonArray, std::string someString);

// Shell.
std::string runCmd(const char* cmd);

// Filesystem.
bool createDir(const char* path);
bool dirExists(const char* path);
std::string readFileContents(std::string path);
bool writeFileContents(std::string contents, std::string path);

// OverlayFS.
mode_t permission_bits(const struct stat *status);
mode_t file_type(const struct stat *status);
bool is_whiteout(const struct stat *status);
int is_opaquedir(const char *path, bool *output);
bool permission_identical(
    const struct stat *lower_status,
    const struct stat *upper_status
);

#endif
