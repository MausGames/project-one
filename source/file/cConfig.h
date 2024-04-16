///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CONFIG_H_
#define _P1_GUARD_CONFIG_H_

// TODO 1: transfer to Core Engine
// TODO 5: should low quality option also affect ship models, or vegetation density, or sprite density, or FB resolution ?
// TODO 5: don't input-check inactive input-sets in multiplayer
// TODO 1: default input should differ between keyboard and joystick, and between sets
// TODO 3: SDL_JoystickCurrentPowerLevel (will it disconnect automatically if empty (triggering focus-loss event), or is manual checking required ?)
// TODO 3: AnyButton should not get triggered by g_MenuInput.bScreenshot (in engine ?)


// ****************************************************************
// configuration definitions
#define CONFIG_FORCE   // # force specific settings (for fairness)

#define CONFIG_GRAPHICS_RENDER      "Graphics", "Render",                               (1)
#define CONFIG_GRAPHICS_SHADOW      "Graphics", "Shadow",                               (1)
#define CONFIG_GRAPHICS_REFLECTION  "Graphics", "Reflection",                           (1)
#define CONFIG_GRAPHICS_GLOW        "Graphics", "Glow",                                 (1)
#define CONFIG_GRAPHICS_DISTORTION  "Graphics", "Distortion",                           (1)
#define CONFIG_GRAPHICS_SHAKE       "Graphics", "Shake",                                (100)
#define CONFIG_GRAPHICS_FLASH       "Graphics", "Flash",                                (1)

#define CONFIG_AUDIO_EFFECT_VOLUME  "Audio",    "EffectVolume",                         (1.0f)
#define CONFIG_AUDIO_AMBIENT_VOLUME "Audio",    "AmbientVolume",                        (1.0f)

#define CONFIG_INPUT_TYPE(p)        "Input",    PRINT("P%zu_Type",      (p)),           (p)
#define CONFIG_INPUT_RUMBLE(p)      "Input",    PRINT("P%zu_Rumble",    (p)),           (0)
#define CONFIG_INPUT_FIRE_MODE(p)   "Input",    PRINT("P%zu_FireMode",  (p)),           (0)
#define CONFIG_INPUT_MOVE_UP(s)     "Input",    PRINT("S%zu_MoveUp",    (s)),           (CORE_INPUT_KEY(W))
#define CONFIG_INPUT_MOVE_LEFT(s)   "Input",    PRINT("S%zu_MoveLeft",  (s)),           (CORE_INPUT_KEY(A))
#define CONFIG_INPUT_MOVE_DOWN(s)   "Input",    PRINT("S%zu_MoveDown",  (s)),           (CORE_INPUT_KEY(S))
#define CONFIG_INPUT_MOVE_RIGHT(s)  "Input",    PRINT("S%zu_MoveRight", (s)),           (CORE_INPUT_KEY(D))
#define CONFIG_INPUT_ACTION(s,n)    "Input",    PRINT("S%zu_Action%zu", (s), (n) + 1u), ((n) + coreUintW(CORE_INPUT_KEY(1)))

#define CONFIG_GAME_TEXT_SIZE       "Game",     "TextSize",                             (0)
#define CONFIG_GAME_GAME_ROTATION   "Game",     "GameRotation",                         (0)
#define CONFIG_GAME_GAME_SCALE      "Game",     "GameScale",                            (100)
#define CONFIG_GAME_GAME_SPEED      "Game",     "GameSpeed",                            (100)
#define CONFIG_GAME_HUD_ROTATION    "Game",     "HudRotation",                          (0)
#define CONFIG_GAME_HUD_SCALE       "Game",     "HudScale",                             (100)
#define CONFIG_GAME_HUD_TYPE        "Game",     "HudType",                              (0)
#define CONFIG_GAME_UPDATE_FREQ     "Game",     "UpdateFreq",                           (60)//(120) TODO 1: 120 makes it slow in menu, on debug
#define CONFIG_GAME_VERSION         "Game",     "Version",                              (0)
#define CONFIG_GAME_MIRROR_MODE     "Game",     "MirrorMode",                           (0)

#define INPUT_TYPES         (PLAYERS)                                     // number of input set selections
#define INPUT_KEYS_MOVE     (4u)                                          // number of move keys per set (left, right, down, up)
#define INPUT_KEYS_ACTION   (4u)                                          // number of action keys per set
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
        coreUint8 iRender;       // render level
        coreUint8 iShadow;       // shadow level
        coreUint8 iReflection;   // water reflection level
        coreUint8 iGlow;         // fullscreen glow level
        coreUint8 iDistortion;   // fullscreen distortion level
        coreUint8 iShake;        // 
        coreUint8 iFlash;        // 
    }
    Graphics;

    struct
    {
        coreFloat fEffectVolume;    // effect sound volume
        coreFloat fAmbientVolume;   // ambient sound volume
    }
    Audio;

    struct
    {
        coreUint8 aiType    [INPUT_TYPES];           // selected input set (each element should have a different value)
        coreUint8 aiRumble  [INPUT_TYPES];           // 
        coreUint8 aiFireMode[INPUT_TYPES];           // 
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
        coreUint8  iTextSize;       // 
        coreUint8  iGameRotation;   // 
        coreUint8  iGameScale;      // 
        coreUint8  iGameSpeed;      // 
        coreUint8  iHudRotation;    // 
        coreUint8  iHudScale;       // 
        coreUint8  iHudType;        // 
        coreUint16 iUpdateFreq;     // 
        coreUint16 iVersion;        // 
        coreUint8  iMirrorMode;     // 
    }
    Game;
};

struct sGameInput final
{
    coreVector2 vMove;            // movement input
    coreUint8   iActionPress;     // action press (bitfields)
    coreUint8   iActionRelease;   // action release
    coreUint8   iActionHold;      // action hold
    coreUint8   iStatus;          // 
};

struct sMenuInput final
{
    coreUint8 iMove;         // 
    coreBool  bAccept;       // 
    coreBool  bCancel;       // 
    coreBool  bPause;        // 
    coreBool  bScreenshot;   // 
};

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