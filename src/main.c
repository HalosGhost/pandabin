#include "main.h"

static signed
pandabin_dir_init (void);

signed
main (void) {

    signed status = EXIT_SUCCESS;
    openlog(PROGNM, LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started\n");

    status = pandabin_dir_init();
    if ( status != EXIT_SUCCESS ) { goto cleanup; }

    status = pandabin_db_init();
    if ( status != EXIT_SUCCESS ) { goto cleanup; }

    cleanup:
        syslog(LOG_INFO, "Ended\n");
        closelog();
        return status;
}

static signed
pandabin_dir_init (void) {

    signed status = EXIT_SUCCESS;

    char cwd [PATH_MAX] = "";
    errno = 0;
    if ( !getcwd(cwd, PATH_MAX - 1) ) {
        FAIL("Failed to store working directory: %s\n", strerror(status));
    }

    errno = 0;
    if ( chdir(PREFIX) == -1 ) {
        FAIL("Failed to cd to %s: %s\n", PREFIX, strerror(status));
    }

    errno = 0;
    if ( chdir(MAINPATH) == -1 ) {
        syslog(LOG_INFO, "Failed to cd to %s: %s\n", MAINPATH, strerror(errno));

        errno = 0;
        if ( mkdir(MAINPATH, 0777) == -1 ) {
            FAIL("Failed to create %s: %s\n", MAINPATH, strerror(status));
        } syslog(LOG_INFO, "Created %s\n", MAINPATH);

        errno = 0;
        if ( chdir(MAINPATH) == -1 ) {
            FAIL("Failed to cd to %s: %s\n", MAINPATH, strerror(status));
        }
    }

    errno = 0;
    if ( chdir(FILEPATH) == -1 ) {
        syslog(LOG_INFO, "Failed to cd to %s: %s\n", FILEPATH, strerror(errno));

        errno = 0;
        if ( mkdir(FILEPATH, 0777) == -1 ) {
            FAIL("Failed to create %s: %s\n", FILEPATH, strerror(status));
        }

        syslog(LOG_INFO, "Created %s\n", FILEPATH);
    }

    cleanup:
        errno = 0;
        if ( chdir(cwd) == -1 ) {
            status = errno;
            syslog(LOG_ERR, "Failed to cd to %s: %s\n", cwd, strerror(status));
        } return status;
}

