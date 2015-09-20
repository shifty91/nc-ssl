#ifndef _CONFIG_H_
#define _CONFIG_H_

struct config {
    int debug;
    int verify_cert;
};

extern struct config config;

#endif /* _CONFIG_H_ */
