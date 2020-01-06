/*
 * Copyright (c) 2010,2012,2015,2016,2018 Apple Inc. All rights reserved.
 *
 * corecrypto Internal Use License Agreement
 *
 * IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
 * in consideration of your agreement to the following terms, and your download or use
 * of this Apple software constitutes acceptance of these terms.  If you do not agree
 * with these terms, please do not download or use this Apple software.
 *
 * 1.    As used in this Agreement, the term "Apple Software" collectively means and
 * includes all of the Apple corecrypto materials provided by Apple here, including
 * but not limited to the Apple corecrypto software, frameworks, libraries, documentation
 * and other Apple-created materials. In consideration of your agreement to abide by the
 * following terms, conditioned upon your compliance with these terms and subject to
 * these terms, Apple grants you, for a period of ninety (90) days from the date you
 * download the Apple Software, a limited, non-exclusive, non-sublicensable license
 * under Apple’s copyrights in the Apple Software to make a reasonable number of copies
 * of, compile, and run the Apple Software internally within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software; provided
 * that you must retain this notice and the following text and disclaimers in all
 * copies of the Apple Software that you make. You may not, directly or indirectly,
 * redistribute the Apple Software or any portions thereof. The Apple Software is only
 * licensed and intended for use as expressly stated above and may not be used for other
 * purposes or in other contexts without Apple's prior written permission.  Except as
 * expressly stated in this notice, no other rights or licenses, express or implied, are
 * granted by Apple herein.
 *
 * 2.    The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
 * OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 * THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
 * SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
 * SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
 * OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
 * WILL CREATE A WARRANTY.
 *
 * 3.    IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
 * IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
 * SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * 4.    This Agreement is effective until terminated. Your rights under this Agreement will
 * terminate automatically without notice from Apple if you fail to comply with any term(s)
 * of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
 * and destroy all copies, full or partial, of the Apple Software. This Agreement will be
 * governed and construed in accordance with the laws of the State of California, without
 * regard to its choice of law rules.
 *
 * You may report security issues about Apple products to product-security@apple.com,
 * as described here:  https://www.apple.com/support/security/.  Non-security bugs and
 * enhancement requests can be made via https://bugreport.apple.com as described
 * here: https://developer.apple.com/bug-reporting/
 *
 * EA1350
 * 10/5/15
 */

/*
 * Parts of this code adapted from LibTomCrypt
 *
 * LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

#include <corecrypto/ccdes.h>
#include "ltc_des.h"

/*!
 Initialize the 3LTC_DES-EDE block cipher
 @param key The symmetric key you wish to pass
 @param keylen The key length in bytes
 @param num_rounds The number of rounds desired (0 for default)
 @param skey The key in as scheduled by this function.
 @return CRYPT_OK if successful
 */
static int ltc_des3_setup(const unsigned char *key, size_t keylen, int num_rounds,
                        ccecb_ctx * skey)
{
    ltc_des3_keysched *des3;

    des3 = (ltc_des3_keysched *)skey;

    if(num_rounds != 0 && num_rounds != 16) {
        return -1; /* CRYPT_INVALID_ROUNDS; */
    }

    if (keylen != 24) {
        return -1; /* CRYPT_INVALID_KEYSIZE; */
    }

    deskey(key,    EN0, des3->ek[0]);
    deskey(key+8,  DE1, des3->ek[1]);
    deskey(key+16, EN0, des3->ek[2]);

    deskey(key,    DE1, des3->dk[2]);
    deskey(key+8,  EN0, des3->dk[1]);
    deskey(key+16, DE1, des3->dk[0]);
    
    /* raise an error if keys are not distinct */
    /* nevertheless, the cipher is initialized */
    /* to avoid breaking clients */
    if (!cc_cmp_safe(8, key, key+8) ||
        !cc_cmp_safe(8, key, key+16) ||
        !cc_cmp_safe(8, key+8, key+16)) {
        return -1;
    }

    return 0; /* CRYPT_OK; */
}

int ccdes3_ltc_setup(const struct ccmode_ecb *ecb CC_UNUSED, ccecb_ctx *key,
                     size_t rawkey_len, const void *rawkey)
{
    return ltc_des3_setup(rawkey, rawkey_len, 0, key);
}

/* 168 bits keys with no parity bits (21 bytes) */
int ccdes168_ltc_setup(const struct ccmode_ecb *ecb CC_UNUSED, ccecb_ctx *key,
                       size_t rawkey_len CC_UNUSED, const void *rawkey)
{
    unsigned char key2[24];
    const unsigned char *key1;
    unsigned char k;
    int i, j, b;

#define MASK(_i)  ((1<<(8-(_i)))-1)
#define SHIFT_L(_i) (_i)
#define SHIFT_R(_i) (8-(_i))
#define BITS_L(x,_i) (((x)<<SHIFT_L(_i)))
#define BITS_R(x,_i) (((x)>>SHIFT_R(_i)))

    key1=rawkey;
    j=0;
    b=0;
    for(i=0; i<24; i++) {
        k=(unsigned char)BITS_L(key1[j],b);
        if(b>1) {
            k&=~MASK(b);
            k|=BITS_R(key1[j+1],b)&MASK(b);
        }
        b+=7;
        if(b>=8) {
            b-=8;
            j++;
        }
        key2[i]=k;
    }

    return ltc_des3_setup(key2, sizeof(key2), 0, key);
}
