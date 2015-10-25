#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>

#include <openssl/ssl.h>

#include "net.h"
#include "config.h"
#include "utils.h"

static volatile int stop = 0;

static void print_usage_and_exit(void)
{
    fprintf(stderr, "usage: nc-ssl [options] <host> <port>\n");
    fprintf(stderr, "  options:\n");
    fprintf(stderr, "    -d: enable debug output\n");
    fprintf(stderr, "    -2: use SSL version 2\n");
    fprintf(stderr, "    -3: use SSL version 3\n");
    fprintf(stderr, "nc-ssl Version 1.0 (C) Kurt Kanzenbach 2015 <kurt@kmk-computers.de>\n");
    exit(EXIT_FAILURE);
}

static void term_handler(int sig)
{
    stop = 1;
}

static void setup_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = term_handler;
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL))
        err("sigaction() failed: %s", strerror(errno));
    if (sigaction(SIGINT, &sa, NULL))
        err("sigaction() failed: %s", strerror(errno));
}

/*
 * This program works rather simple:
 *  A connection to the remote is via TLS is established.
 *  Input is send to the server, output printed to screen.
 *  select(2) is used for waiting on events of the descriptors.
 */
int main(int argc, char *argv[])
{
    SSL *ssl;
    SSL_CTX *ctx;
    int socket , ret = EXIT_FAILURE, c;
    char *host, *service;

    /* parse args */
    while ((c = getopt(argc, argv, "vd23")) != -1) {
        switch (c) {
        case 'd':
            config.debug = 1;
            break;
        case 'v':
            config.verify_cert = 1;
            break;
        case '2':
            config.use_sslv2 = 1;
            break;
        case '3':
            config.use_sslv3 = 1;
            break;
        case '?':
        default:
            print_usage_and_exit();
        }
    }

    if (argc - optind != 2)
        print_usage_and_exit();

    /* tcp connect */
    host    = argv[optind + 0];
    service = argv[optind + 1];
    socket  = tcp_connect(host, service);
    dbg("Connected to host %s on service %s", host, service);

    /* ssl connect */
    ssl_connect(&ssl, &ctx, socket, host);
    dbg("SSL connection to host %s established", host);

    /* setup signals */
    setup_signals();

    /* select loop */
    int stdin_closed = 0;
    while (!stop) {
        int rc;
        fd_set rfds;

        FD_ZERO(&rfds);
        FD_SET(socket, &rfds);
        if (!stdin_closed)
            FD_SET(fileno(stdin), &rfds);

        /* note: select might be interrupted */
        do {
            rc = select(socket + 1, &rfds, NULL, NULL, NULL);
        } while (rc < 0 && errno == EINTR && !stop);

        if (stop) {
            dbg("Catched SIGTERM or SIGINT, cleaning up...");
            break;
        }

        if (rc == -1) {
            log_err("select() failed: %s", strerror(errno));
            goto clean;
        }
        if (rc == 0) {
            ret = EXIT_SUCCESS;
            goto clean;
        }

        /* input available -> send to server */
        if (!stdin_closed && FD_ISSET(fileno(stdin), &rfds)) {
            dbg("STDIN input");
            /* read buffer */
            char buffer[512];
            size_t written = 0;
            ssize_t read_ = read(0, buffer, sizeof(buffer));

            if (read_ < 0) {
                log_err("read() failed %s", strerror(errno));
                goto clean;
            }
            if (read_ == 0) {   /* EOF */
                stdin_closed = 1;
                goto server;
            }

            /* send! */
            do {
                int tmp = SSL_write(ssl, buffer + written, read_ - written);
                if (tmp < 0) {
                    log_err("SSL_write() failed");
                    goto clean;
                }
                written += tmp;
            } while (written < read_);
        }

    server:
        /* server response available -> print to stdout */
        if (FD_ISSET(socket, &rfds)) {
            dbg("Server output");
            do {
                /* read! */
                char buffer[512];
                size_t written = 0;
                int read = SSL_read(ssl, buffer, sizeof(buffer));

                if (read < 0) {
                    log_err("SSL_read() failed");
                    goto clean;
                }
                if (read == 0) {
                    ret = EXIT_SUCCESS;
                    goto clean;
                }

                /* write */
                do {
                    ssize_t tmp = write(fileno(stdout), buffer + written, read - written);
                    if (tmp < 0) {
                        log_err("write() failed: %s", strerror(errno));
                        goto clean;
                    }
                    written += tmp;
                } while (written < read);
            } while (SSL_pending(ssl));
        }
    }

    ret = EXIT_SUCCESS;

clean:
    dbg("Shutting down SSL connection to host %s", host);
    ssl_shutdown(ssl, ctx);
    close(socket);

    return ret;
}
