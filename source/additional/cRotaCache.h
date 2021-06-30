///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ROTACACHE_H_
#define _P1_GUARD_ROTACACHE_H_

// TODO 2: wrap angle


// ****************************************************************
// rotation cache class
class cRotaCache final
{
private:
    coreFloat   m_fAngle;       // cached angle
    coreVector2 m_vDirection;   // cached direction


public:
    constexpr cRotaCache()noexcept;

    ENABLE_COPY(cRotaCache)

    // convert between angle and direction
    inline FUNC_NOALIAS coreFloat   Angle    (const coreVector2 vDirection);
    inline FUNC_NOALIAS coreVector2 Direction(const coreFloat   fAngle);
};


// ****************************************************************
// constructor
constexpr cRotaCache::cRotaCache()noexcept
: m_fAngle     (0.0f)
, m_vDirection (coreVector2(0.0f,1.0f))
{
}


// ****************************************************************
// convert direction to angle
inline FUNC_NOALIAS coreFloat cRotaCache::Angle(const coreVector2 vDirection)
{
    // refresh cached angle if required
    if(!coreMath::IsNear(vDirection.x, m_vDirection.x) ||
       !coreMath::IsNear(vDirection.y, m_vDirection.y))
    {
        m_fAngle     = vDirection.Angle();
        m_vDirection = vDirection;
    }

    // return cached angle
    return m_fAngle;
}


// ****************************************************************
// convert angle to direction
inline FUNC_NOALIAS coreVector2 cRotaCache::Direction(const coreFloat fAngle)
{
    // refresh cached direction if required
    if(!coreMath::IsNear(fAngle, m_fAngle))
    {
        m_fAngle     = fAngle;
        m_vDirection = coreVector2::Direction(fAngle);
    }

    // return cached direction
    return m_vDirection;
}


#endif // _P1_GUARD_ROTACACHE_H_