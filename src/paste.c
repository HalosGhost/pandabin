#include "paste.h"

struct pandabin_paste *
pandabin_paste_new (const char * content, size_t size) {

    signed status = EXIT_SUCCESS;

    struct pandabin_paste * pst = malloc(sizeof(struct pandabin_paste));
    if ( !pst ) {
        FAIL("%s: %s\n", "Failed to allocate new paste", strerror(status));
    }

    pst->hash = malloc(65);
    if ( !pst->hash ) {
        FAIL("%s: %s\n", "Failed to allocate hash hexdigest", strerror(status));
    }

    unsigned char md [SHA256_DIGEST_LENGTH];
    const unsigned char * t = SHA256((const unsigned char * )content, size, md);
    if ( !t ) {
        FAIL("%s: %s\n", "Failed to calculate hash", "unknown");
    }

    signed s = 0;
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++ i ) {
        s = snprintf(pst->hash + i * 2, 3, "%02x", md[i]);
        if ( s < 0 ) {
            FAIL("Failed to get hexadecimal representation of hash\n");
        }
    }

    uuid_generate(pst->uuid);
    pst->size = size;

    size_t pathlen = strlen(settings.file_path) + 20;
    char * path = malloc(pathlen + 71);
    if ( !path ) {
        FAIL("Failed to allocate path: %s\n", strerror(status));
    }

    s = snprintf(path, pathlen, "%s/pandabin/files/%.3s",
                 settings.file_path, pst->hash);

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
    if ( chdir(path) == -1 ) {
        syslog(LOG_INFO, "Failed to cd to %s: %s\n", path, strerror(errno));

        errno = 0;
        if ( mkdir(path, 0777) == -1 ) {
            FAIL("Failed to create %s: %s\n", path, strerror(status));
        } syslog(LOG_INFO, "Created %s\n", path);

        errno = 0;
        if ( chdir(path) == -1 ) {
            FAIL("Failed to cd to %s: %s\n", path, strerror(status));
        }
    }

    errno = 0;
    if ( chdir(cwd) == -1 ) {
        FAIL("Failed to cd %s: %s\n", cwd, strerror(status));
    }

    // `+ 1` and `- 1` to account for the NUL byte
    s = snprintf(path + pathlen - 1, 91 + 1, "/%s", pst->hash);
    if ( s < 0 ) {
        errno = EXIT_FAILURE;
        FAIL("Failed to store path path to file\n");
    }

    FILE * f = fopen(path, "w+");
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
            pandabin_paste_free(&pst);
            pst = NULL;
        } return pst;
}

void
pandabin_paste_free (struct pandabin_paste ** pst) {

    if ( pst && *pst ) {
        if ( (*pst)->hash ) { free((*pst)->hash); }
        (*pst)->hash = 0;
        free(*pst);
        *pst = 0;
    }
}

