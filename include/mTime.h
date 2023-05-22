#ifndef  MTIME_H
#define  MTIME_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#if OS_WIN
#include <windows.h>
typedef struct mTimeState
{
    u32 initialized;
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
}mTimeState;
#else
#include <time.h>
#if !defined(CLOCK_MONOTONIC)
    #define CLOCK_MONOTONIC 0
#endif
typedef struct mTimeState
{
    u32 initialized;
    u64 start;
}mTimeState;
#endif

#if OS_WIN
static inline i64 _ds64_muldiv(i64 value, i64 numer, i64 denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

static mTimeState mtime_state;

static inline void mtime_init(void)
{
    memset(&mtime_state,  0, sizeof(mtime_state));
    mtime_state.initialized = 0xABCDABCD; //why??
    #if OS_WIN
    QueryPerformanceFrequency(&mtime_state.freq);
    QueryPerformanceCounter(&mtime_state.start);
    #else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    mtime_state.start = (u64)ts.tv_sec*1000000000 + (u64)ts.tv_nsec;
    #endif

}

static inline u64 mtime_now(void)
{
    ASSERT(mtime_state.initialized == 0xABCDABCD);
    u64 now;
    #if OS_WIN
    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    now = (u64)_ds64_muldiv(qpc.QuadPart - mtime_state.start.QuadPart, 1000000000, mtime_state.freq.QuadPart);
    #else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    now = ((u64)ts.tv_sec*1000000000 + (u64)ts.tv_nsec) - mtime_state.start;  
    #endif
    return now;
}

static inline u64 mtime_diff(u64 new_ticks, u64 old_ticks)
{
    if (new_ticks > old_ticks)
    {
        return new_ticks - old_ticks;
    }
    else
    {
        return 1;
    }
}

static inline f64 mtime_sec(u64 ticks)
{
    return (f64)ticks / 1000000000.0;
}

static inline f64 mtime_ms(u64 ticks)
{
    return (f64)ticks / 1000000.0;
}

static inline f64 mtime_us(u64 ticks)
{
    return (f64)ticks / 1000.0;
}

static inline f64 mtime_ns(u64 ticks) 
{
    return (f64)ticks;
}

#endif  //MTIME_H