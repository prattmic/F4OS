#include <dev/registers.h>

static inline uint64_t getcount(void) {
    return (*TIM2_CNT << 16)|(*TIM1_CNT);
}
