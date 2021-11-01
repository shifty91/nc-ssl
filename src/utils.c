/*
 * Copyright (C) 2015-2016,2021 Kurt Kanzenbach <kurt@kmk-computers.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "utils.h"

void _log(const char * restrict level, int die, int with_errno,
          const char * restrict file, int line, const char * restrict fmt, ...)
{
    FILE *out;
    va_list args;

    va_start(args, fmt);

    if (!strcmp(level, "ERROR"))
        out = stderr;
    else
        out = stdout;

    fprintf(out, "[%s %s:%d]: ", level, file, line);
    vfprintf(out, fmt, args);
    if (with_errno)
        fprintf(out, ": %s", strerror(errno));
    fprintf(out, "\n");
    fflush(out);

    va_end(args);

    if (die)
        exit(EXIT_FAILURE);
}
