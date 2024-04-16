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
// TODO: should low quality option also affect ship models ?
// TODO: don't input-check for inactive players during game
// TODO: default input should differ between keyboard and joystick, and between sets
// TODO: SDL_JoystickCurrentPowerLevel
// TODO: alt+F4 while having temp changes in the options menu should not save them


// ****************************************************************
// configuration definitions
#define CONFIG_FORCE   // # force specific settings (to increase fairness)

#define CONFIG_GRAPHICS_SHADOW     "Graphics", "Shadow",                               (1)
#define CONFIG_GRAPHICS_REFLECTION "Graphics", "Reflection",                           (1)
#define CONFIG_GRAPHICS_GLOW       "Graphics", "Glow",                                 (1)
#define CONFIG_GRAPHICS_DISTORTION "Graphics", "Distortion",                           (1)

#define CONFIG_AUDIO_AMBIENT       "Audio",    "Ambient",                              (1)

#define CONFIG_INPUT_TYPE(p)       "Input",    PRINT("P%zu_Type",      (p)),           (p)
#define CONFIG_INPUT_RUMBLE(p)     "Input",    PRINT("P%zu_Rumble",    (p)),           (0)
#define CONFIG_INPUT_MOVE_UP(s)    "Input",    PRINT("S%zu_MoveUp",    (s)),           (CORE_INPUT_KEY(W))
#define CONFIG_INPUT_MOVE_LEFT(s)  "Input",    PRINT("S%zu_MoveLeft",  (s)),           (CORE_INPUT_KEY(A))
#define CONFIG_INPUT_MOVE_DOWN(s)  "Input",    PRINT("S%zu_MoveDown",  (s)),           (CORE_INPUT_KEY(S))
#define CONFIG_INPUT_MOVE_RIGHT(s) "Input",    PRINT("S%zu_MoveRight", (s)),           (CORE_INPUT_KEY(D))
#define CONFIG_INPUT_ACTION(s,n)   "Input",    PRINT("S%zu_Action%zu", (s), (n) + 1u), ((n) + coreUintW(CORE_INPUT_KEY(1)))

#define CONFIG_GAME_TRANSPARENT    "Game",     "Transparent",                          (0)
#define CONFIG_GAME_COMBAT_DAMAGE  "Game",     "CombatDamage",                         (1)
#define CONFIG_GAME_COMBAT_CHAIN   "Game",     "CombatChain",                          (1)
#define CONFIG_GAME_COMBAT_COMBO   "Game",     "CombatCombo",                          (1)

#define INPUT_TYPES         (PLAYERS)                                     // number of input set selections
#define INPUT_KEYS_MOVE     (4u)                                          // number of move keys per set (left, right, down, up)
#define INPUT_KEYS_ACTION   (5u)                                          // number of action keys per set
#define INPUT_KEYS          (INPUT_KEYS_MOVE + INPUT_KEYS_ACTION)         // total number of keys
#define INPUT_SETS_KEYBOARD (INPUT_TYPES)                                 // number of keyboard and mouse sets
#define INPUT_SETS_JOYSTICK (INPUT_TYPES)                                 // number of joystick/gamepad sets
#define INPUT_SETS          (INPUT_SETS_KEYBOARD + INPUT_SETS_JOYSTICK)   // total number of sets

STATIC_ASSERT(INPUT_TYPES       <= INPUT_SETS)
STATIC_ASSERT(INPUT_KEYS_ACTION <= sizeof(coreUint8)*8u)


// ****************************************************************
// configuration variables
struct sConfig final
{
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
        coreUint8 iAmbient;   // ambient sound effects
    }
    Audio;

    struct
    {
        coreUint8 aiType  [INPUT_TYPES];             // selected input set (each element should have a different value)
        coreUint8 aiRumble[INPUT_TYPES];             // 
        struct
        {
            coreInt16 iMoveUp;                       // key for moving up (ignored on joystick/gamepad)
            coreInt16 iMoveLeft;                     // key for moving left
            coreInt16 iMoveDown;                     // key for moving down
            coreInt16 iMoveRight;                    // key for moving right
            coreInt16 aiAction[INPUT_KEYS_ACTION];   // action keys (<=0 for mouse)
        }
        aSet[INPUT_SETS];
    }
    Input;

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
};

struct sGameInput final
{
    coreVector2 vMove;            // movement input
    coreUint8   iActionPress;     // action press (bitfields)
    coreUint8   iActionRelease;   // action release
    coreUint8   iActionHold;      // action hold
};

struct sMenuInput final
{
    coreUint8 iMove       : 3;   // 
    coreBool  bAccept     : 1;   // 
    coreBool  bCancel     : 1;   // 
    coreBool  bPause      : 1;   // 
    coreBool  bScreenshot : 1;   // 
};
STATIC_ASSERT(sizeof(sMenuInput) == 1u)

extern sConfig    g_CurConfig;                 // current and active configuration structure
extern sConfig    g_OldConfig;                 // handle for old values
extern sGameInput g_aGameInput[INPUT_TYPES];   // mapped game input values (ready for direct use)
extern sGameInput g_TotalInput;                // combination of all mapped game input values from all available input sets
extern sMenuInput g_MenuInput;                 // 


// ****************************************************************
// configuration functions
extern void LoadConfig();
extern void SaveConfig();
extern void UpdateInput();


#endif // _P1_GUARD_CONFIG_H_