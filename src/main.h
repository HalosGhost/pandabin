#if !defined(PANDABIN_MAIN_H)
#define PANDABIN_MAIN_H

#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

#define PROGNM   "pandabin"
#define PREFIX   "/opt"
#define DBPATH   PREFIX "/" PROGNM "/db.sqlite"
#define FILEPATH PREFIX "/" PROGNM "/files"

#endif
