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

#ifndef FLOAT_H_INCLUDED
#define FLOAT_H_INCLUDED

#define FLT_MAX_EXP         128     /* Not confirmed */
#define FLT_MIN_EXP         -125    /* Not confirmed */

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491c/Babbfeda.html */
#define FLT_MAX             3.40282347e+38F
#define FLT_MIN             1.175494351e-38F

#define FLT_RADIX           2
#define FLT_ROUNDS          1
#define FLT_DIGITS          6

#define FLT_EPSILON         1.19209290e-7F

#endif
