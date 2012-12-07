#define HAVE_USART
#define HAVE_SPI
#define HAVE_I2C
#define HAVE_USBDEV

/* Buddy heap orders
 * The linker script has specified 128kb
 * user heap and 32kb of kernel heap */
#define USER_MAX_ORDER   17
#define USER_MIN_ORDER   4

#define KERNEL_MAX_ORDER   15
#define KERNEL_MIN_ORDER   4
