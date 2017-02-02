#if !defined(PANDABIN_COMMON_H)
#define PANDABIN_COMMON_H

#pragma once

#define FAIL(...) do { \
    status = errno; \
    syslog(LOG_ERR, __VA_ARGS__); \
    goto cleanup; \
} while (false)

#endif
