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

#define _CRT_SECURE_NO_WARNINGS // 在MSVC上若不启用这个宏，很多IO函数无法使用

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <malloc.h>
#include "kmpalg.h"

uint8_t dummy_data[256];
const uint8_t * pattern = (uint8_t*)"abcabcddabcabcddabcabcddabcabcddff";
const uint8_t * disturb = (uint8_t*)"abcabcddabcabcddabcabcddabcabcddabcabcddabcabcddabcabcddabcabcddabcabcdd";
#define PATTERN_LEN 34
#define DISTURB_LEN 72

void dummy_init(void)
{
    for (int i = 0; i < sizeof(dummy_data); i++) {
        dummy_data[i] = (uint8_t)i;
    }
}

void write_dummy(FILE * file, int n)
{
    uint8_t dummy[256];
    
    while (n > 0) {
        int wt = sizeof(dummy);
        if (wt > n) {
            wt = n;
        }
        assert(fwrite(dummy_data, 1, wt, file) == wt);
        n -= wt;
    }
    
}

int make_data_file(const char *filename)
{
    // 产生一个包含模式串的文件
    FILE * file = fopen(filename, "wb");
    assert(file && "open file for write failed");
    write_dummy(file, 5000);
	assert(fwrite(disturb, 1, DISTURB_LEN, file) == DISTURB_LEN);
	assert(fwrite(pattern, 1, PATTERN_LEN, file) == PATTERN_LEN);
	write_dummy(file, 5000);
    fclose(file);
    return 5000 + DISTURB_LEN;
}

int search_in_file_using_malloc(const char * filename)
{
    // 使用动态分配创建KMP的相关成员
    int ch, res = -1, counter = 0;
    kmp_t kmp;
    FILE * file = fopen(filename, "rb");
    assert(file && "open file for read failed");
    kmp_state_t * failure = (kmp_state_t*)malloc(sizeof(kmp_state_t)*PATTERN_LEN);
    assert(failure && "malloc failed");
    kmp_state_t state = 0;
    kmp_init(&kmp, pattern, failure, PATTERN_LEN);
    while ((ch = fgetc(file)) >= 0) { // fgetc 效率低下，这里只是简单演示如何在一个stream里匹配数据
        counter++;
        state = kmp_input(&kmp, state, (uint8_t)ch);
        if (kmp_matched(&kmp, state)) {
            res = counter - kmp.length;
            break;
        }
    }
    free(failure);
    fclose(file);
    return res;
}

int search_in_file_using_static_init(const char * filename)
{
    // 直接静态分配KMP相关成员
    int ch, res = -1, counter = 0;
    kmp_t kmp;
    FILE * file = fopen(filename, "rb");
    kmp_state_t state = 0;
    KMP_STATIC_INIT(&kmp, pattern, PATTERN_LEN);
    while ((ch = fgetc(file)) >= 0) { // fgetc 效率低下，这里只是简单演示如何在一个stream里匹配数据
        counter++;
        state = kmp_input(&kmp, state, (uint8_t)ch);
        if (kmp_matched(&kmp, state)) {
            res = counter - kmp.length;
            break;
        }
    }
    fclose(file);
    return res;
}


int main(void)
{
    const char * testfile = "./testfile.dat";
    int expect, search_result;

    dummy_init();
    expect = make_data_file(testfile);
    
    printf("search_in_file_using_static_init: ");
    search_result = search_in_file_using_static_init(testfile);
    if (expect == search_result) {
        printf("search success, pattern at offset: %d\n", expect);
    } else {
        printf("search failed, expect %d but result is %d\n", expect, search_result);
    }

    printf("     search_in_file_using_malloc: ");
    search_result = search_in_file_using_malloc(testfile);
    if (expect == search_result) {
        printf("search success, pattern at offset: %d\n", expect);
    } else {
        printf("search failed, expect %d but result is %d\n", expect, search_result);
    }
    
    return 0;
}

