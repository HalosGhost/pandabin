#if !defined(PANDABIN_ROUTES_H)
#define PANDABIN_ROUTES_H

LWAN_HANDLER(index) {
    response->mime_type = "text/html; charset=UTF-8";
    response->headers = headers;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char host [NI_MAXHOST] = "";
    int res = getnameinfo((struct sockaddr * )&addr, sizeof addr, host, NI_MAXHOST, 0, 0, 0);

    const char * str = host;
    size_t len = NI_MAXHOST;

    if ( res ) {
        str = gai_strerror(res);
        len = strlen(gai_strerror(res));
    }

    lwan_strbuf_set(response->buffer, str, len);

    return HTTP_OK;
}

#endif

