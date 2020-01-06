/*
 * Copyright (c) 2012,2013,2015,2016,2017,2018 Apple Inc. All rights reserved.
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
#include <corecrypto/cc_config.h>

#if CCAES_MUX

#include "ccaes_ios_mux_cbc.h"

const struct ccmode_cbc *small_cbc_encrypt = &ccaes_arm_cbc_encrypt_mode;
const struct ccmode_cbc *large_cbc_encrypt = &ccaes_ios_hardware_cbc_encrypt_mode;

static int
ccaes_ios_mux_cbc_encrypt_init(const struct ccmode_cbc *cbc CC_UNUSED, cccbc_ctx *key,
                               size_t rawkey_len, const void *rawkey)
{
    int rc;
    
    cccbc_ctx *smallctx = key;
    cccbc_ctx *largectx = (cccbc_ctx *) ((uint8_t *)key + small_cbc_encrypt->size);
    
    rc = small_cbc_encrypt->init(small_cbc_encrypt, smallctx, rawkey_len, rawkey);
    rc |= large_cbc_encrypt->init(large_cbc_encrypt, largectx, rawkey_len, rawkey);
    
    return rc;
}

// This routine now calls the ios hardware routine directly so it can use the number of
// blocks processed in cases of failure to open the device or partial encryption.
static int
ccaes_ios_mux_cbc_encrypt(const cccbc_ctx *cbcctx, cccbc_iv *iv, size_t nblocks, const void *in, void *out)
{
    if (0 == nblocks) return 0;
    
    const cccbc_ctx *smallctx = cbcctx;
    const cccbc_ctx *largectx = (const cccbc_ctx *) ((const uint8_t *)cbcctx + small_cbc_encrypt->size);
    if((nblocks > AES_CBC_SWHW_CUTOVER)) {
        ccaes_hardware_aes_ctx_const_t ctx = (ccaes_hardware_aes_ctx_const_t) largectx;
        size_t processed = ccaes_ios_hardware_crypt(CCAES_HW_ENCRYPT, ctx, (uint8_t*)iv, in, out, nblocks);
        nblocks -= processed;
    }

    if(nblocks) {
        small_cbc_encrypt->cbc(smallctx, iv, nblocks, in, out);
    }
    
    return 0;
}


const struct ccmode_cbc *ccaes_ios_mux_cbc_encrypt_mode(void)
{
    static struct ccmode_cbc ccaes_ios_mux_cbc_encrypt_mode;

    // Check support and performance of HW
    if (!ccaes_ios_hardware_enabled(CCAES_HW_ENCRYPT|CCAES_HW_CBC)) return small_cbc_encrypt;

    ccaes_ios_mux_cbc_encrypt_mode.size = small_cbc_encrypt->size + large_cbc_encrypt->size + CCAES_BLOCK_SIZE;
    ccaes_ios_mux_cbc_encrypt_mode.block_size = CCAES_BLOCK_SIZE;
    ccaes_ios_mux_cbc_encrypt_mode.init = ccaes_ios_mux_cbc_encrypt_init;
    ccaes_ios_mux_cbc_encrypt_mode.cbc = ccaes_ios_mux_cbc_encrypt;
    ccaes_ios_mux_cbc_encrypt_mode.custom = NULL;
    return &ccaes_ios_mux_cbc_encrypt_mode;
}



#endif /* CCAES_MUX */
