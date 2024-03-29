//////////////////////////////////////////////////////////////////////////////////
//*----------------------------------------------------------------------------*//
//| Project One v0.1.0 (https://www.maus-games.at)                             |//
//*----------------------------------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics                                        |//
//|                                                                            |//
//| This software is provided 'as-is', without any express or implied          |//
//| warranty. In no event will the authors be held liable for any damages      |//
//| arising from the use of this software.                                     |//
//|                                                                            |//
//| Permission is granted to anyone to use this software for any purpose,      |//
//| including commercial applications, and to alter it and redistribute it     |//
//| freely, subject to the following restrictions:                             |//
//|                                                                            |//
//| 1. The origin of this software must not be misrepresented; you must not    |//
//|    claim that you wrote the original software. If you use this software    |//
//|    in a product, an acknowledgment in the product documentation would be   |//
//|    appreciated but is not required.                                        |//
//|                                                                            |//
//| 2. Altered source versions must be plainly marked as such, and must not be |//
//|    misrepresented as being the original software.                          |//
//|                                                                            |//
//| 3. This notice may not be removed or altered from any source distribution. |//
//*----------------------------------------------------------------------------*//
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MAIN_H_
#define _P1_GUARD_MAIN_H_

// TODO 4: remove magic numbers (regularly)
// TODO 3: test framerate-lock for g-sync stuff, also test for 144hz displays if render x144 but move x60 is better
// TODO 3: clean up shader modifiers and shaders, also try to look at unused uniforms, varyings and attributes (shadow-matrix is used in ship-shader !? a_v1Blink used in ground-shader but only when instancing), and reduce passing data across shader stages
// TODO 5: implement static/coherent branching interface instead of many shader-permutations ? (maybe only in situations with frequent switching)
// TODO 3: use single-channel texture where possible
// TODO 5: menu optimization by caching into framebuffer (general class for leaderboard, options, etc.)
// TODO 3: check all shaders if alpha is required
// TODO 4: full initializer lists everywhere (don't forget parent classes)
// TODO 3: clean mixing shader defines (x >= y) and (defined(x)) checks (also in engine)
// TODO 3: check for 16-bit half float shader usage
// TODO 2: program enable has to be checked (if(x.Enable()){}) everywhere
// TODO 4: unify "forward" and "transform" comments in shaders
// TODO 3: add own coreRand for various random things which may affect feeling (screen shake), and reset on boss-start
// TODO 3: check issues with all the F&& functions (especially in boss.h and mission.h), also check Core engine, use force_inline on small functions
// TODO 4: RETURN_NONNULL to everything which should never be null (and other attributes, both FUNC and RETURN)
// TODO 3: check all vert shader for CORE_SHADER_OPTION_NO_ROTATION
// TODO 5: control flow guard and buffer security check
// TODO 1: _CORE_SSE_ try to just remove the coreMath low-precision functions (macro replace ? will only work partially)
// TODO 3: reduce number of shader-lights with static_assert, change something like that into static config
// TODO 3: check if hole in object_sphere causes reflection issues, also check if other objects have a hole
// TODO 3: on bosses and missions: don't move or render or test objects outside their phases (e.g. boomerangs active)
// TODO 3: make sure everything with at least 4 instances uses batch-lists
// TODO 4: replace / with RCP where possible
// TODO 5: "pro" shortcut for types (e.g. proEnemy, proGame), dr too
// TODO 4: ENABLE_BITWISE when ?
// TODO 5: remove multisampling for 2d, though may cause artifacts if the objects are fully shaded and moved
// TODO 3: transition shader only needs alpha for menu, but not for background -> create permutations
// TODO 3: check all normalization calls if requires default or context-specific fallback, also check for more unsafe calls
// TODO 3: check all RCP for division by zero
// TODO 2: reshape causes some batch-list to be initialized twice
// TODO 5: look for hot/cold optimizations, e.g. member-list in enemy can be pointer, write wrapper for that, coreCold<...>, check everything already pointer for switching to wrapper
// TODO 3: check for and prevent accumulation of small rounding errors: incremental matrix rotation
// TODO 3: check if outlines are correct on all text (multiple font-height, multiple screen-resolutions)
// TODO 3: return boolean to cancel iteration on ForEachBullet, *Player, *Enemy (do I need this?)
// TODO 3: make sure shaders use 0.5,0.5 for pixel centers
// TODO 3: make energy texture sharper (offline upsampling)
// TODO 5: check for merging varyings with component = # and layoutEx (or merge manually)
// TODO 1: indicator when controls are enabled again (blinking und peeping sound)
// TODO 3: search and remove unused resources from application.cpp (+ folder)
// TODO 3: remove game_icon.png from resource-index if not required anymore
// TODO 5: convert bigger sound-effects (ambient) to music ?
// TODO 3: change all linear interpolation with at least LERPH3 to improve quality, where possible
// TODO 3: find (manual) interpolations and try to use smoothstep for it (engine, application and shader)
// TODO 4: check all lambdas if OUTPUT can be replaced with const
// TODO 3: create animation offset for all gameplay objects (const coreFloat fOffset = I_TO_F(i) * (1.0f/7.0f);), try to use num-per-line + 1, what about bullets ?
// TODO 1: make sure user folder is correctly handled for multi-user (-> corePlatform)
// TODO 3: every boss, enemy, gameplay-objects, player-bullet-interacting object needs a volume (including all enemy-bullet types)
// TODO 1: all sounds need IsUsable checks
// TODO 4: look if coreUintW member variables can be made smaller (also engine)
// TODO 3: skip rendering (like in pause) when update frequency is >= 2x of the refresh rate
// TODO 3: for uneven resolutions, some objects need g_vGameResolution.AspectRatio() (on both axes, with max(1.0f)): menu transition, postprocessing
// TODO 4: change arrays of structs to structs of arrays where possible (also in engine)
// TODO 3: check if more textures can be changed to grayscale (grey+noalpha, black+alpha)
// TODO 2: test maximum number of replays, provide upper limit, define communication when approaching or reaching limit
// TODO 2: prevent shaking of center-aligned rectified animated text
// TODO 3: add gamepad led colors
// TODO 1: clarify and simplify upper-case handling (for all texts, but especially for boss and mission names)
// TODO 3: only disable culling in actual mirror mode


// ****************************************************************
// engine headers
#include "Core.h"

#define _P1_DEBUG_INPUT_ (1)
//#define _P1_DEBUG_RANDOM_ (1)

#if !defined(_CORE_DEBUG_)
    #if defined(_CORE_SSE_) || defined(_CORE_NEON_)
        #pragma message("Warning: Precision reduced!")
    #endif
    #if defined(_P1_DEBUG_INPUT_)
        #pragma message("Warning: Debug input enabled!")
    #endif
    #if defined(_P1_DEBUG_RANDOM_)
        #pragma message("Warning: Debug randomization enabled!")
    #endif
#endif

#define GetVisualRange  GetCollisionRange   // TODO 1: remove again
#define GetVisualRadius GetCollisionRadius


// ****************************************************************
// general definitions
#define PLAYERS              (2u)
#define HELPERS              (9u)
#define MISSIONS             (9u + 2u + 1u)
#define BOSSES               (2u)
#define WAVES                (10u)
#define SEGMENTS             (BOSSES + WAVES)
#define LIVES                (5u)
#define CONTINUES            (3u)
#define SHIELD               (20u)
#define BADGES               (3u)
#define WEAPONS              (6u)
#define SUPPORTS             (2u)
#define EQUIP_WEAPONS        (1u)
#define EQUIP_SUPPORTS       (1u)
#define FRAMERATE_MIN        (60.0f)
#define FRAMERATE_MAX        (240.0f)
#define SCALE_FACTOR         (CORE_GL_SUPPORT(ARB_texture_rg) ? 0.5f : 0.4f)
#define CAMERA_POSITION      (coreVector3(0.0f,  0.0f,  1.0f) * 110.0f)
#define CAMERA_DIRECTION     (coreVector3(0.0f,  0.0f, -1.0f))
#define CAMERA_ORIENTATION   (coreVector3(0.0f,  1.0f,  0.0f))
#define LISTENER_POSITION    (coreVector3(0.0f,  0.0f,  1.0f) * 10.0f)
#define LISTENER_VELOCITY    (coreVector3(0.0f,  0.0f,  0.0f))
#define LIGHT_DIRECTION      (coreVector3(1.0f, -1.1f, -0.85f).Normalized())   // (0.583957136f, -0.642352879f, -0.496363580f)

// color values
#define COLOR_MENU_WHITE     (coreVector3(1.000f, 1.000f, 1.000f) * MENU_CONTRAST_WHITE)
#define COLOR_MENU_BLACK     (coreVector3(1.000f, 1.000f, 1.000f) * MENU_CONTRAST_BLACK)
#define COLOR_MENU_INSIDE    (coreVector3(1.000f, 1.000f, 1.000f) * MENU_CONTRAST_INSIDE)
#define COLOR_MENU_YELLOW    (coreVector3(1.000f, 0.824f, 0.292f))
#define COLOR_MENU_ORANGE    (coreVector3(1.000f, 0.543f, 0.177f))
#define COLOR_MENU_RED       (coreVector3(0.950f, 0.225f, 0.225f))
#define COLOR_MENU_MAGENTA   (coreVector3(1.000f, 0.310f, 0.720f))
#define COLOR_MENU_PURPLE    (coreVector3(0.760f, 0.333f, 1.000f))
#define COLOR_MENU_BLUE      (coreVector3(0.102f, 0.602f, 1.000f))
#define COLOR_MENU_CYAN      (coreVector3(0.000f, 0.776f, 0.806f))
#define COLOR_MENU_GREEN     (coreVector3(0.253f, 0.745f, 0.253f))
#define COLOR_ENERGY_WHITE   (coreVector3(1.000f, 1.000f, 1.000f))
#define COLOR_ENERGY_YELLOW  (coreVector3(0.950f, 0.800f, 0.280f))
#define COLOR_ENERGY_ORANGE  (coreVector3(1.000f, 0.420f, 0.000f))
#define COLOR_ENERGY_RED     (coreVector3(1.000f, 0.290f, 0.290f))
#define COLOR_ENERGY_MAGENTA (coreVector3(1.000f, 0.310f, 0.650f))
#define COLOR_ENERGY_PURPLE  (coreVector3(0.450f, 0.200f, 1.000f))
#define COLOR_ENERGY_BLUE    (coreVector3(0.100f, 0.430f, 1.000f))
#define COLOR_ENERGY_CYAN    (coreVector3(0.184f, 0.635f, 0.635f))
#define COLOR_ENERGY_GREEN   (coreVector3(0.270f, 0.710f, 0.270f))
#define COLOR_FIRE_WHITE     (coreVector3(0.220f, 0.220f, 0.220f))
#define COLOR_FIRE_ORANGE    (coreVector3(0.991f, 0.305f, 0.042f))
#define COLOR_FIRE_BLUE      (coreVector3(0.306f, 0.527f, 1.000f))
#define COLOR_SHIP_YELLOW    (coreVector3(0.950f, 0.778f, 0.170f))
#define COLOR_SHIP_ORANGE    (coreVector3(1.000f, 0.539f, 0.108f))
#define COLOR_SHIP_RED       (coreVector3(0.950f, 0.288f, 0.288f))
#define COLOR_SHIP_MAGENTA   (coreVector3(0.935f, 0.328f, 0.631f))
#define COLOR_SHIP_PURPLE    (coreVector3(0.729f, 0.382f, 0.900f))
#define COLOR_SHIP_BLUE      (coreVector3(0.151f, 0.600f, 1.000f))
#define COLOR_SHIP_CYAN      (coreVector3(0.000f, 0.800f, 0.800f))
#define COLOR_SHIP_GREEN     (coreVector3(0.308f, 0.720f, 0.308f))
#define COLOR_SHIP_GREY      (coreVector3(0.500f, 0.500f, 0.500f))
#define COLOR_HEALTH(x)      (TernaryLerp(COLOR_MENU_RED, COLOR_MENU_YELLOW, COLOR_MENU_GREEN, x))   // TODO 1: remove

// shader modifiers
#define SHADER_TRANSITION(x) "#define _P1_TRANSITION_" " (" #x ") \n"   // full_transition
#define SHADER_SHADOW(x)     "#define _P1_SHADOW_"     " (" #x ") \n"   // outdoor, object_ground
#define SHADER_OVERLAYS(x)   "#define _P1_OVERLAYS_"   " (" #x ") \n"   // weather
#define SHADER_SAMPLES(x)    "#define _P1_SAMPLES_"    " (" #x ") \n"   // ink
#define SHADER_GLOW          "#define _P1_GLOW_"       " (1) \n"        // post, outdoor, object_ship
#define SHADER_DISTORTION    "#define _P1_DISTORTION_" " (1) \n"        // post
#define SHADER_DEBUG         "#define _P1_DEBUG_"      " (1) \n"        // post
#define SHADER_OBJECT3D      "#define _P1_OBJECT3D_"   " (1) \n"        // distortion
#define SHADER_SINGLE        "#define _P1_SINGLE_"     " (1) \n"        // decal, weather
#define SHADER_LIGHT         "#define _P1_LIGHT_"      " (1) \n"        // outdoor, decal, outline
#define SHADER_DARKNESS      "#define _P1_DARKNESS_"   " (1) \n"        // object_ship
#define SHADER_BLINK         "#define _P1_BLINK_"      " (1) \n"        // energy, object_ship, object_meteor
#define SHADER_FLAT          "#define _P1_FLAT_"       " (1) \n"        // outline, energy
#define SHADER_BULLET        "#define _P1_BULLET_"     " (1) \n"        // outline, energy
#define SHADER_SPHERIC       "#define _P1_SPHERIC_"    " (1) \n"        // decal, energy
#define SHADER_INVERT        "#define _P1_INVERT_"     " (1) \n"        // energy
#define SHADER_DIRECT        "#define _P1_DIRECT_"     " (1) \n"        // outline, energy, distortion, menu_border
#define SHADER_RING          "#define _P1_RING_"       " (1) \n"        // energy
#define SHADER_WAVE          "#define _P1_WAVE_"       " (1) \n"        // object
#define SHADER_GREY          "#define _P1_GREY_"       " (1) \n"        // vignette
#define SHADER_LINE          "#define _P1_LINE_"       " (1) \n"        // ink

// collision types
enum eType : coreInt32
{
    TYPE_PLAYER = 1,
    TYPE_ENEMY,

    TYPE_BULLET_PLAYER,
    TYPE_BULLET_ENEMY,

    TYPE_ITEM,

    TYPE_VIRIDO_BALL,
    TYPE_VIRIDO_PADDLE,
    TYPE_VIRIDO_BARRIER,
    TYPE_VIRIDO_LASER,
    TYPE_NEVO_BOMB,
    TYPE_NEVO_BLOCK,
    TYPE_NEVO_CONTAINER,
    TYPE_RUTILUS_TELEPORTER,

    TYPE_DHARUK_BOOMERANG,
    TYPE_LEVIATHAN_RAY
};

// sound categories
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

// 
enum eBadge : coreUint8
{
    BADGE_EASY = 1u,
    BADGE_NORMAL,
    BADGE_HARD
};

extern void InitResolution(const coreVector2 vResolution);   // init resolution properties (1:1)
extern void InitDirection();                                 // 
extern void InitFramerate();                                 // init frame rate properties (lock)


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
extern coreVector2     g_vHudDirection;     // 
extern coreBool        g_bDebugOutput;      // 
extern coreMusicPlayer g_MusicPlayer;       // central music-player

#include "additional/cUtilities.h"
#include "additional/cBindContainer.h"
#include "additional/cRotaCache.h"
#include "additional/cGuiObject.h"
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
#include "visual/cInk.h"
#include "visual/cSnow.h"
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
#include "interface/cArcadeInput.h"
#include "interface/cCombatText.h"
#include "interface/cCreditRoll.h"
#include "interface/cInterface.h"
#include "interface/cMsgBox.h"
#include "interface/cScrollBox.h"
#include "interface/cTooltip.h"
#include "interface/cWorldMap.h"
#include "interface/menu/cMenu.h"
#include "game/cTable.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/cHelper.h"
#include "game/cItem.h"
#include "game/cShield.h"
#include "game/cCrash.h"
#include "game/boss/cBoss.h"
#include "game/mission/cMission.h"
#include "game/cPlayer.h"
#include "game/cGame.h"

extern cForeground*  const g_pForeground;    // main foreground object
extern cEnvironment* const g_pEnvironment;   // main environment object
extern cMenu*        const g_pMenu;          // main menu object
extern cGame*        const g_pGame;          // main game object


#endif // _P1_GUARD_MAIN_H_