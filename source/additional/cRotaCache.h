//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ROTACACHE_H_
#define _P1_GUARD_ROTACACHE_H_


// ****************************************************************
// 
class cRotaCache final
{
private:
    coreFloat   m_fAngle;       // 
    coreVector2 m_vDirection;   // 


public:
    constexpr cRotaCache()noexcept;

    ENABLE_COPY(cRotaCache)

    // 
    inline coreFloat   Angle    (const coreVector2& vDirection);
    inline coreVector2 Direction(const coreFloat    fAngle);
};


// ****************************************************************
// 
constexpr cRotaCache::cRotaCache()noexcept
: m_fAngle     (0.0f)
, m_vDirection (coreVector2(0.0f,1.0f))
{
}


// ****************************************************************
// 
inline coreFloat cRotaCache::Angle(const coreVector2& vDirection)
{
    // 
    if(!coreMath::InRange(vDirection.x, m_vDirection.x, CORE_MATH_PRECISION) ||
       !coreMath::InRange(vDirection.y, m_vDirection.y, CORE_MATH_PRECISION))
    {
        m_fAngle     = vDirection.Angle();
        m_vDirection = vDirection;
    }

    // 
    return m_fAngle;
}


// ****************************************************************
// 
inline coreVector2 cRotaCache::Direction(const coreFloat fAngle)
{
    // 
    if(!coreMath::InRange(fAngle, m_fAngle, CORE_MATH_PRECISION))
    {
        m_fAngle     = fAngle;
        m_vDirection = coreVector2::Direction(fAngle);
    }

    // 
    return m_vDirection;
}


#endif // _P1_GUARD_ROTACACHE_H_