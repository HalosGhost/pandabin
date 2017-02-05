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

    size_t pathlen = strlen(FILEPATH) + 5;
    pst->path = malloc(pathlen + 37);
    if ( !pst->path ) {
        FAIL("Failed to allocate path: %s\n", strerror(status));
    }

    signed s = snprintf(pst->path, pathlen, "%s/%.3s", FILEPATH, pst->hash);
    if ( s < 0 ) {
        errno = EXIT_FAILURE;
        FAIL("Failed to store path to subdirectory\n");
    }

    char cwd [PATH_MAX] = "";
    errno = 0;
    if ( !getcwd(cwd, PATH_MAX - 1) ) {
        FAIL("Failed to store working directory: %s\n", strerror(status));
    }

    errno = 0;
    if ( chdir(pst->path) == -1 ) {
        syslog(LOG_INFO, "Failed to cd to %s: %s\n", pst->path, strerror(errno));

        errno = 0;
        if ( mkdir(pst->path, 0777) == -1 ) {
            FAIL("Failed to create %s: %s\n", pst->path, strerror(status));
        } syslog(LOG_INFO, "Created %s\n", pst->path);

        errno = 0;
        if ( chdir(pst->path) == -1 ) {
            FAIL("Failed to cd to %s: %s\n", pst->path, strerror(status));
        }
    }

    errno = 0;
    if ( chdir(cwd) == -1 ) {
        FAIL("Failed to cd %s: %s\n", cwd, strerror(status));
    }

    // `+ 1` and `- 1` to account for the NUL byte
    s = snprintf(pst->path + pathlen - 1, 37 + 1, "/%s", pst->hash);
    if ( s < 0 ) {
        errno = EXIT_FAILURE;
        FAIL("Failed to store path path to file\n");
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

