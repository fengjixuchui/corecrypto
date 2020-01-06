/*
 * Copyright (c) 2011,2012,2013,2015,2016,2017,2018 Apple Inc. All rights reserved.
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

#ifndef _CCMODE_TEST_H_
#define _CCMODE_TEST_H_

int ccmode_ecb_test_one(const struct ccmode_ecb *ecb, size_t keylen, const void *keydata,
                        size_t nblocks, const void *in, const void *out);

int ccmode_ecb_test_key_self(const struct ccmode_ecb *encrypt, const struct ccmode_ecb *decrypt, size_t nblocks,
                             size_t keylen, const void *keydata, size_t loops);

int ccmode_cbc_test_one(const struct ccmode_cbc *cbc, size_t keylen, const void *keydata,
                        const void *iv, size_t nblocks, const void *in, const void *out);

int ccmode_cbc_test_one_chained(const struct ccmode_cbc *cbc, size_t keylen, const void *keydata,
                                const void *iv, size_t nblocks, const void *in, const void *out);

int ccmode_cbc_test_key_self(const struct ccmode_cbc *encrypt, const struct ccmode_cbc *decrypt, size_t nblocks,
                             size_t keylen, const void *keydata, size_t loops);

int ccmode_cbc_test_chaining_self(const struct ccmode_cbc *encrypt, const struct ccmode_cbc *decrypt, size_t nblocks,
                                  size_t keylen, const void *keydata, size_t loops);

int ccmode_ofb_test_one(const struct ccmode_ofb *ofb, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_ofb_test_one_chained(const struct ccmode_ofb *ofb, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_cfb_test_one(const struct ccmode_cfb *cfb, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_cfb_test_one_chained(const struct ccmode_cfb *cfb, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_cfb8_test_one(const struct ccmode_cfb8 *cfb8, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_cfb8_test_one_chained(const struct ccmode_cfb8 *cfb8, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_ctr_test_one(const struct ccmode_ctr *ctr, size_t keylen, const void *keydata,
                        const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_ctr_test_one_chained(const struct ccmode_ctr *ctr, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_ctr_test_one_chained2(const struct ccmode_ctr *ctr, size_t keylen, const void *keydata,
                                const void *iv, size_t nbytes, const void *in, const void *out);

int ccmode_xts_test_one(const struct ccmode_xts *xts, size_t key_nbytes,
                        const void *dkey, const void *tkey, const void *tweak,
                        size_t nbytes, const void *in, void *out, int dec);

int ccmode_xts_test_one_chained(const struct ccmode_xts *xts,
                                size_t key_nbytes, const void *dkey,
                                const void *tkey, const void *tweak,
                                size_t nbytes, const void *in,
                                void *out, int dec);

int ccmode_gcm_test_one(const struct ccmode_gcm *gcm, size_t keylen, const void *keydata,
                        size_t ivlen, const void *iv, size_t adalen, const void *ada,
                        size_t nbytes, const void *in, const void *out,
                        size_t taglen, const void *tag);

int ccmode_gcm_test_one_chained(const struct ccmode_gcm *gcm, size_t keylen, const void *keydata,
                                size_t ivlen, const void *iv, size_t adalen, const void *ada,
                                size_t nbytes, const void *in, const void *out,
                                size_t taglen, const void *tag);


struct ccmode_ecb_vector {
    size_t keylen;
    const void *key;
    size_t nblocks;
    const void *pt;
    const void *ct;
};

int ccmode_ecb_test_one_vector(const struct ccmode_ecb *ecb, const struct ccmode_ecb_vector *v, int dec);


struct ccmode_cbc_vector {
    size_t keylen;
    const void *key;
    const char *iv;
    size_t nblocks;
    const char *pt;
    const char *ct;
};

int ccmode_cbc_test_one_vector(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec);
int ccmode_cbc_test_one_vector_unaligned(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec);
int ccmode_cbc_test_one_vector_chained(const struct ccmode_cbc *cbc, const struct ccmode_cbc_vector *v, int dec);

struct ccmode_ofb_vector {
    size_t keylen;
    const void *key;
    const char *iv;
    size_t nbytes;
    const char *pt;
    const char *ct;
};

int ccmode_ofb_test_one_vector(const struct ccmode_ofb *ofb, const struct ccmode_ofb_vector *v, int dec);

int ccmode_ofb_test_one_vector_chained(const struct ccmode_ofb *ofb, const struct ccmode_ofb_vector *v, int dec);

struct ccmode_cfb_vector {
    size_t keylen;
    const void *key;
    const char *iv;
    size_t nbytes;
    const char *pt;
    const char *ct;
};

int ccmode_cfb_test_one_vector(const struct ccmode_cfb *cfb, const struct ccmode_cfb_vector *v, int dec);

int ccmode_cfb_test_one_vector_chained(const struct ccmode_cfb *cfb, const struct ccmode_cfb_vector *v, int dec);

struct ccmode_cfb8_vector {
    size_t keylen;
    const void *key;
    const char *iv;
    size_t nbytes;
    const char *pt;
    const char *ct;
};

int ccmode_cfb8_test_one_vector(const struct ccmode_cfb8 *cfb8, const struct ccmode_cfb8_vector *v, int dec);

int ccmode_cfb8_test_one_vector_chained(const struct ccmode_cfb8 *cfb8, const struct ccmode_cfb8_vector *v, int dec);

struct ccmode_xts_vector {
    size_t keylen;
    const void *dkey; /* keylen sized */
    const void *tkey; /* keylen sized */
    const char *tweak; /* 16 bytes */
    size_t nbytes;
    const char *pt;    /* nbytes sized */
    const char *ct;    /* nbytes sized */
};

int ccmode_xts_test_one_vector(const struct ccmode_xts *xts,
                               const struct ccmode_xts_vector *v, void *out,
                               int dec);

int ccmode_xts_test_one_vector_chained(const struct ccmode_xts *xts,
                                       const struct ccmode_xts_vector *v,
                                       void *out, int dec);

struct ccmode_gcm_vector {
    size_t keylen;
    const void *key;
    size_t ivlen;
    const char *iv;
    size_t ptlen;
    const char *pt;
    size_t adalen;
    const char *ada;
    size_t ctlen;
    const char *ct;
    size_t taglen;
    const char *tag;
};

int ccmode_gcm_test_one_vector(const struct ccmode_gcm *gcm, const struct ccmode_gcm_vector *v, int dec);

int ccmode_gcm_test_one_vector_chained(const struct ccmode_gcm *gcm, const struct ccmode_gcm_vector *v, int dec);

struct ccmode_ccm_vector {
    size_t keylen;
    const void *key;
    unsigned noncelen;
    const char *nonce;
    size_t ptlen;
    const char *pt;
    size_t adalen;
    const char *ada;
    size_t ctlen;
    const char *ct;
};

int ccmode_ccm_test_one(const struct ccmode_ccm *ccm, size_t keylen, const void *keydata,
                        unsigned nonce_len, const void *nonce, size_t adalen, const void *ada,
                        size_t nbytes, const void *in, const void *out,
                        unsigned mac_size, const void *mac, int chained);
int ccmode_ccm_test_one_vector(const struct ccmode_ccm *ccm,
                                const struct ccmode_ccm_vector *v, int dec, int chained);


#endif /* _CCMODE_TEST_H_ */
