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
cHarenaMission::cHarenaMission()noexcept
: m_Floor        (HARENA_FLOORS)
, m_apFloorOwner {}
, m_Spike        (HARENA_SPIKES)
, m_SpikeBoard   (HARENA_SPIKES)
, m_afSpikeTime  {}
, m_afSpikeCur   {}
, m_afSpikeMax   {}
, m_iSpikeGood   (0u)
, m_iSpikeBad    (0u)
, m_Egg          (HARENA_EGGS)
, m_EggWave      (HARENA_EGGS)
, m_Flummi       (HARENA_FLUMMIS)
, m_iInsanity    (0u)
, m_fAnimation   (0.0f)
, m_bStory       (!HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiState, STATE_STORY_HARENA))
{
    // 
    m_apBoss[0] = &m_Tiger;

    // 
    m_Floor.DefineProgram("effect_decal_single_inst_program");
    {
        for(coreUintW i = 0u; i < HARENA_FLOORS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pFloor = &m_aFloorRaw[i];
            pFloor->DefineModel  (Core::Manager::Object->GetLowQuad());
            pFloor->DefineTexture(0u, "effect_headlight_point.png");
            pFloor->DefineProgram("effect_decal_single_program");

            // set object properties
            pFloor->SetColor3 (coreVector3(0.0f,0.0f,0.0f));
            pFloor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Floor.BindObject(pFloor);
        }
    }

    // 
    m_Spike     .DefineProgram("object_ship_depth_inst_program");
    m_SpikeBoard.DefineProgram("object_board_inst_program");
    {
        for(coreUintW i = 0u; i < HARENA_SPIKES_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pSpike = &m_aSpikeRaw[i];
            pSpike->DefineModel  (iType ? Core::Manager::Object->GetLowQuad() : Core::Manager::Resource->Get<coreModel>("object_spike.md3"));
            pSpike->DefineTexture(0u, iType ? "menu_background_black.png" : "ship_enemy.png");
            pSpike->DefineProgram(iType ? "object_board_program" : "object_ship_depth_program");

            // set object properties
            pSpike->SetTexOffset        ((coreVector2(0.17f,0.31f) * I_TO_F(i / 2u)).Processed(FRACT));
            pSpike->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.07f);
            pSpike->SetAlpha            (0.7f);
            pSpike->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_SpikeBoard.BindObject(pSpike);
                 else m_Spike     .BindObject(pSpike);
        }
    }

    // 
    m_Correct.DefineModel  (Core::Manager::Object->GetLowQuad());
    m_Correct.DefineTexture(0u, "effect_aim.png");
    m_Correct.DefineProgram("effect_decal_program");
    m_Correct.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 12.0f);
    m_Correct.SetColor3    (COLOR_MENU_BLUE);
    m_Correct.SetAlpha     (0.8f);
    m_Correct.SetTexSize   (coreVector2(0.5f,1.0f));
    m_Correct.SetTexOffset (coreVector2(0.5f,0.0f));
    m_Correct.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Egg    .DefineProgram("effect_energy_flat_invert_inst_program");
    m_EggWave.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < HARENA_EGGS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pEgg = &m_aEggRaw[i];
            pEgg->DefineModel  ("object_sphere.md3");
            pEgg->DefineTexture(0u, "effect_energy.png");
            pEgg->DefineProgram(iType ? "effect_energy_flat_program" : "effect_energy_flat_invert_program");

            // set object properties
            pEgg->SetSize   (coreVector3(1.0f,1.0f,1.0f) * (iType ? 3.5f : 2.2f));
            pEgg->SetColor3 (COLOR_ENERGY_GREEN * 0.8f);
            pEgg->SetTexSize(coreVector2(1.0f,1.0f) * 2.0f);
            pEgg->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_EggWave.BindObject(pEgg);
                 else m_Egg    .BindObject(pEgg);
        }
    }

    // 
    m_Flummi.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < HARENA_FLUMMIS_RAWS; ++i)
        {

            // load object resources
            coreObject3D* pFlummi = &m_aFlummiRaw[i];
            pFlummi->DefineModel  ("object_tetra_top.md3");
            pFlummi->DefineTexture(0u, "effect_energy.png");
            pFlummi->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pFlummi->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
            pFlummi->SetColor3 (COLOR_ENERGY_ORANGE * 0.8f);
            pFlummi->SetTexSize(coreVector2(1.0f,1.0f) * 0.3f);
            pFlummi->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Flummi.BindObject(pFlummi);
        }
    }

    // 
    m_Aim.DefineModel  ("object_arrow_short.md3");
    m_Aim.DefineTexture(0u, "effect_energy.png");
    m_Aim.DefineProgram("effect_energy_flat_invert_program");
    m_Aim.SetColor3    (COLOR_ENERGY_BLUE * 0.8f);
    m_Aim.SetTexSize   (coreVector2(1.0f,1.0f) * 0.3f);
    m_Aim.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    g_pGlow->BindList(&m_Egg);
    g_pGlow->BindList(&m_EggWave);
    g_pGlow->BindList(&m_Flummi);
}


// ****************************************************************
// destructor
cHarenaMission::~cHarenaMission()
{
    // 
    g_pGlow->UnbindList(&m_Egg);
    g_pGlow->UnbindList(&m_EggWave);
    g_pGlow->UnbindList(&m_Flummi);

    // 
    for(coreUintW i = 0u; i < HARENA_FLOORS;  ++i) this->DisableFloor (i, false);
    for(coreUintW i = 0u; i < HARENA_SPIKES;  ++i) this->DisableSpike (i, false);
    for(coreUintW i = 0u; i < HARENA_EGGS;    ++i) this->DisableEgg   (i, false);
    for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i) this->DisableFlummi(i, false);
    this->DisableAim    (false);
    this->DisableCorrect(false);
}


// ****************************************************************
// 
void cHarenaMission::EnableFloor(const coreUintW iIndex, const cShip* pOwner, const coreFloat fScale)
{
    ASSERT(iIndex < HARENA_FLOORS)
    coreObject3D& oFloor = m_aFloorRaw[iIndex];

    // 
    WARN_IF(oFloor.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableFloor(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apFloorOwner[iIndex] = pOwner;

    // 
    oFloor.SetSize   (coreVector3(1.0f,1.0f,1.0f) * fScale);
    oFloor.SetAlpha  (0.0f);
    oFloor.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cHarenaMission::DisableFloor(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < HARENA_FLOORS)
    coreObject3D& oFloor = m_aFloorRaw[iIndex];

    // 
    if(!oFloor.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apFloorOwner[iIndex] = NULL;

    // 
    if(!bAnimated) oFloor.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cHarenaMission::EnableSpike(const coreUintW iIndex, const coreBool bDelayed)
{
    ASSERT(iIndex < HARENA_SPIKES)
    coreObject3D* pSpike = (*m_Spike     .List())[iIndex];
    coreObject3D* pBoard = (*m_SpikeBoard.List())[iIndex];

    // 
    WARN_IF(pSpike->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableSpike(iIndex, false);

    // 
    m_afSpikeTime[iIndex] = bDelayed ? (1.0f - 0.022f * I_TO_F(iIndex)) : 1.0f;
    m_afSpikeCur [iIndex] = 0.0f;
    m_afSpikeMax [iIndex] = 0.0f;
    REMOVE_BIT(m_iSpikeGood, iIndex);
    REMOVE_BIT(m_iSpikeBad,  iIndex);

    // 
    const coreFloat   fScale = (1.0f / I_TO_F(HARENA_SPIKE_DIMENSION)) * FOREGROUND_AREA.x * 2.2f;
    const coreVector2 vPos   = (coreVector2(I_TO_F(iIndex % HARENA_SPIKE_DIMENSION), I_TO_F(iIndex / HARENA_SPIKE_DIMENSION)) - 0.5f * I_TO_F(HARENA_SPIKE_DIMENSION - 1u)).InvertedY() * fScale;

    // 
    pSpike->SetPosition(coreVector3(vPos, 0.0f));
    pBoard->SetPosition(coreVector3(vPos, 0.0f));
    pSpike->SetSize    (coreVector3(0.0f, 0.0f, (fScale - 1.0f)) * 0.1f);
    pBoard->SetSize    (coreVector3(0.0f, 0.0f, (fScale - 1.0f)));
    pSpike->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    pBoard->SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cHarenaMission::DisableSpike(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < HARENA_SPIKES)
    coreObject3D* pSpike = (*m_Spike     .List())[iIndex];
    coreObject3D* pBoard = (*m_SpikeBoard.List())[iIndex];

    // 
    if(!pSpike->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_afSpikeTime[iIndex] > 0.0f) m_afSpikeTime[iIndex] = -1.0f + 0.022f * I_TO_F(iIndex);

    // 
    if(!bAnimated)
    {
        pSpike->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pBoard->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cHarenaMission::EnableCorrect()
{
    // 
    WARN_IF(m_Correct.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableCorrect(false);

    // 
    m_Correct.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    m_Correct.SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cHarenaMission::DisableCorrect(const coreBool bAnimated)
{
    // 
    if(!m_Correct.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Correct.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cHarenaMission::EnableEgg(const coreUintW iIndex)
{
    ASSERT(iIndex < HARENA_EGGS)
    coreObject3D* pEgg  = (*m_Egg    .List())[iIndex];
    coreObject3D* pWave = (*m_EggWave.List())[iIndex];

    // 
    WARN_IF(pEgg->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableEgg(iIndex, false);

    // 
    pEgg ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cHarenaMission::DisableEgg(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < HARENA_EGGS)
    coreObject3D* pEgg  = (*m_Egg    .List())[iIndex];
    coreObject3D* pWave = (*m_EggWave.List())[iIndex];

    // 
    if(!pEgg->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    pEgg ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(pEgg->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cHarenaMission::EnableFlummi(const coreUintW iIndex)
{
    ASSERT(iIndex < HARENA_FLUMMIS)
    coreObject3D& oFlummi = m_aFlummiRaw[iIndex];

    // 
    WARN_IF(oFlummi.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableFlummi(iIndex, false);

    // 
    oFlummi.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cHarenaMission::DisableFlummi(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < HARENA_FLUMMIS)
    coreObject3D& oFlummi = m_aFlummiRaw[iIndex];

    // 
    if(!oFlummi.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oFlummi.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(oFlummi.GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);
}


// ****************************************************************
// 
void cHarenaMission::EnableAim()
{
    // 
    WARN_IF(m_Aim.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableAim(false);

    // 
    m_Aim.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Aim);
}


// ****************************************************************
// 
void cHarenaMission::DisableAim(const coreBool bAnimated)
{
    // 
    if(!m_Aim.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Aim.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Aim);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(m_Aim.GetPosition(), 25.0f, 10u, COLOR_ENERGY_BLUE);
}


// ****************************************************************
// 
void cHarenaMission::CreateExternChild(const coreVector2 vPosition, const coreVector2 vMove, const coreUint8 iType)
{
    // 
    sChildData oData;
    oData.vPosition = vPosition;
    oData.vMove     = vMove;
    oData.iType     = iType;

    // 
    m_avChildData.push_back(oData);
}


// ****************************************************************
// 
void cHarenaMission::PlayInsanity()
{
    if(m_iInsanity == HARENA_INSANITY_P1)
    {
        ASSERT(m_anStage.empty())

        // 
        m_fStageTimeBefore = m_fStageTime;
        m_fStageTime.Update(1.0f);

        // 
        m_fStageSubTimeBefore = m_fStageSubTime;
        m_fStageSubTime.Update(1.0f);

        // 
        m_aInsanityStage[3]();
    }
    else if(m_iInsanity)
    {
        // 
        switch(m_iInsanity)
        {
        default: ASSERT(false)
        case 1u: m_aInsanityStage[0](); break;   // ghost
        case 2u: m_aInsanityStage[1](); break;   // mole
        case 3u: m_aInsanityStage[2](); break;   // tower
        case 4u: m_aInsanityStage[3](); break;   // mother
        case 5u: m_aInsanityStage[4](); break;   // spike
        }
    }
}


// ****************************************************************
// 
void cHarenaMission::ChangeInsanity(const coreUint8 iInsanity)
{
    // 
    WARN_IF(m_iInsanity >= iInsanity) return;
    m_iInsanity = iInsanity;

    // 
    FOR_EACH(it, m_apPath)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apSquad) SAFE_DELETE(*it)

    // 
    m_apPath .clear();
    m_apSquad.clear();

    // 
    if(m_piData) std::memset(m_piData, 0, sizeof(coreUint32) * m_iDataSize);

    // 
    m_fStageTime          = 0.0f;
    m_fStageTimeBefore    = 0.0f;
    m_iStageSub           = 0u;
    m_fStageSubTime       = 0.0f;
    m_fStageSubTimeBefore = 0.0f;
    m_fStageWait          = 0.0f;
}


// ****************************************************************
// 
void cHarenaMission::ChangeInsanityP1()
{
    // 
    this->ChangeInsanity(HARENA_INSANITY_P1);

    // 
    this->Setup(0xFFu, 0xFFu);
    m_anStage.clear();
}


// ****************************************************************
// 
void cHarenaMission::CrashEnemy(cEnemy* OUTPUT pEnemy)const
{
    if(!m_iInsanity || (m_iInsanity == HARENA_INSANITY_P1)) return;

    // 
    pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
    if(pEnemy->ReachedDeath()) pEnemy->Kill(true);

    // 
    if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))
    {
        const coreUint8 iPlayer = pEnemy->LastAttackerIndex();
        const coreUint8 iDamage = TIGER_DAMAGE;

        g_pGame->GetCrashManager()->AddCrash(*pEnemy, m_Tiger.GetPosition().xy(), I_TO_P(BITVALUE(8u, 8u, iPlayer) | BITVALUE(8u, 0u, iDamage)));
    }
}


// ****************************************************************
// 
void cHarenaMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Floor.Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    m_SpikeBoard.Render();
    m_Spike     .Render();
}


// ****************************************************************
// 
void cHarenaMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_EggWave.Render();
    }
    glDepthMask(true);

    // 
    m_Egg.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Egg);

    // 
    m_Flummi.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Flummi);
}


// ****************************************************************
// 
void cHarenaMission::__RenderOwnTop()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_Correct.Render();
    }
    glDepthMask(true);

    // 
    m_Aim.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Aim);
}


// ****************************************************************
// 
void cHarenaMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < HARENA_FLOORS; ++i)
    {
        coreObject3D& oFloor = m_aFloorRaw[i];
        if(!oFloor.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apFloorOwner[i];
        if(pOwner)
        {
            const coreFloat fHeight = pOwner->GetPosition().z;
            const coreFloat fScale  = oFloor.GetSize().z;

            // 
            oFloor.SetPosition(coreVector3(pOwner->GetPosition().xy(), 0.0f));
            oFloor.SetSize    (coreVector3(coreVector2(1.0f,1.0f) * (10.0f + 10.0f * STEPH3(0.0f, 50.0f, fHeight)), 1.0f) * fScale);
            oFloor.SetAlpha   (1.0f - STEPH3(5.0f, 50.0f, fHeight));
        }
        else
        {
            // 
            oFloor.SetAlpha(MAX0(oFloor.GetAlpha() - 5.0f*TIME));

            // 
            if(!oFloor.GetAlpha()) this->DisableFloor(i, false);
        }
    }

    // 
    m_Floor.MoveNormal();

    // 
    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
    {
        coreObject3D* pSpike = (*m_Spike     .List())[i];
        coreObject3D* pBoard = (*m_SpikeBoard.List())[i];
        if(!pSpike->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fBlend;
        if(m_afSpikeTime[i] > 0.0f)
        {
            // 
            m_afSpikeTime[i].UpdateMin(1.0f, 2.0f);
            fBlend = LERPB(0.0f, 1.0f, MAX0(m_afSpikeTime[i] - 1.0f));
        }
        else
        {
            // 
            m_afSpikeTime[i].UpdateMax(-1.0f, -2.0f);
            fBlend = LERPB(1.0f, 0.0f, MAX0(-m_afSpikeTime[i] - 1.0f));

            // 
            if(m_afSpikeTime[i] <= -2.0f) this->DisableSpike(i, false);
        }

        coreFloat   fMark   =  0.0f;
        coreFloat   fHeight = -9.0f;
        coreVector3 vColor  = coreVector3(1.0f,1.0f,1.0f) * 0.3f;
        if(m_afSpikeMax[i])
        {
            // 
            coreFlow& fCur = m_afSpikeCur[i];
            fCur.Update(1.0f);

            // 
            if(HAS_BIT(m_iSpikeGood, i))
            {
                // 
                     if(fCur < m_afSpikeMax[i] - HARENA_SPIKE_FADE) {const coreFloat fVal = MIN1((fCur)                                         * 4.0f); fMark = fVal;        vColor = LERPH3(vColor,            COLOR_ENERGY_BLUE, fVal);}
                else if(fCur < m_afSpikeMax[i])                     {const coreFloat fVal = MIN1((fCur - (m_afSpikeMax[i] - HARENA_SPIKE_FADE)) * 4.0f); fMark = 1.0f - fVal; vColor = LERPH3(COLOR_ENERGY_BLUE, vColor,            fVal);}
                else
                {
                    m_afSpikeCur[i] = 0.0f;
                    m_afSpikeMax[i] = 0.0f;
                }

                // 
                fMark  = LERPH3(0.5f, 1.0f, fMark);
                fMark *= 1.0f + 0.07f * SIN(fCur * (2.0f*PI));
            }
            else
            {
                // 
                     if(fCur < HARENA_SPIKE_LAUNCH)                 {const coreFloat fVal = MIN1((fCur)                                         * 5.0f); fHeight = ParaLerp(-9.0f, -2.95f, -3.0f, fVal); vColor = LERPH3(vColor,            COLOR_SHIP_PURPLE, fVal);}
                else if(fCur < m_afSpikeMax[i] - HARENA_SPIKE_FADE) {const coreFloat fVal = MIN1((fCur - (HARENA_SPIKE_LAUNCH))                 * 5.0f); fHeight = ParaLerp(-3.0f,  0.0f,  -0.5f, fVal); vColor = LERPH3(COLOR_SHIP_PURPLE, COLOR_SHIP_RED,    fVal);}
                else if(fCur < m_afSpikeMax[i])                     {const coreFloat fVal = MIN1((fCur - (m_afSpikeMax[i] - HARENA_SPIKE_FADE)) * 4.0f); fHeight = LERPH3  (-0.5f,         -9.0f, fVal); vColor = LERPH3(COLOR_SHIP_RED,    vColor,            fVal);}
                else
                {
                    m_afSpikeCur[i] = 0.0f;
                    m_afSpikeMax[i] = 0.0f;
                }
            }

            STATIC_ASSERT(HARENA_SPIKES <= sizeof(m_iSpikeGood)*8u)
            STATIC_ASSERT(HARENA_SPIKES <= sizeof(m_iSpikeBad) *8u)
        }

        // 
        vColor *= LERP(0.75f, 1.0f, I_TO_F((i * 3u) % 5u) / 4.0f);

        // 
        const coreFloat   fScale = (0.5f + 0.5f * STEP(-9.0f, 0.0f, fHeight)) * 0.85f;
        const coreVector2 vDir   = coreVector2::Direction(LERP(-1.0f*PI, 0.0f*PI, fBlend));

        // 
        pSpike->SetPosition(coreVector3(pBoard->GetPosition().xy(), fHeight));
        pSpike->SetSize    (coreVector3(fScale, fScale, 1.0f) * pSpike->GetSize().z);
        pSpike->SetColor3  (LERP(coreVector3(1.0f,1.0f,1.0f) * 0.5f, vColor, 0.4f));
        pSpike->SetAlpha   (STEPH3(0.8f, 1.0f, fBlend) * (m_afSpikeCur[i] ? 1.0f : 0.0f));

        // 
        pBoard->SetSize       (coreVector3(fBlend, fBlend, 1.0f) * pBoard->GetSize().z);
        pBoard->SetDirection  (coreVector3(vDir, 0.0f));
        pBoard->SetOrientation(OriRoundDir(vDir, vDir));
        pBoard->SetColor3     (vColor);
        pBoard->SetTexSize    (coreVector2(1.0f,1.0f) * fMark);
    }

    // 
    m_Spike     .MoveNormal();
    m_SpikeBoard.MoveNormal();

    // 
    if(m_Correct.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Correct.SetDirection(coreVector3(coreVector2::Direction((4.0f*PI) * m_fAnimation), 0.0f));
        m_Correct.Move();
    }

    // 
    for(coreUintW i = 0u; i < HARENA_EGGS; ++i)
    {
        coreObject3D* pEgg  = (*m_Egg    .List())[i];
        coreObject3D* pWave = (*m_EggWave.List())[i];
        if(!pEgg->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/8.0f);
        const coreVector2 vDir    = coreVector2::Direction((6.0f*PI) * m_fAnimation);

        // 
        pEgg->SetDirection(coreVector3(vDir, 0.0f));
        pEgg->SetTexOffset(coreVector2(0.0f, FRACT(0.2f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pEgg->GetPosition ());
        pWave->SetDirection(pEgg->GetDirection().InvertedX());
        pWave->SetTexOffset(pEgg->GetTexOffset() * -1.0f);
    }

    // 
    m_Egg    .MoveNormal();
    m_EggWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i)
    {
        coreObject3D& oFlummi  = m_aFlummiRaw[i];
        if(!oFlummi.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction((-6.0f*PI) * m_fAnimation);

        // 
        oFlummi.SetDirection(coreVector3(vDir, 0.0f));
        oFlummi.SetTexOffset(coreVector2(0.0f, FRACT(0.2f * m_fAnimation + fOffset)));
    }

    // 
    m_Flummi.MoveNormal();

    // 
    if(m_Aim.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Aim.SetTexOffset(coreVector2(-0.5f * m_fAnimation, 0.0f));
        m_Aim.Move();
    }
}