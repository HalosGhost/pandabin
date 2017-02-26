#if !defined(PANDABIN_ROUTES_H)
#define PANDABIN_ROUTES_H

#pragma once

#include <lwan/lwan.h>
#include <errno.h>
#include "sql.h"

enum lwan_http_status
serve_index (struct lwan_request *, struct lwan_response *, void *);

enum lwan_http_status
get_paste (struct lwan_request *, struct lwan_response *, void *);

#endif
