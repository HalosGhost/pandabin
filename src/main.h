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

#define PANDABIN_SQL_MEMBERS \
    X(INS, "insert into 'pastes' values " \
           "(?, ?, ?, strftime('%s', 'now', 'utc'));") \
    X(SEL_HASH, "select * from pastes where hash like ?;") \
    X(SEL_UUID, "select * from pastes where uuid = ?;") \
    X(RMV, "delete from pastes where uuid = ?;") \
    X(SET, "select value from settings where name = ?;") \
    X(LAST, "")

#define X(tag, stmt) tag,
enum pandabin_tag {
    PANDABIN_SQL_MEMBERS
};
#undef X

#include "sql.h"
#include "paste.h"
#include "routes.h"

extern sqlite3_stmt * sql_hndls [LAST];
extern struct pandabin_settings settings;

// Configurables
#define PREFIX   "/home/halosghost"
#define MAXSIZE  67108864
#define PROGNM   "pandabin"
#define FILEPATH PREFIX "/" PROGNM "/files"

#define FAIL(...) do { \
    status = errno; \
    syslog(LOG_ERR, __VA_ARGS__); \
    goto cleanup; \
} while (false)

#endif
