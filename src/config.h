/*
 * Copyright (C) 2015-2016,2021 Kurt Kanzenbach <kurt@kmk-computers.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

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
