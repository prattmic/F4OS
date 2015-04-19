/*
 * Copyright (C) 2013, 2015 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
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

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED

typedef __INTMAX_TYPE__         intmax_t;
typedef __UINTMAX_TYPE__        uintmax_t;
typedef __INT8_TYPE__           int8_t;
typedef __INT16_TYPE__          int16_t;
typedef __INT32_TYPE__          int32_t;
typedef __INT64_TYPE__          int64_t;
typedef __UINT8_TYPE__          uint8_t;
typedef __UINT16_TYPE__         uint16_t;
typedef __UINT32_TYPE__         uint32_t;
typedef __UINT64_TYPE__         uint64_t;
typedef __INT_LEAST8_TYPE__     int_least8_t;
typedef __INT_LEAST16_TYPE__    int_least16_t;
typedef __INT_LEAST32_TYPE__    int_least32_t;
typedef __INT_LEAST64_TYPE__    int_least64_t;
typedef __UINT_LEAST8_TYPE__    uint_least8_t;
typedef __UINT_LEAST16_TYPE__   uint_least16_t;
typedef __UINT_LEAST32_TYPE__   uint_least32_t;
typedef __UINT_LEAST64_TYPE__   uint_least64_t;
typedef __INT_FAST8_TYPE__      int_fast8_t;
typedef __INT_FAST16_TYPE__     int_fast16_t;
typedef __INT_FAST32_TYPE__     int_fast32_t;
typedef __INT_FAST64_TYPE__     int_fast64_t;
typedef __UINT_FAST8_TYPE__     uint_fast8_t;
typedef __UINT_FAST16_TYPE__    uint_fast16_t;
typedef __UINT_FAST32_TYPE__    uint_fast32_t;
typedef __UINT_FAST64_TYPE__    uint_fast64_t;
typedef __INTPTR_TYPE__         intptr_t;
typedef __UINTPTR_TYPE__        uintptr_t;

#define INTMAX_MAX              __INTMAX_MAX__
#define UINTMAX_MAX             __UINTMAX_MAX__
#define INT8_MAX                __INT8_MAX__
#define INT16_MAX               __INT16_MAX__
#define INT32_MAX               __INT32_MAX__
#define INT64_MAX               __INT64_MAX__
#define UINT8_MAX               __UINT8_MAX__
#define UINT16_MAX              __UINT16_MAX__
#define UINT32_MAX              __UINT32_MAX__
#define UINT64_MAX              __UINT64_MAX__
#define INT_LEAST8_MAX          __INT_LEAST8_MAX__
#define INT_LEAST16_MAX         __INT_LEAST16_MAX__
#define INT_LEAST32_MAX         __INT_LEAST32_MAX__
#define INT_LEAST64_MAX         __INT_LEAST64_MAX__
#define UINT_LEAST8_MAX         __UINT_LEAST8_MAX__
#define UINT_LEAST16_MAX        __UINT_LEAST16_MAX__
#define UINT_LEAST32_MAX        __UINT_LEAST32_MAX__
#define UINT_LEAST64_MAX        __UINT_LEAST64_MAX__
#define INT_FAST8_MAX           __INT_FAST8_MAX__
#define INT_FAST16_MAX          __INT_FAST16_MAX__
#define INT_FAST32_MAX          __INT_FAST32_MAX__
#define INT_FAST64_MAX          __INT_FAST64_MAX__
#define UINT_FAST8_MAX          __UINT_FAST8_MAX__
#define UINT_FAST16_MAX         __UINT_FAST16_MAX__
#define UINT_FAST32_MAX         __UINT_FAST32_MAX__
#define UINT_FAST64_MAX         __UINT_FAST64_MAX__
#define INTPTR_MAX              __INTPTR_MAX__
#define UINTPTR_MAX             __UINTPTR_MAX__

#define SIZE_MAX                __SIZE_MAX__
#define PTRDIFF_MAX             __PTRDIFF_MAX__

#define INTMAX_MIN              (-INTMAX_MAX - 1)
#define INT8_MIN                (-INT8_MAX - 1)
#define INT16_MIN               (-INT16_MAX - 1)
#define INT32_MIN               (-INT32_MAX - 1)
#define INT64_MIN               (-INT64_MAX - 1)
#define INT_LEAST8_MIN          (-INT_LEAST8_MAX - 1)
#define INT_LEAST16_MIN         (-INT_LEAST16_MAX - 1)
#define INT_LEAST32_MIN         (-INT_LEAST32_MAX - 1)
#define INT_LEAST64_MIN         (-INT_LEAST64_MAX - 1)
#define INT_FAST8_MIN           (-INT_FAST8_MAX - 1)
#define INT_FAST16_MIN          (-INT_FAST16_MAX - 1)
#define INT_FAST32_MIN          (-INT_FAST32_MAX - 1)
#define INT_FAST64_MIN          (-INT_FAST64_MAX - 1)
#define INTPTR_MIN              (-INTPTR_MAX - 1)

#define PTRDIFF_MIN             (-PTRDIFF_MAX - 1)

#endif
