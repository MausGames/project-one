//////////////////////////////////////////////////////////////////////////////////
//*----------------------------------------------------------------------------*//
//| Project One v1.2.6 (https://www.maus-games.at)                             |//
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

// TODO 4: remove magic numbers
// TODO 3: clean up shader modifiers and shaders, also try to look at unused uniforms, varyings and attributes (shadow-matrix is used in ship-shader !? a_v1Blink used in ground-shader but only when instancing), and reduce passing data across shader stages
// TODO 5: implement static/coherent branching interface instead of many shader-permutations ? (maybe only in situations with frequent switching)
// TODO 3: check all shaders if alpha is required
// TODO 4: full initializer lists everywhere (don't forget parent classes)
// TODO 3: clean mixing shader defines (x >= y) and (defined(x)) checks (also in engine)
// TODO 4: unify "forward" and "transform" comments in shaders
// TODO 3: check issues with all the F&& functions (especially in boss.h and mission.h), also check Core engine, use force_inline on small functions
// TODO 4: RETURN_NONNULL to everything which should never be null (and other attributes, both FUNC and RETURN)
// TODO 3: reduce number of shader-lights with static_assert, change something like that into static config
// TODO 3: on bosses and missions: don't move or render or test objects outside their phases (e.g. boomerangs active)
// TODO 3: make sure everything with at least 4 instances uses batch-lists
// TODO 4: replace / with RCP where possible
// TODO 4: ENABLE_BITWISE when ?
// TODO 2: reshape causes some batch-list to be initialized twice
// TODO 5: look for hot/cold optimizations, e.g. member-list in enemy can be pointer, write wrapper for that, coreCold<...>, check everything already pointer for switching to wrapper
// TODO 3: return boolean to cancel iteration on ForEachBullet, *Player, *Enemy (do I need this?)
// TODO 5: check for merging varyings with component = # and layoutEx (or merge manually)
// TODO 1: indicator when controls are enabled again (blinking und peeping sound)
// TODO 5: convert bigger sound-effects (ambient) to music ?
// TODO 3: change all linear interpolation with at least LERPH3 to improve quality, where possible
// TODO 3: find (manual) interpolations and try to use smoothstep for it (engine, application and shader)
// TODO 3: create animation offset for all gameplay objects (const coreFloat fOffset = I_TO_F(i) * (1.0f/7.0f);), try to use num-per-line + 1, what about bullets ?
// TODO 3: every boss, enemy, gameplay-objects, player-bullet-interacting object needs a volume (including all enemy-bullet types, blender decimate tool factor ~0.1)
// TODO 1: all sounds need IsUsable checks
// TODO 4: look if coreUintW member variables can be made smaller (also engine)
// TODO 3: skip rendering (like in pause) when update frequency is >= 2x of the refresh rate
// TODO 3: for uneven resolutions, some objects need g_vGameResolution.AspectRatio() (on both axes, with max(1.0f)): menu transition, postprocessing
// TODO 4: change arrays of structs to structs of arrays where possible (also in engine)
// TODO 2: test maximum number of replays, provide upper limit, define communication when approaching or reaching limit
// TODO 2: prevent shaking of center-aligned rectified animated text (did I mean something like seconds?)
// TODO 3: make sure bullet->disable has correct positioned impact-effect everywhere, especially with fast ray-bullets going deep into other objects (manual correction or ray-cast)
// TODO 2: fix broken pw-database printing on MacOS (maybe put TODO into engine) (maybe related to geteuid<>getuid) (# replaced geteuid with getuid, which seems to be correct'er, just need to check if that was the issue on macos)
// TODO 1: look if enemies with health 10 should be changed to 4
// TODO 1: make frequency rounding corrections: boss ticker, player weapon
// TODO 1: remove unused waves and associated objects from default missions, if not required anymore at the end
// TODO 1: all health-based boss-transitions need to take affect on specific % -> create own % and value check-functions with rounding
// TODO 1: check for coreVector2::Direction and .Angle() and .Length() calls in loops with more than N iterations and replace them if possible (e.g. relative rotation)
// TODO 3: object_tetra_top und object_cube_top brauchen gute outline
// TODO 3: menu outlines kaputt in transition bei 1759x990 (allgemein bei ungeradeXgerade), menü-line-objekte verschieben ihre textur -> resolution muss gleich bleiben X=Y, also sollte position shift eingebaut werden (aber ALLE 2d-objekte dann auch ?)
// TODO 4: wenn möglich sollten statische variablen in funktionen raus-gezogen werden, damit nicht ständig ein init-check gemacht wird
// TODO 3: multiplicative rotation for bullet-waves, to create better interleaving (orb-bullets in geminga) -> only where it makes sense or improves the visuals
// TODO 3: sphere model kann ungenau sein (visual), vor allem bei der area-bubble bei P1 magenta phase
// TODO 3: pfeile der switch-boxen sind leicht vertikal verschoben bei 1920x1080
// TODO 1: [MF] [HIGH] [ULTRA] swap visuals for task objects (not all of them are used anymore, so show the best looking first)
// TODO 3: rogue like mode (grey mode), with negative or positive+negative (combined) effects to select (shoot speed, shoot damage, move speed, score multiplier, player size, player bullet size, enemy size, enemy bullet size, enemy bullet/attack speed, enemy speed, side-modifier (more damage to the right), extra/minus life, extra/minus shield, 
// TODO 1: [MF] [HIGH] [ULTRA] add more coop special cases (like residue and ghosts), at least one per mission, not bosses except if something cool comes to mind
// TODO 3: GetCollisionRange().xy().Length() inefficient in some places (bullets), hab aber teils wieder auf GetCollisionRadius() geswechselt, weils richtiger is

// Small Task List:
// TODO 3: better player bullet creation effect (muzzle flash)
// TODO 5: plant movement
// TODO 2: [MF] normal enemies with BOTTOM or TOP, are rendered two times, once in bottom/top, and once in their batch-list, this causes various issues, especially for BOTTOM   (ALSO FOR OUTLINE) (maybe with separation of active-list), eine liste für shadow
// TODO 3: improve big boss explosion: Sakurai https://www.youtube.com/watch?v=ZDopYzDX-Jg   https://youtu.be/D-4RsUI3ZNI?t=246   energy line explosion: https://youtu.be/j56eUNx4sZk?t=1311
// TODO 1: [MF] fixup update frequency (allow less than 60?, how to communicate increases? (on low speed))
// TODO 1: [MF] praise the player ! https://www.youtube.com/watch?v=fryDyXROp8A

// Public Feedback and Suggestions:
// TODO 3: improve player ship visuals
// TODO 3: improve bloom without affecting visibility, or make configurable
// TODO 5: (match-2 mechanic ?)
// TODO 3: D4Windows anschauen, ob sie maus steuert und controller gleichzeitig verwendet (vielleicht wegen gyro oder accelerometer ?)

// Merged List:
// TODO 3: add more details to cloud background (transparent enemy ships far in the back ? like Ikagura and RSG)
// TODO 3: einen anderen bottom-background im wolken-level ? stadt, andere textur, outdoor ?
// TODO 3: etwas muss gemacht werden, wenn sich die player hitboxen überlappen (vielleicht nicht möglich, schieben ?)

// Cleanup Task List:
// TODO 4: more cleanup surrounding Eigengrau: game, postprocessing (cpp + shader), camera, player, weapon, bullet
// TODO 4: enemy bottom/top + special bottom, menu transition, interface, menu changes (INSIDE), mission data, (pause/msgbox stretch)
// TODO 4: Torus, Leviathan


// ****************************************************************
// engine headers
#include "Core.h"

#if defined(_CORE_MSVC_)
    #pragma warning(disable : 4189)   // local variable is initialized but not referenced
#else
    #pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#if defined(_CORE_DEBUG_)
    #define _P1_DEBUG_INPUT_ (1)
    //#define _P1_DEBUG_RANDOM_ (1)
#endif

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

#if defined(_CORE_DEBUG_)
    #define _P1_VERSION_1_1_0_
#endif


// ****************************************************************
// general definitions
#define PLAYERS              (2u)
#define HELPERS              (9u)
#define MISSIONS             (9u + 2u + 1u)
#define BOSSES               (2u)
#define WAVES                (5u)
#define SEGMENTS             (BOSSES + WAVES)
#define LIVES                (5u)
#define CONTINUES            (3u)
#define SHIELD_DEFAULT       (30u)
#define SHIELD_MAX           (255u)
#define SHIELD_INVINCIBLE    (1000u)
#define BADGES               (3u + 3u)
#define FRAGMENTS            (9u)
#define WEAPONS              (6u)
#define SUPPORTS             (2u)
#define EQUIP_WEAPONS        (1u)
#define EQUIP_SUPPORTS       (1u)
#define FRAMERATE_MIN        (60.0f)
#define FRAMERATE_MAX        (360.0f * 2.0f)
#define SCALE_FACTOR         (0.5f * (CORE_GL_SUPPORT(ARB_texture_rg) ? 1.0f : 0.8f) * (g_CurConfig.Graphics.iRender ? 1.0f : 0.8f))
#define CAMERA_POSITION      (coreVector3(0.0f,  0.0f,  1.0f) * 110.0f)
#define CAMERA_DIRECTION     (coreVector3(0.0f,  0.0f, -1.0f))
#define CAMERA_ORIENTATION   (coreVector3(0.0f,  1.0f,  0.0f))
#define LISTENER_POSITION    (coreVector3(0.0f,  0.0f,  1.0f) * 10.0f)
#define LISTENER_VELOCITY    (coreVector3(0.0f,  0.0f,  0.0f))
#define LIGHT_DIRECTION      (coreVector3(1.0f, -1.1f, -0.85f).Normalized())   // (0.583957136f, -0.642352879f, -0.496363580f)
#define MUSIC_VOLUME         (0.4f)

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
#define COLOR_ENERGY_RED     (coreVector3(1.000f, 0.280f, 0.280f))
#define COLOR_ENERGY_MAGENTA (coreVector3(1.000f, 0.310f, 0.650f))
#define COLOR_ENERGY_PURPLE  (coreVector3(0.450f, 0.200f, 1.000f))
#define COLOR_ENERGY_BLUE    (coreVector3(0.100f, 0.430f, 1.000f))
#define COLOR_ENERGY_CYAN    (coreVector3(0.184f, 0.635f, 0.635f))
#define COLOR_ENERGY_GREEN   (coreVector3(0.270f, 0.710f, 0.270f))
#define COLOR_FIRE_WHITE     (coreVector3(0.220f, 0.220f, 0.220f))
#define COLOR_FIRE_ORANGE    (coreVector3(0.991f, 0.305f, 0.042f))
#define COLOR_FIRE_BLUE      (coreVector3(0.306f, 0.527f, 1.000f))
#define COLOR_LED_WHITE      (coreVector3(1.000f, 1.000f, 1.000f))
#define COLOR_LED_YELLOW     (coreVector3(1.000f, 0.900f, 0.000f))
#define COLOR_LED_ORANGE     (coreVector3(1.000f, 0.420f, 0.000f))
#define COLOR_LED_RED        (coreVector3(1.000f, 0.000f, 0.000f))
#define COLOR_LED_MAGENTA    (coreVector3(1.000f, 0.010f, 0.550f))
#define COLOR_LED_PURPLE     (coreVector3(0.250f, 0.000f, 1.000f))
#define COLOR_LED_BLUE       (coreVector3(0.050f, 0.380f, 1.000f))
#define COLOR_LED_CYAN       (coreVector3(0.184f, 0.635f, 0.635f))
#define COLOR_LED_GREEN      (coreVector3(0.170f, 1.000f, 0.070f))
#define COLOR_SHIP_YELLOW    (coreVector3(0.950f, 0.778f, 0.170f))
#define COLOR_SHIP_ORANGE    (coreVector3(1.000f, 0.539f, 0.108f))
#define COLOR_SHIP_RED       (coreVector3(0.950f, 0.288f, 0.288f))
#define COLOR_SHIP_MAGENTA   (coreVector3(0.935f, 0.328f, 0.631f))
#define COLOR_SHIP_PURPLE    (coreVector3(0.729f, 0.382f, 0.900f))
#define COLOR_SHIP_BLUE      (coreVector3(0.151f, 0.600f, 1.000f))
#define COLOR_SHIP_CYAN      (coreVector3(0.000f, 0.800f, 0.800f))
#define COLOR_SHIP_GREEN     (coreVector3(0.308f, 0.730f, 0.308f))
#define COLOR_SHIP_WHITE     (coreVector3(1.000f, 1.000f, 1.000f))
#define COLOR_SHIP_GREY      (coreVector3(0.500f, 0.500f, 0.500f))
#define COLOR_SHIP_BLACK     (COLOR_SHIP_GREY      * 0.6f)
#define COLOR_PLAYER_WHITE   (COLOR_ENERGY_WHITE   * 0.5f)
#define COLOR_PLAYER_YELLOW  (COLOR_ENERGY_YELLOW  * 0.7f)
#define COLOR_PLAYER_ORANGE  (COLOR_ENERGY_ORANGE  * 0.95f)
#define COLOR_PLAYER_RED     (COLOR_ENERGY_RED     * 0.9f)
#define COLOR_PLAYER_MAGENTA (COLOR_ENERGY_MAGENTA * 0.9f)
#define COLOR_PLAYER_PURPLE  (COLOR_ENERGY_PURPLE  * 1.0f)
#define COLOR_PLAYER_BLUE    (COLOR_ENERGY_BLUE    * 1.1f)
#define COLOR_PLAYER_CYAN    (COLOR_ENERGY_CYAN    * 1.0f)
#define COLOR_PLAYER_GREEN   (COLOR_ENERGY_GREEN   * 0.77f)

// shader modifiers
#define SHADER_TRANSITION(x) "#define _P1_TRANSITION_"  " (" #x ") \n"   // full_transition
#define SHADER_SHADOW(x)     "#define _P1_SHADOW_"      " (" #x ") \n"   // outdoor, object_ground
#define SHADER_OVERLAYS(x)   "#define _P1_OVERLAYS_"    " (" #x ") \n"   // weather
#define SHADER_GLOW          "#define _P1_GLOW_"        " (1) \n"        // post, outdoor, object_ship
#define SHADER_DISTORTION    "#define _P1_DISTORTION_"  " (1) \n"        // post
#define SHADER_TRANSPARENT   "#define _P1_TRANSPARENT_" " (1) \n"        // post
#define SHADER_CHROMA        "#define _P1_CHROMA_"      " (1) \n"        // post
#define SHADER_DEBUG         "#define _P1_DEBUG_"       " (1) \n"        // post
#define SHADER_OBJECT3D      "#define _P1_OBJECT3D_"    " (1) \n"        // distortion
#define SHADER_LABEL         "#define _P1_LABEL_"       " (1) \n"        // swipe
#define SHADER_SINGLE        "#define _P1_SINGLE_"      " (1) \n"        // decal, weather
#define SHADER_LIGHT         "#define _P1_LIGHT_"       " (1) \n"        // outdoor, decal, outline
#define SHADER_DARKNESS      "#define _P1_DARKNESS_"    " (1) \n"        // object_ship
#define SHADER_DEPTH         "#define _P1_DEPTH_"       " (1) \n"        // object_ship
#define SHADER_DETAIL        "#define _P1_DETAIL_"      " (1) \n"        // object_ship
#define SHADER_BLINK         "#define _P1_BLINK_"       " (1) \n"        // energy, object_ship, object_meteor
#define SHADER_THICK         "#define _P1_THICK_"       " (1) \n"        // outline
#define SHADER_FLAT          "#define _P1_FLAT_"        " (1) \n"        // outline, energy
#define SHADER_BULLET        "#define _P1_BULLET_"      " (1) \n"        // outline, energy
#define SHADER_SPHERIC       "#define _P1_SPHERIC_"     " (1) \n"        // energy
#define SHADER_INVERT        "#define _P1_INVERT_"      " (1) \n"        // energy
#define SHADER_DIRECT        "#define _P1_DIRECT_"      " (1) \n"        // outline, energy, distortion, menu_border
#define SHADER_WAVE          "#define _P1_WAVE_"        " (1) \n"        // object
#define SHADER_GREY          "#define _P1_GREY_"        " (1) \n"        // vignette
#define SHADER_ROTATED       "#define _P1_ROTATED_"     " (1) \n"        // energy


// 
struct sVersion final
{
    const coreChar*  pcString;
    const coreUint16 iNumber;
};
constexpr sVersion g_aVersion[] =
{
    {"1.2.0", 1u},
    {"1.2.1", 2u},
    {"1.2.2", 3u},
    {"1.2.3", 4u},
    {"1.2.4", 5u},
    {"1.2.5", 6u},
    {"1.2.6", 7u}
};
constexpr sVersion g_Version = g_aVersion[ARRAY_SIZE(g_aVersion) - 1u];


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
    SOUND_AMBIENT,
    SOUND_MENU
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
    MEDAL_TYPE_ARCADE = MEDAL_TYPE_MISSION,
    MEDAL_TYPE_MAX,

    MEDAL_MARGIN_MISSION = 3u,
    MEDAL_MARGIN_ARCADE  = 25u
};

// 
enum eBadge : coreUint8
{
    BADGE_EASY = 1u,
    BADGE_NORMAL,
    BADGE_HARD,
    BADGE_ACHIEVEMENT
};

extern void InitResolution(const coreVector2 vResolution);                                       // init resolution properties (1:1)
extern void InitDirection();                                                                     // 
extern void InitFramerate(const coreUint16 iUpdateFreq = 0u, const coreUint8 iGameSpeed = 0u);   // init frame rate properties (lock)

extern FUNC_PURE coreFloat RoundFreq(const coreFloat fFreq);

extern coreVector2 CalcFinalDirection();


// ****************************************************************
// forward declarations
class cShip;
class cPlayer;
class cEnemy;
class cBoss;
class cMission;


// ****************************************************************
// game headers
extern coreVector2     g_vGameResolution;   // pre-calculated 1:1 
extern coreFloat       g_fGameRate;         // 
extern coreVector2     g_vHudDirection;     // 
extern coreBool        g_bTiltMode;         // 
extern coreFloat       g_fShiftMode;        // 
extern coreBool        g_bDemoVersion;      // 
extern coreBool        g_bLeaderboards;     // 
extern coreBool        g_bSteamDeck;        // 
extern coreBool        g_bHandheld;         // 
extern coreBool        g_bDebugOutput;      // 
extern coreMusicPlayer g_MusicPlayer;       // central music-player

#include "additional/cUtilities.h"
#include "additional/cBindContainer.h"
#include "additional/cRotaCache.h"
#include "additional/cGuiObject.h"
#include "additional/cLodObject.h"
#include "additional/cAchievements.h"
#include "additional/cPasswords.h"
#include "interface/cFigure.h"
#include "interface/cNewIndicator.h"
#include "additional/cMenuNavigator.h"
#include "file/cConfig.h"
#include "file/cSave.h"
#include "file/cReplay.h"
#include "visual/cIcon.h"
#include "visual/cShadow.h"
#include "visual/cOutline.h"
#include "visual/cBlur.h"
#include "visual/cGlow.h"
#include "visual/cDistortion.h"
#include "visual/cExhaust.h"
#include "visual/cHeadlight.h"
#include "visual/cInk.h"
#include "visual/cSnow.h"
#include "visual/cTurf.h"
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
#include "interface/menu/cMenu.h"
#include "game/cTable.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/cHelper.h"
#include "game/cTracker.h"
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