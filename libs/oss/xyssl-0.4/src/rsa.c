/*
 *  The RSA PK cryptosystem
 *
 *  Copyright (C) 2006  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
/*
 *  RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
 *
 *  http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
 *  http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rsa.h"

/*
 * Generate an RSA keypair
 */
int rsa_gen_key( rsa_context *ctx, int nbits, int exponent,
                 ulong (*rng_fn)(void *), void *rng_st )
{
    int ret;
    mpi P1, Q1, H, G;

    mpi_init( &P1, &Q1, &H, &G, NULL );

    memset( ctx, 0, sizeof( rsa_context ) );

    /*
     * find primes P and Q with Q < P so that:
     * GCD( E, (P-1)*(Q-1) ) == 1
     */
    CHK( mpi_lset( &ctx->E, exponent ) );

    nbits >>= 1;

    do
    {
        CHK( mpi_gen_prime( &ctx->P, nbits, 0, rng_fn, rng_st ) );
        CHK( mpi_gen_prime( &ctx->Q, nbits, 0, rng_fn, rng_st ) );

        if( mpi_cmp_mpi( &ctx->P, &ctx->Q ) < 0 )
            mpi_swap( &ctx->P, &ctx->Q );

        CHK( mpi_mul_mpi( &ctx->N, &ctx->P, &ctx->Q ) );
        CHK( mpi_sub_int( &P1, &ctx->P, 1 ) );
        CHK( mpi_sub_int( &Q1, &ctx->Q, 1 ) );
        CHK( mpi_mul_mpi( &H, &P1, &Q1 ) );
        CHK( mpi_gcd( &G, &ctx->E, &H  ) );
    }
    while( mpi_cmp_int( &G, 1 ) != 0 );

    /*
     * D  = E^-1 mod ((P-1)*(Q-1))
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    CHK( mpi_inv_mod( &ctx->D , &ctx->E, &H  ) );
    CHK( mpi_mod_mpi( &ctx->DP, &ctx->D, &P1 ) );
    CHK( mpi_mod_mpi( &ctx->DQ, &ctx->D, &Q1 ) );
    CHK( mpi_inv_mod( &ctx->QP, &ctx->Q, &ctx->P ) );

    ctx->len = ( mpi_size( &ctx->N ) + 7 ) >> 3;

cleanup:

    mpi_free( &P1, &Q1, &H, &G, NULL );

    if( ret != 0 )
    {
        rsa_free( ctx );
        return( ERR_RSA_KEYGEN_FAILED | ret );
    }

    return( 0 );   
}

/*
 * Perform an RSA public key operation
 */
int rsa_public( rsa_context *ctx, uchar *input,  int ilen,
                                  uchar *output, int olen )
{
    int ret;
    mpi T;

    if( ilen != ctx->len || olen != ctx->len )
        return( ERR_RSA_PUBLIC_FAILED );

    mpi_init( &T, NULL );

    CHK( mpi_import( &T, input, ilen ) );

    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        mpi_free( &T, NULL );
        return( ERR_RSA_PUBLIC_FAILED );
    }

    CHK( mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    CHK( mpi_export( &T, output, &olen ) );

cleanup:

    mpi_free( &T, NULL );

    if( ret != 0 )
        return( ERR_RSA_PUBLIC_FAILED | ret );

    return( 0 );
}

/*
 * Perform an RSA private key operation
 */
int rsa_private( rsa_context *ctx, uchar *input,  int ilen,
                                   uchar *output, int olen )
{
    int ret;
    mpi T, T1, T2;

    if( ilen != ctx->len || olen != ctx->len )
        return( ERR_RSA_PRIVATE_FAILED );

    mpi_init( &T, &T1, &T2, NULL );

    CHK( mpi_import( &T, input, ilen ) );

    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        mpi_free( &T, NULL );
        return( ERR_RSA_PRIVATE_FAILED );
    }

#if 0
    CHK( mpi_exp_mod( &T, &T, &ctx->D, &ctx->N, &ctx->RN ) );
#else
    /*
     * faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    CHK( mpi_exp_mod( &T1, &T, &ctx->DP, &ctx->P, &ctx->RP ) );
    CHK( mpi_exp_mod( &T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    CHK( mpi_sub_mpi( &T, &T1, &T2 ) );
    CHK( mpi_mul_mpi( &T1, &T, &ctx->QP ) );
    CHK( mpi_mod_mpi( &T, &T1, &ctx->P ) );

    /*
     * output = T2 + T * Q
     */
    CHK( mpi_mul_mpi( &T1, &T, &ctx->Q ) );
    CHK( mpi_add_mpi( &T, &T2, &T1 ) );
#endif

    CHK( mpi_export( &T, output, &olen ) );

cleanup:

    mpi_free( &T, &T1, &T2, NULL );

    if( ret != 0 )
        return( ERR_RSA_PRIVATE_FAILED | ret );

    return( 0 );
}

/*
 * Return 0 if the public key is valid or ERR_RSA_KEY_CHECK_FAILED
 */
int rsa_check_pubkey( rsa_context *ctx )
{
    if( ( ctx->N.p[0] & 1 ) == 0 || 
        ( ctx->E.p[0] & 1 ) == 0 )
        return( ERR_RSA_KEY_CHECK_FAILED );

    if( mpi_size( &ctx->N ) < 128 ||
        mpi_size( &ctx->N ) > 4096 )
        return( ERR_RSA_KEY_CHECK_FAILED );

    if( mpi_size( &ctx->E ) < 2 ||
        mpi_size( &ctx->E ) > 64 )
        return( ERR_RSA_KEY_CHECK_FAILED );

    return( 0 );
}

/*
 * Return 0 if the private key is valid or ERR_RSA_KEY_CHECK_FAILED
 */
int rsa_check_privkey( rsa_context *ctx )
{
    int ret = 0;
    mpi TN, P1, Q1, H, G;

    mpi_init( &TN, &P1, &Q1, &H, &G, NULL );

    CHK( mpi_mul_mpi( &TN, &ctx->P, &ctx->Q ) );
    CHK( mpi_sub_int( &P1, &ctx->P, 1 ) );
    CHK( mpi_sub_int( &Q1, &ctx->Q, 1 ) );
    CHK( mpi_mul_mpi( &H, &P1, &Q1 ) );
    CHK( mpi_gcd( &G, &ctx->E, &H  ) );

    if( mpi_cmp_mpi( &TN, &ctx->N ) == 0 &&
        mpi_cmp_int( &G, 1 ) == 0 )
    {
        mpi_free( &TN, &P1, &Q1, &H, &G, NULL );
        return( 0 );
    }

cleanup:

    mpi_free( &TN, &P1, &Q1, &H, &G, NULL );
    return( ERR_RSA_KEY_CHECK_FAILED | ret );
}

/*
 * Add the PKCS1 v1.5 padding and do a public RSA
 */
int rsa_pkcs1_encrypt( rsa_context *ctx,
                       uchar *input,  int ilen,
                       uchar *output, int olen )
{
    int nb_pad;
    uchar *p = output;

    if( olen != ctx->len || olen < ilen + 11 )
        return( ERR_RSA_ENCRYPT_FAILED );

    nb_pad = olen - 3 - ilen;

    *p++ = 0;
    *p++ = RSA_CRYPT;

    while( nb_pad-- > 0 )
    {
        do { *p = (uchar) rand(); } while( *p == 0 );
        p++;
    }

    *p++ = 0;
    memcpy( p, input, ilen );

    if( rsa_public( ctx, output, olen, output, olen ) != 0 )
        return( ERR_RSA_ENCRYPT_FAILED );

    return( 0 );
}

/*
 * Do a private RSA and remove the PKCS1 v1.5 padding
 */
int rsa_pkcs1_decrypt( rsa_context *ctx,
                       uchar *input,  int  ilen,
                       uchar *output, int *olen )
{
    uchar *p, tmp[512];

    if( ilen != ctx->len || ilen < 48 || ilen > 512 )
        return( ERR_RSA_DECRYPT_FAILED );

    if( rsa_private( ctx, input, ilen, tmp, ilen ) != 0 )
        return( ERR_RSA_DECRYPT_FAILED );

    p = tmp;

    if( *p++ != 0 || *p++ != RSA_CRYPT )
        return( ERR_RSA_DECRYPT_FAILED );

    while( *p != 0 )
    {
        if( p >= tmp + ilen - 1 )
            return( ERR_RSA_DECRYPT_FAILED );
        p++;
    }
    p++;

    if( *olen < ilen - (int)(p - tmp) )
        return( ERR_RSA_DECRYPT_FAILED );

    *olen = ilen - (int)(p - tmp);
    memcpy( output, p, *olen );

    return( 0 );
}

/*
 * Perform a private RSA to sign a message digest
 */
int rsa_pkcs1_sign( rsa_context *ctx, int alg_id,
                    uchar *hash, int hashlen,
                    uchar *sig,  int siglen )
{
    int nb_pad;
    uchar *p = sig;

    if( siglen != ctx->len || siglen < 48 )
        return( ERR_RSA_SIGN_FAILED );

    switch( alg_id )
    {
        case RSA_MD2:
        case RSA_MD4:
        case RSA_MD5:
            nb_pad = siglen - 3 - 34;
            break;

        case RSA_SHA1:
            nb_pad = siglen - 3 - 35;
            break;

        case RSA_NONE:
            nb_pad = siglen - 3 - hashlen;
            break;

        default:
            return( ERR_RSA_SIGN_FAILED );
    }

    if( nb_pad < 8 )
        return( ERR_RSA_SIGN_FAILED );

    *p++ = 0;
    *p++ = RSA_SIGN;

    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    switch( alg_id )
    {
        case RSA_MD2:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 2; break;

        case RSA_MD4:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 4; break;

        case RSA_MD5:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 5; break;

        case RSA_SHA1:
            memcpy( p, ASN1_HASH_SHA1, 15 );
            memcpy( p + 15, hash, 20 );
            break;

        case RSA_NONE:
            memcpy( p, hash, hashlen );
            break;

        default:
            return( ERR_RSA_SIGN_FAILED );
    }

    if( rsa_private( ctx, sig, siglen, sig, siglen ) != 0 )
        return( ERR_RSA_SIGN_FAILED );

    return( 0 );
}

/*
 * Perform a public RSA and check the message digest
 */
int rsa_pkcs1_verify( rsa_context *ctx, int alg_id,
                      uchar *hash, int hashlen,
                      uchar *sig,  int siglen )
{
    int len;
    uchar *p, c, buf[512];

    if( siglen != ctx->len || siglen < 48 || siglen > 512 )
        return( ERR_RSA_VERIFY_FAILED );

    if( rsa_public( ctx, sig, siglen, buf, siglen ) != 0 )
        return( ERR_RSA_VERIFY_FAILED );

    p = buf;

    if( *p++ != 0 || *p++ != RSA_SIGN )
        return( ERR_RSA_VERIFY_FAILED );

    while( *p != 0 )
    {
        if( p >= buf + siglen - 1 || *p != 0xFF )
            return( ERR_RSA_VERIFY_FAILED );
        p++;
    }
    p++;

    len = siglen - (int)( p - buf );

    if( len == 34 )
    {
        c = p[13];
        p[13] = 0;

        if( memcmp( p, ASN1_HASH_MDX, 18 ) != 0 )
            return( ERR_RSA_VERIFY_FAILED );

        if( ( c == 2 && alg_id == RSA_MD2 ) ||
            ( c == 4 && alg_id == RSA_MD4 ) ||
            ( c == 5 && alg_id == RSA_MD5 ) )
        {
            if( memcmp( p + 18, hash, 16 ) == 0 ) 
                return( 0 );
        }
    }

    if( len == 35 && alg_id == RSA_SHA1 )
    {
        if( memcmp( p, ASN1_HASH_SHA1, 15 ) == 0 &&
            memcmp( p + 15, hash, 20 ) == 0 )
            return( 0 );
    }

    if( len == hashlen && alg_id == RSA_NONE )
    {
        if( memcmp( p, hash, hashlen ) == 0 )
            return( 0 );
    }

    return( ERR_RSA_VERIFY_FAILED );
}

/*
 * Free the components of an RSA key
 */
void rsa_free( rsa_context *ctx )
{
    mpi_free( &ctx->N,  &ctx->E,  &ctx->D,
              &ctx->P,  &ctx->Q,  &ctx->DP,
              &ctx->DQ, &ctx->QP, &ctx->RN,
              &ctx->RP, &ctx->RQ, NULL );
}

#ifdef SELF_TEST

#include "md5.h"

uchar plaintext[] =
    "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
    "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD";

#define PTLEN   24
#define CTLEN  128

/*
 * Checkup routine
 */
int rsa_self_test( void )
{
    int len;
    rsa_context rsa;
    uchar md5sum[16];
    uchar decrypted[PTLEN];
    uchar ciphertext[CTLEN];

    memset( &rsa, 0, sizeof( rsa ) );

    rsa.len = 128;
#if 0
    mpi_read( &rsa.N , "9292758453063D803DD603D5E777D788" \
                       "8ED1D5BF35786190FA2F23EBC0848AEA" \
                       "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                       "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                       "93A89813FBF3C4F8066D2D800F7C38A8" \
                       "1AE31942917403FF4946B0A83D3D3E05" \
                       "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                       "5E94BB77B07507233A0BC7BAC8F90F79", 16 );

    mpi_read( &rsa.E , "10001", 16 );
    mpi_read( &rsa.D , "24BF6185468786FDD303083D25E64EFC" \
                       "66CA472BC44D253102F8B4A9D3BFA750" \
                       "91386C0077937FE33FA3252D28855837" \
                       "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                       "DF79C5CE07EE72C7F123142198164234" \
                       "CABB724CF78B8173B9F880FC86322407" \
                       "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                       "071513A1E85B5DFA031F21ECAE91A34D", 16 );

    mpi_read( &rsa.P , "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                       "2C01CAD19EA484A87EA4377637E75500" \
                       "FCB2005C5C7DD6EC4AC023CDA285D796" \
                       "C3D9E75E1EFC42488BB4F1D13AC30A57", 16 );
    mpi_read( &rsa.Q , "C000DF51A7C77AE8D7C7370C1FF55B69" \
                       "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                       "910E4168387E3C30AA1E00C339A79508" \
                       "8452DD96A9A5EA5D9DCA68DA636032AF", 16 );

    mpi_read( &rsa.DP, "C1ACF567564274FB07A0BBAD5D26E298" \
                       "3C94D22288ACD763FD8E5600ED4A702D" \
                       "F84198A5F06C2E72236AE490C93F07F8" \
                       "3CC559CD27BC2D1CA488811730BB5725", 16 );
    mpi_read( &rsa.DQ, "4959CBF6F8FEF750AEE6977C155579C7" \
                       "D8AAEA56749EA28623272E4F7D0592AF" \
                       "7C1F1313CAC9471B5C523BFE592F517B" \
                       "407A1BD76C164B93DA2D32A383E58357", 16 );
    mpi_read( &rsa.QP, "9AE7FBC99546432DF71896FC239EADAE" \
                       "F38D18D2B2F0E2DD275AA977E2BF4411" \
                       "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                       "A74206CEC169D74BF5A8C50D6F48EA08", 16 );
#else
    mpi_read( &rsa.N , "EEF43DF231F4FEFDA3FF0576F864912B" \
                       "F5D51D627C5911F4794F54C8BE178C66" \
                       "FD9C447BE512735818E93CF88AB1696C" \
                       "1C634A898DBFCE384F74CD347B715419" \
                       "EAE05016842B752F127CC224535C4708" \
                       "8DE7566D50F0CFC013B2592BAB1E042A" \
                       "76239E5262D931B84BDAB640028AFE7C" \
                       "39E2B75A353EABF827854EE249C6EA45", 16 );

    mpi_read( &rsa.E , "010001", 16 );
    mpi_read( &rsa.D , "B6F6044861BFF94E34379BF3901550A2" \
                       "9C44658F772EABF4C8BDD9692B43D499" \
                       "372E63B189A02AF91579E0D95D38A243" \
                       "C928AD75CD3743AB120B98E3CA70E7B6" \
                       "C5B3C1EA2065EF5A6347F80B247044D4" \
                       "775C4379C2286F8724E0DFE859F808E8" \
                       "BFBE3D257EF84E3A455C5BC452F5600E" \
                       "5CDD62818D7E937C7D4C9819C1FAF331", 16 );

    mpi_read( &rsa.P , "FBD24AF8F6132E9E1D07B73CFD6D0ECE" \
                       "6E49DD602EF0F4D6FE6DF66493F016EA" \
                       "C19FF290749194145C3229D0CC57B31F" \
                       "199AE2819572271CFE40279063B5BEAB", 16 );
    mpi_read( &rsa.Q , "F2EB4A3E41438F2690EC2DED0198E4BD" \
                       "7ABA01D374A27C92BDAEA3803FF8584C" \
                       "2B923C95868B4C53DCEEA3A750D7B702" \
                       "748522C8BF781CCED4E76B52A9DD3ACF", 16 );

    mpi_read( &rsa.DP, "3947752C39F4D506BBFDB44D582BC551" \
                       "693EBDEF11DE5722CC0EC11BD196ABEF" \
                       "CC0910C890EB482E756627A2C9C82D03" \
                       "26F4D70EB8AA9580FFC821F7B2E6752F", 16 );
    mpi_read( &rsa.DQ, "5A71D28DC55CF322A7D8D7ECA3A89A9A" \
                       "15E4C5A3468CED16F1BAE133721DF43A" \
                       "400ACDB5DA8768DEDCA69996455A5BD0" \
                       "7533D0D4AFBD77F4667ED78DCAA30D2F", 16 );
    mpi_read( &rsa.QP, "81267EDB140CE8F07CA92F508FEA134B" \
                       "23C871D428C6EF870F08FFF2AD46D210" \
                       "8FCD67E28FF95E8E332B5EEE16EB8784" \
                       "AB3D1E59B078CB93EF5C6E0F12419439", 16 );
#endif
    printf( "  RSA key validation: " );

    if( rsa_check_pubkey(  &rsa ) != 0 ||
        rsa_check_privkey( &rsa ) != 0 )
    {
        printf( "failed\n" );
        return( 1 );
    }

    printf( "passed\n  PKCS#1 encryption : " );

    if( rsa_pkcs1_encrypt( &rsa, plaintext,  PTLEN,
                                 ciphertext, CTLEN ) != 0 )
    {
        printf( "failed\n" );
        return( 1 );
    }

    printf( "passed\n  PKCS#1 decryption : " );

    len = sizeof( decrypted );

    if( rsa_pkcs1_decrypt( &rsa, ciphertext, CTLEN,
                                 decrypted,  &len ) != 0 ||
        memcmp( decrypted, plaintext, len ) != 0 )
    {
        printf( "failed\n" );
        return( 1 );
    }

    printf( "passed\n  PKCS#1 data sign  : " );

    md5_csum( plaintext, PTLEN, md5sum );

    if( rsa_pkcs1_sign( &rsa, RSA_MD5, md5sum, 16,
                        ciphertext, CTLEN ) != 0 )
    {
        printf( "failed\n" );
        return( 1 );
    }

    printf( "passed\n  PKCS#1 sig. verify: " );

    if( rsa_pkcs1_verify( &rsa, RSA_MD5, md5sum, 16,
                          ciphertext, CTLEN ) != 0 )
    {
        printf( "failed\n" );
        return( 1 );
    }

    printf( "passed\n\n" );

    rsa_free( &rsa );
    return( 0 );
}
#else
int rsa_self_test( void )
{
    return( 0 );
}
#endif
