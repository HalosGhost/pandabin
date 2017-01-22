#include "paste.h"

struct pandabin_paste *
pandabin_paste_new (const char * content, size_t size) {

    signed status = EXIT_SUCCESS;

    struct pandabin_paste * new_paste = malloc(sizeof(struct pandabin_paste));
    if ( !new_paste ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate new paste",
                                    strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    new_paste->content = malloc(size);
    if ( !new_paste->content ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate content",
                                    strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    new_paste->hash = malloc(SHA256_DIGEST_LENGTH);
    if ( !new_paste->hash ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to allocate hash",
                                    strerror(ENOMEM));
        status = EXIT_FAILURE;
        goto cleanup;
    }

    memcpy(new_paste->content, content, size);
    unsigned char * md = SHA256((unsigned char * )new_paste->content, size,
                                new_paste->hash);
    if ( !md ) {
        syslog(LOG_ERR, "%s: %s\n", "Failed to calculate hash", "unknown");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    uuid_generate(new_paste->uuid);
    new_paste->size = size;

    cleanup:
        if ( status != EXIT_SUCCESS ) {
            pandabin_paste_free(new_paste);
            new_paste = NULL;
        } return new_paste;
}

void
pandabin_paste_free (struct pandabin_paste * pst) {

    if ( pst ) {
        if ( pst->hash ) { free(pst->hash); }
        if ( pst->content ) { free(pst->content); }
        free(pst);
    }
}
