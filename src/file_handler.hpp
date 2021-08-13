#pragma once

#include <cstdio>
#include <string>

#include <unistd.h>
#include <string.h>

#include "utils/error_exit.hpp"

using string = std::string;

int
create_symlink(const char* filepath, const char* linkpath)
{
    int ret;

    ret = symlink(filepath, linkpath);
    if (ret != 0) {
        errMsg(
            "Could not create symlink from `%s` to `%s`",
            filepath,
            linkpath
        );
    }
    return ret;
}

int
move_file(const char* filepath, const char* newpath)
{
    int ret;

    ret = rename(filepath, newpath);
    if (ret != 0) {
        errMsg(
            "Could not create move file from `%s` to `%s`",
            filepath,
            newpath
        );
    }
    return ret;
}

int
move_and_link(const string& filename,
    const string& storedir,
    const string& filehash,
    string& storepath)
{
    int ret;

    storepath = storedir + '/' + filehash[0] + filehash[1] +
		((storedir.empty() || (storedir.back() != '/')) ? "/" : "") +
		basename(filename.c_str()) + '-' + filehash;

    ret = move_file(filename.c_str(), storepath.c_str());
    if (ret != 0) {
        return ret;
    }

    ret = create_symlink(storepath.c_str(), filename.c_str());
    return ret;
}

int
remove_and_link(const string& filename,
    const string& storepath)
{
    int ret;

    ret = remove(filename.c_str());
    if (ret != 0) {
        errMsg(
            "Could not remove file `%s`",
            filename.c_str()
        );
        return ret;
    }

    ret = create_symlink(storepath.c_str(), filename.c_str());
    return ret;
}
