/* stdarg.h
 * Provides va_list and family for a variable number of arguments
 *
 * Michael Pratt <michael@pratt.im> */

#ifndef STARG_H_INCLUDED
#define STARG_H_INCLUDED

typedef void* va_list;

#define va_start(v, l)  \
    v = (va_list) ((uint32_t) &l + (sizeof(l)<4 ? 4 : sizeof(l)))

#define va_arg(v, type) \
    *(type *) v;           \
    v = (va_list) ((uint32_t) v + (sizeof(type)<4 ? 4 : sizeof(type)))

#define va_end(v)       \
    v = NULL

#endif
