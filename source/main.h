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
//| Project One v0.1.0a (http://www.maus-games.at)                                |//
//*-------------------------------------------------------------------------------*//
//| Copyright (c) 2010-2016 Martin Mauersics                                      |//
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
// TODO: test framerate-lock for g-sync stuff
// TODO: clean up shader modifiers and shaders, also try to look at unused uniforms, varyings and attributes (shadow-matrix is used in ship-shader !?)
// TODO: use single-channel texture where possible


// ****************************************************************
// engine header
#include "Core.h"
#include <deque>


// ****************************************************************
// general definitions
#define PLAYERS              (2u)
#define FRAMERATE_VALUE      (60.0f)
#define FRAMERATE_TIME       (1.0f / FRAMERATE_VALUE)
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
#define COLOR_ENERGY_YELLOW  (coreVector3(0.950f, 0.800f, 0.280f)) // (coreVector3(0.900f, 0.800f, 0.380f))
#define COLOR_ENERGY_ORANGE  (coreVector3(1.000f, 0.400f, 0.000f))
#define COLOR_ENERGY_RED     (coreVector3(1.000f, 0.275f, 0.275f))
#define COLOR_ENERGY_PURPLE  (coreVector3(0.450f, 0.200f, 1.000f))
#define COLOR_ENERGY_BLUE    (coreVector3(0.100f, 0.430f, 1.000f))
#define COLOR_ENERGY_GREEN   (coreVector3(0.270f, 0.710f, 0.270f))
#define COLOR_FIRE_ORANGE    (coreVector3(0.991f, 0.305f, 0.042f))
#define COLOR_FIRE_BLUE      (coreVector3(0.306f, 0.527f, 1.000f))
#define COLOR_SHIP_YELLOW    (coreVector3( 50.0f/360.0f, 100.0f/100.0f,  85.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_ORANGE    (coreVector3( 34.0f/360.0f, 100.0f/100.0f, 100.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_RED       (coreVector3(  0.0f/360.0f,  68.0f/100.0f,  90.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_PURPLE    (coreVector3(287.0f/360.0f,  55.0f/100.0f,  85.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_BLUE      (coreVector3(201.0f/360.0f,  74.0f/100.0f,  85.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_CYAN      (coreVector3(183.0f/360.0f,  70.0f/100.0f,  85.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_GREEN     (coreVector3(118.0f/360.0f,  58.0f/100.0f,  70.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_BROWN     (coreVector3( 40.0f/360.0f,  95.0f/100.0f,  70.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_GREY      (coreVector3(  0.0f/360.0f,   0.0f/100.0f,  60.0f/100.0f).HSVtoRGB())
#define COLOR_SHIP_ICE       (coreVector3(208.0f/360.0f,  32.0f/100.0f,  90.0f/100.0f).HSVtoRGB())
#define COLOR_HEALTH(x)      (TernaryLerp(COLOR_MENU_RED, COLOR_MENU_YELLOW, COLOR_MENU_GREEN, x))
#define COLOR_CHAIN(x)       (TernaryLerp(COLOR_MENU_RED, COLOR_MENU_PURPLE, COLOR_MENU_BLUE,  x))

// shader modifiers
#define SHADER_TRANSITION(x) "#define _P1_TRANSITION_ (" #x ") \n"   // full_transition
#define SHADER_SHADOW(x)     "#define _P1_SHADOW_     (" #x ") \n"   // outdoor, object_ground
#define SHADER_GLOW          "#define _P1_GLOW_       (1) \n"        // post, outdoor, object_ship
#define SHADER_DISTORTION    "#define _P1_DISTORTION_ (1) \n"        // post
#define SHADER_DEBUG         "#define _P1_DEBUG_      (1) \n"        // post
#define SHADER_SINGLE        "#define _P1_SINGLE_     (1) \n"        // decal
#define SHADER_LIGHT         "#define _P1_LIGHT_      (1) \n"        // outdoor, decal
#define SHADER_DARKNESS      "#define _P1_DARKNESS_   (1) \n"        // object_ship
#define SHADER_BULLET        "#define _P1_BULLET_     (1) \n"        // energy
#define SHADER_SPHERIC       "#define _P1_SPHERIC_    (1) \n"        // decal, energy
#define SHADER_INVERT        "#define _P1_INVERT_     (1) \n"        // energy
#define SHADER_DIRECT        "#define _P1_DIRECT_     (1) \n"        // outline, energy, effect_distortion
#define SHADER_RING          "#define _P1_RING_       (1) \n"        // energy
#define SHADER_WAVE          "#define _P1_WAVE_       (1) \n"        // object

// collision types
#define TYPE_PLAYER          (1)
#define TYPE_ENEMY           (2)
#define TYPE_BULLET_PLAYER   (11)
#define TYPE_BULLET_ENEMY    (12)
#define TYPE_OBJECT(x)       (100 + (x))

// attack elements
#define ELEMENT_YELLOW       (0u)   // speed (ray) 
#define ELEMENT_ORANGE       (1u)   // fire
#define ELEMENT_RED          (2u)   // (antimatter) 
#define ELEMENT_BLUE         (3u)   // homing (tesla) 
#define ELEMENT_PURPLE       (4u)   // power (pulse) 
#define ELEMENT_GREEN        (5u)   // (wave) 
#define ELEMENT_NEUTRAL      (6u)   // 
#define ELEMENT_LIGHT        (7u)   // 
#define ELEMENT_DARK         (8u)   // 

// sub-class and object ID macros
#define ENABLE_ID                                           \
    virtual const coreInt32 GetID  ()const = 0;             \
    virtual const coreChar* GetName()const = 0;
#define ASSIGN_ID(i,n)                                      \
    static const coreInt32 ID = i;                          \
    inline const coreInt32 GetID  ()const final {return i;} \
    inline const coreChar* GetName()const final {return n;}

// angle difference helper-function
inline FUNC_CONST coreFloat AngleDiff(const coreFloat x, const coreFloat y)
{
    coreFloat A = (x - y);
    while(A < -PI) A += 2.0f*PI;
    while(A >  PI) A -= 2.0f*PI;
    return A;
}

// 
inline FUNC_CONST coreFloat LerpBreakRev(const coreFloat x, const coreFloat y, const coreFloat s)
{
    return LERPB(y, x, 1.0f - s);
}

// value range helper-function
template <typename T, typename S, typename R> constexpr FUNC_CONST coreBool InBetween(const T& x, const S& a, const R& b)
{
    return (a <= x) && (x < b);
}
template <typename T, typename S, typename R> constexpr FUNC_CONST coreInt32 InBetweenExt(const T& x, const S& a, const R& b)
{
    return (a <= b) ? (((a <= x) && (x < b)) ?  1 : 0) :
                      (((b <= x) && (x < a)) ? -1 : 0);
}

// ternary interpolation helper-function
template <typename T, typename S, typename R> constexpr FUNC_CONST T TernaryLerp(const T& x, const S& y, const R& z, const coreFloat s)
{
    return (s >= 0.5f) ? LERP(y, z, s*2.0f - 1.0f) :
                         LERP(x, y, s*2.0f);
}

extern void InitResolution(const coreVector2& vResolution);   // init resolution properties (1:1)
extern void InitFramerate();                                  // init framerate properties (lock)


// ****************************************************************
// forward declarations
class cShip;
class cPlayer;
class cEnemy;
class cBoss;
class cMission;


// ****************************************************************
// game header files
extern coreVector2      g_vGameResolution;   // pre-calculated 1:1 resolution
extern coreVector2      g_vMenuCenter;       // pre-calculated menu center modifier
extern coreMusicPlayer  g_MusicPlayer;       // central music-player

#include "additional/cBindContainer.h"
#include "file/cConfig.h"
#include "visual/cShadow.h"
#include "visual/cOutline.h"
#include "visual/cBlur.h"
#include "visual/cGlow.h"
#include "visual/cDistortion.h"
#include "visual/cSpecialEffects.h"
#include "visual/cForeground.h"
#include "visual/cPostProcessing.h"

extern cOutline*        g_pOutline;          // main outline-layer object
extern cGlow*           g_pGlow;             // main glow-effect object
extern cDistortion*     g_pDistortion;       // main distortion-effect object
extern cSpecialEffects* g_pSpecialEffects;   // main special-effects object
extern cPostProcessing* g_pPostProcessing;   // main post-processing object

#include "environment/cOutdoor.h"
#include "environment/cWater.h"
#include "environment/background/cBackground.h"
#include "environment/cEnvironment.h"
#include "interface/cCombatStats.h"
#include "interface/cCombatText.h"
#include "interface/cInterface.h"
#include "interface/cMsgBox.h"
#include "interface/cTooltip.h"
#include "interface/menu/cMenu.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/cShield.h"
#include "game/boss/cBoss.h"
#include "game/mission/cMission.h"
#include "game/cPlayer.h"
#include "game/cTheater.h"
#include "game/cGame.h"

extern cForeground*     g_pForeground;       // main foreground object
extern cEnvironment*    g_pEnvironment;      // main environment object
extern cMenu*           g_pMenu;             // main menu object
extern cTheater*        g_pTheater;          // main theater object
extern cGame*           g_pGame;             // main game object


#endif // _P1_GUARD_MAIN_H_