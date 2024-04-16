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
// TODO 1: [MF] controller input mapping should be tied to the GUID
// TODO 3: SDL_JoystickCurrentPowerLevel (will it disconnect automatically if empty (triggering focus-loss event), or is manual checking required ?)
// TODO 3: AnyButton should not get triggered by g_MenuInput.bScreenshot (in engine ?)
// TODO 3: last-used input type might get disconnected, without reset
// TODO 3: how to properly go back to the correct last-input keyboard, if player just uses mouse, currently it's initialized to 0, but gets set to the set with the mouse-button when navigating menu with mouse
// TODO 3: HRTF from core-config (improves spatial acuity with headphones, makes only sense with 3D sound)
// TODO 3: [MF] vielleicht bei toggle fire-mode single-feuer solange bei aktivierung gedrückt gehalten wird
// TODO 3: weitere optionen: particle effects (%)
// TODO 1: [MF] test if disable back rotation and set speed option works everywhere: tiger, zeroth, messier, p1 (blue and magenta), rutilus-rotation
// TODO 3: EMSCRIPTEN: gamepad calibration feature required, buttons on different gamepads are always different -> man muss aber alle buttons sehn können, damit man fire-up, fire-down etc. versteht  (or warning: gamepads might not work properly, due to browser limitations, for better support, please download the desktop versions for Windows, Linux, or macOS


// ****************************************************************
// configuration definitions
#define CONFIG_FORCE   // # force specific settings (for fairness)

#define CONFIG_GRAPHICS_RENDER       "Graphics", "Render",                               (1)
#define CONFIG_GRAPHICS_SHADOW       "Graphics", "Shadow",                               (2)
#define CONFIG_GRAPHICS_REFLECTION   "Graphics", "Reflection",                           (1)
#define CONFIG_GRAPHICS_GLOW         "Graphics", "Glow",                                 (1)
#define CONFIG_GRAPHICS_DISTORTION   "Graphics", "Distortion",                           (1)
#define CONFIG_GRAPHICS_SHAKE        "Graphics", "Shake",                                (100)
#define CONFIG_GRAPHICS_FLASH        "Graphics", "Flash",                                (1)
#define CONFIG_GRAPHICS_HIT_STOP     "Graphics", "HitStop",                              (1)
#define CONFIG_GRAPHICS_CHROMA       "Graphics", "Chroma",                               (1)

#define CONFIG_AUDIO_EFFECT_VOLUME   "Audio",    "EffectVolume",                         (1.0f)
#define CONFIG_AUDIO_AMBIENT_VOLUME  "Audio",    "AmbientVolume",                        (1.0f)
#define CONFIG_AUDIO_MENU_VOLUME     "Audio",    "MenuVolume",                           (1.0f)
#define CONFIG_AUDIO_QUALITY         "Audio",    "Quality",                              (0)
#define CONFIG_AUDIO_3D_SOUND        "Audio",    "3DSound",                              (1)

#define CONFIG_INPUT_TYPE(p)         "Input",    PRINT("P%zu_Type",        (p)),         (p + INPUT_SETS_KEYBOARD)
#define CONFIG_INPUT_RUMBLE(p)       "Input",    PRINT("P%zu_Rumble",      (p)),         (0)
#define CONFIG_INPUT_FIRE_MODE(p)    "Input",    PRINT("P%zu_FireMode",    (p)),         (0)
#define CONFIG_INPUT_CONTROL_MODE(p) "Input",    PRINT("P%zu_ControlMode", (p)),         (1)
#define CONFIG_INPUT_MOVE_UP(s)      "Input",    PRINT("S%zu_MoveUp",      (s))
#define CONFIG_INPUT_MOVE_LEFT(s)    "Input",    PRINT("S%zu_MoveLeft",    (s))
#define CONFIG_INPUT_MOVE_DOWN(s)    "Input",    PRINT("S%zu_MoveDown",    (s))
#define CONFIG_INPUT_MOVE_RIGHT(s)   "Input",    PRINT("S%zu_MoveRight",   (s))
#define CONFIG_INPUT_ACTION(s,n)     "Input",    PRINT("S%zu_Action%zu",   (s), (n) + 1u)

#define CONFIG_GAME_GAME_DIRECTION   "Game",     "GameDirection",                        (0)
#define CONFIG_GAME_GAME_SPEED       "Game",     "GameSpeed",                            (100)
#define CONFIG_GAME_MIRROR_MODE      "Game",     "MirrorMode",                           (0)
#define CONFIG_GAME_HUD_DIRECTION    "Game",     "HudDirection",                         (0)
#define CONFIG_GAME_HUD_TYPE         "Game",     "HudType",                              (0)
#define CONFIG_GAME_COMBAT_TEXT      "Game",     "CombatText",                           (0xFF)
#define CONFIG_GAME_BACK_ROTATION    "Game",     "BackRotation",                         (1)
#define CONFIG_GAME_BACK_SPEED       "Game",     "BackSpeed",                            (100)
#define CONFIG_GAME_UPDATE_FREQ      "Game",     "UpdateFreq",                           (0)
#define CONFIG_GAME_VERSION          "Game",     "Version",                              (0)

#define INPUT_TYPES         (PLAYERS)                                     // number of input set selections
#define INPUT_KEYS_MOVE     (4u)                                          // number of move keys per set (left, right, down, up)
#define INPUT_KEYS_ACTION   (8u)                                          // number of action keys per set
#define INPUT_KEYS          (INPUT_KEYS_MOVE + INPUT_KEYS_ACTION)         // total number of keys
#define INPUT_SETS_KEYBOARD (INPUT_TYPES)                                 // number of keyboard and mouse sets
#define INPUT_SETS_JOYSTICK (4u)                                          // number of joystick/gamepad sets
#define INPUT_SETS          (INPUT_SETS_KEYBOARD + INPUT_SETS_JOYSTICK)   // total number of sets

STATIC_ASSERT(INPUT_TYPES       <= INPUT_SETS)
STATIC_ASSERT(INPUT_KEYS_ACTION <= sizeof(coreUint8)*8u)

#define DEFAULT_KEYBOARD_1_MOVE_UP    (CORE_INPUT_KEY(W))
#define DEFAULT_KEYBOARD_1_MOVE_LEFT  (CORE_INPUT_KEY(A))
#define DEFAULT_KEYBOARD_1_MOVE_DOWN  (CORE_INPUT_KEY(S))
#define DEFAULT_KEYBOARD_1_MOVE_RIGHT (CORE_INPUT_KEY(D))
#define DEFAULT_KEYBOARD_1_ACTION(n)  (((n) == 0u) ? -1 : ((n) == 1u) ? CORE_INPUT_KEY(Q) : ((n) == 2u) ? CORE_INPUT_KEY(E) : ((n) == 3u) ? CORE_INPUT_KEY(I) : ((n) == 4u) ? CORE_INPUT_KEY(J) : ((n) == 5u) ? CORE_INPUT_KEY(K) : ((n) == 6u) ? CORE_INPUT_KEY(L) : 0)

#define DEFAULT_KEYBOARD_2_MOVE_UP    (CORE_INPUT_KEY(UP))
#define DEFAULT_KEYBOARD_2_MOVE_LEFT  (CORE_INPUT_KEY(LEFT))
#define DEFAULT_KEYBOARD_2_MOVE_DOWN  (CORE_INPUT_KEY(DOWN))
#define DEFAULT_KEYBOARD_2_MOVE_RIGHT (CORE_INPUT_KEY(RIGHT))
#define DEFAULT_KEYBOARD_2_ACTION(n)  (((n) == 0u) ? CORE_INPUT_KEY(SPACE) : ((n) == 1u) ? CORE_INPUT_KEY(Z) : ((n) == 2u) ? CORE_INPUT_KEY(X) : ((n) == 3u) ? CORE_INPUT_KEY(KP_8) : ((n) == 4u) ? CORE_INPUT_KEY(KP_4) : ((n) == 5u) ? CORE_INPUT_KEY(KP_2) : ((n) == 6u) ? CORE_INPUT_KEY(KP_6) : 0)

#define DEFAULT_JOYSTICK_MOVE_UP      (0)
#define DEFAULT_JOYSTICK_MOVE_LEFT    (0)
#define DEFAULT_JOYSTICK_MOVE_DOWN    (0)
#define DEFAULT_JOYSTICK_MOVE_RIGHT   (0)
#define DEFAULT_JOYSTICK_ACTION(n)    (((n) == 0u) ? SDL_CONTROLLER_BUTTON_A : (((n) == 1u) ? SDL_CONTROLLER_BUTTON_LEFTSHOULDER : (((n) == 2u) ? SDL_CONTROLLER_BUTTON_RIGHTSHOULDER : ((n) == 3u) ? SDL_CONTROLLER_BUTTON_Y : ((n) == 4u) ? SDL_CONTROLLER_BUTTON_X : ((n) == 5u) ? SDL_CONTROLLER_BUTTON_A : ((n) == 6u) ? SDL_CONTROLLER_BUTTON_B : SDL_CONTROLLER_BUTTON_START)))

#define DEFAULT_MOVE_UP(x)            (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_UP    : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_UP    : DEFAULT_JOYSTICK_MOVE_UP)
#define DEFAULT_MOVE_LEFT(x)          (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_LEFT  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_LEFT  : DEFAULT_JOYSTICK_MOVE_LEFT)
#define DEFAULT_MOVE_DOWN(x)          (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_DOWN  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_DOWN  : DEFAULT_JOYSTICK_MOVE_DOWN)
#define DEFAULT_MOVE_RIGHT(x)         (((x) == 0u) ? DEFAULT_KEYBOARD_1_MOVE_RIGHT : ((x) == 1u) ? DEFAULT_KEYBOARD_2_MOVE_RIGHT : DEFAULT_JOYSTICK_MOVE_RIGHT)
#define DEFAULT_ACTION(x,n)           (((x) == 0u) ? DEFAULT_KEYBOARD_1_ACTION(n)  : ((x) == 1u) ? DEFAULT_KEYBOARD_2_ACTION(n)  : DEFAULT_JOYSTICK_ACTION(n))

STATIC_ASSERT(INPUT_SETS_KEYBOARD == 2u)


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
        coreUint8 iChroma;       // 
    }
    Graphics;

    struct
    {
        coreFloat fEffectVolume;    // effect sound volume
        coreFloat fAmbientVolume;   // ambient sound volume
        coreFloat fMenuVolume;      // menu sound volume
        coreUint8 iQuality;         // 
        coreUint8 i3DSound;         // 
    }
    Audio;

    struct
    {
        coreUint8 aiType       [INPUT_TYPES];        // selected input set (each element should have a different value)
        coreUint8 aiRumble     [INPUT_TYPES];        // 
        coreUint8 aiFireMode   [INPUT_TYPES];        // 
        coreUint8 aiControlMode[INPUT_TYPES];        // 
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
        coreUint8  iGameDirection;   // 
        coreUint8  iGameSpeed;       // 
        coreUint8  iMirrorMode;      // 
        coreUint8  iHudDirection;    // 
        coreUint8  iHudType;         // 
        coreUint8  iCombatText;      // 
        coreUint8  iBackRotation;    // 
        coreUint8  iBackSpeed;       // 
        coreUint16 iUpdateFreq;      //   TODO 1: cannot be handled by switchbox8 (make sure to handle cMenu::UpdateSwitchBox)
        coreUint16 iVersion;         //   TODO 1: cannot be handled by switchbox8
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
extern coreUint8  g_iTotalType;                // 
extern sMenuInput g_MenuInput;                 // 


// ****************************************************************
// configuration functions
extern void LoadConfig();
extern void SaveConfig();
extern void UpdateInput();


#endif // _P1_GUARD_CONFIG_H_