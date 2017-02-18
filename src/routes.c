#include "routes.h"


enum lwan_http_status
serve_index (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )rq; (void )d;

    static const char msg [] =
        "<!DOCTYPE html>"
        "<html><body>"
        "<h1>Hello, Pandas!</h1>"
        "</body></html>";

    rsp->mime_type = "text/html";
    strbuf_set_static(rsp->buffer, msg, sizeof(msg) - 1);

    return HTTP_OK;
}

