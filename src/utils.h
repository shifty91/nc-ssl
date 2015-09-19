#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

/* printing */
#define err(fmt, ...)                                                   \
    do {                                                                \
        fprintf(stderr, "[ERROR %s:%d]: " fmt "\n", __FILE__, __LINE__,  ##__VA_ARGS__); \
        exit(EXIT_FAILURE);                                             \
    } while (0)

#define log_err(fmt, ...)                                               \
    do {                                                                \
        fprintf(stderr, "[ERROR %s:%d]: " fmt "\n", __FILE__, __LINE__,  ##__VA_ARGS__); \
    } while (0)

#define dbg(fmt, ...)                                                   \
    do {                                                                \
        if (config.debug)                                               \
            printf("[DEBUG %s:%d]: " fmt "\n", __FILE__, __LINE__,  ##__VA_ARGS__); \
    } while (0)

#define info(fmt, ...)                                                  \
    do {                                                                \
        printf("[INFO %s:%d]: " fmt "\n", __FILE__, __LINE__,  ##__VA_ARGS__); \
    } while (0)

/* functions */
void *kurt_malloc(size_t size);
void kurt_free(void **ptr);

#endif /* _UTILS_H_ */
