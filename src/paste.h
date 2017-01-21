#if !defined(PANDABIN_PASTE_H)
#define PANDABIN_PASTE_H

#pragma once

struct pandabin_paste {
    uuid_t uuid;
    unsigned char * hash;
    char * content;
};

struct pandabin_paste *
pandabin_paste_new (char *);

void
pandabin_paste_free (struct pandabin_paste *);

#endif
