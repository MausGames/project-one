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
cSnow::cSnow()noexcept
: m_fVisibility (0.0f)
, m_fDelay      (0.0f)
, m_bDirty      (true)
{
    // 
    m_pSnowMap   = Core::Manager::Resource->LoadNew<coreTexture>();
    m_piSnowData = ALIGNED_NEW(coreUint8, SNOW_SIZE * SNOW_SIZE, ALIGNMENT_PAGE);
    STATIC_ASSERT(SNOW_SIZE * SNOW_SIZE == ALIGNMENT_PAGE)

    // 
    this->__Reset(CORE_RESOURCE_RESET_INIT);

    // load object resources
    this->DefineProgram("effect_snow_program");
    this->DefineTexture(0u, m_pSnowMap);
    this->DefineTexture(1u, "menu_background_black.png");

    // set object properties
    this->SetSize (coreVector2(1.0f,1.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// destructor
cSnow::~cSnow()
{
    // 
    ALIGNED_DELETE(m_piSnowData)

    // free resources
    this->DefineTexture(0u, NULL);
    Core::Manager::Resource->Free(&m_pSnowMap);
}


// ****************************************************************
// 
void cSnow::Render()
{
    if(!m_fVisibility) return;

    // 
    if(m_bDirty)
    {
        m_bDirty = false;

        // 
        m_pSnowMap->Invalidate(0u);
        m_pSnowMap->Modify(0u, 0u, SNOW_SIZE, SNOW_SIZE, SNOW_SIZE * SNOW_SIZE, m_piSnowData);
    }

    // 
    this->coreFullscreen::Render();
}


// ****************************************************************
// 
void cSnow::Move()
{
    if(!m_fVisibility) return;

    // 
    if(m_fDelay) m_fVisibility = MAX(m_fVisibility - m_fDelay * TIME, 0.0f);
    this->SetAlpha(m_fVisibility);

    // 
    this->coreFullscreen::Move();
}


// ****************************************************************
// 
void cSnow::Enable()
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
void cSnow::Disable(const coreFloat fDelay)
{
    // 
    m_fDelay = fDelay ? fDelay : FRAMERATE_MAX;
}


// ****************************************************************
// 
coreUintW cSnow::DrawPoint(const coreVector2 vPosition, const coreFloat fSize, const coreUint8 iType)
{
    coreUintW iHit = 0u;

    // 
    const coreUintW iFromX = cSnow::__GetMapIndex(vPosition.x - (fSize + 1.0f));
    const coreUintW iToX   = cSnow::__GetMapIndex(vPosition.x + (fSize + 1.0f));
    const coreUintW iFromY = cSnow::__GetMapIndex(vPosition.y - (fSize + 1.0f));
    const coreUintW iToY   = cSnow::__GetMapIndex(vPosition.y + (fSize + 1.0f));

    // 
    for(coreUintW j = iFromY; j <= iToY; ++j)
    {
        for(coreUintW i = iFromX; i <= iToX; ++i)
        {
            // 
            coreUint8& iByte = m_piSnowData[j * SNOW_SIZE + i];
            if(iType ? !iByte : iByte)
            {
                // 
                const coreFloat fPosX = cSnow::__GetMapValue(i);
                const coreFloat fPosY = cSnow::__GetMapValue(j);

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
coreUintW cSnow::DrawLine(const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const coreUint8 iType)
{
    coreUintW iHit = 0u;

    // 
    const coreUintW iFromX =  bHorizontal ? (0u)             : cSnow::__GetMapIndex(vPosition.x - fSize);
    const coreUintW iToX   =  bHorizontal ? (SNOW_SIZE - 1u) : cSnow::__GetMapIndex(vPosition.x + fSize);
    const coreUintW iFromY = !bHorizontal ? (0u)             : cSnow::__GetMapIndex(vPosition.y - fSize);
    const coreUintW iToY   = !bHorizontal ? (SNOW_SIZE - 1u) : cSnow::__GetMapIndex(vPosition.y + fSize);

    // 
    for(coreUintW j = iFromY; j <= iToY; ++j)
    {
        for(coreUintW i = iFromX; i <= iToX; ++i)
        {
            // 
            coreUint8& iByte = m_piSnowData[j * SNOW_SIZE + i];
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
void cSnow::DrawAll(const coreUint8 iType)
{
    // 
    std::memset(m_piSnowData, iType ? 0xFFu : 0x00u, SNOW_SIZE * SNOW_SIZE);
    m_bDirty = true;
}


// ****************************************************************
// 
coreBool cSnow::TestCollision(const coreVector2 vPosition)const
{
    ASSERT(m_fVisibility)

    // 
    const coreUintW iX = cSnow::__GetMapIndex(vPosition.x);
    const coreUintW iY = cSnow::__GetMapIndex(vPosition.y);

    // 
    return (m_piSnowData[iY * SNOW_SIZE + iX] != 0x00u);
}


// ****************************************************************
// reset with the resource manager
void cSnow::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_pSnowMap->Create(SNOW_SIZE, SNOW_SIZE, CORE_TEXTURE_SPEC_R8, CORE_TEXTURE_MODE_DEFAULT); m_bDirty = true;}
         else {m_pSnowMap->Unload();}
}


// ****************************************************************
// 
coreUintW cSnow::__GetMapIndex(const coreFloat fValue)
{
    STATIC_ASSERT(FOREGROUND_AREA.x == FOREGROUND_AREA.y)
    return F_TO_UI(CLAMP((fValue + (FOREGROUND_AREA.x * 1.1f)) / (FOREGROUND_AREA.x * 2.2f), 0.0f, 1.0f - CORE_MATH_PRECISION) * I_TO_F(SNOW_SIZE));
}

coreFloat cSnow::__GetMapValue(const coreUintW iIndex)
{
    STATIC_ASSERT(FOREGROUND_AREA.x == FOREGROUND_AREA.y)
    return ((I_TO_F(iIndex) + 0.5f) / I_TO_F(SNOW_SIZE)) * (FOREGROUND_AREA.x * 2.2f) - (FOREGROUND_AREA.x * 1.1f);
}