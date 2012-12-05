#define HAVE_USART

/* Buddy heap orders
 * The linker script has specified 16kb
 * user heap and 8kb of kernel heap */
#define USER_MAX_ORDER   14
#define USER_MIN_ORDER   4

#define KERNEL_MAX_ORDER   13
#define KERNEL_MIN_ORDER   4
