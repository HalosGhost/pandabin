#include "main.h"

static signed
pandabin_dir_init (void);

signed
main (void) {

    signed status = EXIT_SUCCESS;
    openlog(PROGNM, LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started\n");

    sqlite3 * db = 0;
    struct lwan l;
    struct lwan_config c = *lwan_get_default_config();
    struct pandabin_settings settings;

    status = pandabin_dir_init();
    if ( status != EXIT_SUCCESS ) { goto cleanup; }

    db = pandabin_db_init();
    if ( !db ) { status = EXIT_FAILURE; goto cleanup; }

    pandabin_settings_fetch(db, &settings);

    c.max_post_data_size = settings.maxsize;

    lwan_init_with_config(&l, &c);

    const struct lwan_url_map map [] = {
        { .prefix = "/home", .handler = serve_index },
        { .prefix = NULL }
    };

    lwan_set_url_map(&l, map);
    lwan_main_loop(&l);

    cleanup:
        lwan_shutdown(&l);
        pandabin_db_cleanup(db);
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

