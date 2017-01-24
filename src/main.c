#include "main.h"

signed
main (void) {

    signed status = EXIT_SUCCESS;
    openlog(PROGNM, LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started\n");

    cleanup:
        syslog(LOG_INFO, "Ended\n");
        closelog();
        return status;
}
