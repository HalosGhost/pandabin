#if !defined(PANDABIN_SQL_H)
#define PANDABIN_SQL_H

#pragma once

#include <sqlite3.h>
#include "paste.h"

struct pandabin_settings {
    size_t maxsize;
};

sqlite3 *
pandabin_db_init (void);

signed
pandabin_db_insert (struct pandabin_paste *);

struct pandabin_paste *
pandabin_db_select (sqlite3_stmt **, const char * restrict);

signed
pandabin_db_delete (struct pandabin_paste *);

void
pandabin_settings_fetch (sqlite3 *, struct pandabin_settings *);

signed
pandabin_db_cleanup (sqlite3 *);

#define X(tag, stmt) [tag] = stmt,
static const char * sql_stmts [] = {
    PANDABIN_SQL_MEMBERS
};
#undef X

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
    "  );"

    "insert or ignore into 'settings'"
    "  values ('max size', 67108864);";

#endif
