#include "routes.h"

enum lwan_http_status
read_index (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )rq; (void )d;

    FILE * f = 0;
    char * content = 0;

    enum lwan_http_status status = HTTP_OK;

    content = malloc(289); // hard-coded for now
    if ( !content ) {
        syslog(LOG_ERR, "Failed to allocate buffer\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    f = fopen("res/index.htm", "r");
    if ( !f ) {
        syslog(LOG_ERR, "Failed to open index.htm\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    errno = 0;
    size_t res = fread(content, 289, 1, f);
    if ( !res ) {
        syslog(LOG_ERR, "Failed to read content into buffer: %s\n", strerror(errno));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    rsp->mime_type = "text/html";
    strbuf_set(rsp->buffer, content, 289);

    cleanup:
        if ( f ) { fclose(f); }
        if ( content ) { free(content); }
        return status;
}

enum lwan_http_status
create_paste (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )rsp; (void )d;

    enum lwan_http_status status = HTTP_OK;

    if ( lwan_request_get_method(rq) != REQUEST_METHOD_POST ) {
        goto cleanup;
    }

    const char * content = lwan_request_get_post_param(rq, "content");

    struct pandabin_paste * pst = pandabin_paste_new(content, strlen(content));

    if ( !pst ) { goto cleanup; }

    signed s = pandabin_db_insert(pst);
    if ( s != SQLITE_OK ) {
        goto cleanup;
    }

    status = HTTP_OK;

    cleanup:
        return status;
}

enum lwan_http_status
read_paste (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )d;

    enum lwan_http_status status = HTTP_OK;

    const char * hash = lwan_request_get_query_param(rq, "hash");
    FILE * f = 0;
    struct pandabin_paste * pst = 0;
    char * content = 0;

    if ( lwan_request_get_method(rq) != REQUEST_METHOD_GET ) {
        goto cleanup;
    }

    if ( !hash ) {
        syslog(LOG_ERR, "Failed to parse parameter\n");
        status = HTTP_BAD_REQUEST;
        goto cleanup;
    }

    pst = pandabin_db_select(&sql_hndls[SEL_HASH], hash);
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
        return status;
}

enum lwan_http_status
delete_paste (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )d;

    enum lwan_http_status status = HTTP_OK;

    const char * uuid = lwan_request_get_query_param(rq, "uuid");
    struct pandabin_paste * pst = 0;

    if ( lwan_request_get_method(rq) != REQUEST_METHOD_DELETE ) {
        goto cleanup;
    }

    pst = pandabin_db_select(&sql_hndls[SEL_UUID], uuid);
    if ( !pst ) {
        syslog(LOG_ERR, "Failed to retrieve paste\n");
        status = HTTP_NOT_FOUND;
        goto cleanup;
    }

    signed res = pandabin_db_delete(pst);
    if ( res != EXIT_SUCCESS ) {
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    rsp->mime_type = "text/plain";
    strbuf_set_static(rsp->buffer, "paste deleted", 13);

    cleanup:
        return status;
}

