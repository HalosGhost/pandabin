#include "common.h"

signed
main (void) {

    struct lwan l;
    struct lwan_config conf = *lwan_get_default_config();
    conf.listener = "0.0.0.0:8080";
    lwan_init_with_config(&l, &conf);

    lwan_straitjacket_enforce(&jacket);

    #define ACMEDIR "/.well-known/acme-challenge"

    const struct lwan_url_map default_map [] = {
        { .prefix = ACMEDIR, PRIV_SERVE_FILES("." ACMEDIR) },
        { .prefix = "/",  REDIRECT_CODE("", HTTP_TEMPORARY_REDIRECT) },
        { .prefix = NULL }
    };

    lwan_set_url_map(&l, default_map);
    lwan_main_loop(&l);
    lwan_shutdown(&l);

    return EXIT_SUCCESS;
}
