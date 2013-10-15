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

#ifndef USR_SHELL_SHELL_H_INCLUDED
#define USR_SHELL_SHELL_H_INCLUDED

void shell(void);
void help(int argc, char **argv);
void history(int argc, char **argv);

#define SHELL_BUF_MAX       256
#define SHELL_ARG_BUF_MAX   256
#define SHELL_PROMPT        "$ "
#define SHELL_PROMPT_LEN    (2)  // strlen(SHELL_PROMPT)
#define SHELL_HISTORY       (10) // Command history length

#define CLEARLINE           "\e[K"
#define RIGHT               "\e[C"
#define LEFT                "\e[D"

#endif
