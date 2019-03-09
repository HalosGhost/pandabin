#if !defined(COMMON_H)
#define COMMON_H

#pragma once

#include <lwan/lwan.h>
#include <lwan/lwan-template.h>
#include <lwan/lwan-mod-serve-files.h>
#include <lwan/lwan-mod-redirect.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define PRIV_SERVE_FILES(path) \
    .module = LWAN_MODULE_REF(serve_files), \
    .args = &(struct lwan_serve_files_settings ){ \
        .root_path = path, \
        .serve_precompressed_files = true, \
        .auto_index = false \
    } \

#define PREFIX "/srv/http"

static struct lwan_straitjacket jacket = {
    .user_name = "http",
    .chroot_path = PREFIX,
    .drop_capabilities = true
};

#endif // COMMON_H
