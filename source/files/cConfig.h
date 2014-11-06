//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CONFIG_H_
#define _P1_GUARD_CONFIG_H_


// ****************************************************************
// configuration definitions
#define CONFIG_GRAPHICS_SHADOW     "Graphics", "Shadow",                             (1)
#define CONFIG_GRAPHICS_REFLECTION "Graphics", "Reflection",                         (1)
                                                                                     
#define CONFIG_INPUT_TYPE(p)       "Input",    PRINT("P%d_Type",      (p)),          (p)
#define CONFIG_INPUT_MOVE_LEFT(s)  "Input",    PRINT("S%d_MoveLeft",  (s)),          (CORE_INPUT_KEY(A))
#define CONFIG_INPUT_MOVE_RIGHT(s) "Input",    PRINT("S%d_MoveRight", (s)),          (CORE_INPUT_KEY(D))
#define CONFIG_INPUT_MOVE_DOWN(s)  "Input",    PRINT("S%d_MoveDown",  (s)),          (CORE_INPUT_KEY(S))
#define CONFIG_INPUT_MOVE_UP(s)    "Input",    PRINT("S%d_MoveUp",    (s)),          (CORE_INPUT_KEY(W))
#define CONFIG_INPUT_BUTTON(s,n)   "Input",    PRINT("S%d_Button%d",  (s), (n) + 1), ((n) + int(CORE_INPUT_KEY(1)))

#define INPUT_TYPES         (2u)                                          // number of input set selections (independent from number of players, but should be equal or more)
#define INPUT_BUTTONS       (4u)                                          // number of input buttons per set
#define INPUT_SETS_KEYBOARD (INPUT_TYPES)                                 // number of keyboard and mouse sets
#define INPUT_SETS_JOYSTICK (INPUT_TYPES)                                 // number of joystick/gamepad sets
#define INPUT_SETS          (INPUT_SETS_KEYBOARD + INPUT_SETS_JOYSTICK)   // total number of sets

STATIC_ASSERT(INPUT_TYPES   <= INPUT_SETS);
STATIC_ASSERT(INPUT_BUTTONS <= sizeof(coreByte)*8);


// ****************************************************************
// configuration variables
struct cConfig
{
    struct
    {
        int iShadow;       // shadow level
        int iReflection;   // water reflection level
    }
    Graphics;
    
    struct
    {
        int aiType[INPUT_TYPES];           // selected input set (each element should have a different value)
        struct
        {
            int iMoveLeft;                 // key for moving left (ignored on joystick/gamepad)
            int iMoveRight;                // key for moving right
            int iMoveDown;                 // key for moving down
            int iMoveUp;                   // key for moving up
            int aiButton[INPUT_BUTTONS];   // input buttons (<=0 for mouse)
        }
        aSet[INPUT_SETS];
    }
    Input;
};

struct cInput
{
    coreVector2 vMove;            // movement input
    coreByte    iButtonPress;     // button press (bitfields)
    coreByte    iButtonRelease;   // button release
    coreByte    iButtonHold;      // button hold
};

extern cConfig g_CurConfig;            // current and active configuration structure
extern cConfig g_OldConfig;            // handle for old values
extern cInput  g_aInput[INPUT_SETS];   // mapped input values (ready for direct use)


// ****************************************************************
// configuration functions
extern void LoadConfig();
extern void SaveConfig();
extern void UpdateInput();


#endif // _P1_GUARD_CONFIG_H_