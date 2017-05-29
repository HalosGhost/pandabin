#include "sql.h"

sqlite3 *
pandabin_db_init (const char * path) {

    signed status = EXIT_SUCCESS;
    sqlite3 * db = 0;

    status = sqlite3_open(path, &db);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to open database: %s\n", sqlite3_errmsg(db));
    }

    char * err = 0;
    status = sqlite3_exec(db, pandabin_schema, NULL, NULL, &err);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to initialize schema: %s\n", err);
    }

    const char * left;
    for ( enum pandabin_tag i = INS; i < LAST; ++ i ) {
        status = sqlite3_prepare_v2(db, sql_stmts[i], -1, &sql_hndls[i], &left);
        if ( status != SQLITE_OK ) {
            errno = status;
            FAIL("Failed to prepare statement (%d): %s\n", i, sqlite3_errmsg(db));
        }
    }

    cleanup:
        if ( status != SQLITE_OK ) {
            pandabin_db_cleanup(db);
            return NULL;
        } return db;
}

signed
pandabin_db_insert (struct pandabin_paste * pst) {

    signed status = SQLITE_OK;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(sql_hndls[INS], 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind uuid: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_int(sql_hndls[INS], 2, (signed )pst->size);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind size: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_text(sql_hndls[INS], 3, pst->hash, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind hash: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(sql_hndls[INS]);
    if ( status != SQLITE_DONE ) {
        errno = status;
        FAIL("Failed to execute insert: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_reset(sql_hndls[INS]);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to reset insert: %s\n", sqlite3_errstr(status));
    }

    cleanup:
        return status;
}

struct pandabin_paste *
pandabin_db_select (sqlite3_stmt ** handle, const char * restrict val) {

    signed status = EXIT_SUCCESS;
    struct pandabin_paste * pst = 0;

    if ( !handle || !*handle ) {
        FAIL("Failed to acquire select handle\n");
        goto cleanup;
    }

    pst = malloc(sizeof(struct pandabin_paste));
    if ( !pst ) {
        FAIL("Failed to allocate paste struct: %s\n", strerror(ENOMEM));
    }

    pst->hash = malloc(65);
    if ( !pst->hash ) {
        FAIL("Failed to allocate hash hexdigest: %s\n", strerror(ENOMEM));
    }

    status = sqlite3_bind_text(*handle, 1, val, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind value: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(*handle);
    if ( status != SQLITE_ROW ) {
        errno = status;
        FAIL("Failed to retrieve paste: %s\n", sqlite3_errstr(status));
    }

    status = uuid_parse((const char * )sqlite3_column_text(*handle, 0),
                        pst->uuid);
    if ( status ) {
        errno = status;
        FAIL("Failed to parse uuid: %s\n", "unknown error");
    }

    pst->size = (size_t )sqlite3_column_int(*handle, 1);

    strncpy(pst->hash, (const char * )sqlite3_column_text(*handle, 2), 65);

    status = EXIT_SUCCESS;

    cleanup:
        if ( handle && *handle ) { sqlite3_reset(*handle); }
        if ( status != EXIT_SUCCESS ) { pandabin_paste_free(&pst); }
        return pst;
}

signed
pandabin_db_delete (struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;
    char * path = 0;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(sql_hndls[RMV], 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind uuid: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(sql_hndls[RMV]);
    if ( status != SQLITE_DONE ) {
        errno = status;
        FAIL("Failed to execute delete: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_reset(sql_hndls[RMV]);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to reset delete: %s\n", sqlite3_errstr(status));
    }

    path = pandabin_paste_path(pst->hash);
    if ( !path ) {
        FAIL("Failed to retrieve path: %s\n", strerror(status));
    }

    status = unlink(path);
    if ( status ) {
        FAIL("Failed to delete paste content: %s\n", strerror(status));
    }

    status = EXIT_SUCCESS;

    cleanup:
        if ( path ) { free(path); }
        return status;
}

void
pandabin_settings_fetch (sqlite3 * db) {

    signed status = EXIT_SUCCESS;
    if ( !db ) { return; }

    errno = 0;
    status = sqlite3_bind_text(sql_hndls[SET], 1, "max size", 8, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind name: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(sql_hndls[SET]);
    if ( status != SQLITE_ROW ) {
        syslog(LOG_ERR, "Failed to retrieve setting (using default): %s\n",
               sqlite3_errstr(status));
        settings.maxsize = MAXSIZE;
    }

    const char * setting = (const char * )sqlite3_column_text(sql_hndls[SET], 0);
    if ( setting ) {
        sscanf(setting, "%zu", &settings.maxsize);
    }

    status = sqlite3_reset(sql_hndls[SET]);
    if ( status ) {
        errno = status;
        FAIL("Failed to reset setting: %s\n", sqlite3_errstr(status));
    }

    errno = 0;
    status = sqlite3_bind_text(sql_hndls[SET], 1, "file path", 9, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind name: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(sql_hndls[SET]);
    if ( status != SQLITE_ROW ) {
        syslog(LOG_ERR, "Failed to retrieve setting (using default): %s\n",
               sqlite3_errstr(status));
        snprintf(settings.file_path, PATH_MAX - 1, "%s", FILEPATH);
    }

    setting = (const char * )sqlite3_column_text(sql_hndls[SET], 0);
    if ( setting ) {
        sscanf(setting, "%s", settings.file_path);
    }

    status = sqlite3_reset(sql_hndls[SET]);
    if ( status ) {
        errno = status;
        FAIL("Failed to reset setting: %s\n", sqlite3_errstr(status));
    }
    cleanup:
        return;
}

signed
pandabin_db_cleanup (sqlite3 * db) {

    for ( enum pandabin_tag i = INS; i < LAST; ++ i ) {
        if ( sql_hndls[i] ) { sqlite3_finalize(sql_hndls[i]); }
    }

    if ( db ) { sqlite3_close(db); }
    return EXIT_SUCCESS;
}

char *
pandabin_paste_path (const char * hash) {

    signed status = EXIT_SUCCESS;
    char * path = 0;

    if ( !hash ) {
        syslog(LOG_ERR, "Specified hash was NULL\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    size_t pathlen = strlen(settings.file_path) + 91;
    path = malloc(pathlen);
    if ( !path ) {
        status = errno;
        syslog(LOG_ERR, "Failed to allocate path: %s\n", strerror(status));
        goto cleanup;
    }

    status = snprintf(path, pathlen, "%s/pandabin/files/%.3s/%s",
                      settings.file_path, hash, hash);

    if ( status < 0 ) {
        syslog(LOG_ERR, "Failed to store path\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    status = EXIT_SUCCESS;
    syslog(LOG_INFO, "Paste path: %s\n", path);

    cleanup:
        if ( status != EXIT_SUCCESS ) {
            if ( path ) { free(path); path = NULL; }
        } return path;
}

