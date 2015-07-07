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

// TODO: transfer to Core Engine
// TODO: low quality settings affect also ship models


// ****************************************************************
// configuration definitions
#define CONFIG_GAME_TRANSPARENT    "Game",     "Transparent",                         (0)
#define CONFIG_GAME_COMBAT_DAMAGE  "Game",     "CombatDamage",                        (1)
#define CONFIG_GAME_COMBAT_CHAIN   "Game",     "CombatChain",                         (1)
#define CONFIG_GAME_COMBAT_COMBO   "Game",     "CombatCombo",                         (1)

#define CONFIG_GRAPHICS_SHADOW     "Graphics", "Shadow",                              (1)
#define CONFIG_GRAPHICS_REFLECTION "Graphics", "Reflection",                          (1)
#define CONFIG_GRAPHICS_GLOW       "Graphics", "Glow",                                (1)
#define CONFIG_GRAPHICS_DISTORTION "Graphics", "Distortion",                          (1)

#define CONFIG_INPUT_TYPE(p)       "Input",    PRINT("P%u_Type",      (p)),           (p)
#define CONFIG_INPUT_MOVE_LEFT(s)  "Input",    PRINT("S%u_MoveLeft",  (s)),           (CORE_INPUT_KEY(A))
#define CONFIG_INPUT_MOVE_RIGHT(s) "Input",    PRINT("S%u_MoveRight", (s)),           (CORE_INPUT_KEY(D))
#define CONFIG_INPUT_MOVE_DOWN(s)  "Input",    PRINT("S%u_MoveDown",  (s)),           (CORE_INPUT_KEY(S))
#define CONFIG_INPUT_MOVE_UP(s)    "Input",    PRINT("S%u_MoveUp",    (s)),           (CORE_INPUT_KEY(W))
#define CONFIG_INPUT_BUTTON(s,n)   "Input",    PRINT("S%u_Button%u",  (s), (n) + 1u), ((n) + coreUintW(CORE_INPUT_KEY(1)))

#define CONFIG_FORCE   // force specific settings to increase fairness

#define INPUT_TYPES         (PLAYERS)                                     // number of input set selections
#define INPUT_BUTTONS       (4u)                                          // number of input buttons per set
#define INPUT_SETS_KEYBOARD (INPUT_TYPES)                                 // number of keyboard and mouse sets
#define INPUT_SETS_JOYSTICK (INPUT_TYPES)                                 // number of joystick/gamepad sets
#define INPUT_SETS          (INPUT_SETS_KEYBOARD + INPUT_SETS_JOYSTICK)   // total number of sets

STATIC_ASSERT(INPUT_TYPES   <= INPUT_SETS)
STATIC_ASSERT(INPUT_BUTTONS <= sizeof(coreUint8)*8u)


// ****************************************************************
// configuration variables
struct cConfig
{
    struct
    {
        coreUint8 iTransparent;   // transparent player bullets
        struct
        {
            coreUint8 iDamage;    // show damage as combat text
            coreUint8 iChain;     // show chain as combat text
            coreUint8 iCombo;     // show combo as combat text
        }
        Combat;
    }
    Game;

    struct
    {
        coreUint8 iShadow;       // shadow level
        coreUint8 iReflection;   // water reflection level
        coreUint8 iGlow;         // fullscreen glow level
        coreUint8 iDistortion;   // fullscreen distortion level
    }
    Graphics;

    struct
    {
        coreUint8 aiType[INPUT_TYPES];           // selected input set (each element should have a different value)
        struct
        {
            coreInt16 iMoveLeft;                 // key for moving left (ignored on joystick/gamepad)
            coreInt16 iMoveRight;                // key for moving right
            coreInt16 iMoveDown;                 // key for moving down
            coreInt16 iMoveUp;                   // key for moving up
            coreInt16 aiButton[INPUT_BUTTONS];   // input buttons (<=0 for mouse)
        }
        aSet[INPUT_SETS];
    }
    Input;
};

struct cInput
{
    coreVector2 vMove;            // movement input
    coreUint8   iButtonPress;     // button press (bitfields)
    coreUint8   iButtonRelease;   // button release
    coreUint8   iButtonHold;      // button hold
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