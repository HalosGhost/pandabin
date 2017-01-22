#if !defined(PANDABIN_PASTE_H)
#define PANDABIN_PASTE_H

#pragma once

#include "main.h"
#include "openssl/sha.h"
#include "uuid/uuid.h"

struct pandabin_paste {
    unsigned char * hash;
    char * path;
    uuid_t uuid;
    size_t size;
};

struct pandabin_paste *
pandabin_paste_new (const char *, size_t);

void
pandabin_paste_free (struct pandabin_paste *);

#endif
