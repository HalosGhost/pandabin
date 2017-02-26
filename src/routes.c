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

enum lwan_http_status
get_paste (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )d;

    enum lwan_http_status status = HTTP_OK;

    const char * hash = lwan_request_get_query_param(rq, "hash");
    FILE * f = 0;
    struct pandabin_paste * pst = 0;
    char * content = 0;

    if ( !hash ) {
        syslog(LOG_ERR, "Failed to parse parameter\n");
        status = HTTP_BAD_REQUEST;
        goto cleanup;
    }

    pst = pandabin_db_select(hash);
    if ( !pst ) {
        syslog(LOG_ERR, "Failed to retrieve paste\n");
        status = HTTP_NOT_FOUND;
        goto cleanup;
    }

    content = malloc(pst->size);
    if ( !content ) {
        syslog(LOG_ERR, "Failed to allocate buffer\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    f = fopen(pst->path, "r");
    if ( !f ) {
        syslog(LOG_ERR, "Failed to open paste\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    errno = 0;
    size_t res = fread(content, pst->size, 1, f);
    if ( !res ) {
        syslog(LOG_ERR, "Failed to read content into buffer: %s\n", strerror(errno));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    rsp->mime_type = "text/plain";
    strbuf_set(rsp->buffer, content, pst->size);

    cleanup:
        if ( f ) { fclose(f); }
        if ( content ) { free(content); }
        if ( pst ) { pandabin_paste_free(&pst); }
        fclose(f);
        return status;
}

