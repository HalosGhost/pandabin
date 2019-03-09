#if !defined(MAIN_H)
#define MAIN_H

#pragma once

#include "common.h"
#include "sql.h"

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

static struct lwan_key_value headers [] = {
    { .key = "content-security-policy", .value = "default-src 'self'" },
    { .key = "x-frame-options", .value = "SAMEORIGIN" },
    { .key = "x-xss-protection", .value = "1; mode=block" },
    { .key = "x-content-type-options", .value = "nosniff" },
    { .key = "referrer-policy", .value = "no-referrer" },
    { .key = "strict-transport-security", .value = "max-age=31536000; includeSubDomains" },
    { .key = NULL, .value = NULL }
};

LWAN_HANDLER(index) {
    response->mime_type = "text/html; charset=UTF-8";
    response->headers = headers;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char host [NI_MAXHOST] = "";
    int res = getnameinfo((struct sockaddr * )&addr, sizeof addr, host, NI_MAXHOST, 0, 0, NI_NAMEREQD);

    const char * str = host;
    size_t len = NI_MAXHOST;

    if ( res ) {
        str = gai_strerror(res);
        len = strlen(gai_strerror(res));
    }

    lwan_strbuf_set(response->buffer, str, len);

    return HTTP_OK;
}

#endif // MAIN_H

