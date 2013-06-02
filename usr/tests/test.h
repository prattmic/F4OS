#ifndef USR_TEST_TEST_H_INCLUDED
#define USR_TEST_TEST_H_INCLUDED

/* Tests return 0 on pass, else on error.
 * The error code will be displayed. */
struct test {
    char *name;
    int (*func)(void);
};

/* Create a test with name nm using function f */
#define DEFINE_TEST(nm, f) struct test f##_test __attribute__((section(".user")))= {\
        .name = nm,        \
        .func = f          \
    };

#endif
