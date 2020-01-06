/*
 * Copyright (c) 2011,2012,2014,2015,2016,2018 Apple Inc. All rights reserved.
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

#include <corecrypto/cczp.h>
#include <corecrypto/cczp_priv.h>
#include "crypto_test_cczp_inv.h"

// Reference implementations for modular inverse. They are for test purpose only.
// Do not use them in corecrypto, because they are slow.
int cczp_inv_slow(cczp_const_t zp, cc_unit *r, const cc_unit *d) {
    cc_size nd = cczp_n(zp);
    cczp_inv_slown(zp, r, nd, d);

    return 0;
}

/* Given d, p coprime with d > 0, cczp_prime(zp) > 1, this routine forces
   r := d^(-1) mod cczp_prime(zp).   */
void cczp_inv_slown(cczp_const_t zp, cc_unit *r, const cc_size nd, const cc_unit *d) {

    cc_size n = cczp_n(zp);
    cc_unit U[n];
    cc_unit V[n];

    cc_unit a[n], q[n], t2n[2 * n];
    cc_unit *u = U;
    cc_unit *v = V;
	unsigned int k = 0;
    assert(nd <= 2*n);

	/* 1) (u, v, a, r, k) = (d mod p, p, 0, 1, 0); */
    ccn_zero(2*n-nd, t2n + nd);
    ccn_set(nd, t2n, d);
    cczp_modn(zp, u,2*n,t2n);
    ccn_set(n, v, cczp_prime(zp));
    ccn_zero(n, a);
    ccn_seti(n, r, 1);

	/* 2) while(u > 1) {
     q = v div u;
     (u, v, a, x, k) = (v mod u, u, x, (a + q*x) mod y, k + 1);
     }
     */
    ccn_zero(n, t2n + n);
	while (!ccn_is_zero(n, u) && !ccn_is_one(n, u)) {
		/* (q,u) := (v div u, v mod u) */
        ccn_set(n, t2n, v);
        ccn_div_euclid(n, q, n, v, 2*n, t2n, n, u);
        CC_SWAP(u, v);

		/* calc new r := a + q*r mod p */
        cczp_mul(zp,t2n,q,r);
        cc_unit carry = ccn_add(n, t2n, a, t2n);
		/* new a := old r */
        ccn_set(n, a, r);
        ccn_zero(n, t2n + n);
        ccn_sub(n, r, t2n, cc_muxp(carry!=0, cczp_prime(zp), t2n+n)); // Set r <- t2n
		k++;
    }

	/* 3) if (k odd) r = p - r;    // Unsigned (normal_subg) subtract. */
	if (k & 1) {
        ccn_sub(n, r, cczp_prime(zp), r);
	}
}
