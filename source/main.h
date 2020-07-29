/////////////////////////////////////////////////////////////////////////////////////
//*-------------------------------------------------------------------------------*//
//|    _____  _____   ____       _ ______ _____ _______     ____  _   _ ______    |//
//|   |  __ \|  __ \ / __ \     | |  ____/ ____|__   __|   / __ \| \ | |  ____|   |//
//|   | |__) | |__) | |  | |    | | |__ | |       | |     | |  | |  \| | |__      |//
//|   |  ___/|  _  /| |  | |_   | |  __|| |       | |     | |  | |     |  __|     |//
//|   | |    | | \ \| |__| | |__| | |___| |____   | |     | |__| | |\  | |____    |//
//|   |_|    |_|  \_\\____/ \____/|______\_____|  |_|      \____/|_| \_|______|   |//
//|                                                                               |//
//*-------------------------------------------------------------------------------*//
/////////////////////////////////////////////////////////////////////////////////////
//*-------------------------------------------------------------------------------*//
//| Project One v0.1.0a (https://www.maus-games.at)                               |//
//*-------------------------------------------------------------------------------*//
//| Copyright (c) 2010-2020 Martin Mauersics                                      |//
//|                                                                               |//
//| This software is provided 'as-is', without any express or implied             |//
//| warranty. In no event will the authors be held liable for any damages         |//
//| arising from the use of this software.                                        |//
//|                                                                               |//
//| Permission is granted to anyone to use this software for any purpose,         |//
//| including commercial applications, and to alter it and redistribute it        |//
//| freely, subject to the following restrictions:                                |//
//|                                                                               |//
//|   1. The origin of this software must not be misrepresented; you must not     |//
//|   claim that you wrote the original software. If you use this software        |//
//|   in a product, an acknowledgment in the product documentation would be       |//
//|   appreciated but is not required.                                            |//
//|                                                                               |//
//|   2. Altered source versions must be plainly marked as such, and must not be  |//
//|   misrepresented as being the original software.                              |//
//|                                                                               |//
//|   3. This notice may not be removed or altered from any source                |//
//|   distribution.                                                               |//
//*-------------------------------------------------------------------------------*//
/////////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MAIN_H_
#define _P1_GUARD_MAIN_H_

// TODO: create timer and int-value as tick-multiplier for sustained damage
// TODO: remove magic numbers (regularly)
// TODO: test framerate-lock for g-sync stuff, also test for 144hz displays if render x144 but move x60 is better
// TODO: clean up shader modifiers and shaders, also try to look at unused uniforms, varyings and attributes (shadow-matrix is used in ship-shader !? a_v1Blink used in ground-shader but only when instancing), and reduce passing data across shader stages
// TODO: implement static/coherent branching interface instead of many shader-permutations ? (maybe only in situations with frequent switching)
// TODO: use single-channel texture where possible
// TODO: menu optimization by caching into framebuffer (general class for leaderboard, options, etc.)
// TODO: protect main (LockFramerate)
// TODO: check all shaders if alpha is required
// TODO: full initializer lists everywhere
// TODO: clean mixing shader defines (x >= y) and (defined(x)) checks (also in engine)
// TODO: check for 16-bit float shader usage
// TODO: program enable has to be checked (if(x.Enable()){}) everywhere
// TODO: change 0.5 FB factors from 0.5 to 0.4 (-36% pixel) if CORE_GL_SUPPORT(ARB_texture_rg) not available ?
// TODO: unify "forward" and "transform" comments in shaders
// TODO: add own coreRand for various random things which may affect feeling (screen shake), and reset on boss-start
// TODO: check issues with all the F&& functions (especially in boss.h and mission.h), also check Core engine, use force_inline on small functions
// TODO: RETURN_NONNULL to everything which should never be null (and other attributes, both FUNC and RETURN)
// TODO: check all vert shader for CORE_SHADER_OPTION_NO_ROTATION
// TODO: control flow guard and buffer security check
// TODO: _CORE_SSE_ try to just remove the coreMath low-precision functions
// TODO: reduce number of shader-lights with static_assert
// TODO: check if hole in object_sphere causes reflection issues, also check if other objects have a hole
// TODO: on bosses and missions: don't move or render or test objects outside their phases (e.g. boomerangs)
// TODO: make sure everything with at least 5 copies uses batch-lists
// TODO: change SendUniform(PRINT("u_av3OverlayTransform[%zu]", i) to cached hashstrings like in coreShader
// TODO: replace / with RCP where possible
// TODO: "pro" shortcut for types (e.g. proEnemy, proGame), dr too
// TODO: remove game_icon.png from resource-index if not required anymore
// TODO: ENABLE_BITWISE when ?
// TODO: remove multisampling for 2d, though may cause artifacts if the objects are fully shaded and moved
// TODO: transition shader only needs alpha for menu, but not for background -> create permutations


// ****************************************************************
// engine headers
#include "Core.h"

#define _P1_DEBUG_INPUT_ (1)
//#define _P1_DEBUG_RANDOM_ (1)

#if !defined(_CORE_DEBUG_)
    #if defined(_CORE_SSE_)
        #pragma message("Warning: Precision reduced!")
    #endif
    #if defined(_P1_DEBUG_INPUT_)
        #pragma message("Warning: Debug input enabled!")
    #endif
    #if defined(_P1_DEBUG_RANDOM_)
        #pragma message("Warning: Debug randomization enabled!")
    #endif
#endif


// ****************************************************************
// general definitions
#define PLAYERS              (2u)
#define MISSIONS             (9u + 1u)
#define BOSSES               (3u)
#define WAVES                (15u)
#define SEGMENTS             (BOSSES + WAVES)
#define LIVES                (5u)
#define CONTINUES            (3u)
#define SHIELD               (100u)
#define WEAPONS              (6u)
#define SUPPORTS             (2u)
#define EQUIP_WEAPONS        (1u)
#define EQUIP_SUPPORTS       (1u)
#define FRAMERATE_MIN        (60.0f)
#define FRAMERATE_MAX        (240.0f)
#define CAMERA_POSITION      (coreVector3(0.0f, 0.0f, 110.0f))
#define CAMERA_DIRECTION     (coreVector3(0.0f, 0.0f,  -1.0f))
#define CAMERA_ORIENTATION   (coreVector3(0.0f, 1.0f,   0.0f))
#define LIGHT_DIRECTION      (coreVector3(0.583953857f, -0.642349243f, -0.496360779f))

// color values
#define COLOR_MENU_WHITE     (coreVector3(1.000f, 1.000f, 1.000f) * MENU_CONTRAST_WHITE)
#define COLOR_MENU_BLACK     (coreVector3(1.000f, 1.000f, 1.000f) * MENU_CONTRAST_BLACK)
#define COLOR_MENU_YELLOW    (coreVector3(1.000f, 0.824f, 0.392f))
#define COLOR_MENU_ORANGE    (coreVector3(1.000f, 0.443f, 0.227f))
#define COLOR_MENU_RED       (coreVector3(1.000f, 0.275f, 0.275f))
#define COLOR_MENU_PURPLE    (coreVector3(0.710f, 0.333f, 1.000f))
#define COLOR_MENU_BLUE      (coreVector3(0.102f, 0.702f, 1.000f))
#define COLOR_MENU_GREEN     (coreVector3(0.118f, 0.745f, 0.353f))
#define COLOR_MENU_BRONZE    (coreVector3(0.925f, 0.663f, 0.259f))
#define COLOR_MENU_SILVER    (coreVector3(0.855f, 0.855f, 0.878f))
#define COLOR_MENU_GOLD      (coreVector3(1.000f, 0.859f, 0.000f))
#define COLOR_ENERGY_WHITE   (coreVector3(1.000f, 1.000f, 1.000f))
#define COLOR_ENERGY_YELLOW  (coreVector3(0.950f, 0.800f, 0.280f))
#define COLOR_ENERGY_ORANGE  (coreVector3(1.000f, 0.420f, 0.000f))
#define COLOR_ENERGY_RED     (coreVector3(1.000f, 0.290f, 0.290f))
#define COLOR_ENERGY_MAGENTA (coreVector3(1.000f, 0.310f, 0.650f))
#define COLOR_ENERGY_PURPLE  (coreVector3(0.450f, 0.200f, 1.000f))
#define COLOR_ENERGY_BLUE    (coreVector3(0.100f, 0.430f, 1.000f))
#define COLOR_ENERGY_CYAN    (coreVector3(0.184f, 0.569f, 0.635f))
#define COLOR_ENERGY_GREEN   (coreVector3(0.270f, 0.710f, 0.270f))
#define COLOR_FIRE_ORANGE    (coreVector3(0.991f, 0.305f, 0.042f))
#define COLOR_FIRE_BLUE      (coreVector3(0.306f, 0.527f, 1.000f))
#define COLOR_SHIP_YELLOW    (coreVector3( 50.0f/360.0f, 100.0f/100.0f,  85.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_ORANGE    (coreVector3( 34.0f/360.0f, 100.0f/100.0f, 100.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_RED       (coreVector3(  0.0f/360.0f,  68.0f/100.0f,  90.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_MAGENTA   (coreVector3(330.0f/360.0f,  65.0f/100.0f,  85.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_PURPLE    (coreVector3(287.0f/360.0f,  55.0f/100.0f,  85.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_BLUE      (coreVector3(201.0f/360.0f,  74.0f/100.0f,  85.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_CYAN      (coreVector3(183.0f/360.0f,  70.0f/100.0f,  85.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_GREEN     (coreVector3(118.0f/360.0f,  58.0f/100.0f,  70.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_BROWN     (coreVector3( 40.0f/360.0f,  95.0f/100.0f,  70.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_GREY      (coreVector3(  0.0f/360.0f,   0.0f/100.0f,  60.0f/100.0f).HsvToRgb())
#define COLOR_SHIP_ICE       (coreVector3(208.0f/360.0f,  32.0f/100.0f,  90.0f/100.0f).HsvToRgb())
#define COLOR_HEALTH(x)      (TernaryLerp(COLOR_MENU_RED, COLOR_MENU_YELLOW, COLOR_MENU_GREEN, x))
#define COLOR_CHAIN(x)       (TernaryLerp(COLOR_MENU_RED, COLOR_MENU_PURPLE, COLOR_MENU_BLUE,  x))

// shader modifiers
#define SHADER_TRANSITION(x) "#define _P1_TRANSITION_ (" #x ") \n"   // full_transition
#define SHADER_SHADOW(x)     "#define _P1_SHADOW_     (" #x ") \n"   // outdoor, object_ground
#define SHADER_OVERLAYS(x)   "#define _P1_OVERLAYS_   (" #x ") \n"   // weather
#define SHADER_SAMPLES(x)    "#define _P1_SAMPLES_    (" #x ") \n"   // ink
#define SHADER_GLOW          "#define _P1_GLOW_       (1) \n"        // post, outdoor, object_ship
#define SHADER_DISTORTION    "#define _P1_DISTORTION_ (1) \n"        // post
#define SHADER_DEBUG         "#define _P1_DEBUG_      (1) \n"        // post
#define SHADER_OBJECT3D      "#define _P1_OBJECT3D_   (1) \n"        // distortion
#define SHADER_SINGLE        "#define _P1_SINGLE_     (1) \n"        // decal, weather
#define SHADER_LIGHT         "#define _P1_LIGHT_      (1) \n"        // outdoor, decal
#define SHADER_DARKNESS      "#define _P1_DARKNESS_   (1) \n"        // object_ship
#define SHADER_BLINK         "#define _P1_BLINK_      (1) \n"        // energy, object_ship
#define SHADER_FLAT          "#define _P1_FLAT_       (1) \n"        // outline, energy
#define SHADER_BULLET        "#define _P1_BULLET_     (1) \n"        // outline, energy
#define SHADER_SPHERIC       "#define _P1_SPHERIC_    (1) \n"        // decal, energy
#define SHADER_INVERT        "#define _P1_INVERT_     (1) \n"        // energy
#define SHADER_DIRECT        "#define _P1_DIRECT_     (1) \n"        // outline, energy, distortion, menu_border
#define SHADER_RING          "#define _P1_RING_       (1) \n"        // energy
#define SHADER_WAVE          "#define _P1_WAVE_       (1) \n"        // object
#define SHADER_GREY          "#define _P1_GREY_       (1) \n"        // vignette
#define SHADER_LINE          "#define _P1_LINE_       (1) \n"        // ink

// collision types
enum eType : coreInt32
{
    TYPE_PLAYER = 1,
    TYPE_ENEMY,

    TYPE_BULLET_PLAYER,
    TYPE_BULLET_ENEMY,

    TYPE_CHROMA,
    TYPE_ITEM,

    TYPE_VIRIDO_BALL,
    TYPE_VIRIDO_PADDLE,
    TYPE_VIRIDO_BARRIER,
    TYPE_VIRIDO_LASER,
    TYPE_NEVO_CONTAINER,

    TYPE_DHARUK_BOOMERANG,
    TYPE_LEVIATHAN_RAY
};

// 
enum eSound : coreUint8
{
    SOUND_EFFECT = 1u,
    SOUND_AMBIENT
};

// attack elements
enum eElement : coreUint8
{
    ELEMENT_WHITE = 1u,
    ELEMENT_YELLOW,
    ELEMENT_ORANGE,
    ELEMENT_RED,
    ELEMENT_MAGENTA,
    ELEMENT_PURPLE,
    ELEMENT_BLUE,
    ELEMENT_CYAN,
    ELEMENT_GREEN,
    ELEMENT_NEUTRAL
};

// 
enum eMedal : coreUint8
{
    MEDAL_NONE = 0u,
    MEDAL_BRONZE,
    MEDAL_SILVER,
    MEDAL_GOLD,
    MEDAL_PLATINUM,
    MEDAL_DARK,
    MEDAL_MAX,

    MEDAL_TYPE_WAVE = 0u,
    MEDAL_TYPE_BOSS,
    MEDAL_TYPE_MISSION,
    MEDAL_TYPE_MAX
};

extern void InitResolution(const coreVector2& vResolution);   // init resolution properties (1:1)
extern void InitFramerate();                                  // init frame rate properties (lock)


// ****************************************************************
// forward declarations
class cShip;
class cPlayer;
class cEnemy;
class cBoss;
class cMission;


// ****************************************************************
// game headers
extern coreVector2     g_vGameResolution;   // pre-calculated 1:1 resolution
extern coreVector2     g_vMenuCenter;       // pre-calculated menu center modifier
extern coreBool        g_bDebugOutput;      // 
extern coreMusicPlayer g_MusicPlayer;       // central music-player

#include "additional/cUtilities.h"
#include "additional/cBindContainer.h"
#include "additional/cRotaCache.h"
#include "additional/cLodObject.h"
#include "file/cConfig.h"
#include "file/cReplay.h"
#include "file/cSave.h"
#include "visual/cShadow.h"
#include "visual/cOutline.h"
#include "visual/cBlur.h"
#include "visual/cGlow.h"
#include "visual/cDistortion.h"
#include "visual/cHeadlight.h"
#include "visual/cWindscreen.h"
#include "visual/cSpecialEffects.h"
#include "visual/cForeground.h"
#include "visual/cPostProcessing.h"

extern cReplay*         const g_pReplay;           // 
extern cSave*           const g_pSave;             // 
extern cOutline*        const g_pOutline;          // main outline-layer object
extern cGlow*           const g_pGlow;             // main glow-effect object
extern cDistortion*     const g_pDistortion;       // main distortion-effect object
extern cWindscreen*     const g_pWindscreen;       // 
extern cSpecialEffects* const g_pSpecialEffects;   // main special-effects object
extern cPostProcessing* const g_pPostProcessing;   // main post-processing object

#include "environment/cOutdoor.h"
#include "environment/cWater.h"
#include "environment/background/cBackground.h"
#include "environment/cEnvironment.h"
#include "interface/cCombatText.h"
#include "interface/cInterface.h"
#include "interface/cMsgBox.h"
#include "interface/cTooltip.h"
#include "interface/cWorldMap.h"
#include "interface/menu/cMenu.h"
#include "game/cTable.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/cHelper.h"
#include "game/cChroma.h"
#include "game/cItem.h"
#include "game/cShield.h"
#include "game/boss/cBoss.h"
#include "game/mission/cMission.h"
#include "game/cPlayer.h"
#include "game/cGame.h"

extern cForeground*  const g_pForeground;    // main foreground object
extern cEnvironment* const g_pEnvironment;   // main environment object
extern cMenu*        const g_pMenu;          // main menu object
extern cGame*        const g_pGame;          // main game object


#endif // _P1_GUARD_MAIN_H_