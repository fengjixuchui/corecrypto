/*
 * Copyright (c) 2017,2018 Apple Inc. All rights reserved.
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
#ifndef _CORECRYPTO_FIPSPOST_TRACE_PRIV_H_
#define _CORECRYPTO_FIPSPOST_TRACE_PRIV_H_

/*
 * FIPSPOST_TRACE writes, using a supplied fipspost_trace_writer function, a
 * buffer in three steps.  The first is the fipspost_trace_hdr, which includes
 * contextual information such as the file version, the fips_mode, the hmac
 * that's associated with this file, and any compilation flags as are
 * considered important.
 *
 * Subsequently are a long stream of fipspost_trace_id_t elements, each of
 * which represents a single trace event.
 *
 * Finally, a FIPSPOST_TRACE_TABLE_ID event is written, and the string table
 * that was constructued is dumped in pascal format (length+string).
 * Currently, all of the strings are NULL terminated.
 */

/* Current version produced by this file. */
#define FIPSPOST_TRACE_PROTOCOL_VERSION 1

/* Magic to start each fipspost trace file with. */
#define FIPSPOST_TRACE_MAGIC            0x66707472

/* Some flags to set to help further identify the system. */
#define FIPSPOST_TRACE_SYSFLAG_IPHONE   (1 << 0)
#define FIPSPOST_TRACE_SYSFLAG_OSX      (1 << 1)
#define FIPSPOST_TRACE_SYSFLAG_L4       (1 << 2)
#define FIPSPOST_TRACE_SYSFLAG_KERNEL   (1 << 3)

/* Header prepended onto each file. */
struct fipspost_trace_hdr {
    uint32_t magic;
    uint32_t version;
    uint32_t fips_mode;
    uint8_t  integ_hmac[FIPSPOST_PRECALC_HMAC_SIZE];
    uint64_t system_flags;
} __attribute__((packed));

/*
 * After the header are the trace events, each of which is a
 * fipspost_trace_id_t actings as an index to a string table which is sent
 * after the events.
 *
 * An indiviual trace event is an index into the string table that's persisted
 * until the fipspost_trace_end call.
 */
typedef uint8_t fipspost_trace_id_t;

/*
 * Supplied writer function that returns 0 on success.
 *
 * Failure results in fipspost_trace_clear being called (disabling further
 * tracing). No expected action taken by the caller.
 */
typedef int (*fipspost_trace_writer_t)(void *ctx, const uint8_t *buf, size_t len);

/*
 * Maximum length of an event string.
 */
#define FIPSPOST_TRACE_MAX_EVENT_LEN 0xFF

/*
 * Maximum number of unique functions that can be recorded, with space for
 * additional flags before the maxint of fipspost_trace_id_t.
 */
#define FIPSPOST_TRACE_MAX_HOOKS    0xF0    // uint8_t size limitation

#define FIPSPOST_TRACE_SUCCESS_STR "-POST_SUCCESS"
#define FIPSPOST_TRACE_FAILURE_STR "-POST_FAILURE: 0x"

/*
 * Reserve a value for dumping the trace_hooks table after the samples.
 */
#define FIPSPOST_TRACE_TABLE_ID     (fipspost_trace_id_t)(FIPSPOST_TRACE_MAX_HOOKS + 1)

/*
 * Maximum number of trace events to record.
 */
#define FIPSPOST_TRACE_MAX_EVENTS   65535

/*
 * Maximum size of the result buffer:
 *   struct fipspost_trace_hdr
 *   FIPSPOST_TRACE_MAX_EVENTS * fipspost_trace_id_t
 *   FIPSPOST_TRACE_MAX_HOOKS * (FIPSPOST_TRACE_MAX_EVENT_LEN + 1 (for the length))
 */
#define FIPSPOST_TRACE_MAX_BUFFER                                       \
    (sizeof(struct fipspost_trace_hdr) +                                \
    (FIPSPOST_TRACE_MAX_EVENTS * sizeof(fipspost_trace_id_t)) +         \
    (FIPSPOST_TRACE_MAX_HOOKS * (FIPSPOST_TRACE_MAX_EVENT_LEN + 1)))

/*
 * Certain users may desire the types and constants for parsing, but not
 * require the symbols to execute tracing events.
 */

#if CC_FIPSPOST_TRACE
/* Start the tracing. */
int fipspost_trace_start(uint32_t fips_mode,
        fipspost_trace_writer_t trace_writer, void *ctx);

/*
 * Finish the tracing; returns 0 if successful, or -1 if no valid output was
 * collected.
 */
int fipspost_trace_end(uint32_t result);

/* Reset back to defaults. */
void fipspost_trace_clear(void);
#endif

/*
 * Provide a lookup table to allow an app or kext to link against both
 * a dylib/kernel providing POST trace capabilities and a version that does
 * not.
 *
 * This allows the fipscavs kext to link against a non-development kernel.
 */
typedef struct {
    int (*fipspost_trace_start)(uint32_t, fipspost_trace_writer_t, void *);
    int (*fipspost_trace_end)(uint32_t result);
    void (*fipspost_trace_clear)(void);
} fipspost_trace_vtable_t;

extern fipspost_trace_vtable_t fipspost_trace_vtable;

#endif
