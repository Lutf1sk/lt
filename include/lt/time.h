#ifndef LT_TIME_H
#define LT_TIME_H 1

#include <lt/primitives.h>

#define LT_SEC_TO_MSEC(n) ((n) * 1000)
#define LT_SEC_TO_USEC(n) ((n) * 1000000)

#define LT_MSEC_TO_SEC(n) ((n) / 1000)
#define LT_MSEC_TO_USEC(n) ((n) * 1000)

#define LT_USEC_TO_SEC(n) ((n) / 1000000)
#define LT_USEC_TO_MSEC(n) ((n) / 1000)

u64 lt_hfreq_time_usec(void);
u64 lt_hfreq_time_msec(void);
u64 lt_hfreq_time_sec(void);

#endif
