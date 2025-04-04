///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ROTACACHE_H_
#define _P1_GUARD_ROTACACHE_H_


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
    inline const coreFloat&   Angle    (const coreVector2 vDirection);
    inline const coreVector2& Direction(const coreFloat   fAngle);
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
inline const coreFloat& cRotaCache::Angle(const coreVector2 vDirection)
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
inline const coreVector2& cRotaCache::Direction(const coreFloat fAngle)
{
    // wrap around angle
    const coreFloat fWrap = FmodRange(fAngle, -PI, PI);

    // refresh cached direction if required
    if(!coreMath::IsNear(fWrap, m_fAngle))
    {
        m_fAngle     = fWrap;
        m_vDirection = coreVector2::Direction(fWrap);
    }

    // return cached direction
    return m_vDirection;
}


#endif // _P1_GUARD_ROTACACHE_H_