#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

void memset32(void *p, int32_t value, uint32_t size);
void memset(void *p, int32_t value, uint32_t size);
void memcpy(void *dst, void *src, int n);
void itoa(int n, char buf[]);
void uitoa(uint32_t n, char buf[]);
void ftoa(float num, float tolerance, char buf[], uint32_t n);
void strreverse(char *s);
int strncmp(char *s, char *p, uint32_t n);

#endif
