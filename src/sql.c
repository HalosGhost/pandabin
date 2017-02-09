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

    cleanup:
        if ( status != SQLITE_OK ) {
            sqlite3_close(db);
            return NULL;
        } return db;
}

signed
pandabin_db_insert (sqlite3 * db, struct pandabin_paste * pst) {

    signed status = EXIT_SUCCESS;

    (void )db; (void )pst;
    goto cleanup;

    cleanup:
        return status;
}

