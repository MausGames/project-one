///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CONFIG_H_
#define _P1_GUARD_CONFIG_H_

// TODO 1: transfer to Core Engine
// TODO 5: should low quality option also affect ship models, or vegetation density, or sprite density, or FB resolution ?
// TODO 5: don't input-check inactive input-sets in multiplayer
// TODO 1: default input should differ between keyboard and joystick, and between sets
// TODO 1: controller input mapping should use gamepad-actions instead of button-IDs
// TODO 1: controller input mapping should be tied to the GUID
// TODO 3: SDL_JoystickCurrentPowerLevel (will it disconnect automatically if empty (triggering focus-loss event), or is manual checking required ?)
// TODO 3: AnyButton should not get triggered by g_MenuInput.bScreenshot (in engine ?)
// TODO 2: some options, like mirror-mode, should only take affect if it is allowed


// ****************************************************************
// configuration definitions
#define CONFIG_FORCE   // # force specific settings (for fairness)

#define CONFIG_GRAPHICS_RENDER      "Graphics", "Render",                               (1)
#define CONFIG_GRAPHICS_SHADOW      "Graphics", "Shadow",                               (2)
#define CONFIG_GRAPHICS_REFLECTION  "Graphics", "Reflection",                           (1)
#define CONFIG_GRAPHICS_GLOW        "Graphics", "Glow",                                 (1)
#define CONFIG_GRAPHICS_DISTORTION  "Graphics", "Distortion",                           (1)
#define CONFIG_GRAPHICS_SHAKE       "Graphics", "Shake",                                (100)
#define CONFIG_GRAPHICS_FLASH       "Graphics", "Flash",                                (1)
#define CONFIG_GRAPHICS_HIT_STOP    "Graphics", "HitStop",                              (1)

#define CONFIG_AUDIO_EFFECT_VOLUME  "Audio",    "EffectVolume",                         (1.0f)
#define CONFIG_AUDIO_AMBIENT_VOLUME "Audio",    "AmbientVolume",                        (1.0f)
#define CONFIG_AUDIO_3D_SOUND       "Audio",    "3DSound",                              (1)
// TODO 1: HRTF from core-config  (might improve headphone sound quality)
// TODO 1: turn effect
// TODO 1: shooting effect

#define CONFIG_INPUT_TYPE(p)        "Input",    PRINT("P%zu_Type",      (p)),           (p + INPUT_SETS_KEYBOARD)
#define CONFIG_INPUT_RUMBLE(p)      "Input",    PRINT("P%zu_Rumble",    (p)),           (0)
#define CONFIG_INPUT_FIRE_MODE(p)   "Input",    PRINT("P%zu_FireMode",  (p)),           (0)
#define CONFIG_INPUT_MOVE_UP(s)     "Input",    PRINT("S%zu_MoveUp",    (s))
#define CONFIG_INPUT_MOVE_LEFT(s)   "Input",    PRINT("S%zu_MoveLeft",  (s))
#define CONFIG_INPUT_MOVE_DOWN(s)   "Input",    PRINT("S%zu_MoveDown",  (s))
#define CONFIG_INPUT_MOVE_RIGHT(s)  "Input",    PRINT("S%zu_MoveRight", (s))
#define CONFIG_INPUT_ACTION(s,n)    "Input",    PRINT("S%zu_Action%zu", (s), (n) + 1u)

#define CONFIG_GAME_TEXT_SIZE       "Game",     "TextSize",                             (0)
#define CONFIG_GAME_GAME_ROTATION   "Game",     "GameRotation",                         (0)
#define CONFIG_GAME_GAME_SCALE      "Game",     "GameScale",                            (100)
#define CONFIG_GAME_GAME_SPEED      "Game",     "GameSpeed",                            (100)
#define CONFIG_GAME_HUD_ROTATION    "Game",     "HudRotation",                          (0)
#define CONFIG_GAME_HUD_SCALE       "Game",     "HudScale",                             (100)
#define CONFIG_GAME_HUD_TYPE        "Game",     "HudType",                              (0)
#define CONFIG_GAME_UPDATE_FREQ     "Game",     "UpdateFreq",                           (0)//((60)//(120) TODO 1: 120 makes it slow in menu, on debug
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

#define DEFAULT_KEYBOARD_1_MOVE_UP    (CORE_INPUT_KEY(W))
#define DEFAULT_KEYBOARD_1_MOVE_LEFT  (CORE_INPUT_KEY(A))
#define DEFAULT_KEYBOARD_1_MOVE_DOWN  (CORE_INPUT_KEY(S))
#define DEFAULT_KEYBOARD_1_MOVE_RIGHT (CORE_INPUT_KEY(D))
#define DEFAULT_KEYBOARD_1_ACTION(n)  (((n) == 0u) ? -1 : (((n) == 1u) ? CORE_INPUT_KEY(Q) : (((n) == 2u) ? CORE_INPUT_KEY(E) : 0)))

#define DEFAULT_KEYBOARD_2_MOVE_UP    (CORE_INPUT_KEY(UP))
#define DEFAULT_KEYBOARD_2_MOVE_LEFT  (CORE_INPUT_KEY(LEFT))
#define DEFAULT_KEYBOARD_2_MOVE_DOWN  (CORE_INPUT_KEY(DOWN))
#define DEFAULT_KEYBOARD_2_MOVE_RIGHT (CORE_INPUT_KEY(RIGHT))
#define DEFAULT_KEYBOARD_2_ACTION(n)  (((n) == 0u) ? CORE_INPUT_KEY(SPACE) : (((n) == 1u) ? CORE_INPUT_KEY(Z) : (((n) == 2u) ? CORE_INPUT_KEY(X) : 0)))

#define DEFAULT_JOYSTICK_1_MOVE_UP    (0)
#define DEFAULT_JOYSTICK_1_MOVE_LEFT  (0)
#define DEFAULT_JOYSTICK_1_MOVE_DOWN  (0)
#define DEFAULT_JOYSTICK_1_MOVE_RIGHT (0)
#define DEFAULT_JOYSTICK_1_ACTION(n)  (((n) == 0u) ? SDL_CONTROLLER_BUTTON_A : (((n) == 1u) ? SDL_CONTROLLER_BUTTON_LEFTSHOULDER : (((n) == 2u) ? SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : SDL_CONTROLLER_BUTTON_START)))

#define DEFAULT_JOYSTICK_2_MOVE_UP    (0)
#define DEFAULT_JOYSTICK_2_MOVE_LEFT  (0)
#define DEFAULT_JOYSTICK_2_MOVE_DOWN  (0)
#define DEFAULT_JOYSTICK_2_MOVE_RIGHT (0)
#define DEFAULT_JOYSTICK_2_ACTION(n)  (((n) == 0u) ? SDL_CONTROLLER_BUTTON_A : (((n) == 1u) ? SDL_CONTROLLER_BUTTON_LEFTSHOULDER : (((n) == 2u) ? SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : SDL_CONTROLLER_BUTTON_START)))

#define DEFAULT_MOVE_UP(x)    (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_UP    : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_UP    : ((x) == 2u) ? DEFAULT_JOYSTICK_1_MOVE_UP    : DEFAULT_JOYSTICK_2_MOVE_UP)
#define DEFAULT_MOVE_LEFT(x)  (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_LEFT  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_LEFT  : ((x) == 2u) ? DEFAULT_JOYSTICK_1_MOVE_LEFT  : DEFAULT_JOYSTICK_2_MOVE_LEFT)
#define DEFAULT_MOVE_DOWN(x)  (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_DOWN  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_DOWN  : ((x) == 2u) ? DEFAULT_JOYSTICK_1_MOVE_DOWN  : DEFAULT_JOYSTICK_2_MOVE_DOWN)
#define DEFAULT_MOVE_RIGHT(x) (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_RIGHT : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_RIGHT : ((x) == 2u) ? DEFAULT_JOYSTICK_1_MOVE_RIGHT : DEFAULT_JOYSTICK_2_MOVE_RIGHT)
#define DEFAULT_ACTION(x,n)   (((x) == 0u) ? DEFAULT_KEYBOARD_1_ACTION(n)  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_ACTION(n)  : ((x) == 2u) ? DEFAULT_JOYSTICK_1_ACTION(n)  : DEFAULT_JOYSTICK_2_ACTION(n))


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
        coreUint8 iHitStop;      // 
    }
    Graphics;

    struct
    {
        coreFloat fEffectVolume;    // effect sound volume
        coreFloat fAmbientVolume;   // ambient sound volume
        coreUint8 i3DSound;         // 
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
        coreUint16 iUpdateFreq;     //   TODO 1: cannot be handled by switchbox8 (make sure to handle cMenu::UpdateSwitchBox)
        coreUint16 iVersion;        //   TODO 1: cannot be handled by switchbox8
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