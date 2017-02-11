#include "sql.h"

sqlite3 *
pandabin_db_init (void) {

    signed status = EXIT_SUCCESS;
    sqlite3 * db = 0;

    status = sqlite3_open(DBPATH, &db);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to open database: %s\n", sqlite3_errmsg(db));
    }

    char * err = 0;
    status = sqlite3_exec(db, pandabin_schema, NULL, NULL, &err);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to initialize schema: %s\n", err);
    }

    const char * leftovers;
    status = sqlite3_prepare_v2(db, ins_stmt, -1, &ins_handle, &leftovers);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
    }

    cleanup:
        if ( status != SQLITE_OK ) {
            sqlite3_close(db);
            return NULL;
        } return db;
}

signed
pandabin_db_insert (sqlite3 * db, struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(ins_handle, 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to bind uuid: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_bind_text(ins_handle, 2, pst->path, -1, NULL);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to bind path: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_bind_int(ins_handle, 3, (signed )pst->size);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to bind size: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_bind_text(ins_handle, 4, pst->hash, -1, NULL);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to bind hash: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_step(ins_handle);
    if ( status != SQLITE_DONE ) {
        FAIL("Failed to execute insert: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_reset(ins_handle);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to reset insert: %s\n", sqlite3_errmsg(db));
    }
    cleanup:
        return status;
}

signed
pandabin_db_delete (sqlite3 * db, struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;

    char uuid [37] = "";
    uuid_unparse_lower(pst->uuid, uuid);
    status = sqlite3_bind_text(rmv_handle, 1, uuid, 36, NULL);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to bind uuid: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_step(rmv_handle);
    if ( status != SQLITE_DONE ) {
        FAIL("Failed to execute delete: %s\n", sqlite3_errmsg(db));
    }

    status = sqlite3_reset(rmv_handle);
    if ( status != SQLITE_OK ) {
        FAIL("Failed to reset delete: %s\n", sqlite3_errmsg(db));
    }

    status = unlink(pst->path);
    if ( status ) {
        FAIL("Failed to delete paste content: %s\n", strerror(status));
    }

    cleanup:
        return status;
}

signed
pandabin_db_cleanup (sqlite3 * db) {

    if ( ins_handle ) { sqlite3_finalize(ins_handle); }
    if ( db ) {
        sqlite3_close(db);
    } return EXIT_SUCCESS;
}
