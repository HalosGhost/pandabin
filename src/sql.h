#if !defined(PANDABIN_SQL_H)
#define PANDABIN_SQL_H

#pragma once

#include <sqlite3.h>
#include "paste.h"

sqlite3 *
pandabin_db_init (void);

signed
pandabin_db_insert (sqlite3 *, struct pandabin_paste *);

signed
pandabin_db_cleanup (sqlite3 *);

static sqlite3_stmt * ins_handle;
static const char * ins_stmt =
    "insert into 'pastes' values (?, ?, ?, ?, strftime('%s', 'now', 'utc'));";

static const char * pandabin_schema =
    "create table if not exists 'pastes'"
    "  ( 'uuid'    text    primary key"
    "  , 'path'    text    not null"
    "  , 'size'    integer not null"
    "  , 'hash'    text    not null"
    "  , 'created' text    not null"
    "  );"

    "create table if not exists 'settings'"
    "  ( 'name'    text    primary key"
    "  , 'value'           not null"
    "  );";

#endif
