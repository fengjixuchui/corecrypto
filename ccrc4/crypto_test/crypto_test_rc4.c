/*
 * Copyright (c) 2012,2015,2018 Apple Inc. All rights reserved.
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

#include "testmore.h"
#include "testbyteBuffer.h"
#include <corecrypto/ccrc4.h>
#include <corecrypto/cc_runtime_config.h>


// static int verbose = 1;

#if (CCRC4_CIPHER == 0)
entryPoint(ccrc4_cipher_tests,"ccrc4 cipher")
#else

typedef struct stream_test_vector_t {
    char *keyStr;
    char *plainStr;
    char *cipherStr;
} stream_test_vector_s, *stream_test_vector;

/* some simple test vectors from wikipedia... */

stream_test_vector_s rc4_test_vectors[] = {
    { "Key", "Plaintext", "bbf316e8d940af0ad3" },
    { "Wiki", "pedia", "1021bf0420" },
    { "Secret", "Attack at dawn", "45a01f645fc35b383552544b9bf5" },
    { NULL, NULL, NULL },
};


static int test_ccrc4(const struct ccrc4_info *rc4, stream_test_vector v)
{
    ccrc4_ctx_decl(rc4->size, skey);
    size_t keylen = strlen(v->keyStr);
    size_t ptlen = strlen(v->plainStr);
    byteBuffer ct = hexStringToBytes(v->cipherStr);
    byteBuffer r = mallocByteBuffer(ptlen);
    char retStr[ptlen+1];
    
    rc4->init(skey, keylen, v->keyStr);
    rc4->crypt(skey, ptlen, v->plainStr, r->bytes);
    ok_or_fail(bytesAreEqual(ct, r), "Encrypt Results == CipherText");
    
    retStr[ptlen] = 0;
    rc4->init(skey, keylen, v->keyStr);
    rc4->crypt(skey, ct->len, ct->bytes, retStr);
    ok_or_fail(strncmp(v->plainStr, retStr, ptlen) == 0, "Decrypt Results == PlainText");
    
    free(ct);
    free(r);
    return 1;
}

static const int kTestTestCount = 9;

int ccrc4_cipher_tests(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
	plan_tests(kTestTestCount);
    
    for(int i=0; rc4_test_vectors[i].keyStr != NULL; i++) {
        ok(test_ccrc4(ccrc4(), &rc4_test_vectors[i]), "RC4 Tests OK");
    }
    return 0;
}
#endif

