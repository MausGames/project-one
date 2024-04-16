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
    m_Spike     .DefineProgram("object_ship_glow_inst_program");
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
            pSpike->DefineProgram(iType ? "object_board_program" : "object_ship_glow_program");

            // set object properties
            pSpike->SetTexOffset        (coreVector2(0.17f,0.31f) * I_TO_F(i / 2u));
            pSpike->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.07f);
            pSpike->SetAlpha            (0.7f);
            pSpike->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_SpikeBoard.BindObject(pSpike);
                 else m_Spike     .BindObject(pSpike);
        }
    }
}


// ****************************************************************
// destructor
cHarenaMission::~cHarenaMission()
{
    // 
    for(coreUintW i = 0u; i < HARENA_FLOORS; ++i) this->DisableFloor(i, false);
    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i) this->DisableSpike(i, false);
}


// ****************************************************************
// 
void cHarenaMission::EnableFloor(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < HARENA_FLOORS)
    coreObject3D& oFloor = m_aFloorRaw[iIndex];

    // 
    WARN_IF(oFloor.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apFloorOwner[iIndex] = pOwner;

    // 
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
void cHarenaMission::EnableSpike(const coreUintW iIndex)
{
    ASSERT(iIndex < HARENA_SPIKES)
    coreObject3D* pSpike = (*m_Spike     .List())[iIndex];
    coreObject3D* pBoard = (*m_SpikeBoard.List())[iIndex];

    // 
    WARN_IF(pSpike->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afSpikeTime[iIndex] = 1.0f - 0.022f * I_TO_F(iIndex);
    m_afSpikeCur [iIndex] = 0.0f;
    m_afSpikeMax [iIndex] = 0.0f;

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
void cHarenaMission::__MoveOwnAfter()
{
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

            // 
            oFloor.SetPosition(coreVector3(pOwner->GetPosition().xy(), 0.0f));
            oFloor.SetSize    (coreVector3(1.0f,1.0f,1.0f) * (10.0f + 10.0f * STEPH3(0.0f, 50.0f, fHeight)));
            oFloor.SetAlpha   (1.0f - STEPH3(45.0f, 50.0f, fHeight));
        }
        else
        {
            // 
            oFloor.SetAlpha(MAX(oFloor.GetAlpha() - 5.0f*TIME, 0.0f));

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
            fBlend = LERPB(0.0f, 1.0f, MAX(m_afSpikeTime[i] - 1.0f, 0.0f));
        }
        else
        {
            // 
            m_afSpikeTime[i].UpdateMax(-1.0f, -2.0f);
            fBlend = LERPB(1.0f, 0.0f, MAX(-m_afSpikeTime[i] - 1.0f, 0.0f));

            // 
            if(m_afSpikeTime[i] <= -2.0f) this->DisableSpike(i, false);
        }

        coreFloat   fHeight = -9.0f;
        coreVector3 vColor  = coreVector3(1.0f,1.0f,1.0f) * 0.3f;
        if(m_afSpikeMax[i])
        {
            // 
            coreFlow& fCur = m_afSpikeCur[i];
            fCur.Update(1.0f);

            // 
                 if(fCur < 1.0f)                   {const coreFloat fVal = MIN((fCur)                            * 5.0f, 1.0f); fHeight = ParaLerp(-9.0f, -2.95f, -3.0f, fVal); vColor = LERPH3(vColor,            COLOR_SHIP_PURPLE, fVal);}
            else if(fCur < m_afSpikeMax[i] - 0.5f) {const coreFloat fVal = MIN((fCur - 1.0f)                     * 5.0f, 1.0f); fHeight = ParaLerp(-3.0f,  0.0f,  -0.5f, fVal); vColor = LERPH3(COLOR_SHIP_PURPLE, COLOR_SHIP_RED,    fVal);}
            else if(fCur < m_afSpikeMax[i])        {const coreFloat fVal = MIN((fCur - (m_afSpikeMax[i] - 0.5f)) * 4.0f, 1.0f); fHeight = LERPH3  (-0.5f,         -9.0f, fVal); vColor = LERPH3(COLOR_SHIP_RED,    vColor,            fVal);}
            else
            {
                m_afSpikeCur[i] = 0.0f;
                m_afSpikeMax[i] = 0.0f;
            }
        }

        // 
        const coreFloat   fScale = (0.5f + 0.5f * STEP(-9.0f, 0.0f, fHeight)) * 0.85f;
        const coreVector2 vDir   = coreVector2::Direction(LERP(-1.0f*PI, 0.0f*PI, fBlend));

        // 
        pSpike->SetPosition(coreVector3(pSpike->GetPosition().xy(), fHeight));
        pSpike->SetSize    (coreVector3(fScale, fScale, 1.0f) * pSpike->GetSize().z);
        pSpike->SetColor3  (LERP(coreVector3(1.0f,1.0f,1.0f) * 0.5f, vColor, 0.4f));
        pSpike->SetAlpha   (STEPH3(0.8f, 1.0f, fBlend));

        // 
        pBoard->SetSize       (coreVector3(fBlend, fBlend, 1.0f) * pBoard->GetSize().z);
        pBoard->SetDirection  (coreVector3(vDir, 0.0f));
        pBoard->SetOrientation(OriRoundDir(vDir, vDir));
        pBoard->SetColor3     (vColor);
    }

    // 
    m_Spike     .MoveNormal();
    m_SpikeBoard.MoveNormal();
}