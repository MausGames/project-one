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
cTurf::cTurf()noexcept
: m_fVisibility (0.0f)
, m_fDelay      (0.0f)
, m_fFlash      (0.0f)
, m_bDirty      (true)
{
    // 
    m_pTurfMap   = Core::Manager::Resource->LoadNew<coreTexture>();
    m_piTurfData = ALIGNED_NEW(coreUint8, TURF_SIZE * TURF_SIZE, ALIGNMENT_PAGE);
    STATIC_ASSERT(TURF_SIZE * TURF_SIZE == ALIGNMENT_PAGE)

    // 
    this->__Reset(CORE_RESOURCE_RESET_INIT);

    // load object resources
    this->DefineProgram("effect_turf_program");
    this->DefineTexture(0u, m_pTurfMap);
    this->DefineTexture(1u, "menu_background_black.png");

    // set object properties
    this->SetSize (coreVector2(1.0f,1.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// destructor
cTurf::~cTurf()
{
    // 
    ALIGNED_DELETE(m_piTurfData)

    // free resources
    this->DefineTexture(0u, NULL);
    Core::Manager::Resource->Free(&m_pTurfMap);
}


// ****************************************************************
// 
void cTurf::Render()
{
    if(!m_fVisibility) return;

    // 
    if(m_bDirty)
    {
        m_bDirty = false;

        // 
        m_pTurfMap->Invalidate(0u);
        m_pTurfMap->Modify(0u, 0u, TURF_SIZE, TURF_SIZE, TURF_SIZE * TURF_SIZE, m_piTurfData);
    }

    this->DefineTexture(2u, g_pEnvironment->GetBackground()->GetResolvedTexture()->GetColorTarget(0u).pTexture);
    // 
    this->coreFullscreen::Render();
    
    this->DefineTexture(2u, NULL);
}


// ****************************************************************
// 
void cTurf::Move()
{
    if(!m_fVisibility) return;

    // 
    if(m_fDelay) m_fVisibility = MAX(m_fVisibility - m_fDelay * TIME, 0.0f);
    this->SetAlpha(m_fVisibility);
    
    
    this->SetColor3(coreVector3(LERPH3(0.0f, 1.0f, MIN(m_fFlash * 1.3f, 1.0f)), 0.0f, 0.0f));
    m_fFlash.UpdateMax(-1.0f, 0.0f);

    // 
    this->coreFullscreen::Move();
}


// ****************************************************************
// 
void cTurf::Enable()
{
    // 
    ASSERT(!m_fVisibility)
    m_fVisibility = 1.0f;

    // 
    m_fDelay = 0.0f;

    // 
    this->DrawAll(0u);
}


// ****************************************************************
// 
void cTurf::Disable(const coreFloat fDelay)
{
    // 
    m_fDelay = fDelay ? fDelay : FRAMERATE_MAX;
}


// ****************************************************************
// 
coreUintW cTurf::DrawPoint(const coreVector2 vPosition, const coreFloat fSize, const coreUint8 iType)
{
    coreUintW iHit = 0u;

    // 
    const coreUintW iFromX = cTurf::__GetMapIndex(vPosition.x - (fSize + 1.0f));
    const coreUintW iToX   = cTurf::__GetMapIndex(vPosition.x + (fSize + 1.0f));
    const coreUintW iFromY = cTurf::__GetMapIndex(vPosition.y - (fSize + 1.0f));
    const coreUintW iToY   = cTurf::__GetMapIndex(vPosition.y + (fSize + 1.0f));

    // 
    for(coreUintW j = iFromY; j <= iToY; ++j)
    {
        for(coreUintW i = iFromX; i <= iToX; ++i)
        {
            // 
            coreUint8& iByte = m_piTurfData[j * TURF_SIZE + i];
            if(iType ? !iByte : iByte)
            {
                // 
                const coreFloat fPosX = cTurf::__GetMapValue(i);
                const coreFloat fPosY = cTurf::__GetMapValue(j);

                // 
                if((coreVector2(fPosX, fPosY) - vPosition).LengthSq() < POW2(fSize))
                {
                    iByte = iType ? 0xFFu : 0x00u;
                    iHit += 1u;
                }
            }
        }
    }

    // 
    if(iHit) m_bDirty = true;
    return iHit;
}


// ****************************************************************
// 
coreUintW cTurf::DrawLine(const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const coreUint8 iType)
{
    coreUintW iHit = 0u;

    // 
    const coreUintW iFromX =  bHorizontal ? (0u)             : cTurf::__GetMapIndex(vPosition.x - fSize);
    const coreUintW iToX   =  bHorizontal ? (TURF_SIZE - 1u) : cTurf::__GetMapIndex(vPosition.x + fSize);
    const coreUintW iFromY = !bHorizontal ? (0u)             : cTurf::__GetMapIndex(vPosition.y - fSize);
    const coreUintW iToY   = !bHorizontal ? (TURF_SIZE - 1u) : cTurf::__GetMapIndex(vPosition.y + fSize);

    // 
    for(coreUintW j = iFromY; j <= iToY; ++j)
    {
        for(coreUintW i = iFromX; i <= iToX; ++i)
        {
            // 
            coreUint8& iByte = m_piTurfData[j * TURF_SIZE + i];
            if(iType ? !iByte : iByte)
            {
                iByte = iType ? 0xFFu : 0x00u;
                iHit += 1u;
            }
        }
    }

    // 
    if(iHit) m_bDirty = true;
    return iHit;
}


// ****************************************************************
// 
void cTurf::DrawAll(const coreUint8 iType)
{
    // 
    std::memset(m_piTurfData, iType ? 0xFFu : 0x00u, TURF_SIZE * TURF_SIZE);
    m_bDirty = true;
}


// ****************************************************************
// 
coreFloat cTurf::CalcPercent()
{
    coreUint32 iCount = 0u;

    // 
    for(coreUintW i = 0u, ie = TURF_SIZE * TURF_SIZE; i < ie; i += 4u)
    {
        iCount += m_piTurfData[i + 0u] ? 1u : 0u;
        iCount += m_piTurfData[i + 1u] ? 1u : 0u;
        iCount += m_piTurfData[i + 2u] ? 1u : 0u;
        iCount += m_piTurfData[i + 3u] ? 1u : 0u;
    }

    // 
    return I_TO_F(iCount) / I_TO_F(TURF_SIZE * TURF_SIZE);
}


// ****************************************************************
// 
void cTurf::Flash()
{
    m_fFlash = 1.0f;
}


// ****************************************************************
// 
coreBool cTurf::TestCollision(const coreVector2 vPosition)const
{
    ASSERT(m_fVisibility)

    // 
    const coreUintW iX = cTurf::__GetMapIndex(vPosition.x);
    const coreUintW iY = cTurf::__GetMapIndex(vPosition.y);

    // 
    return (m_piTurfData[iY * TURF_SIZE + iX] != 0x00u);
}


// ****************************************************************
// reset with the resource manager
void cTurf::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_pTurfMap->Create(TURF_SIZE, TURF_SIZE, CORE_TEXTURE_SPEC_R8, CORE_TEXTURE_MODE_DEFAULT); m_bDirty = true;}
         else {m_pTurfMap->Unload();}
}


// ****************************************************************
// 
coreUintW cTurf::__GetMapIndex(const coreFloat fValue)
{
    STATIC_ASSERT(FOREGROUND_AREA.x == FOREGROUND_AREA.y)
    return F_TO_UI(CLAMP((fValue + (FOREGROUND_AREA.x * 1.1f)) / (FOREGROUND_AREA.x * 2.2f), 0.0f, 1.0f - CORE_MATH_PRECISION) * I_TO_F(TURF_SIZE));
}

coreFloat cTurf::__GetMapValue(const coreUintW iIndex)
{
    STATIC_ASSERT(FOREGROUND_AREA.x == FOREGROUND_AREA.y)
    return ((I_TO_F(iIndex) + 0.5f) / I_TO_F(TURF_SIZE)) * (FOREGROUND_AREA.x * 2.2f) - (FOREGROUND_AREA.x * 1.1f);
}