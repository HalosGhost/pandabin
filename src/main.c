#include "main.h"

signed
main (void) {

    struct lwan l;
    struct lwan_config conf = *lwan_get_default_config();
    conf.listener = "0.0.0.0:8443";
    lwan_init_with_config(&l, &conf);

    lwan_straitjacket_enforce(&jacket);

    const struct lwan_url_map default_map [] = {
        { .prefix = "/",         PRIV_SERVE_FILES("./pages")           },
        { .prefix = "/assets",   PRIV_SERVE_FILES("./assets")          },
        { .prefix = "/media",    PRIV_SERVE_FILES("./media")           },
        { .prefix = NULL }
    };

    lwan_set_url_map(&l, default_map);
    lwan_main_loop(&l);
    lwan_shutdown(&l);

    return EXIT_SUCCESS;
}

