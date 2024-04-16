///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cCalorMission::cCalorMission()noexcept
: m_fSnowTick       (0.0f)
, m_afSnowStuck     {}
, m_Load            (CALOR_LOADS)
, m_pLoadOwner      (NULL)
, m_afLoadPower     {}
, m_Hail            (CALOR_HAILS)
, m_HailWave        (CALOR_HAILS)
, m_Chest           (CALOR_HAILS)
, m_ChestWave       (CALOR_HAILS)
, m_afChestTime     {}
, m_pAimOwner       (NULL)
, m_fAimTime        (0.0f)
, m_fAimAlpha       (0.0f)
, m_fAimFade        (0.0f)
, m_Star            (CALOR_STARS)
, m_StarChain       (CALOR_STARS * CALOR_CHAINS)
, m_apStarOwner     {}
, m_avStarOffset    {}
, m_afStarLength    {}
, m_iStarSwing      (0u)
, m_iStarAnimate    (0u)
, m_iStarConnect    (0u)
, m_fSwingSpeed     (1.0f)
, m_fSwingStart     (0.0f)
, m_afSwingValue    {}
, m_apCatchObject   {}
, m_avCatchPos      {}
, m_avCatchDir      {}
, m_fCatchTransfer  (0.0f)
, m_fStoryRangeAnim (0.0f)
, m_fAnimation      (0.0f)
, m_bStory          (g_pSave->GetHeader().oProgress.aiAdvance[6] < 7u)
{
    // 
    m_apBoss[0] = &m_Zeroth;

#if defined(_P1_UNUSED_)

    // 
    m_Load.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_LOADS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pLoad = &m_aLoadRaw[i];
            pLoad->DefineModel  ("object_sphere.md3");
            pLoad->DefineTexture(0u, "effect_energy.png");
            pLoad->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLoad->SetTexSize(coreVector2(3.0f,3.0f));
            pLoad->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Load.BindObject(pLoad);
        }
    }

    // 
    m_LoadCopy.DefineTexture(0u, "effect_energy.png");
    m_LoadCopy.DefineProgram("effect_energy_invert_program");
    m_LoadCopy.SetColor3    (COLOR_ENERGY_RED);
    m_LoadCopy.SetTexSize   (coreVector2(3.0f,3.0f));
    m_LoadCopy.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

#endif

    // 
    m_Hail    .DefineProgram("effect_energy_flat_inst_program");
    m_HailWave.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_HAILS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pHail = &m_aHailRaw[i];
            pHail->DefineModel  ("object_tetra_top.md3");
            pHail->DefineTexture(0u, "effect_energy.png");
            pHail->DefineProgram("effect_energy_flat_program");

            // set object properties
            pHail->SetSize   (coreVector3(1.0f,1.0f,1.0f) * (iType ? 2.6f : 2.2f));
            pHail->SetColor3 (COLOR_ENERGY_YELLOW * 0.7f);
            pHail->SetAlpha  (iType ? 1.0f : 0.7f);
            pHail->SetTexSize(coreVector2(1.0f,1.0f) * 0.3f);
            pHail->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_HailWave.BindObject(pHail);
                 else m_Hail    .BindObject(pHail);
        }
    }

    // 
    m_Chest    .DefineProgram("effect_energy_flat_invert_inst_program");
    m_ChestWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_CHESTS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pChest = &m_aChestRaw[i];
            pChest->DefineModel  ("object_cube_top.md3");
            pChest->DefineVolume ("object_cube_volume.md3");
            pChest->DefineTexture(0u, "effect_energy.png");
            pChest->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_invert_program");

            // set object properties
            pChest->SetColor3 (COLOR_ENERGY_MAGENTA * 0.7f);
            pChest->SetTexSize(coreVector2(1.0f,1.0f) * 0.5f);
            pChest->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_ChestWave.BindObject(pChest);
                 else m_Chest    .BindObject(pChest);
        }
    }

    // 
    m_AimArrow.DefineModel  ("object_arrow_short.md3");
    m_AimArrow.DefineTexture(0u, "effect_energy.png");
    m_AimArrow.DefineProgram("effect_energy_flat_invert_program");
    m_AimArrow.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 2.2f);
    m_AimArrow.SetColor3    (COLOR_ENERGY_CYAN * 0.6f);
    m_AimArrow.SetTexSize   (coreVector2(1.0f,1.0f) * 0.4f);
    m_AimArrow.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aAimSphere); ++i)
    {
        m_aAimSphere[i].DefineModel  ("object_sphere.md3");
        m_aAimSphere[i].DefineTexture(0u, "effect_energy.png");
        m_aAimSphere[i].DefineProgram("effect_energy_flat_spheric_program");
        m_aAimSphere[i].SetColor3    (COLOR_ENERGY_CYAN * 0.6f);
        m_aAimSphere[i].SetTexSize   (coreVector2(1.0f,1.0f) * 3.5f);
        m_aAimSphere[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Bull.DefineModelHigh("object_cube_top.md3");
    m_Bull.DefineModelLow ("object_cube_top.md3");
    m_Bull.DefineVolume   ("object_cube_volume.md3");
    m_Bull.DefineTexture  (0u, "effect_energy.png");
    m_Bull.DefineProgram  ("effect_energy_flat_invert_program");
    m_Bull.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
    m_Bull.SetTexSize     (coreVector2(1.0f,1.0f) * 0.4f);
    m_Bull.Configure      (1000, 0u, COLOR_ENERGY_ORANGE * 0.8f);
    m_Bull.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBullWave); ++i)
    {
        m_aBullWave[i].DefineModel  ("object_cube_top.md3");
        m_aBullWave[i].DefineTexture(0u, "effect_energy.png");
        m_aBullWave[i].DefineProgram("effect_energy_flat_invert_program");
        m_aBullWave[i].SetColor3    (COLOR_ENERGY_ORANGE * 0.8f);
        m_aBullWave[i].SetTexSize   (coreVector2(1.0f,1.0f) * 0.4f);
        m_aBullWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Star     .DefineProgram("effect_energy_flat_invert_inst_program");
    m_StarChain.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_STARS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (CALOR_CHAINS + 1u);

            // load object resources
            coreObject3D* pStar = &m_aStarRaw[i];
            pStar->DefineModel  (iType ? "object_sphere.md3" : "object_star.md3");
            pStar->DefineTexture(0u, "default_black.png");
            pStar->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pStar->SetSize   ((iType ? coreVector3(0.7f,0.7f,0.7f) : coreVector3(1.5f,1.5f,1.5f)) * 1.3f);
            pStar->SetColor3 (COLOR_ENERGY_WHITE * 0.8f);
            pStar->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_StarChain.BindObject(pStar);
                 else m_Star     .BindObject(pStar);
        }
    }

    // 
    m_Boulder.DefineModelHigh("ship_meteor.md3");
    m_Boulder.DefineModelLow ("ship_meteor.md3");
    m_Boulder.DefineTexture  (0u, "environment_stone_diff.png");
    m_Boulder.DefineTexture  (1u, "environment_stone_norm.png");
    m_Boulder.DefineProgram  ("object_meteor_blink_program");
    m_Boulder.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
    m_Boulder.Configure      (50, 0u, coreVector3(1.0f,1.0f,1.0f));
    m_Boulder.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_GHOST | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_SECRET | ENEMY_STATUS_KEEPVOLUME);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i)
    {
        m_aStoryRange[i].DefineModel  (i ? "object_cube_top.md3" : "object_penta_top.md3");
        m_aStoryRange[i].DefineTexture(0u, "effect_energy.png");
        m_aStoryRange[i].DefineProgram("effect_energy_flat_invert_program");
        m_aStoryRange[i].SetPosition  (coreVector3(0.0f, i ? -0.2f : 0.2f, 0.0f) * FOREGROUND_AREA3);
        m_aStoryRange[i].SetColor3    (i ? COLOR_PLAYER_BLUE : COLOR_PLAYER_GREEN);
        m_aStoryRange[i].SetTexSize   (coreVector2(0.1f,0.1f));
        m_aStoryRange[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    g_pGlow->BindList(&m_Load);
    g_pGlow->BindList(&m_Hail);
    g_pGlow->BindList(&m_HailWave);
    g_pGlow->BindList(&m_Chest);
    g_pGlow->BindList(&m_ChestWave);
    g_pGlow->BindList(&m_Star);
    g_pGlow->BindList(&m_StarChain);

    if(m_bStory)
    {
        // 
        constexpr const coreChar* apcName[] =
        {
            "default_normal.png",
            "default_white.png",
            "environment_block_diff.png",
            "environment_block_norm.png",
            "environment_clouds_blue.png",
            "environment_clouds_high.png",
            "environment_plant.png",
            "environment_snow_diff.png",
            "environment_water_norm.png",
            "effect_snow.png",

            "environment_plant_03.md3",
            "object_sting.md3",
            "object_cube_ice.md3",

            "effect_weather_snow_program",
            "environment_clouds_program",
            "environment_clouds_inst_program",
            "environment_ice_program",
            "object_ground_program",
            "object_ground_inst_program"
        };
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_apResCache); ++i)
        {
            m_apResCache[i] = Core::Manager::Resource->Get<coreResourceDummy>(apcName[i]);
            STATIC_ASSERT(ARRAY_SIZE(m_apResCache) == ARRAY_SIZE(apcName))
        }

        // 
        m_pNightmareSound = Core::Manager::Resource->Get<coreSound>("effect_nightmare.wav");
    }

    STATIC_ASSERT(CALOR_STARS <= sizeof(m_iStarSwing)  *8u)
    STATIC_ASSERT(CALOR_STARS <= sizeof(m_iStarAnimate)*8u)
}


// ****************************************************************
// destructor
cCalorMission::~cCalorMission()
{
    // 
    m_Boulder.Kill(false);

    // 
    g_pGlow->UnbindList(&m_Load);
    g_pGlow->UnbindList(&m_Hail);
    g_pGlow->UnbindList(&m_HailWave);
    g_pGlow->UnbindList(&m_Chest);
    g_pGlow->UnbindList(&m_ChestWave);
    g_pGlow->UnbindList(&m_Star);
    g_pGlow->UnbindList(&m_StarChain);

    // 
    this->DisableSnow  (false);
    this->DisableLoad  (false);
    this->DisableAim   (false);
    this->DisableBull  (false);
    this->DisableRanges(false);
    for(coreUintW i = 0u; i < CALOR_HAILS;  ++i) this->DisableHail (i, false);
    for(coreUintW i = 0u; i < CALOR_CHESTS; ++i) this->DisableChest(i, false);
    for(coreUintW i = 0u; i < CALOR_STARS;  ++i) this->DisableStar (i, false);

    if(m_bStory)
    {
        // 
        if(m_pNightmareSound->EnableRef(this)) m_pNightmareSound->Stop();
    }
}


// ****************************************************************
// 
void cCalorMission::EnableSnow()
{
    // 
    m_Snow.Enable();
}


// ****************************************************************
// 
void cCalorMission::DisableSnow(const coreBool bAnimated)
{
    // 
    m_Snow.Disable(bAnimated ? 0.5f : 0.0f);

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetMoveSpeed(1.0f);
    });
}


// ****************************************************************
// 
void cCalorMission::EnableLoad(const cShip* pOwner)
{
#if defined(_P1_UNUSED_)

    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    WARN_IF(oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableLoad(false);

    // 
    m_pLoadOwner     = pOwner;
    m_afLoadPower[0] = 0.0f;
    m_afLoadPower[1] = 0.0f;
    m_afLoadPower[2] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nInitFunc(&oLoad + i);
    nInitFunc(&m_LoadCopy);

    // 
    m_LoadCopy.DefineModel(pOwner->GetModelHigh());
    g_pGlow->BindObject(&m_LoadCopy);

#endif
}


// ****************************************************************
// 
void cCalorMission::DisableLoad(const coreBool bAnimated)
{
#if defined(_P1_UNUSED_)

    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pLoadOwner = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [&](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nExitFunc(&oLoad + i);
        nExitFunc(&m_LoadCopy);

        // 
        m_LoadCopy.DefineModel(NULL);
        g_pGlow->UnbindObject(&m_LoadCopy);
    }

#endif
}


// ****************************************************************
// 
void cCalorMission::EnableHail(const coreUintW iIndex)
{
    ASSERT(iIndex < CALOR_HAILS)
    coreObject3D* pHail = (*m_Hail    .List())[iIndex];
    coreObject3D* pWave = (*m_HailWave.List())[iIndex];

    // 
    WARN_IF(pHail->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableHail(iIndex, false);

    // 
    pHail->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cCalorMission::DisableHail(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < CALOR_HAILS)
    coreObject3D* pHail = (*m_Hail    .List())[iIndex];
    coreObject3D* pWave = (*m_HailWave.List())[iIndex];

    // 
    if(!pHail->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    pHail->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(pHail->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);
}


// ****************************************************************
// 
void cCalorMission::EnableChest(const coreUintW iIndex)
{
    ASSERT(iIndex < CALOR_CHESTS)
    coreObject3D* pChest = (*m_Chest    .List())[iIndex];
    coreObject3D* pWave  = (*m_ChestWave.List())[iIndex];

    // 
    WARN_IF(pChest->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableChest(iIndex, false);

    // 
    m_afChestTime[iIndex] = 0.0f;

    // 
    pChest->SetAlpha  (0.0f);
    pChest->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cCalorMission::DisableChest(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < CALOR_CHESTS)
    coreObject3D* pChest = (*m_Chest    .List())[iIndex];
    coreObject3D* pWave  = (*m_ChestWave.List())[iIndex];

    // 
    if(!pChest->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(!m_afChestTime[iIndex]) m_afChestTime[iIndex] = 1.0f;

    // 
    if(!bAnimated)
    {
        pChest->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cCalorMission::EnableAim(const cShip* pOwner)
{
    // 
    WARN_IF(m_AimArrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableAim(false);

    // 
    ASSERT(pOwner)
    m_pAimOwner = pOwner;
    m_fAimTime  = 0.0f;
    m_fAimAlpha = 0.0f;
    m_fAimFade  = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_AimArrow);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aAimSphere); ++i) nInitFunc(&m_aAimSphere[i]);

}


// ****************************************************************
// 
void cCalorMission::DisableAim(const coreBool bAnimated)
{
    // 
    if(!m_AimArrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pAimOwner = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(pObject);
        };
        nExitFunc(&m_AimArrow);
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aAimSphere); ++i) nExitFunc(&m_aAimSphere[i]);
    }
}


// ****************************************************************
// 
void cCalorMission::EnableBull()
{
    WARN_IF(m_aBullWave[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBull(false);

    // 
    m_Bull.Resurrect();

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBullWave); ++i) nInitFunc(&m_aBullWave[i]);
}


// ****************************************************************
// 
void cCalorMission::DisableBull(const coreBool bAnimated)
{
    if(!m_aBullWave[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Bull.Kill(false);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(pObject);
    };
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBullWave); ++i) nExitFunc(&m_aBullWave[i]);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(m_Bull.GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_ORANGE);
}


// ****************************************************************
// 
void cCalorMission::EnableStar(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vOffset)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    WARN_IF(pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableStar(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apStarOwner [iIndex] = pOwner;
    m_avStarOffset[iIndex] = vOffset;
    m_afStarLength[iIndex] = CALOR_CHAIN_CONSTRAINT1;

    // 
    ADD_BIT(m_iStarConnect, iIndex)

    // 
    if(pOwner == g_pGame->GetPlayer(iIndex)) g_pGame->SetRepairMove(iIndex, false);

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(pOwner->GetPosition() + coreVector3(0.0f,1.0f,0.0f));
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pStar);
    for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nInitFunc(pChain + i);
}


// ****************************************************************
// 
void cCalorMission::DisableStar(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_apStarOwner[iIndex] == g_pGame->GetPlayer(iIndex)) g_pGame->SetRepairMove(iIndex, true);

    if(bAnimated)
    {
        // 
        ADD_BIT(m_iStarAnimate, iIndex)
    }
    else
    {
        // 
        REMOVE_BIT(m_iStarAnimate, iIndex)

        // 
        m_apStarOwner [iIndex] = NULL;
        m_avStarOffset[iIndex] = coreVector2(0.0f,0.0f);
        m_afStarLength[iIndex] = 0.0f;

        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        nExitFunc(pStar);
        for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nExitFunc(pChain + i);
    }
}


// ****************************************************************
// 
void cCalorMission::EnableRanges()
{
    // 
    WARN_IF(m_aStoryRange[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableRanges(false);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i)
    {
        m_aStoryRange[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aStoryRange[i]);
    }
}


// ****************************************************************
// 
void cCalorMission::DisableRanges(const coreBool bAnimated)
{
    // 
    if(!m_aStoryRange[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i)
    {
        m_aStoryRange[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_aStoryRange[i]);
    }
}


// ****************************************************************
// 
void cCalorMission::StartSwing(const coreFloat fSpeed)
{
    ASSERT(!m_iStarSwing)

    // 
    m_iStarSwing  = BITLINE(CALOR_STARS);
    m_fSwingSpeed = fSpeed;
    m_fSwingStart = 0.0f;

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(m_apStarOwner[i])
        {
            const coreObject3D* pStar = (*m_Star.List())[i];
            m_afSwingValue[i] = ((pStar->GetPosition().xy() - m_apStarOwner[i]->GetPosition().xy()) * coreVector2(i ? -1.0f : 1.0f, 1.0f)).Angle();
            
            
            cPlayer* pPlayer = g_pGame->GetPlayer(i);
            if(pPlayer == m_apStarOwner[i])
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE))   // should not be changed on intro
                {
                    pPlayer->SetPosition(coreVector3(pPlayer->GetOldPos(), 0.0f));
                    pPlayer->SetMoveSpeed(0.0f);
                }
            }
        }
    }
}

// ****************************************************************
// 
void cCalorMission::StopSwing()
{
    // 
    m_iStarSwing  = 0u;
    m_fSwingSpeed = 1.0f;
    m_fSwingStart = 0.0f;
}


// ****************************************************************
// 
void cCalorMission::CatchObject(const coreUintW iIndex, cShip* pObject)
{
    ASSERT(iIndex < CALOR_STARS)
    const coreObject3D* pStar = (*m_Star.List())[iIndex];

    // 
    ASSERT(m_apStarOwner[iIndex])
    const coreVector2 vDir = (pStar->GetPosition().xy() - m_apStarOwner[iIndex]->GetPosition().xy()).Normalized();

    // 
    ASSERT(!m_apCatchObject[iIndex])
    m_apCatchObject[iIndex] = pObject;
    m_avCatchPos   [iIndex] = MapToAxisInv(pObject->GetPosition ().xy() - pStar->GetPosition().xy(), vDir);
    m_avCatchDir   [iIndex] = MapToAxisInv(pObject->GetDirection().xy(),                             vDir);
}

void cCalorMission::UncatchObject(const coreUintW iIndex)
{
    ASSERT(iIndex < CALOR_STARS)

    // 
    m_apCatchObject[iIndex] = NULL;
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnBottom()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Snow.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_ChestWave.Render();

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aAimSphere); ++i) m_aAimSphere[i].Render();
    }
    glDepthMask(true);

    // 
    m_Chest.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Chest);

    // 
    m_HailWave.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_HailWave);

    // 
    m_StarChain.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_StarChain);

    DEPTH_PUSH

    // 
    m_Hail.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Hail);

    // 
    m_Star.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Star);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBullWave); ++i) m_aBullWave[i].Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i) m_aStoryRange[i].Render();
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aStoryRange[i]);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnOver()
{
#if defined(_P1_UNUSED_)

    DEPTH_PUSH

    // 
    m_LoadCopy.Render();

#endif
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnTop()
{
    DEPTH_PUSH

#if defined(_P1_UNUSED_)

    // 
    m_Load.Render();

#endif

    // 
    m_AimArrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_AimArrow);
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnBefore()
{
    if(m_Snow.IsActive())
    {
        coreBool bTick = false;

        m_fSnowTick.Update(20.0f);
        if(m_fSnowTick >= 1.0f)
        {
            m_fSnowTick = FRACT(m_fSnowTick);
            bTick       = true;
        }

        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            const auto nHitFunc = [this](const coreVector2 vPosition, const coreFloat fSize)
            {
                return m_Snow.DrawPoint(vPosition, fSize, SNOW_TYPE_REMOVE);
            };

            if(nHitFunc(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f) +
               nHitFunc(pBullet->GetPosition().xy(),                                4.0f) +
               nHitFunc(pBullet->GetPosition().xy() - 0.5f * pBullet->GetFlyMove(), 3.0f))
            {
                pBullet->Deactivate(true);
            }
        });

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            const coreBool bHit  = m_Snow.TestCollision(pPlayer->GetPosition().xy());
            const coreBool bMove = !pPlayer->GetInput()->vMove.IsNull();

            if(!pPlayer->IsRolling() && bHit)
            {
                if(bMove)
                {
                    const coreFloat fOldStuck = m_afSnowStuck[i];
                    m_afSnowStuck[i].Update(1.0f/2.0f);

                    if((fOldStuck < 1.0f) && (m_afSnowStuck[i] >= 1.0f))
                    {
                        g_pSpecialEffects->CreateSplashDot(pPlayer->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_WHITE);
                        g_pSpecialEffects->PlaySound(pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
                    }
                }
            }
            else
            {
                m_afSnowStuck[i] = 0.0f;
            }

            pPlayer->SetMoveSpeed(m_afSnowStuck[i] ? LERP(0.2f, 0.0f, MIN1(m_afSnowStuck[i])) : 1.0f);

            if(bHit && bMove && bTick)
            {
                g_pSpecialEffects->CreateSplashDot(pPlayer->GetPosition(), 10.0f, 1u, COLOR_ENERGY_WHITE);
            }
        });
    }
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnMiddle()
{
    // 
    m_fSwingStart   .UpdateMin( 0.2f, 1.0f);
    m_fCatchTransfer.UpdateMax(-1.0f, 0.0f);

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        coreObject3D* pStar = (*m_Star.List())[i];
        if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afSwingValue[i].UpdateMod(CALOR_SWING_SPEED * m_fSwingSpeed * m_fSwingStart, 2.0f*PI);

        // 
        const cShip* pOwner = m_apStarOwner[i];
        if(pOwner)
        {
            cPlayer* pPlayer = g_pGame->GetPlayer(i);

            // 
            if(pPlayer == pOwner)
            {
                if(HAS_BIT(m_iStarSwing, i))
                {
                    const coreVector2 vDir = coreVector2::Direction(m_afSwingValue[i]) * coreVector2(i ? -1.0f : 1.0f, 1.0f);

                    // 
                    pStar->SetPosition (coreVector3(pPlayer->GetPosition().xy() + vDir * m_afStarLength[i], pPlayer->GetPosition().z));   // # for intro animation
                    pStar->SetDirection(coreVector3(vDir, 0.0f));

                    // 
                    pPlayer->SetMoveSpeed(m_fSwingStart);
                }

                if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    pStar  ->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                }
            }
            else
            {
                const coreVector2 vBase = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());
                const coreVector2 vDiff = vBase - pStar->GetPosition().xy();

                // 
                pStar->SetDirection(coreVector3(-vDiff.Normalized(), 0.0f));
            }

            // 
            const coreVector2 vBase = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());   // # again
            const coreVector2 vDiff = vBase - pStar->GetPosition().xy();
            const coreVector2 vDir  = vDiff.IsNull() ? coreVector2(0.0f,1.0f) : vDiff.Normalized();

            // 
            if((pPlayer == pOwner) && !coreMath::IsNear(vDiff.LengthSq(), 0.0f))
            {
                //if(!pPlayer->HasStatus(PLAYER_STATUS_DEAD))
                //if(!HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    // 
                    Core::Manager::Object->TestCollision(TYPE_ENEMY, pStar, [&](cEnemy* OUTPUT pEnemy, const coreObject3D* pStar, const coreVector3 vIntersection, const coreBool bFirstHit)
                    {
                        if(m_apCatchObject[i] || (pEnemy == &m_Bull) || pEnemy->HasStatus(ENEMY_STATUS_BOSS) || pEnemy->IsChild() || (pEnemy->GetID() == cRepairEnemy::ID)) return;

                        if(pEnemy == m_apCatchObject[1u - i])
                        {
                            // 
                            if(false) if(!m_fCatchTransfer)
                            {
                                m_fCatchTransfer = 0.5f;

                                // 
                                this->UncatchObject(1u - i);
                                this->CatchObject(i, pEnemy);
                            }
                        }
                        else
                        {
                            // 
                            this->CatchObject(i, pEnemy);
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.5f, SOUND_EFFECT_DUST);
                            //g_pSpecialEffects->RumblePlayer(pPlayer, SPECIAL_RUMBLE_SMALL, 250u);

                            // 
                            pPlayer->GetScoreTable()->RefreshCooldown();

                            // 
                            //pEnemy->ChangeToTop();
                            pEnemy->AddStatus           (ENEMY_STATUS_GHOST | ENEMY_STATUS_KEEPVOLUME);
                            pEnemy->RemoveStatus        (ENEMY_STATUS_INVINCIBLE);
                            pEnemy->SetCollisionModifier(pEnemy->GetCollisionModifier() * coreVector3(1.0f,1.0f,0.1f));
                        }
                    });

                    if(m_apCatchObject[i])
                    {
                        cEnemy* pCopy = d_cast<cEnemy*>(m_apCatchObject[i]);

                        // 
                        pCopy->SetPosition (coreVector3(MapToAxis(m_avCatchPos[i], -vDir), 0.0f) + pStar->GetPosition());
                        pCopy->SetDirection(coreVector3(MapToAxis(m_avCatchDir[i], -vDir), 0.0f));

                        // 
                        Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pCopy, [](cBullet* OUTPUT pBullet, const cEnemy* pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
                        {
                            if((pBullet->GetID() != cFlipBullet::ID) && (pBullet->GetID() != cTriangleBullet::ID)) return;

                            pBullet->Deactivate(true, vIntersection.xy());

                            g_pGame->PlayVanishSound(pBullet->GetPosition());
                        });

                        // 
                        Core::Manager::Object->TestCollision(TYPE_ENEMY, pCopy, [&](cEnemy* OUTPUT pEnemy, cEnemy* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
                        {
                            if(!m_apCatchObject[i])                   return;
                            if(pEnemy == &m_Bull)                     return;
                            if(pObject->HasStatus(ENEMY_STATUS_BOSS)) return;   // boss should not collide with own children
                            if(pEnemy->GetID() == cRepairEnemy::ID)   return;

                            // 
                            const coreBool bEnemyBig  = (pEnemy ->GetMaxHealth() >= 10) || pEnemy->HasStatus(ENEMY_STATUS_BOSS) || pEnemy->IsChild();
                            const coreBool bObjectBig = (pObject->GetMaxHealth() >= 10);

                            if(bObjectBig || !bEnemyBig)
                            {
                                // 
                                const coreBool bOther      = (pEnemy == m_apCatchObject[1u - i]);
                                const coreBool bInvincible = pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE);

                                // 
                                pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                                pEnemy->TakeDamage  (pObject->GetCurHealth(), ELEMENT_NEUTRAL, vIntersection.xy(), bOther ? g_pGame->GetPlayer(1u - i) : pPlayer, true);

                                // 
                                if(bOther) this->UncatchObject(1u - i);
                                
                                //g_pSpecialEffects->RumblePlayer(pPlayer, SPECIAL_RUMBLE_SMALL, 250u);

                                // 
                                if(bInvincible) pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
                            }

                            if(bEnemyBig || !bObjectBig)
                            {
                                // 
                                pObject->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                                pObject->TakeDamage  (10000, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer, true);

                                // 
                                this->UncatchObject(i);
                            }
                        });
                    }
                }
            }

            STATIC_ASSERT(CALOR_STARS == 2u)
        }
        else
        {
            // 
            m_afStarLength[i] += 1.0f * TIME;
            if(m_afStarLength[i] >= 2.0f) this->DisableStar(i, false);
            
            
            const coreFloat fSpeed = 120.0f * TIME * m_fSwingSpeed;//m_avStarOffset[i];
            const coreVector2 vDir1  = -coreVector2::Direction(m_avStarOffset[i].x);
            
            // 
            for(coreUintW j = 0u; j < CALOR_CHAINS; ++j)
            {
                coreObject3D* pChain = (*m_StarChain.List())[i*CALOR_CHAINS + j];
                if(!pChain->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                // 
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) * -0.3f + (1.0f*PI)) * coreVector2(i ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vPos = pChain->GetPosition().xy() + MapToAxis(vDir, vDir1) * fSpeed;

                // 
                pChain->SetPosition(coreVector3(vPos, 0.0f));
            }

#if 0
            // 
            const coreFloat   fSpeed = 6.0f * TIME * m_avStarOffset[i].y;
            const coreVector2 vDir1  = coreVector2::Direction(m_avStarOffset[i].x);
            const coreVector2 vPos1  = pStar->GetPosition().xy() + vDir1 * fSpeed;

            // 
            pStar->SetPosition(coreVector3(vPos1, 0.0f));

            // 
            for(coreUintW j = 0u; j < CALOR_CHAINS; ++j)
            {
                coreObject3D* pChain = (*m_StarChain.List())[i*CALOR_CHAINS + j];
                if(!pChain->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                // 
                const coreVector2 vDir2 = coreVector2::Direction(I_TO_F(j) * -0.3f + (1.0f*PI));
                const coreVector2 vPos2 = pChain->GetPosition().xy() + (vDir2 + vDir1 * 20.0f).Normalized() * fSpeed;

                // 
                pChain->SetPosition(coreVector3(vPos2, 0.0f));
            }

            if(m_apCatchObject[i])
            {
                cShip* pCopy = m_apCatchObject[i];

                // 
                pCopy->SetPosition (coreVector3(MapToAxis(m_avCatchPos[i], -vDir1), 0.0f) + pStar->GetPosition());
                pCopy->SetDirection(coreVector3(MapToAxis(m_avCatchDir[i], -vDir1), 0.0f));
            }
            
#endif
        }
    }

    // 
    m_Star.MoveNormal();
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_Snow.Move();

#if defined(_P1_UNUSED_)

    // 
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i)
    {
        coreObject3D& oLoad = m_aLoadRaw[i];
        if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vDir    = coreVector2::Direction((I_TO_F(i) / I_TO_F(CALOR_LOADS)) * (2.0f*PI)).InvertedX();
        const coreFloat   fScale  = CLAMP(m_afLoadPower[0] - I_TO_F(i), 0.0f, 1.0f);
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/12.0f);

        // 
        const cShip* pOwner = m_pLoadOwner;
        if(pOwner)
        {
            // 
            oLoad.SetPosition(coreVector3(pOwner->GetPosition().xy() + vDir * (pOwner->GetVisualRadius() + 3.5f), 0.0f));
            oLoad.SetAlpha   (1.0f);
        }
        else
        {
            // 
            oLoad.SetPosition(coreVector3(oLoad.GetPosition().xy() + vDir * (50.0f * TIME), 0.0f));
            oLoad.SetAlpha   (MAX0(oLoad.GetAlpha() - 3.0f * TIME));

            // 
            if(!i && !oLoad.GetAlpha()) this->DisableLoad(false);
        }

        // 
        oLoad.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.7f * fScale);
        oLoad.SetColor3   (COLOR_ENERGY_RED * ((fScale >= 1.0f) ? 1.0f : 0.5f));
        oLoad.SetTexOffset(coreVector2(0.0f, FRACT(-2.0f * m_fAnimation + fOffset)));
    }

    // 
    m_Load.MoveNormal();

    // 
    if(m_pLoadOwner)
    {
        // 
        m_LoadCopy.SetPosition   (m_pLoadOwner->GetPosition   ());
        m_LoadCopy.SetSize       (m_pLoadOwner->GetSize       ());
        m_LoadCopy.SetDirection  (m_pLoadOwner->GetDirection  ());
        m_LoadCopy.SetOrientation(m_pLoadOwner->GetOrientation());
        m_LoadCopy.SetTexOffset  (coreVector2(FRACT(1.4f * m_fAnimation), 0.0f));   // TODO 1: fix model tex coords

        // 
        if(F_TO_UI(m_afLoadPower[0]) != F_TO_UI(m_afLoadPower[1]))
        {
            const coreUintW iIndex = F_TO_UI(m_afLoadPower[1]);
            ASSERT(iIndex < CALOR_LOADS)

            g_pSpecialEffects->CreateSplashColor(m_aLoadRaw[iIndex].GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
        }
    }
    m_LoadCopy.SetAlpha(m_afLoadPower[2] * m_aLoadRaw[0].GetAlpha());
    m_LoadCopy.Move();

    // 
    m_afLoadPower[0].UpdateMax(-0.6f, FLOOR(m_afLoadPower[0]));
    m_afLoadPower[1] = m_afLoadPower[0];
    m_afLoadPower[2].UpdateMax(-1.0f, 0.0f);

#endif

    // 
    for(coreUintW i = 0u; i < CALOR_HAILS; ++i)
    {
        coreObject3D* pHail = (*m_Hail    .List())[i];
        coreObject3D* pWave = (*m_HailWave.List())[i];
        if(!pHail->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/3.0f);
        const coreVector2 vDir    = coreVector2::Direction((2.0f*PI) * m_fAnimation);
        //const coreVector2 vDir    = coreVector2::Direction((-6.0f*PI) * m_fAnimation);

        // 
        pHail->SetDirection(coreVector3(vDir, 0.0f));
        pHail->SetTexOffset(coreVector2(0.0f, FRACT(0.7f * m_fAnimation + fOffset)));
        
        
       // const coreVector2 vDir2    = coreVector2::Direction((2.0f*PI) * m_fAnimation);

        // 
        pWave->SetPosition (pHail->GetPosition ());
        pWave->SetDirection(pHail->GetDirection() * -1.0f);
        //pWave->SetDirection(coreVector3(vDir2, 0.0f));
        pWave->SetTexOffset(pHail->GetTexOffset());
    }

    // 
    m_Hail    .MoveNormal();
    m_HailWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < CALOR_CHESTS; ++i)
    {
        coreObject3D* pChest = (*m_Chest    .List())[i];
        coreObject3D* pWave  = (*m_ChestWave.List())[i];
        if(!pChest->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        if(m_afChestTime[i])
        {
            // 
            m_afChestTime[i].Update(1.0f);

            // 
            if(m_afChestTime[i] >= 2.0f) this->DisableChest(i, false);
        }

        // 
        const coreFloat   fBang   = STEPB(1.0f, 2.0f, m_afChestTime[i]);
        const coreVector3 vSize   = coreVector3(1.0f,1.0f,1.0f) * 7.5f;
        const coreFloat   fAlpha  = m_afChestTime[i] ? BLENDH3(1.0f - fBang) : 0.0f;
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/6.0f);

        // 
        pChest->SetSize     (vSize * LERP(1.0f, 0.0f, fBang));
        pChest->SetAlpha    (fAlpha * 0.95f + 0.05f);
        pChest->SetTexOffset(coreVector2(0.0f, FRACT(0.5f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pChest->GetPosition ());
        pWave->SetSize     (vSize * LERP(1.0f, 1.6f, fBang));
        pWave->SetAlpha    (fAlpha);
        pWave->SetTexOffset(pChest->GetTexOffset());
    }

    // 
    m_Chest    .MoveNormal();
    m_ChestWave.MoveNormal();

    // 
    if(m_AimArrow.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        if(m_pAimOwner)
        {
            // 
            m_fAimTime.UpdateMin(1.0f, 1.0f);

            // 
            const coreVector2 vTarget = m_pAimOwner->GetPosition().xy();
            const coreVector2 vDir    = vTarget.Normalized();
            const coreVector2 vClamp  = (vTarget - vDir * 8.0f).Processed(CLAMP, -FOREGROUND_AREA.x * 0.9f, FOREGROUND_AREA.x * 0.9f);
            const coreVector2 vPos    = vClamp + (vClamp - vTarget).Normalized() * LERPB(100.0f, 2.0f + 2.0f * SIN(m_fAnimation * (8.0f*PI)), m_fAimTime);

            // 
            m_AimArrow.SetPosition (coreVector3(vPos, 0.0f));
            m_AimArrow.SetDirection(coreVector3(vDir, 0.0f));

            // 
            m_fAimFade = STEPH3(2.0f, 6.0f, vTarget.Length());
        }

        // 
        if(m_pAimOwner) m_fAimAlpha.UpdateMin( 5.0f, 1.0f);
                   else m_fAimAlpha.UpdateMax(-5.0f, 0.0f);

        // 
        if(!m_fAimAlpha) this->DisableAim(false);

        // 
        m_AimArrow.SetAlpha    (BLENDH3(m_fAimAlpha) * m_fAimFade);
        m_AimArrow.SetTexOffset(coreVector2(0.0f, -m_fAnimation));
        m_AimArrow.Move();

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aAimSphere); ++i)
        {
            const coreFloat fTime   = FmodRange((I_TO_F(i) + m_fAnimation * 10.0f) / 3.0f, 0.0f, 1.0f);
            const coreFloat fOffset = I_TO_F(i) * (1.0f/3.0f);

            // 
            if(m_pAimOwner) m_aAimSphere[i].SetPosition(m_pAimOwner->GetPosition());

            // 
            m_aAimSphere[i].SetSize     (coreVector3(1.0f,1.0f,1.0f) * 7.0f * LERP(2.0f, 0.0f, fTime));
            m_aAimSphere[i].SetTexOffset(coreVector2(0.0f, m_fAnimation + fOffset));
            m_aAimSphere[i].SetAlpha    (BLENDH3(m_fAimAlpha) * LERP(0.0f, 1.0f, fTime));
            m_aAimSphere[i].Move();
        }
    }
    
    
    // 
    if(!m_Bull.HasStatus(ENEMY_STATUS_DEAD))
    {
        // 
        m_Bull.SetDirection(coreVector3(coreVector2::Direction((-4.0f*PI) * m_fAnimation), 0.0f));
        m_Bull.SetTexOffset(coreVector2(0.0f, -0.5f * m_fAnimation));
        m_Bull.coreObject3D::Move();

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBullWave); ++i)
        {
            const coreFloat   fStep   = I_TO_F(ARRAY_SIZE(m_aBullWave) - i);
            const coreFloat   fOffset = I_TO_F(i) * (1.0f/3.0f);
            const coreVector2 vDir    = coreVector2::Direction((-4.0f*PI) * m_fAnimation * (fStep + 1.0f) - (0.2f*PI) * fStep);

            m_aBullWave[i].SetPosition (m_Bull.GetPosition());
            m_aBullWave[i].SetSize     (m_Bull.GetSize    () * (1.0f + 0.2f * fStep));
            m_aBullWave[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aBullWave[i].SetTexOffset(coreVector2(0.0f, m_fAnimation + fOffset));
            m_aBullWave[i].SetAlpha    (0.5f / fStep);
            m_aBullWave[i].Move();
        }
    }
    
    
    

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        coreObject3D* pStar = (*m_Star.List())[i];
        if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apStarOwner[i];
        if(pOwner)
        {
            cPlayer* pPlayer = g_pGame->GetPlayer(i);

            if(pPlayer == pOwner)
            {
                if(!HAS_BIT(m_iStarSwing, i))
                {
                    const coreVector2 vBase = pPlayer->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pPlayer->GetDirection().xy());
                    const coreVector2 vDiff = vBase - pStar->GetPosition().xy();

                    // 
                    pPlayer->SetPosition(coreVector3(pStar->GetPosition().xy() + vDiff.Normalized() * MIN(vDiff.Length(), m_afStarLength[i]), 0.0f));
                }

                if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    pStar  ->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                }
            }

            // 
            const coreVector2 vBase = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());   // # again
            const coreVector2 vDiff = vBase - pStar->GetPosition().xy();
            const coreVector2 vDir  = vDiff.IsNull() ? coreVector2(0.0f,1.0f) : vDiff.Normalized();
            const coreFloat   fLen  = vDiff.Length();

            // 
            for(coreUintW j = 0u; j < CALOR_CHAINS; ++j)
            {
                coreObject3D* pChain = (*m_StarChain.List())[i*CALOR_CHAINS + j];

                // 
                const coreFloat   fOffset = MIN(I_TO_F(j) * 2.5f + 4.7f, fLen);
                const coreVector2 vPos    = vBase - vDir * fOffset;

                // 
                pChain->SetPosition(coreVector3(vPos, 0.0f));
                pChain->SetColor3  (COLOR_ENERGY_WHITE * 0.8f);
                pChain->SetAlpha   (STEPH3(1.7f, 2.2f, fLen - fOffset));
                pChain->SetEnabled ((pChain->GetAlpha() && HAS_BIT(m_iStarConnect, i)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            }

            // 
            if(pPlayer != pOwner)
            {
                if(m_apCatchObject[i])
                {
                    cShip* pCopy = m_apCatchObject[i];

                    // 
                    pCopy->SetPosition (coreVector3(MapToAxis(m_avCatchPos[i], -vDir), 0.0f) + pStar->GetPosition());
                    pCopy->SetDirection(coreVector3(MapToAxis(m_avCatchDir[i], -vDir), 0.0f));

                    // 
                    Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pCopy, [](cBullet* OUTPUT pBullet, const cShip* pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
                    {
                        if((pBullet->GetID() != cFlipBullet::ID) && (pBullet->GetID() != cTriangleBullet::ID)) return;

                        pBullet->Deactivate(true, vIntersection.xy());

                        g_pGame->PlayVanishSound(pBullet->GetPosition());
                    });
                }
            }

            // 
            if(HAS_BIT(m_iStarAnimate, i))
            {
                REMOVE_BIT(m_iStarAnimate, i)

                // 
                m_apStarOwner [i] = NULL;
                m_avStarOffset[i] = coreVector2(vDiff.Angle(), fLen * m_fSwingSpeed);
                m_afStarLength[i] = 0.0f;
                
                g_pSpecialEffects->CreateSplashColor(pStar->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->PlaySound(pStar->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_02);
                (*m_Star.List())[i]->SetPosition(coreVector3(HIDDEN_POS, 0.0f));                 
                
                this->UncatchObject(i);
            }

            STATIC_ASSERT(CALOR_STARS == 2u)
        }
    }

    // 
    m_StarChain.MoveNormal();

    // 
    if(m_aStoryRange[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryRange); ++i)
        {
            m_fStoryRangeAnim.Update(0.4f);

            const coreVector2 vDir = coreVector2::Direction(m_fStoryRangeAnim * (-1.6f*PI));

            m_aStoryRange[i].SetSize     (coreVector3(1.0f,1.0f,1.0f) * (i ? 0.03f : 0.032f) * 50.0f);              
            m_aStoryRange[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aStoryRange[i].SetTexOffset(m_aStoryRange[i].GetTexOffset() - m_aStoryRange[i].GetDirection().xy() * (0.04f * TIME));
            m_aStoryRange[i].Move();
        }
    }
}