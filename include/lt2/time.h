#pragma once

#include <lt2/common.h>

#define S_TO_MS(n) ((n) * 1000)
#define S_TO_US(n) ((n) * 1000000)
#define S_TO_NS(n) ((n) * 1000000000)

#define MS_TO_S(n) ((n) / 1000)
#define MS_TO_US(n) ((n) * 1000)
#define MS_TO_NS(n) ((n) * 1000000)

#define US_TO_S(n) ((n) / 1000000)
#define US_TO_MS(n) ((n) / 1000)
#define US_TO_NS(n) ((n) * 1000)

#define NS_TO_S(n) ((n) / 1000000000)
#define NS_TO_MS(n) ((n) / 1000000)
#define NS_TO_US(n) ((n) / 1000)

// time.c

u64 time_ns(void);
u64 time_us(void);
u64 time_ms(void);
u64 time_s(void);

// sleep.c

void sleep_ns(u64 usec);
void sleep_us(u64 usec);
void sleep_ms(u64 msec);
void sleep_s(u64 sec);

