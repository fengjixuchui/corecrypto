/*
 * Copyright (c) 2014,2015,2016,2017,2018 Apple Inc. All rights reserved.
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

#include <corecrypto/ccec_priv.h>
#include "cc_debug.h"
#include <corecrypto/cc_macros.h>

int ccec_verify_internal(ccec_pub_ctx_t key, size_t digest_len, const uint8_t *digest,
                const cc_unit *r, const cc_unit *s, bool *valid)
{
    ccec_const_cp_t cp = ccec_ctx_cp(key);
    cczp_const_t zq= ccec_cp_zq(cp);
    cc_size n=ccec_cp_n(cp);
    int result=-1;
    *valid = false;
    cc_unit e[n], w[n], d0[n], d1[n];
    cc_size e_n=ccn_nof_size(digest_len);
    size_t qbitlen=ccec_cp_order_bitlen(cp);
    ccec_point_decl_cp(cp, mg);
    ccec_point_decl_cp(cp, mk);

    /* check r and s for sanity */
    if (ccn_is_zero(n, r) || ccn_is_zero(n, s) || ccn_cmp(n, r, cczp_prime(zq)) != -1 || ccn_cmp(n, s, cczp_prime(zq)) != -1) {
        result = -1; /* Signature packet invalid. */
        goto errOut;
    }

    if (digest_len*8>qbitlen) {
        /* Case where the hash size is bigger than the curve size
         eg. SHA384 with P-256 */
        cc_unit e_big[e_n];
        cc_require(((result = ccn_read_uint(e_n, e_big, digest_len, digest)) >= 0),errOut);

        /* Keep the leftmost bits of the hash */
        ccn_shift_right_multi(e_n,e_big,e_big,(digest_len*8-qbitlen));
        ccn_set(n,e,e_big);
    }
    else {
        cc_require(((result = ccn_read_uint(n, e, digest_len, digest)) >= 0),errOut);
    }
    cczp_modn(zq,e,n,e);

    cc_require_action(cczp_inv_odd(zq, w, s)==0,errOut,result=-2);  // w  = s^-1 mod q
    cczp_mul(zq, d0, e, w);                               // d0 = ew mod q
    cczp_mul(zq, d1, r, w);                               // d1 = rw mod q

    // The key may be equal to the base point
    ccec_projective_point_t pub_key_point=ccec_ctx_point(key);

    /* Key point needs to be affine */
    cc_require_action(ccn_is_one(n, ccec_const_point_z(pub_key_point,cp)),errOut,result=-2);

#if CCEC_USE_TWIN_MULT
    ccec_const_affine_point_t curve_base_point=ccec_cp_g(cp);

    // Verifies whether P= +/- G since ccec_twin_mult can't apply then.
    if (ccn_cmp(n,ccec_const_point_x(curve_base_point, cp),ccec_const_point_x(pub_key_point,cp))!=0)
    {
        ccec_projectify(cp, mg, ccec_cp_g(cp),NULL);
        ccec_projectify(cp, mk, (ccec_const_affine_point_t)pub_key_point,NULL);
        cc_require_action(ccec_is_point_projective(cp, mk),errOut,result=-1);
        cc_require((result=ccec_twin_mult(cp, mg, d0, mg, d1, mk))==0,errOut);
    }
    else
#endif // CCEC_USE_TWIN_MULT
    {
        ccec_projectify(cp, mg, ccec_cp_g(cp),NULL);
        ccec_projectify(cp, mk, (ccec_const_affine_point_t)pub_key_point,NULL);
        ccec_point_decl_cp(cp, tp);
        cc_require_action(ccec_is_point_projective(cp, mk),errOut,result=-1);
        cc_require_action(ccec_mult(cp, tp, d0, mg, NULL)==0,errOut,result=-1);
        cc_require_action(ccec_mult(cp, mg, d1, mk, NULL)==0,errOut,result=-1);
        CC_DECL_WORKSPACE_OR_FAIL(ws,CCEC_ADD_SUB_WORKSPACE_SIZE(n));
        ccec_full_add_ws(ws,cp, mg, mg, tp);
        CC_CLEAR_AND_FREE_WORKSPACE(ws);
    }

    // convert to affine coordinate
    cc_require_action(ccec_affinify_x_only(cp, ccec_point_x(mg, cp), mg,0)==0,errOut,result=-1);

    // x mod q
    if (ccn_cmp(n, ccec_point_x(mg, cp), cczp_prime(zq)) >= 0) {
        ccn_sub(n, ccec_point_x(mg, cp), ccec_point_x(mg, cp), cczp_prime(zq));
    }

    // Final comparison
    if (ccn_cmp(n, ccec_point_x(mg, cp), r) == 0) {
        *valid = true;
    }
    
    result = 0;

errOut:
    ccec_point_clear_cp(cp, mg);
    return result;
}
