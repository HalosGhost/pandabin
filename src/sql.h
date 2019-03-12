#if !defined(PANDABIN_SQL_H)
#define PANDABIN_SQL_H

#pragma once

#include <sqlite3.h>

static const char * pandabin_schema =
    "create table if not exists 'pastes'"
    "  ( 'uuid'    text    primary key"
    "  , 'size'    integer not null"
    "  , 'hash'    text    not null unique"
    "  , 'content' blob    not null"
    "  , 'sunset'  integer null"
    "  , 'units'   text    null"
    "  , 'created' text    not null"
    "  , 'views'   integer not null"
    //"  , 'user'    text    not null"
    //"  , foreign key (user) references users(user)"
    "  );"

    "create table if not exists 'users'"
    "  ( 'user'    text    not null unique"
    "  );"

    "create table if not exists 'settings'"
    "  ( 'name'    text    primary key"
    "  , 'value'   text    not null"
    "  );"

    "insert or ignore into 'settings' values"
    "  ('max size', 67108864);"
    ;

#endif
