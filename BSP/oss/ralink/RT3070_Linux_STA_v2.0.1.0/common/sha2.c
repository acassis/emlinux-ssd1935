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

    Module Name:
    sha.c

    Abstract:
    Serure hash algorithem (SHA)
    
    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
	  Eddy		    2008/07/23		  create SHA256, hmac_SHA256
*/

#include "sha2.h"

/* Basic operations */
#define SHR(x,n) (x >> n) /* SHR(x)^n, right shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTR(x,n,w) ((x >> n) | (x << (w - n))) /* ROTR(x)^n, circular right shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTL(x,n,w) ((x << n) | (x >> (w - n))) /* ROTL(x)^n, circular left shift n bits , x is w-bit word, 0 <= n <= w */
#define ROTR32(x,n) ROTR(x,n,32)
#define ROTL32(x,n) ROTL(x,n,32) 

/* Basic functions */
#define Ch(x,y,z) ((x & y) ^ ((~x) & z))
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
#define Parity(x,y,z) (x ^ y ^ z)

#ifdef SHA256
/* SHA256 functions */
#define Zsigma_256_0(x) (ROTR32(x,2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define Zsigma_256_1(x) (ROTR32(x,6) ^ ROTR32(x,11) ^ ROTR32(x,25))
#define Sigma_256_0(x) (ROTR32(x,7) ^ ROTR32(x,18) ^ SHR(x,3))
#define Sigma_256_1(x) (ROTR32(x,17) ^ ROTR32(x,19) ^ SHR(x,10))
/* SHA256 constants */
#define SHA256_BLOCK_SIZE	64 /* 512 bits = 64 bytes */
#define SHA256_DIGEST_SIZE	32 /* 256 bits = 32 bytes */
static const UINT32 K_256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL, 
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,	0x06ca6351UL, 0x14292967UL, 
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,	0x682e6ff3UL, 
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};
static const UINT32 hashValue256[8] = {
		0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
		0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL
};
#endif /* SHA256 */


#ifdef SHA256

/* sha256_init: Initial Hash Value, 8*32 = SHA256_DIGEST_SIZE
 * @hashValue: Hash Value
 */
VOID sha256_init(UINT32 hashValue[])
{
	memcpy(hashValue,hashValue256, sizeof(UINT32) * 8);
//	memcpy((UCHAR *) &hashValue[0],(UCHAR *) &hashValue256[0],SHA256_DIGEST_SIZE);
} //end sha256_init()


/* sha256_output: transform the Hash Value to digest message
 * @hv: Hash Value
 * @out: digest message
 */
VOID sha256_output(const UINT32 hashValue[], UCHAR out[])
{
	UINT i;
	
	/* Return message digest, transform the UINT32 hash value to bytes */
	for (i=0; i < 8;i++) {
		out[i*4]= hashValue[i] >> 24;
		out[i*4 + 1]= hashValue[i] >> 16;
		out[i*4 + 2]= hashValue[i] >> 8;
		out[i*4 + 3]=	hashValue[i];
	} //end for	
} //end sha256_output()


/* SHA256 algorithms 
 * @out: digest message
 * @data: message context
 * @data_len: the length of message in bytes
 * @append_len: append length in end of message, 0:not append
 */
VOID sha256_hash(UINT32 hashValue[], const UCHAR data[], UINT data_len, UINT append_len)
//void sha256_hash(UINT32 hashValue[], const UCHAR data[], UINT data_len, UINT isAppend)
{
	UINT32 i,j,N,t,tmp;
	UINT32 block[16];
	UINT64 len64,front = 0;
	UINT32 a,b,c,d,e,f,g,h,T1,T2;


	/* Calculate the number of block */
	/* need append 1 bits to end of the message (1 byte),  64 bits express the message length (8 bytes)*/
	N = data_len / SHA256_BLOCK_SIZE;
	if (append_len)	{
		if ((data_len % SHA256_BLOCK_SIZE) < 56) {
			N += 1;
		} else {
			N += 2;
		} //end if
	} else {
		if ((data_len % SHA256_BLOCK_SIZE) != 0) {
			N += 1;
		} //end if 
	} //end if

	for (i = 1; i <= N; i++) {
		/* SHA256 preprocessing */
		if (front > data_len) {
			memset(block, 0, SHA256_BLOCK_SIZE);
		} else if ((data_len - front) < SHA256_BLOCK_SIZE) {
			memset(block, 0, SHA256_BLOCK_SIZE);
			memcpy(block, data + front, (data_len - front));

			// Little Endian Swap
			for (j=0;j<16;j++) {
				block[j] = cpu2be32(block[j]);
			} //end for

			// append 1 bits to end of the message
			if (append_len) {
				tmp = data_len - front;
				block[(tmp/4)] = (block[(tmp/4)] | (0x80000000 >> (tmp % 4)*8));
			} //end if
		} else {
			memcpy(block,data + front, SHA256_BLOCK_SIZE);
			// Little Endian Swap
			for (j=0;j<16;j++) {
				block[j] = cpu2be32(block[j]);
			} //end for
		} //end if		

		// Append the length of message in rightmost 64 bits
		if ((i == N) && (append_len)) {
			len64 = (UINT64) append_len*8;
			block[14] = SHR(len64,32);
			block[15] = len64;
		} //end if

		/* SHA256 hash computation */
		// Initialize the working variables
		a = hashValue[0];
		b = hashValue[1];
		c = hashValue[2];
		d = hashValue[3];
		e = hashValue[4];
		f = hashValue[5];
		g = hashValue[6];
		h = hashValue[7];

		// 64 rounds
		for (t=0;t<64;t++) {
			if (t > 15) {
				// Prepare the message schedule, {W_i}
				tmp = t % 16;
				block[tmp] = Sigma_256_1(block[(tmp + 14) & 15]) + block[(tmp + 9) & 15] + Sigma_256_0(block[(tmp + 1) & 15]) + block[tmp];
			} //end if
			T1 = h + Zsigma_256_1(e) + Ch(e,f,g) + K_256[t] + block[t % 16];
			T2 = Zsigma_256_0(a) + Maj(a,b,c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;						
		} //end for t
			
		// Compute the i^th intermediate hash value H^(i)
		hashValue[0] += a;
		hashValue[1] += b;
		hashValue[2] += c;
		hashValue[3] += d;
		hashValue[4] += e;
		hashValue[5] += f;
		hashValue[6] += g;
		hashValue[7] += h;

		front += SHA256_BLOCK_SIZE;
	} //end for N

} //end sha256_hash()


VOID sha256(UCHAR out[], const UCHAR data[], UINT data_len)
{
	UINT32 hashValue[8];

	sha256_init(hashValue);
	sha256_hash(hashValue,data,data_len,data_len);
	sha256_output(hashValue,out);
} //end sha256();

/**
 * hmac_sha256 - HMAC using SHA256 hash function
 * @key: secret key
 * @key_len: the length of the key in bytes
 * @data: message context
 * @data_len: the length of the message in bytes
 * @mac: message authentication code
 * @mac_len: the length of the mac in bytes
 */
VOID hmac_sha(const UCHAR key[], UINT key_len, const UCHAR data[], UINT data_len, UCHAR mac[], UINT mac_len)
{
	UCHAR K0[SHA256_BLOCK_SIZE];
	UCHAR digest[SHA256_DIGEST_SIZE];
	UINT32 hashValue[8];
	UINT i;
	//unsigned long j1,j2;
	//j1 = (unsigned long)jiffies_64;

  /*
   * If the length of K = B(Block size): K0 = K.
   * If the length of K > B: hash K to obtain an L byte string, then append (B-L) zeros to create a B-byte string K0 (i.e., K0 = H(K) || 00...00).
   * If the length of K < B: append zeros to the end of K to create a B-byte string K0
   */
	if (key_len == SHA256_BLOCK_SIZE) {
		memcpy(K0, data, key_len);
	} else if (key_len > SHA256_BLOCK_SIZE) {
		sha256_init(hashValue);
		sha256_hash(&hashValue[0], key,key_len, key_len);
		sha256_output(hashValue,K0);
		memset(K0 + SHA256_DIGEST_SIZE, 0, (SHA256_BLOCK_SIZE - SHA256_DIGEST_SIZE));
	} else if (key_len < SHA256_BLOCK_SIZE) {
		memcpy(K0, key, key_len);
		memset(K0 + key_len, 0, (SHA256_BLOCK_SIZE - key_len));		
	} //end if

	// Exclusive-Or K0 with ipad
	// ipad: Inner pad; the byte x・36・ repeated B times.
	for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
		K0[i] ^= 0x36;
  	} //end for

	// digest = H((K0^ipad) || data)
	sha256_init(hashValue);
	sha256_hash(&hashValue[0],K0,SHA256_BLOCK_SIZE,0);
	sha256_hash(&hashValue[0],data,data_len,(data_len + SHA256_BLOCK_SIZE));
	sha256_output(hashValue,digest);

	// Exclusive-Or K0 with opad and remove ipad
	// opad: Outer pad; the byte x・5c・ repeated B times.
	for (i = 0; i < SHA256_BLOCK_SIZE; i++)
		K0[i] ^= 0x36^0x5c;

	sha256_init(hashValue);
	sha256_hash(&hashValue[0],K0,SHA256_BLOCK_SIZE,0);
	sha256_hash(&hashValue[0],digest,SHA256_DIGEST_SIZE,(SHA256_DIGEST_SIZE + SHA256_BLOCK_SIZE));
	sha256_output(hashValue,mac);

	//j2 = (unsigned long)jiffies_64;
//printk(KERN_ALERT "hmac_sha256=%lu,%lu,%lu,%lu\n",j1,j2,(j2-j1),HZ);

} //end hmac_sha256()

#endif /* SHA256 */
