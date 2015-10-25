#ifndef _CONFIG_H_
#define _CONFIG_H_

struct config {
    int debug;
    int verify_cert;
    int use_sslv2;
    int use_sslv3;
};

extern struct config config;

#endif /* _CONFIG_H_ */
