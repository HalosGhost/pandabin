#include "paste.h"

struct pandabin_paste *
pandabin_paste_new (const char * content, size_t size) {

    signed status = EXIT_SUCCESS;

    struct pandabin_paste * pst = malloc(sizeof(struct pandabin_paste));
    if ( !pst ) {
        FAIL("%s: %s\n", "Failed to allocate new paste", strerror(status));
    }

    pst->hash = malloc(SHA256_DIGEST_LENGTH);
    if ( !pst->hash ) {
        FAIL("%s: %s\n", "Failed to allocate hash", strerror(status));
    }

    unsigned char * md = SHA256((const unsigned char * )content, size,
                                pst->hash);
    if ( !md ) {
        FAIL("%s: %s\n", "Failed to calculate hash", "unknown");
    }

    uuid_generate(pst->uuid);
    pst->size = size;

    size_t pathlen = strlen(FILEPATH) + 39;
    pst->path = malloc(pathlen);
    if ( !pst->path ) {
        FAIL("%s: %s\n", "Failed to allocate path", strerror(status));
    }

    signed s = snprintf(pst->path, pathlen, "%s/%s", FILEPATH, pst->hash);
    if ( s < 0 ) {
        errno = EXIT_FAILURE;
        FAIL("%s\n", "Failed to store file path");
    }

    FILE * f = fopen(pst->path, "w+");
    if ( !f ) {
        FAIL("%s: %s\n", "Failed to open file", strerror(status));
    }

    fwrite(content, size, 1, f);
    s = fclose(f);
    if ( s ) {
        FAIL("%s: %s\n", "Failed to close file", strerror(status));
    }

    cleanup:
        if ( status != EXIT_SUCCESS ) {
            pandabin_paste_free(pst);
            pst = NULL;
        } return pst;
}

void
pandabin_paste_free (struct pandabin_paste * pst) {

    if ( pst ) {
        if ( pst->hash ) { free(pst->hash); }
        if ( pst->path ) { free(pst->path); }
        free(pst);
    }
}

