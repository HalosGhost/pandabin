#include "paste.h"

struct pandabin_paste *
pandabin_paste_new (const char * content, size_t size) {

    signed status = EXIT_SUCCESS;

    struct pandabin_paste * pst = malloc(sizeof(struct pandabin_paste));
    if ( !pst ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate new paste",
               strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    pst->hash = malloc(SHA256_DIGEST_LENGTH);
    if ( !pst->hash ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate hash",
               strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    unsigned char * md = SHA256((unsigned char * )content, size,
                                pst->hash);
    if ( !md ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to calculate hash", "unknown");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    uuid_generate(pst->uuid);
    pst->size = size;

    size_t pathlen = strlen(FILEPATH) + 38;
    pst->path = malloc(pathlen);
    if ( !pst->path ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate path",
               strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    signed s = snprintf(pst->path, pathlen, "%s/%s", FILEPATH, pst->hash);
    if ( s < 0 ) {
        status = EXIT_FAILURE;
        goto cleanup;
    }

    FILE * f = fopen(pst->path, "w+");
    if ( !f ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to open file", strerror(errno));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    fwrite(content, size, 1, f);
    s = fclose(f);
    if ( s ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to close file", strerror(errno));
        status = EXIT_FAILURE;
        goto cleanup;
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
