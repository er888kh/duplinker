#pragma once

#include <asm-generic/errno-base.h>

#include <fcntl.h>
#include <sys/stat.h>
#include "utils/error_exit.hpp"

#include "constants.hpp"

void
init_store(const int par_fd)
{
    int i;
    int res;
    char buf[4];

    mode_t create_mode = 0;

    create_mode |= S_IRUSR | S_IWUSR | S_IXUSR;
    create_mode |= S_IRGRP | S_IWGRP | S_IXGRP;
    create_mode |= S_IROTH;

    buf[2] = '\0';

    for (i = 0; i < 256; i++) {
        buf[0] = hex_chars[i / 16];
        buf[1] = hex_chars[i % 16];

        if ((res = mkdirat(par_fd, buf, create_mode)) != 0 && errno != EEXIST) {
            errExit("Could not create store directory %s", buf);
        }
    }
}