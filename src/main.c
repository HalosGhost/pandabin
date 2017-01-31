#include "main.h"

signed
main (void) {

    signed status = EXIT_SUCCESS;
    openlog(PROGNM, LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started\n");

    { // directory initialization
        char cwd [PATH_MAX] = "";
        errno = 0;
        if ( !getcwd(cwd, PATH_MAX - 1) ) {
            status = errno;
            syslog(LOG_ERR, "Could not store working directory: %s\n", strerror(status));
            goto cleanup;
        }

        errno = 0;
        if ( chdir(PREFIX) == -1 ) {
            status = errno;
            syslog(LOG_ERR, "Could not cd to %s: %s\n", PREFIX, strerror(status));
            goto cleanup;
        }

        errno = 0;
        if ( chdir(MAINPATH) == -1 ) {
            syslog(LOG_INFO, "Could not cd to %s: %s\n", MAINPATH, strerror(errno));

            errno = 0;
            if ( mkdir(MAINPATH, 0777) == -1 ) {
                status = errno;
                syslog(LOG_ERR, "Could not create %s: %s\n", MAINPATH, strerror(status));
                goto cleanup;
            }

            syslog(LOG_INFO, "Created %s\n", MAINPATH);
            errno = 0;
            if ( chdir(MAINPATH) == -1 ) {
                status = errno;
                syslog(LOG_ERR, "Still Could not cd to %s: %s\n", MAINPATH, strerror(status));
                goto cleanup;
            }
        }

        errno = 0;
        if ( chdir(FILEPATH) == -1 ) {
            syslog(LOG_INFO, "Could not cd to %s: %s\n", FILEPATH, strerror(errno));

            errno = 0;
            if ( mkdir(FILEPATH, 0777) == -1 ) {
                status = errno;
                syslog(LOG_ERR, "Could not create %s: %s\n", FILEPATH, strerror(status));
                goto cleanup;
            }

            syslog(LOG_INFO, "Created %s\n", FILEPATH);
        }

        errno = 0;
        if ( chdir(cwd) == -1 ) {
            status = errno;
            syslog(LOG_ERR, "Could not cd to %s: %s\n", cwd, strerror(status));
            goto cleanup;
        }
    }

    cleanup:
        syslog(LOG_INFO, "Ended\n");
        closelog();
        return status;
}
