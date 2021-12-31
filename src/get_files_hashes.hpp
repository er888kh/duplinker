#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <ftw.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "constants.hpp"
#include "utils/error_exit.hpp"
#include "utils/hasher.hpp"

using string = std::string;

struct candidate_file {
    string name;
    int fd;

    candidate_file(){}
    candidate_file(const string& _name, const int _fd): name(_name), fd(_fd) {}
};

typedef std::map<string, std::vector<candidate_file>> file_hashes_t;
typedef std::map<string, candidate_file> store_file_hashes_t;

static file_hashes_t result;
static store_file_hashes_t store;

static int
nftw_work_callback(const char* pathname, const struct stat* statbuf,
    const int typeflag, const struct FTW *ftwbuf)
{
    const static int required_mode = S_IRUSR | S_IWUSR;

    if ((typeflag & FTW_DNR) != 0) {
        errMsg("Could not open directory `%s`, continuing\n", pathname);
        return 0;
    }
    if ((typeflag & (FTW_D | FTW_DP | FTW_SL | FTW_SLN)) != 0) {
        return 0;
    }

    if ((typeflag & FTW_NS) != 0) {
        errMsg("Could not stat file `%s`, continuing\n", pathname);
        return 0;
    }

    if (statbuf->st_size < min_file_size) {
        return 0;
    }

    if ((statbuf->st_mode & required_mode) != required_mode) {
        errMsg("Insufficient privileges for file `%s`, continuing\n", pathname);
        return 0;
    }

    candidate_file file;
    file.fd = -1;
    file.name = string(pathname);

    try {
        result[get_file_hash(file.name)].push_back(file);
    } catch(const std::exception &e) {
        errMsg("Error while hashing file `%s`: %s", pathname, e.what());
    }

    return 0;
}

file_hashes_t
get_files_hashes(const char* work_dir)
{
    int ntfw_flags = 0;

    result.clear();

    ntfw_flags |= FTW_PHYS | FTW_MOUNT;   
    nftw(
        work_dir,
        (__nftw_func_t) nftw_work_callback,
        20,
        ntfw_flags
    );

    return result;
}

static string
get_hash_from_filename(const string& filename)
{
    static const string zero_hash = string(HASH_SIZE_GOOD, '0');

    string::const_iterator result_ptr, ptr;

    if (filename.size() < HASH_SIZE_GOOD) {
        return zero_hash;
    }

    result_ptr = filename.end() - HASH_SIZE_GOOD;

    //check whether this can be a valid hash
    for (ptr = result_ptr; ptr != filename.end(); ptr++) {
        if (std::binary_search(hex_chars, hex_chars + 16, *ptr) == 0) {
            return zero_hash;
        }
    }

    return string(result_ptr, filename.end());
}

static int
nftw_store_callback(const char* pathname, const struct stat* statbuf,
    const int typeflag, const struct FTW *ftwbuf)
{
    if ((typeflag & FTW_DNR) != 0) {
        errMsg("Could not open store directory `%s`, continuing\n", pathname);
        return 0;
    }
    if ((typeflag & (FTW_D | FTW_DP | FTW_SL | FTW_SLN)) != 0) {
        return 0;
    }

    if ((typeflag & FTW_NS) != 0) {
        errMsg("Could not stat store file `%s`, continuing\n", pathname);
        return 0;
    }

    candidate_file file;
    file.fd = -1;
    file.name = string(pathname);

    try {
        store[get_hash_from_filename(file.name)] = file;
    } catch(const std::exception &e) {
        errMsg("Error while hashing store file `%s`: %s", pathname, e.what());
    }

    return 0;
}

store_file_hashes_t
get_store_file_hashes(const char* work_dir)
{
    int ntfw_flags = 0;

    store.clear();

    ntfw_flags |= FTW_PHYS | FTW_MOUNT;   
    nftw(
        work_dir,
        (__nftw_func_t) nftw_store_callback,
        20,
        ntfw_flags
    );

    return store;
}