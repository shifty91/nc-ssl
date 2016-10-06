#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "config.h"

/* printing */
#define err(...)                                                        \
    do {                                                                \
        _log("ERROR", 1, 0, basename(__FILE__), __LINE__, __VA_ARGS__); \
    } while (0)

#define log_err(...)                                                    \
    do {                                                                \
        _log("ERROR", 0, 0, basename(__FILE__), __LINE__, __VA_ARGS__); \
    } while (0)

#define err_errno(...)                                                  \
    do {                                                                \
        _log("ERROR", 1, 1, basename(__FILE__), __LINE__, __VA_ARGS__); \
    } while (0)

#define log_err_errno(...)                                              \
    do {                                                                \
        _log("ERROR", 0, 1, basename(__FILE__), __LINE__, __VA_ARGS__); \
    } while (0)

#define info(...)                                                       \
    do {                                                                \
        _log("INFO", 0, 0, basename(__FILE__), __LINE__, __VA_ARGS__);  \
    } while (0)

#define unlikely(x) __builtin_expect((x), 0)

#define dbg(...)                                                        \
    do {                                                                \
        if (unlikely(config.debug))                                     \
            _log("DEBUG", 0, 0, basename(__FILE__), __LINE__,           \
                 __VA_ARGS__);                                          \
    } while (0)

#define dbg_errno(...)                                                  \
    do {                                                                \
        if (unlikely(config.debug))                                     \
            _log("DEBUG", 0, 1, basename(__FILE__), __LINE__,           \
                 __VA_ARGS__);                                          \
    } while (0)

void _log(const char * restrict level, int die, int with_errno,
          const char * restrict file, int line, const char * restrict fmt, ...);

#endif /* _UTILS_H_ */
