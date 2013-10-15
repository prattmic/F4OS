/*
 * Copyright (C) 2013 F4OS Authors
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

#ifndef LIMITS_H_INCLUDED
#define LIMITS_H_INCLUDED

#define MB_LEN_MAX    1

#define CHAR_BIT __CHAR_BIT__

/* Minimum and maximum values a 'signed char' can hold.  */
#define SCHAR_MAX __SCHAR_MAX__
#define SCHAR_MIN (-SCHAR_MAX-1)

/* Maximum value an 'unsigned char' can hold.  (Minimum is 0).  */
#define UCHAR_MAX 255

/* Minimum and maximum values a 'char' can hold.  */
#define CHAR_MIN 0
#define CHAR_MAX 255

/* Minimum and maximum values a 'signed short int' can hold.  */
#define SHRT_MAX __SHRT_MAX__
#define SHRT_MIN (-SHRT_MAX-1)

/* Maximum value an 'unsigned short int' can hold.  (Minimum is 0).  */
#define USHRT_MAX __UINT16_MAX__

/* Minimum and maximum values a 'signed int' can hold.  */
#define INT_MAX __INT_MAX__
#define INT_MIN (-INT_MAX-1)

/* Maximum value an 'unsigned int' can hold.  (Minimum is 0).  */
#define UINT_MAX (INT_MAX * 2U + 1)

/* Minimum and maximum values a 'signed long int' can hold.
   (Same as `int').  */
#define LONG_MAX __LONG_MAX__
#define LONG_MIN (-LONG_MAX-1)

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#endif
