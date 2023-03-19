#ifndef MINPUT_H
#define MINPUT_H

#include "base.h"



typedef enum
{
    MK_A = 1,
    MK_B,
    MK_C,
    MK_D,
    MK_E,
    MK_F,
    MK_G,
    MK_H,
    MK_I,
    MK_J,
    MK_K,
    MK_L,
    MK_M,
    MK_N,
    MK_O,
    MK_P,
    MK_Q,
    MK_R,
    MK_S,
    MK_T,
    MK_U,
    MK_V,
    MK_W,
    MK_X,
    MK_Y,
    MK_Z,

    MK_0,
    MK_1,
    MK_2,
    MK_3,
    MK_4,
    MK_5,
    MK_6,
    MK_7,
    MK_8,
    MK_9,

    MK_NUMPAD_0,
    MK_NUMPAD_1,
    MK_NUMPAD_2,
    MK_NUMPAD_3,
    MK_NUMPAD_4,
    MK_NUMPAD_5,
    MK_NUMPAD_6,
    MK_NUMPAD_7,
    MK_NUMPAD_8, MK_NUMPAD_9,
    MK_NUMPAD_MULTIPLY,
    MK_NUMPAD_ADD,
    MK_NUMPAD_SUBTRACT,
    MK_NUMPAD_DECIMAL,
    MK_NUMPAD_DIVIDE,

    MK_LEFT,
    MK_UP,
    MK_RIGHT,
    MK_DOWN,

    MK_BACKSPACE,
    MK_TAB,
    MK_CTRL,
    MK_RETURN,
    MK_SPACE,
    MK_LSHIFT,
    MK_RSHIFT,
    MK_LCONTROL,
    MK_RCONTROL,
    MK_ALT,
    MK_LSUPER,
    MK_RSUPER,
    MK_CAPSLOCK,
    MK_ESCAPE,
    MK_PAGEUP,
    MK_PAGEDOWN,
    MK_HOME,
    MK_END,
    MK_INSERT,
    MK_DELETE,
    MK_PAUSE,
    MK_NUMLOCK,
    MK_PRINTSCREEN,

    MK_F1,
    MK_F2,
    MK_F3,
    MK_F4,
    MK_F5,
    MK_F6,
    MK_F7,
    MK_F8,
    MK_F9,
    MK_F10,
    MK_F11,
    MK_F12,

    MK_LMB,
    MK_RMB,
    MK_MMB,

    MK_MAX
}mKey;

//The input state of the engine for the current frame (its a singleton!)
typedef struct
{
    mKey keys[MK_MAX];
    mKey prev_keys[MK_MAX];

    i32 mouse_pos_x, mouse_pos_y;
    i32 mouse_delta_x, mouse_delta_y;
}mInputState;

//This is the input singleton
extern mInputState mis;

static inline void minput_init(void)
{
    MEMSET(&mis, 0, sizeof(mInputState));
}

static inline v2 minput_get_mouse_pos(void)
{
    return (v2){mis.mouse_pos_x, mis.mouse_pos_y};
}
static inline v2 minput_get_mouse_delta(void)
{
    return (v2){mis.mouse_delta_x, mis.mouse_delta_y};
}
static inline b32 mkey_pressed(mKey k)
{
    return (mis.keys[k] && (mis.prev_keys[k] == 0));
}

static inline b32 mkey_released(mKey k)
{
    return (mis.prev_keys[k] && (mis.keys[k] == 0));
}

static inline b32 mkey_up(mKey k)
{
    return (mis.keys[k] == 0);
}

static inline b32 mkey_down(mKey k)
{
    return (mis.keys[k]);
}


//updates input state (should be called once every frame)
void minput_update(void);



#endif