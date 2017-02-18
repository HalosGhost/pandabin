#if !defined(PANDABIN_MAIN_H)
#define PANDABIN_MAIN_H

#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <lwan/lwan.h>
#include "sql.h"
#include "paste.h"
#include "routes.h"

// Configurables
#define PREFIX   "/opt"
#define MAXSIZE  67108864

#define PROGNM   "pandabin"
#define MAINPATH PREFIX "/" PROGNM
#define DBPATH   MAINPATH "/db.sqlite"
#define FILEPATH MAINPATH "/files"

#define FAIL(...) do { \
    status = errno; \
    syslog(LOG_ERR, __VA_ARGS__); \
    goto cleanup; \
} while (false)

#endif
