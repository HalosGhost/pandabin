#include "sql.h"

signed
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
        sqlite3_close(db);
        return status == SQLITE_OK ? EXIT_SUCCESS : status;
}


