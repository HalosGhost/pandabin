#if !defined(PANDABIN_SQL)
#define PANDABIN_SQL

#pragma once

#include <sqlite3.h>
#include "main.h"

signed
pandabin_db_init (void);

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
