///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cHarenaMission::cHarenaMission()noexcept
: m_Spike       (HARENA_SPIKES)
, m_SpikeBoard  (HARENA_SPIKES)
, m_afSpikeTime {}
, m_afSpikeCur  {}
, m_afSpikeMax  {}
{
    // 
    m_apBoss[0] = &m_Tiger;

    // 
    m_Spike     .DefineProgram("object_ship_glow_inst_program");
    m_SpikeBoard.DefineProgram("object_tile_inst_program");
    {
        for(coreUintW i = 0u; i < HARENA_SPIKES_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pSpike = &m_aSpikeRaw[i];
            pSpike->DefineModel  (iType ? Core::Manager::Object->GetLowQuad() : Core::Manager::Resource->Get<coreModel>("object_spike.md3"));
            pSpike->DefineTexture(0u, iType ? "menu_background_black.png" : "ship_enemy.png");
            pSpike->DefineProgram(iType ? "object_tile_program" : "object_ship_glow_program");

            // set object properties
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
    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i) this->DisableSpike(i, false);
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

    // 
    m_SpikeBoard.Render();
    m_Spike     .Render();
}


// ****************************************************************
// 
void cHarenaMission::__MoveOwnAfter()
{
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

        coreFloat fHeight = -9.0f;
        if(m_afSpikeMax[i])
        {
            // 
            coreFlow& fCur = m_afSpikeCur[i];
            fCur.Update(1.0f);

            // 
                 if(fCur < 1.0f)                   fHeight = ParaLerp(-9.0f, -2.95f, -3.0f, MIN((fCur)                            * 5.0f, 1.0f));
            else if(fCur < m_afSpikeMax[i] - 0.5f) fHeight = ParaLerp(-3.0f,  0.0f,  -0.5f, MIN((fCur - 1.0f)                     * 5.0f, 1.0f));
            else if(fCur < m_afSpikeMax[i])        fHeight = LERPH3  (-0.5f, -9.0f,         MIN((fCur - (m_afSpikeMax[i] - 0.5f)) * 4.0f, 1.0f));
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
        pSpike->SetColor3  (LERP(coreVector3(1.0f,1.0f,1.0f) * 0.5f, COLOR_SHIP_RED * 0.6f, STEPH3(-2.0f, -0.5f, fHeight)));
        pSpike->SetAlpha   (STEPH3(0.8f, 1.0f, fBlend));

        // 
        pBoard->SetSize       (coreVector3(fBlend, fBlend, 1.0f) * pBoard->GetSize().z);
        pBoard->SetDirection  (coreVector3(vDir, 0.0f));
        pBoard->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
        pBoard->SetColor3     (LERP(coreVector3(1.0f,1.0f,1.0f) * 0.9f, COLOR_SHIP_RED, STEPH3(-6.0f, -3.0f, fHeight) * 0.5f));
    }

    // 
    m_Spike     .MoveNormal();
    m_SpikeBoard.MoveNormal();
}