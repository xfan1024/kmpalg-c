/* 
 * MIT License
 * 
 * Copyright (c) 2018 xiaofan <xfan1024@live.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __KMPALG_H__
#define __KMPALG_H__

#include <stdio.h> // for NULL

#ifdef KMPALG_CONFIG_FAILURE_TYPE
typedef KMPALG_CONFIG_FAILURE_TYPE kmp_state_t;
#else
typedef unsigned char kmp_state_t;
#endif

#ifndef KMPALG_CONFIG_CHECK_LENGTH
#define KMPALG_CONFIG_CHECK_LENGTH 1
#endif

#ifndef KMPALG_CONFIG_CHECK_ASSUME_NO_BACKTRACK
#define KMPALG_CONFIG_CHECK_ASSUME_NO_BACKTRACK 1
#endif

#ifndef KMPALG_CONFIG_IGNORE_CASE
#define KMPALG_CONFIG_IGNORE_CASE 0
#endif

#if KMPALG_CONFIG_IGNORE_CASE
#include <ctype.h> // for toupper
#define KMPALG_CONFIG_COMPARER(x, y) (toupper(x) == toupper(y) )
#endif

#ifndef KMPALG_CONFIG_COMPARER
#define KMPALG_CONFIG_COMPARER(x, y) ((x) == (y))
#endif

#if KMPALG_CONFIG_CHECK_LENGTH
#include <stddef.h> // for size_t
typedef size_t kmp_param_length_t;
#else
typedef kmp_state_t kmp_param_length_t;
#endif

#if KMPALG_CONFIG_CHECK_ASSUME_NO_BACKTRACK || KMPALG_CONFIG_CHECK_LENGTH
#include <assert.h>
#endif

#define kmp_state_t_max ((kmp_state_t)(-1LL))
#if defined(__GNUC__) || defined(__GNUG__) || defined(__CC_ARM)
#define kmp_static_assert(cond) (void)sizeof(char[1-2*!(cond)])
#else
#define kmp_static_assert(cond) ((void)0) // some compiler don't support this feature
#endif
typedef struct
{
    const unsigned char* pattern;
    kmp_state_t *       failure;
    kmp_state_t         length;
} kmp_t;

static inline void kmp_build_failure(const kmp_t * kmp)
{
    if (kmp->length == 0) {
        return;
    }

    const unsigned char * pattern = (unsigned char*)kmp->pattern;
    kmp_state_t * failure = kmp->failure;

    failure[0] = kmp_state_t_max;
    for (kmp_state_t i = 1, j = 0; i < kmp->length; i++) {
        if (KMPALG_CONFIG_COMPARER(pattern[i], pattern[j])) {
            failure[i] = failure[j];
            j++;
        } else {
            failure[i] = j;
            j = 0;
        }
    }
}

static inline kmp_state_t kmp_failure(const kmp_t * kmp, kmp_state_t state)
{
    if (kmp->failure) {
        return kmp->failure[state];
    } else {
        // assume this pattern needn't backtrack
        // NOTE: if kmp->failure is null and kmp->pattern need backtrack, you will got incorrect result
        return state ? 0 : kmp_state_t_max;
    }
}

static inline kmp_state_t kmp_input(const kmp_t * kmp, kmp_state_t state, unsigned char input)
{
    const unsigned char * pattern = (unsigned char*)kmp->pattern;
    while (1) {
        if (KMPALG_CONFIG_COMPARER(pattern[state], input)) {
            return ++state;
        }
        state = kmp_failure(kmp, state);
        if (state == kmp_state_t_max) {
            return 0;
        }
    }
}

static inline int kmp_matched(const kmp_t *kmp, kmp_state_t state)
{
    return kmp->length == state;
}


static inline void kmp_init(kmp_t * kmp, 
                            const unsigned char * pattern, 
                            kmp_state_t * failure, 
                            kmp_param_length_t len
                            )
{
    kmp_static_assert(kmp_state_t_max > 0 && "KMPALG_CONFIG_FAILURE_TYPE must be unsigned type");

#if KMPALG_CONFIG_CHECK_LENGTH
    assert(len <= kmp_state_t_max && "param of length is too large, consider use a larger KMPALG_CONFIG_FAILURE_TYPE");
#endif

    kmp->pattern = pattern;
    kmp->length = (kmp_state_t)len;
    kmp->failure = failure;
    if (failure) {
        kmp_build_failure(kmp);
    }
}

#define KMP_STATIC_INIT(kmp, pat, len) \
    { static kmp_state_t failure[len]; kmp_init((kmp), (pat), failure, (len)); }

#if KMPALG_CONFIG_CHECK_ASSUME_NO_BACKTRACK
#define KMP_STATIC_INIT_ASSUME_NO_BACKTRACE(kmp, pat, len)                                                  \
    {                                                                                                       \
        kmp_t * _kmp = (kmp);                                                                               \
        kmp_param_length_t length = (len);                                                                  \
        kmp_state_t failure[length];                                                                        \
        kmp_init(_kmp, (pat), failure, length);                                                             \
        for (kmp_param_length_t i = 0; i < length; i++) {                                                   \
            assert((failure[i] == 0 || failure[i] == kmp_state_t_max) && "no backtrace check failed");      \
        }                                                                                                   \
        _kmp->failure = NULL;                                                                               \
    }
#else
#define KMP_STATIC_INIT_ASSUME_NO_BACKTRACE(kmp, pat, len) \
    { kmp_init((kmp), (pat), NULL, (len)); }
#endif

#endif

