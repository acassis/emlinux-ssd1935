/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 5F, No. 36, Taiyuan St.
 * Jhubei City,
 * Hsin-chu County 302, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************
*/

#ifndef __SHA_H__
#define __SHA_H__

#include "rt_config.h"

/* Algorithm options */
//#define SHA1
#define SHA256
//#define SHA512

VOID sha256_init(UINT32 hashValue[]);
VOID sha256_output(const UINT32 hashValue[], UCHAR out[]);
VOID sha256_hash(UINT32 hashValue[], const UCHAR data[], UINT data_len, UINT append_len);
VOID sha256(UCHAR out[], const UCHAR data[], const UINT data_len);
VOID hmac_sha(const UCHAR key[], UINT key_len, const UCHAR data[], UINT data_len, UCHAR mac[], UINT mac_len);


#endif /* __SHA_H__ */
