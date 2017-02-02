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
#include "common.h"

#define PROGNM   "pandabin"
#define PREFIX   "/opt"
#define MAINPATH PREFIX "/" PROGNM
#define DBPATH   MAINPATH "/db.sqlite"
#define FILEPATH MAINPATH "/files"

#endif
