#include <asm-generic/errno-base.h>
#include <cerrno>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "utils/error_exit.hpp"

#include "file_handler.hpp"
#include "config_reader.hpp"
#include "init_store_dir.hpp"
#include "get_files_hashes.hpp"

int
main(const int argc, const char** argv)
{
    int i;
    int store_fd, work_fd;

    config cfg;
    file_hashes_t new_files;
    std::string tmp_storepath;
    store_file_hashes_t store_files;
    store_file_hashes_t::iterator hash_file;

    DIR *store_dir, *work_dir;
    mode_t mkdir_mode = 0;

    mkdir_mode |= S_IRUSR | S_IWUSR | S_IXUSR;
    mkdir_mode |= S_IRGRP | S_IWGRP | S_IXGRP;
    mkdir_mode |= S_IROTH;

    tmp_storepath.reserve(BUF_SIZE);

    cfg.read(argc, argv);

    if (mkdir(cfg.store_dir.c_str(), mkdir_mode) == -1 && errno != EEXIST) {
        errExit("Error cannot create store_dir ( `%s` )", cfg.store_dir.c_str());
    }

    if (mkdir(cfg.work_dir.c_str(), mkdir_mode) == -1 && errno != EEXIST) {
        errExit("Error cannot create work_dir ( `%s` )", cfg.work_dir.c_str());
    }

    errno = 0;

    work_dir = opendir(cfg.work_dir.c_str());

    if (work_dir == NULL) {
        errExit("Error while opening work_dir ( `%s` )", cfg.work_dir.c_str());
    }

    store_dir = opendir(cfg.store_dir.c_str());

    if (store_dir == NULL) {
        errExit("Error while opening store_dir ( `%s` )", cfg.store_dir.c_str());
    }

    work_fd = dirfd(work_dir);
    store_fd = dirfd(store_dir);

    if (work_fd == -1 || store_fd == -1) {
        errExit("Invalid fd for work_dir/store_dir");
    }

    init_store(store_fd);
    new_files = get_files_hashes(cfg.work_dir.c_str());
    store_files = get_store_file_hashes(cfg.store_dir.c_str());

    for(const auto& [hash, file_vec]:new_files) {
        if (file_vec.empty()) {
            errMsg("Warning: hash value %s had empty file list!", hash.c_str());
            continue;
        }
        if ((hash_file = store_files.find(hash)) != store_files.end()) {
            for (const auto& file:file_vec) {
                if (remove_and_link(file.name, hash_file->second.name) != 0) {
                    errExit(
                        "Failure at remove_and_link for `%s` and `%s`, not continuing",
                        file.name.c_str(),
                        hash_file->second.name.c_str()
                    );
                }
            }
        } else {
            const auto& first_file = file_vec.back();
            if (move_and_link(first_file.name, cfg.store_dir, hash, tmp_storepath)) {
                    errExit(
                        "Failure at move_and_link for `%s` and `%s` with hash %s, not continuing",
                        first_file.name.c_str(),
                        cfg.store_dir.c_str(),
                        hash.c_str()
                    );
            }
            store_files[hash] = candidate_file(tmp_storepath, -1);
            for (i = 0; i < (int)(file_vec.size() - 1); i++) {
                const auto &file = file_vec[i];
                if (remove_and_link(file.name, tmp_storepath) != 0) {
                    errExit(
                        "Failure at remove_and_link for `%s` and `%s`, not continuing",
                        file.name.c_str(),
                        tmp_storepath.c_str()
                    );
                }
            }
        }
    }
    return 0;
}