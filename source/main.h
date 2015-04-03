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
//| Copyright (c) 2010-2015 Martin Mauersics                                      |//
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
// TODO: implement time-delta and FPS lock, patterns (movements, bullets) are time dependant, a lock would prevent this dependency


// ****************************************************************
// engine header
#include "Core.h"


// ****************************************************************
// general definitions
#define PLAYERS            (2u)
#define FRAMERATE          (60u)
#define FRAMERATE_TIME     (1.0f / I_TO_F(FRAMERATE))

#define COLOR_YELLOW_F     coreVector3(1.000f, 0.824f, 0.392f)
#define COLOR_ORANGE_F     coreVector3(1.000f, 0.443f, 0.227f)
#define COLOR_RED_F        coreVector3(1.000f, 0.275f, 0.275f)
#define COLOR_PURPLE_F     coreVector3(0.710f, 0.333f, 1.000f)
#define COLOR_BLUE_F       coreVector3(0.102f, 0.702f, 1.000f)
#define COLOR_GREEN_F      coreVector3(0.118f, 0.745f, 0.353f)
#define COLOR_BRONZE_F     coreVector3(0.925f, 0.663f, 0.259f)
#define COLOR_SILVER_F     coreVector3(0.855f, 0.855f, 0.878f)
#define COLOR_GOLD_F       coreVector3(1.000f, 0.859f, 0.000f)

#define COLOR_YELLOW_L     255u, 210u, 100u
#define COLOR_ORANGE_L     255u, 113u,  58u
#define COLOR_RED_L        255u,  70u,  70u
#define COLOR_PURPLE_L     181u,  85u, 255u
#define COLOR_BLUE_L        26u, 179u, 255u
#define COLOR_GREEN_L       30u, 190u,  90u
#define COLOR_BRONZE_L     236u, 169u,  66u
#define COLOR_SILVER_L     218u, 218u, 224u
#define COLOR_GOLD_L       255u, 219u,   0u

#define CAMERA_POSITION    coreVector3(0.0f, 0.0f, 110.0f)
#define CAMERA_DIRECTION   coreVector3(0.0f, 0.0f,  -1.0f)
#define CAMERA_ORIENTATION coreVector3(0.0f, 1.0f,   0.0f)

#define LIGHT_DIRECTION    coreVector3(0.583953857f, -0.642349243f, -0.496360779f)

#define SHADER_SHADOW      "#define _P1_SHADOW_  (1) \n"   // outdoor, object_ground
#define SHADER_GLOW        "#define _P1_GLOW_    (1) \n"   // post
#define SHADER_SPHERIC     "#define _P1_SPHERIC_ (1) \n"   // decal, energy
#define SHADER_DIRECT      "#define _P1_DIRECT_  (1) \n"   // outline, energy

#define TYPE_PLAYER        (1)
#define TYPE_ENEMY         (2)
#define TYPE_BULLET_PLAYER (11)
#define TYPE_BULLET_ENEMY  (12)

#define ENABLE_ID                               \
    virtual const coreInt32 GetID  ()const = 0; \
    virtual const coreChar* GetName()const = 0;

#define ASSIGN_ID(i,n)                                          \
    static const coreInt32 ID = i;                              \
    inline const coreInt32 GetID  ()const override {return ID;} \
    inline const coreChar* GetName()const override {return n;}

#define CONTAINS(c,i) (std::find((c).begin(), (c).end(), (i)) != (c).end())


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

#include "additional/cBindContainer.h"
#include "file/cConfig.h"
#include "visual/cShadow.h"
#include "visual/cOutline.h"
#include "visual/cGlow.h"
#include "visual/cDistortion.h"
#include "visual/cSpecialEffects.h"
#include "visual/cForeground.h"
#include "visual/cPostProcessing.h"

extern cOutline*        g_pOutlineFull;      // main full outline-effect object
extern cOutline*        g_pOutlineDirect;    // main direct outline-effect object
extern cGlow*           g_pGlow;             // main glow-effect object
extern cDistortion*     g_pDistortion;       // main distortion-effect object
extern cSpecialEffects* g_pSpecialEffects;   // main special-effects object
extern cPostProcessing* g_pPostProcessing;   // main post-processing object

#include "environment/cOutdoor.h"
#include "environment/cWater.h"
#include "environment/background/cBackground.h"
#include "environment/cEnvironment.h"
#include "interface/cCombatText.h"
#include "interface/cInterface.h"
#include "interface/cMsgBox.h"
#include "interface/cTooltip.h"
#include "interface/menu/cMenu.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/boss/cBoss.h"
#include "game/mission/cMission.h"
#include "game/cPlayer.h"
#include "game/cGame.h"

extern cForeground*     g_pForeground;       // main foreground object
extern cEnvironment*    g_pEnvironment;      // main environment object
extern cMenu*           g_pMenu;             // main menu object
extern cGame*           g_pGame;             // main game object


#endif // _P1_GUARD_MAIN_H_