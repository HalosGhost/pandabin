#include "routes.h"

enum lwan_http_status
read_index (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )rq; (void )d;

    char * content = 0;
    enum lwan_http_status status = HTTP_OK;

    FILE * f = fopen("pages/index.htm", "r");
    if ( !f ) {
        syslog(LOG_ERR, "Failed to open index.htm\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    fseek(f, 0, SEEK_END);
    size_t size = (size_t )ftell(f);
    rewind(f);

    content = malloc((size_t )size + 1);
    if ( !content ) {
        syslog(LOG_ERR, "Failed to allocate buffer\n");
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    errno = 0;
    size_t res = fread(content, size, 1, f);
    if ( !res ) {
        syslog(LOG_ERR, "Failed to read content into buffer: %s\n", strerror(errno));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    rsp->mime_type = "text/html";
    lwan_strbuf_set_static(rsp->buffer, content, size);

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

    if ( !rq->header.body ) {
        status = HTTP_BAD_REQUEST;
        goto cleanup;
    }

    struct pandabin_paste * pst = pandabin_paste_new(rq->header.body->value, rq->header.body->len);

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
    signed s = EXIT_SUCCESS;

    const char * hash = lwan_request_get_query_param(rq, "hash"),
               * ext  = lwan_request_get_query_param(rq, "ext");
    FILE * f = 0;
    struct pandabin_paste * pst = 0;
    char * content = 0;
    char * path = 0;

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

    path = pandabin_paste_path(hash);
    if ( !path ) {
        s = errno;
        syslog(LOG_ERR, "Failed to retrieve path: %s\n", strerror(s));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    f = fopen(path, "r");
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

    rsp->mime_type = ext ? lwan_determine_mime_type_for_file_name(ext) : "text/plain";
    lwan_strbuf_set_static(rsp->buffer, content, pst->size);

    cleanup:
        if ( f ) { fclose(f); }
        if ( content ) { free(content); }
        if ( pst ) { pandabin_paste_free(&pst); }
        if ( path ) { free(path); }
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
    lwan_strbuf_set_static(rsp->buffer, "paste deleted", 13);

    cleanup:
        return status;
}

