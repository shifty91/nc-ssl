/*
 * Copyright (C) 2015-2016,2021 Kurt Kanzenbach <kurt@kmk-computers.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/err.h>
#include <openssl/x509v3.h>

#include "utils.h"
#include "net.h"
#include "config.h"

static inline
void print_ip(const struct addrinfo *sa)
{
    char addr[INET6_ADDRSTRLEN + 1];
    const char *res = NULL;

    if (!sa)
        err("Invalid arguments passed to %s", __func__);

    switch (sa->ai_family) {
    case AF_INET6:
        res = inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa->ai_addr)->sin6_addr),
                        addr, sizeof(addr));
        break;
    case AF_INET:
        res = inet_ntop(AF_INET, &(((struct sockaddr_in*)sa->ai_addr)->sin_addr),
                        addr, sizeof(addr));
        break;
    }

    if (!res) {
        dbg_errno("inet_ntop() failed");
        return;
    }

    dbg("Remote IP '%s'", addr);
}

int tcp_connect(const char *host, const char *service)
{
    /* tcp connect */
    int res;
    int sock = -1;
    struct addrinfo *sa_head, *sa, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    /* prefer IPv6 */
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags  = AI_ADDRCONFIG;

    res = getaddrinfo(host, service, &hints, &sa_head);
    if (res)
        err("getaddrinfo() for host %s failed: %s", host, gai_strerror(res));

    /* try to connect to some record... */
    for (sa = sa_head; sa != NULL; sa = sa->ai_next) {
        sock = socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
        if (sock < 0) {
            freeaddrinfo(sa_head);
            err_errno("socket() failed: %s");
        }

        if (!connect(sock, sa->ai_addr, sa->ai_addrlen)) {
            print_ip(sa);
            break;
        }

        close(sock);
    }

    if (!sa)
        err_errno("connect() for host %s on service %s failed: %s", host,
                  service);

    freeaddrinfo(sa_head);

    return sock;
}

#define SSL_ERROR(msg, label)                   \
    do {                                        \
        log_err(msg);                           \
        ERR_print_errors_fp(stderr);            \
        goto label;                             \
    } while (0)

void ssl_connect(SSL **ssl, SSL_CTX **ctx, int sock, const char *host)
{
    if (!ssl || !ctx || sock < 0)
        err("Invalid arguments passed to %s", __func__);

    SSL_load_error_strings();
    SSL_library_init();

    *ctx = SSL_CTX_new(SSLv23_client_method());
    if (*ctx == NULL) {
        ERR_print_errors_fp(stderr);
        err("SSL_CTX_new() failed");
    }

    /* set method: default: TLS */
    if (!config.use_sslv2)
        SSL_CTX_set_options(*ctx, SSL_OP_NO_SSLv2);
    if (!config.use_sslv3)
        SSL_CTX_set_options(*ctx, SSL_OP_NO_SSLv3);

    /* Set better cipher suits */
    static const char * const PREFERRED_CIPHERS = "HIGH:MEDIUM:!RC4:!SRP:!PSK:!MD5:!aNULL@STRENGTH";
    if (SSL_CTX_set_cipher_list(*ctx, PREFERRED_CIPHERS) != 1) {
        log_err("Failed to set ciphers");
        goto clean0;
    }

    SSL_CTX_set_default_verify_paths(*ctx);

    *ssl = SSL_new(*ctx);
    if (*ssl == NULL)
        SSL_ERROR("SSL_new() failed:", clean0);

    if (!SSL_set_fd(*ssl, sock))
        SSL_ERROR("SSL_set_fd() failed:", clean1);

    /* verify certificate with hostname */
    if (config.verify_cert) {
        X509_VERIFY_PARAM *param;
        param = SSL_get0_param(*ssl);

        X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        X509_VERIFY_PARAM_set1_host(param, host, 0);
        SSL_set_verify(*ssl, SSL_VERIFY_PEER, 0);
    }

    if (SSL_connect(*ssl) != 1)
        SSL_ERROR("SSL_connect() failed:", clean1);

    /* certificate verified? */
    long res = SSL_get_verify_result(*ssl);
    if (res != X509_V_OK)
        dbg("Server's certificate not verfified: %ld", res);
    else
        dbg("Server's certificate verified.");

    dbg("SSL connection is using %s encryption", SSL_get_cipher(*ssl));

    return;

clean1:
    SSL_free(*ssl);
clean0:
    SSL_CTX_free(*ctx);
    close(sock);

    exit(EXIT_FAILURE);
}

void ssl_shutdown(SSL *ssl, SSL_CTX *ctx)
{
    if (!ssl || !ctx)
        err("Invalid arguments passed to %s", __func__);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}
