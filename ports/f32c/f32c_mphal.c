/*
 * The MIT License (MIT)
 *
 * Copyright 2024 Marko Zec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <time.h>

#include "py/mphal.h"


#ifndef mp_hal_ticks_ms
mp_uint_t mp_hal_ticks_ms(void) {
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
}
#endif

#ifndef mp_hal_ticks_us
mp_uint_t mp_hal_ticks_us(void) {
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
}
#endif

uint64_t mp_hal_time_ns(void) {
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (uint64_t)tv.tv_sec * 1000000000ULL + (uint64_t)tv.tv_nsec;
}

void mp_hal_delay_us(mp_uint_t us) {
    clock_t t;

#if (CLOCKS_PER_SEC != 1000000)
#error "CLOCKS_PER_SEC"
#endif

    for (t = clock() + us; t > clock();) {}
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_hal_delay_us(ms * 1000);
}


/*
 * XXX FIX THOSE, THEN MOVE TO LIBS:
 */
int32_t
__clzdi2(int64_t a) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

/* XXX only riscv needs those: */
int32_t
poll(int a) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

int32_t
__extendhfdf2(int16_t a) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

int32_t
__truncsfhf2(int32_t a) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

double tgamma(double x) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

int lgamma_r(int x) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}

int signgam(int x) {
	printf("%s() unimplemented, aborting!\n", __FUNCTION__);
	exit(1);
}
