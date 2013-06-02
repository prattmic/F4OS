#include <stdlib.h>
#include <string.h>

char *strndup(char *str, int n) {
    int len = strnlen(str, n);

    char *dup = malloc(len+1);  // Room for NULL
    if (!dup) {
        return NULL;
    }

    strncpy(dup, str, len);
    dup[len] = '\0';

    return dup;
}
