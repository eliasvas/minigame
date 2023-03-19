#include "mProfiler.h"


mProfiler global_profiler;


void mprofiler_init(mProfiler *prof) {
    if (prof == NULL)prof = &global_profiler;
    memset(prof->tags, 0, sizeof(prof->tags));
    prof->tag_count = 0;
}


void mprofiler_add_sample(mProfiler *prof, mProfilerSample *sample){
if (prof == NULL)prof = &global_profiler;
    i32 index = -1;
    for (u32 i = 0; i < prof->tag_count; ++i){
        if (strcmp(sample->name, prof->tags[i].name) == 0){
            index = i;
        }
    }
    if (index == -1)
    {
        //if tag not found, get next free tag slot, and insert in hashmap {tag's name hash, index of tag array}
        index = prof->tag_count++;
        ASSERT(prof->tag_count < MPROFILER_MAX_TAGS);
        //hmput(prof->name_hash, name_hash_code, index);
    }
    u32 frame_count = 0;

    if(prof->tags[index].frames[frame_count % MAX_SAMPLES_PER_NAME] != frame_count){
        prof->tags[index].samples[frame_count % MAX_SAMPLES_PER_NAME] = 0;
        prof->tags[index].frames[frame_count % MAX_SAMPLES_PER_NAME] = frame_count;
    }
    

    u64 elapsed_cycles = sample->end_cycles - sample->start_cycles;
	u64 elapsed_time = sample->end_time - sample->start_time;
    //prof->tags[index].samples[frame_count % MAX_SAMPLES_PER_NAME] += val;
    prof->tags[index].samples[frame_count % MAX_SAMPLES_PER_NAME] = mtime_ms(elapsed_time);
    prof->tags[index].cycles[frame_count % MAX_SAMPLES_PER_NAME] = elapsed_cycles;

    if (prof->tags[index].name[0] == '\0')//because we set all thos to zero in the beginning
        strcpy(prof->tags[index].name,sample->name);
}