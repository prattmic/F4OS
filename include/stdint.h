#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED

typedef __UINT8_TYPE__  uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INT8_TYPE__   int8_t;
typedef __INT16_TYPE__  int16_t;
typedef __INT32_TYPE__  int32_t;
typedef __INT64_TYPE__  int64_t;
// These have to match __WORDSIZE
typedef __INT32_TYPE__  intptr_t;
typedef __UINT32_TYPE__ uintptr_t;

#endif
