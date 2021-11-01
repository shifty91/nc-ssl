/*
 * Copyright (C) 2015-2016,2021 Kurt Kanzenbach <kurt@kmk-computers.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _NET_H_
#define _NET_H_

#include <openssl/ssl.h>

int tcp_connect(const char *host, const char *service);
void ssl_connect(SSL **ssl, SSL_CTX **ctx, int sock, const char *host);
void ssl_shutdown(SSL *ssl, SSL_CTX *ctx);

#endif /* _NET_H_ */
