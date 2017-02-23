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
pandabin_db_select (const char * restrict);

signed
pandabin_db_delete (struct pandabin_paste *);

void
pandabin_settings_fetch (sqlite3 *, struct pandabin_settings *);

signed
pandabin_db_cleanup (sqlite3 *);

static sqlite3_stmt * ins_handle;
static const char * ins_stmt =
    "insert into 'pastes' values (?, ?, ?, ?, strftime('%s', 'now', 'utc'));";

static sqlite3_stmt * sel_handle;
static const char * sel_stmt = "select * from pastes where hash like ?;";

static sqlite3_stmt * rmv_handle;
static const char * rmv_stmt = "delete from pastes where uuid = ?;";

static sqlite3_stmt * set_handle;
static const char * set_stmt = "select value from settings where name = ?;";

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
