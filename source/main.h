//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MAIN_H_
#define _P1_GUARD_MAIN_H_

// NOTE: Update Order (C = view change): distortion -C-> glow -C-> [(water -C->) (shadow --->) background] -C-> foreground ---> *


// ****************************************************************
// engine header
#include "Core.h"


// ****************************************************************
// general definitions
#define COLOR_YELLOW_F     coreVector3(1.000f, 0.824f, 0.392f)
#define COLOR_ORANGE_F     coreVector3(1.000f, 0.443f, 0.227f)
#define COLOR_RED_F        coreVector3(1.000f, 0.275f, 0.275f)
#define COLOR_PURPLE_F     coreVector3(0.710f, 0.333f, 1.000f)
#define COLOR_BLUE_F       coreVector3(0.102f, 0.702f, 1.000f)
#define COLOR_GREEN_F      coreVector3(0.118f, 0.745f, 0.353f)
#define COLOR_BRONZE_F     coreVector3(0.925f, 0.663f, 0.259f)
#define COLOR_SILVER_F     coreVector3(0.855f, 0.855f, 0.878f)
#define COLOR_GOLD_F       coreVector3(1.000f, 0.859f, 0.000f)

#define COLOR_YELLOW_L     255, 210, 100
#define COLOR_ORANGE_L     255, 113,  58
#define COLOR_RED_L        255,  70,  70
#define COLOR_PURPLE_L     181,  85, 255
#define COLOR_BLUE_L        26, 179, 255
#define COLOR_GREEN_L       30, 190,  90
#define COLOR_BRONZE_L     236, 169,  66
#define COLOR_SILVER_L     218, 218, 224
#define COLOR_GOLD_L       255, 219,   0

#define CAMERA_POSITION    coreVector3(0.0f, 0.0f, 110.0f)
#define CAMERA_DIRECTION   coreVector3(0.0f, 0.0f,  -1.0f)
#define CAMERA_ORIENTATION coreVector3(0.0f, 1.0f,   0.0f)

#define LIGHT_DIRECTION    coreVector3(0.583953857f, -0.642349243f, -0.496360779f)

#define SHADER_SHADOW      "#define _P1_SHADOW_  (1) \n"
#define SHADER_SIMPLE      "#define _P1_SIMPLE_  (1) \n"
#define SHADER_SPHERIC     "#define _P1_SPHERIC_ (1) \n"

#define ENABLE_ID                           \
    virtual const int   GetID  ()const = 0; \
    virtual const char* GetName()const = 0;

#define ASSIGN_ID(i,n)                                      \
    static const int ID = i;                                \
    inline const int   GetID  ()const override {return ID;} \
    inline const char* GetName()const override {return n;}


// ****************************************************************
// game header files
#include "files/cConfig.h"
#include "visuals/cForeground.h"
#include "visuals/cShadow.h"
#include "visuals/cOutline.h"
#include "visuals/cSpecialEffects.h"
#include "visuals/cPostProcessing.h"
#include "environment/cOutdoor.h"
#include "environment/cWater.h"
#include "environment/cEnvironment.h"
#include "game/cBullet.h"
#include "game/cWeapon.h"
#include "game/cShip.h"
#include "game/cEnemy.h"
#include "game/cPlayer.h"
#include "game/cGame.h"


// ****************************************************************
// global variables
extern coreVector2      g_vGameResolution;       // pre-calculated 1:1 resolution

extern cPostProcessing* g_pPostProcessing;       // main post-processing object
extern cOutline*        g_pOutline;              // main outline-effect object
extern cForeground*     g_pForeground;           // main foreground object
extern cEnvironment*    g_pEnvironment;          // main environment object
extern cGame*           g_pGame;                 // main game object


#endif // _P1_GUARD_MAIN_H_