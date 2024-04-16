///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_UTILITIES_H_
#define _P1_GUARD_UTILITIES_H_

// TODO: optimize AlongStar, AlongStarNormal, SmoothAim (and AngleDiff ?)


// ****************************************************************
// 
#if defined(_CORE_MSVC_)
    #define UNITY_BUILD __pragma(warning(disable : 4005))
#else
    #define UNITY_BUILD _Pragma("GCC diagnostic ignored \"-Wmacro-redefined\"")
#endif

// sub-class type information macros
#define ENABLE_ID                                           \
    virtual       coreInt32 GetID  ()const = 0;             \
    virtual const coreChar* GetName()const = 0;
#define ASSIGN_ID(i,n)                                      \
    static constexpr const coreInt32 ID   = i;              \
    static constexpr const coreChar* Name = n;              \
    inline       coreInt32 GetID  ()const final {return i;} \
    inline const coreChar* GetName()const final {return n;}

// 
#define LIST_KEY (CORE_MEMORY_SHARED)


// ****************************************************************
// angle difference helper-function
constexpr FUNC_CONST coreFloat AngleDiff(const coreFloat x, const coreFloat y)
{
    coreFloat A = (x - y);
    while(A <  -PI) A += 2.0f*PI;
    while(A >=  PI) A -= 2.0f*PI;
    return A;
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat LerpSmoothRev(const coreFloat x, const coreFloat y, const coreFloat s)
{
    return (s >= 0.5f) ? LERPB(y, (x + y) * 0.5f, 2.0f - s * 2.0f) :
                         LERPB(x, (x + y) * 0.5f,        s * 2.0f);
}

inline FUNC_CONST coreFloat LerpBreakRev(const coreFloat x, const coreFloat y, const coreFloat s)
{
    return LERPB(y, x, 1.0f - s);
}


// ****************************************************************
// value range helper-functions
template <typename T, typename S, typename R> inline FUNC_LOCAL coreBool InBetween(const T& x, const S& a, const R& b)
{
    ASSERT(a <= b)
    return (x >= a) && (x < b);
}

template <typename T, typename S, typename R> constexpr FUNC_LOCAL coreInt32 InBetweenExt(const T& x, const S& a, const R& b)
{
    return (a <= b) ? (((x >= a) && (x < b)) ?  1 : 0) :
                      (((x >= b) && (x < a)) ? -1 : 0);
}


// ****************************************************************
// ternary interpolation helper-function
template <typename T, typename S, typename R> constexpr FUNC_LOCAL T TernaryLerp(const T& x, const S& y, const R& z, const coreFloat s)
{
    return (s >= 0.5f) ? LERP(y, z, s * 2.0f - 1.0f) :
                         LERP(x, y, s * 2.0f);
}


// ****************************************************************
// 
inline FUNC_PURE coreFloat FrictionFactor(const coreFloat fStrength)
{
    return POW(1.0f - fStrength * (1.0f / FRAMERATE_MIN), Core::System->GetTime() * FRAMERATE_MIN);
}


// ****************************************************************
// 
inline FUNC_PURE coreVector2 SmoothAim(const coreVector2 vOldDir, const coreVector2 vNewDir, const coreFloat fStrength)
{
    ASSERT(vOldDir.IsNormalized() && vNewDir.IsNormalized())

    const coreFloat fFrom = vOldDir.Angle();
    const coreFloat fTo   = vNewDir.Angle();
    const coreFloat fDiff = AngleDiff(fTo, fFrom);

    return coreVector2::Direction(fFrom + fDiff * (1.0f - FrictionFactor(fStrength)));
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat SmoothTowards(const coreFloat fDistance, const coreFloat fThreshold)
{
    ASSERT((fDistance >= 0.0f) && (fThreshold > 0.0f))
    return MIN(fDistance, fThreshold) * RCP(fThreshold);
}


// ****************************************************************
// direction restriction and packing helper-functions
inline FUNC_LOCAL coreUint8 PackDirection(const coreVector2& vDirection)
{
    return vDirection.IsNull() ? 8u : (F_TO_UI(ROUND(vDirection.Angle() / (0.25f*PI))) & 0x07u);
}

inline FUNC_CONST coreVector2 UnpackDirection(const coreUint8 iPack)
{
    // 1 0 7
    // 2 8 6
    // 3 4 5
    switch(iPack)
    {
    default: ASSERT(false)
    case 0u: return coreVector2( 0.0f, 1.0f);
    case 1u: return coreVector2(-1.0f, 1.0f) * (1.0f/SQRT2);
    case 2u: return coreVector2(-1.0f, 0.0f);
    case 3u: return coreVector2(-1.0f,-1.0f) * (1.0f/SQRT2);
    case 4u: return coreVector2( 0.0f,-1.0f);
    case 5u: return coreVector2( 1.0f,-1.0f) * (1.0f/SQRT2);
    case 6u: return coreVector2( 1.0f, 0.0f);
    case 7u: return coreVector2( 1.0f, 1.0f) * (1.0f/SQRT2);
    case 8u: return coreVector2( 0.0f, 0.0f);
    }
}


// ****************************************************************
// 
inline FUNC_CONST coreVector2 StepRotated45(const coreUint8 iStep)
{
    ASSERT(iStep < 8u)
    return UnpackDirection(iStep);
}

inline FUNC_CONST coreVector2 StepRotated90(const coreUint8 iStep)
{
    ASSERT(iStep < 4u)
    return UnpackDirection(iStep * 2u);
}

inline FUNC_CONST coreVector2 StepRotated90X(const coreUint8 iStep)
{
    ASSERT(iStep < 4u)
    return UnpackDirection(iStep * 2u + 1u);
}


// ****************************************************************
// 
inline FUNC_CONST coreVector2 MapStepRotated45(const coreVector2& vDirection, const coreUint8 iStep)
{
    switch(iStep)
    {
    default: ASSERT(false)
    case 0u: return  vDirection;
    case 1u: return  vDirection.Rotated45();
    case 2u: return  vDirection.Rotated90();
    case 3u: return  vDirection.Rotated135();
    case 4u: return -vDirection;
    case 5u: return -vDirection.Rotated45();
    case 6u: return -vDirection.Rotated90();
    case 7u: return -vDirection.Rotated135();
    }
}

inline FUNC_CONST coreVector2 MapStepRotated90(const coreVector2& vDirection, const coreUint8 iStep)
{
    switch(iStep)
    {
    default: ASSERT(false)
    case 0u: return  vDirection;
    case 1u: return  vDirection.Rotated90();
    case 2u: return -vDirection;
    case 3u: return -vDirection.Rotated90();
    }
}


// ****************************************************************
// 
inline FUNC_LOCAL coreBool IsHorizontal(const coreVector2& v)
{
    ASSERT(!v.IsNull())
    return ABS(v.x) > ABS(v.y);
}

inline FUNC_LOCAL coreVector2 AlongCross(const coreVector2& v)
{
    ASSERT(!v.IsNull())
    return IsHorizontal(v) ? coreVector2(v.x, 0.0f) : coreVector2(0.0f, v.y);
}

inline FUNC_LOCAL coreVector2 AlongStar(const coreVector2& v)
{
    ASSERT(!v.IsNull())
    return UnpackDirection(PackDirection(v)) * v.Length();
}

inline FUNC_LOCAL coreVector2 AlongCrossNormal(const coreVector2& v)
{
    ASSERT(v.IsNormalized())
    return IsHorizontal(v) ? coreVector2(SIGN(v.x), 0.0f) : coreVector2(0.0f, SIGN(v.y));
}

inline FUNC_LOCAL coreVector2 AlongStarNormal(const coreVector2& v)
{
    ASSERT(v.IsNormalized())
    return UnpackDirection(PackDirection(v));
}


// ****************************************************************
// 
inline FUNC_LOCAL coreVector2 MapToAxis(const coreVector2& vVector, const coreVector2& vAxis)
{
    return (vVector.x * vAxis.Rotated90()) +
           (vVector.y * vAxis);
}


#endif // _P1_GUARD_UTILITIES_H_