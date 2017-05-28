#include "routes.h"

enum lwan_http_status
read_index (struct lwan_request * rq, struct lwan_response * rsp, void * d) {

    (void )rq; (void )d;

    FILE * f = 0;
    char * content = 0;

    enum lwan_http_status status = HTTP_OK;

    #define INDEXSIZE 1188
    content = malloc(INDEXSIZE); // hard-coded for now
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
    size_t res = fread(content, INDEXSIZE, 1, f);
    if ( !res ) {
        syslog(LOG_ERR, "Failed to read content into buffer: %s\n", strerror(errno));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    rsp->mime_type = "text/html";
    strbuf_set(rsp->buffer, content, INDEXSIZE);

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

    size_t pathlen = strlen(settings.file_path) + 71;
    char * path = malloc(pathlen);
    if ( !path ) {
        s = errno;
        syslog(LOG_ERR, "Failed to allocate path: %s\n", strerror(s));
        status = HTTP_INTERNAL_ERROR;
        goto cleanup;
    }

    s = snprintf(path, pathlen, "%s/%.3s/%s",
                 settings.file_path, pst->hash, pst->hash);

    if ( s < 0 ) {
        syslog(LOG_ERR, "Failed to store path\n");
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

