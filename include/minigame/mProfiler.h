#ifndef MPROFILER_H
#define MPROFILER_H
#include "base.h"
#include "mTime.h"
#if OS_WIN
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

//maybe use hashes instead of strings on mProfiler?????

static inline u64 readTSC() {
    // _mm_lfence();  // optionally wait for earlier insns to retire before reading the clock
    return __rdtsc();
    // _mm_lfence();  // optionally block later instructions until rdtsc retires
}


#define MPROFILER_MAX_TAGS 64

typedef struct{
    char name[32];
    u64 start_time;
    u64 end_time;
    u64 start_cycles;
    u64 end_cycles;
}mProfilerSample;
#define MAX_SAMPLES_PER_NAME 8 
typedef struct {
    char name[64];
    f32 samples[MAX_SAMPLES_PER_NAME]; //how many millis it took
    u64 cycles[MAX_SAMPLES_PER_NAME]; //how many cycles it took
    u64 frames[MAX_SAMPLES_PER_NAME]; //for what frame this sample is (without that we cant add to the same sample e.g multiple function invocations)
}mProfilerTag;

//each name is associated with an index {name, index}, which points to a profilertag that stores data!
typedef struct {
    mProfilerTag tags[MPROFILER_MAX_TAGS];
    u32 tag_count;
}mProfiler;

void mprofiler_add_sample(mProfiler *prof, mProfilerSample *sample);

static inline mProfilerSample mprofiler_sample_start(char *name)
{
    mProfilerSample sample = {0};
    sample.start_time = mtime_now();
	sample.start_cycles = readTSC();
    MEMCPY(sample.name, name, strlen(name));

    return sample;
}

static inline f64 mprofiler_sample_end(mProfilerSample *sample) {
    sample->end_time = mtime_now();   
	sample->end_cycles = readTSC();

 
    mprofiler_add_sample(NULL, sample);

    return mtime_ms(sample->end_time - sample->start_time);
}

#define MPROFILER_START(name) mProfilerSample s = mprofiler_sample_start(name);
#define MPROFILER_END() mprofiler_sample_end(&s);



extern mProfiler global_profiler;

void mprofiler_init(mProfiler *prof);



#endif