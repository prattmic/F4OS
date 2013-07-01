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
