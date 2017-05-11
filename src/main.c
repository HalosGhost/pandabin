#include "main.h"

static signed
pandabin_dir_init (void);

sqlite3_stmt * sql_hndls [LAST];

struct pandabin_settings settings;

signed
main (void) {

    signed status = EXIT_SUCCESS;
    openlog(PROGNM, LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started\n");

    sqlite3 * db = 0;
    struct lwan l;
    struct lwan_config c = *lwan_get_default_config();

    db = pandabin_db_init(PREFIX "/" PROGNM "/db.sqlite");
    if ( !db ) { status = EXIT_FAILURE; goto cleanup; }

    pandabin_settings_fetch(db);
    printf("%s\n", settings.file_path);
    printf("%zu\n", settings.maxsize);

    c.max_post_data_size = settings.maxsize;
    c.allow_post_temp_file = true;

    status = pandabin_dir_init();
    if ( status != EXIT_SUCCESS ) { goto cleanup; }

    lwan_init_with_config(&l, &c);

    const struct lwan_url_map map [] = {
        { .prefix = "/", .handler = read_index },
        { .prefix = "/create", .handler = create_paste },
        { .prefix = "/read", .handler = read_paste },
        { .prefix = "/delete", .handler = delete_paste },
        { .prefix = NULL }
    };

    lwan_set_url_map(&l, map);
    lwan_main_loop(&l);
    lwan_shutdown(&l);

    cleanup:
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
    if ( chdir(settings.file_path) == -1 ) {
        FAIL("Failed to cd to %s: %s\n", settings.file_path, strerror(status));
    }

    errno = 0;
    if ( chdir("pandabin") == -1 ) {
        syslog(LOG_INFO, "Failed to cd to pandabin: %s\n", strerror(errno));

        errno = 0;
        if ( mkdir("pandabin", 0777) == -1 ) {
            FAIL("Failed to create pandabin: %s\n", strerror(status));
        } syslog(LOG_INFO, "Created pandabin\n");

        errno = 0;
        if ( chdir("pandabin") == -1 ) {
            FAIL("Failed to cd to pandabin: %s\n", strerror(status));
        }
    }

    errno = 0;
    if ( chdir("files") == -1 ) {
        syslog(LOG_INFO, "Failed to cd to files: %s\n", strerror(errno));

        errno = 0;
        if ( mkdir("files", 0777) == -1 ) {
            FAIL("Failed to create files: %s\n", strerror(status));
        }

        syslog(LOG_INFO, "Created files\n");
    }

    cleanup:
        errno = 0;
        if ( chdir(cwd) == -1 ) {
            status = errno;
            syslog(LOG_ERR, "Failed to cd to %s: %s\n", cwd, strerror(status));
        } return status;
}

