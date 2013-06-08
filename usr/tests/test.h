#ifndef USR_TEST_TEST_H_INCLUDED
#define USR_TEST_TEST_H_INCLUDED

/* Tests return 0 on pass, else on error.
 * The first arguement is a buffer to copy
 * an error message into, whose length is the
 * second arguement */
struct test {
    char *name;
    int (*func)(char *, int);
};

/* Create a test with name nm using function f */
#define DEFINE_TEST(nm, f) struct test f##_test __attribute__((section(".user")))= {\
        .name = nm,        \
        .func = f          \
    };

/* Standard test return codes */
enum {
    PASSED = 0,
    FAILED,
};

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

#endif
