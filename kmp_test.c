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

#include "kmpalg.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int kmp_search(const kmp_t * kmp, const uint8_t * data, int len)
{
    if (kmp->length == 0) {
        return 0;
    }

    const uint8_t * p = data;
    kmp_state_t state = 0;

    while (len--) {
        state = kmp_input(kmp, state, *p++);
        if (kmp_matched(kmp, state)) {
            return p - data - kmp->length; 
        }
    }
    return -1;
}

void putnspeace(int n)
{
    while (n--) {
        putchar(' ');
    }
}

int main(int argc, char *argv[])
{
    kmp_state_t  failure[255];
    kmp_t        kmp;
    const char * pattern;
    const char * mainstr;
    int          res = 0;

    if (argc != 2 && argc != 3) {
        return -1;
    }
    pattern = argv[1];
    if (argc == 3) {
        mainstr = argv[2];
    } else {
        mainstr = NULL;
    }

    int len = strlen(pattern);
    kmp_init(&kmp, (uint8_t*)pattern, failure, len);

    printf("%s: ", pattern);
    for (kmp_state_t i = 0; i < kmp.length; i++) {
        int failure_jump = failure[i];
        printf("%d ", failure_jump == kmp_state_t_max ? -1 : failure_jump);
    }
    putchar('\n');
    if (mainstr) {
        int ret = kmp_search(&kmp, (uint8_t*)mainstr, strlen(mainstr));
        printf("mainstr: %s\n", mainstr);
        printf("         ");
        if (ret < 0) {
            printf("not matched\n");
            res = 1;
        } else {
            putnspeace(ret);
            printf("^\n");
        }
        printf("result: %d\n", ret);
    }
    return res;
}
