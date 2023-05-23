#ifndef MSOUND_H
#define MSOUND_H
#include "base.h"


typedef struct {
    char filename[64];
}mSoundDesc;

typedef struct {
    mSoundDesc desc;    
    //specific to each API implementing textures :)
    void *internal_state; 
} mSound;

M_RESULT msound_load(mSoundDesc *desc, mSound *s);
M_RESULT msound_play(mSound *s);
M_RESULT msound_destroy(mSound *s);

#endif