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
: m_fFlash (0.0f)
{
    // load object resources
    //this->DefineProgram("effect_turf_program");
}


// ****************************************************************
// 
void cTurf::Render()
{
    if(!m_fVisibility) return;

    this->DefineTexture(2u, g_pEnvironment->GetBackground()->GetResolvedTexture()->GetColorTarget(0u).pTexture);
    
    // 
    this->cSnow::Render();
    
    this->DefineTexture(2u, NULL);
}


// ****************************************************************
// 
void cTurf::Move()
{
    if(!m_fVisibility) return;
    
    this->SetColor3(coreVector3(LERPH3(0.0f, 1.0f, MIN1(m_fFlash * 1.3f)), 0.0f, 0.0f));
    m_fFlash.UpdateMax(-1.0f, 0.0f);

    // 
    this->cSnow::Move();
}


// ****************************************************************
// 
coreFloat cTurf::CalcPercent()const
{
    ASSERT(m_fVisibility)

    STATIC_ASSERT(coreMath::IsAligned(SNOW_SIZE * SNOW_SIZE, 32u))

    coreUint32 iCount = 0u;

    // 
    coreUint64* piSnowData64 = r_cast<coreUint64*>(m_piSnowData);
    for(coreUintW i = 0u; i < SNOW_SIZE * SNOW_SIZE / 8u; i += 4u)
    {
        const coreUint64 iQuad = ((piSnowData64[i+3u] & 0x8080808080808080u) >> 3u) |
                                 ((piSnowData64[i+2u] & 0x8080808080808080u) >> 2u) |
                                 ((piSnowData64[i+1u] & 0x8080808080808080u) >> 1u) |
                                 ((piSnowData64[i]    & 0x8080808080808080u));

        iCount += coreMath::PopCount(iQuad);
    }

    // 
    return I_TO_F(iCount) / I_TO_F(SNOW_SIZE * SNOW_SIZE);
}