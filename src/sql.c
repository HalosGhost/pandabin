#include "sql.h"

sqlite3 *
pandabin_db_init (void) {

    signed status = EXIT_SUCCESS;
    sqlite3 * db = 0;

    status = sqlite3_open(DBPATH, &db);
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

    const char * leftovers;
    status = sqlite3_prepare_v2(db, ins_stmt, -1, &ins_handle, &leftovers);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_prepare_v2(db, sel_stmt, -1, &sel_handle, &leftovers);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_prepare_v2(db, rmv_stmt, -1, &rmv_handle, &leftovers);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
    }

    cleanup:
        if ( status != SQLITE_OK ) {
            sqlite3_close(db);
            return NULL;
        } return db;
}

signed
pandabin_db_insert (struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(ins_handle, 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind uuid: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_text(ins_handle, 2, pst->path, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind path: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_int(ins_handle, 3, (signed )pst->size);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind size: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_text(ins_handle, 4, pst->hash, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind hash: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(ins_handle);
    if ( status != SQLITE_DONE ) {
        errno = status;
        FAIL("Failed to execute insert: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_reset(ins_handle);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to reset insert: %s\n", sqlite3_errstr(status));
    }

    cleanup:
        return status;
}

struct pandabin_paste *
pandabin_db_select (const char * restrict key, const char * restrict val) {

    signed status = EXIT_SUCCESS;

    struct pandabin_paste * pst = malloc(sizeof(struct pandabin_paste));
    if ( !pst ) {
        FAIL("Failed to allocate paste struct: %s\n", strerror(ENOMEM));
    }

    pst->hash = malloc(65);
    if ( !pst->hash ) {
        FAIL("Failed to allocate hash hexdigest: %s\n", strerror(ENOMEM));
    }

    size_t pathlen = strlen(FILEPATH) + 71;
    pst->path = malloc(pathlen);
    if ( !pst->path ) {
        FAIL("Failed to allocate path: %s\n", strerror(ENOMEM));
    }

    status = sqlite3_bind_text(sel_handle, 1, key, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind key: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_bind_text(sel_handle, 2, val, -1, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind value: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(sel_handle);
    if ( status != SQLITE_ROW ) {
        errno = status;
        FAIL("Failed to retrieve paste: %s\n", sqlite3_errstr(status));
    }

    status = uuid_parse((const char * )sqlite3_column_text(sel_handle, 0),
                        pst->uuid);
    if ( status ) {
        errno = status;
        FAIL("Failed to parse uuid: %s\n", "unknown error");
    }

    strncpy(pst->path, (const char * )sqlite3_column_text(sel_handle, 1),
                       pathlen);

    pst->size = (size_t )sqlite3_column_int(sel_handle, 2);

    strncpy(pst->hash, (const char * )sqlite3_column_text(sel_handle, 3), 65);

    cleanup:
        sqlite3_reset(sel_handle);
        if ( status != EXIT_SUCCESS ) { pandabin_paste_free(&pst); }
        return pst;
}

signed
pandabin_db_delete (struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(rmv_handle, 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to bind uuid: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_step(rmv_handle);
    if ( status != SQLITE_DONE ) {
        errno = status;
        FAIL("Failed to execute delete: %s\n", sqlite3_errstr(status));
    }

    status = sqlite3_reset(rmv_handle);
    if ( status != SQLITE_OK ) {
        errno = status;
        FAIL("Failed to reset delete: %s\n", sqlite3_errstr(status));
    }

    status = unlink(pst->path);
    if ( status ) {
        FAIL("Failed to delete paste content: %s\n", strerror(status));
    }

    status = EXIT_SUCCESS;

    cleanup:
        return status;
}

signed
pandabin_db_cleanup (sqlite3 * db) {

    if ( ins_handle ) { sqlite3_finalize(ins_handle); }
    if ( sel_handle ) { sqlite3_finalize(sel_handle); }
    if ( rmv_handle ) { sqlite3_finalize(rmv_handle); }
    if ( db ) {
        sqlite3_close(db);
    } return EXIT_SUCCESS;
}
