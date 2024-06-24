///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_UTILITIES_H_
#define _P1_GUARD_UTILITIES_H_

// TODO 4: move ID color directly into background class if not used otherwise (emphasize "highlight" usage)
// TODO 1: move a lot of those utilities into engine if proven well, look for other stuff in whole game (also from other projects)


// ****************************************************************
// 
#if defined(_CORE_MSVC_)
    #define UNITY_BUILD __pragma(warning(disable : 4005))   // macro identifier redefinition
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
#define ENABLE_ID_EX                                        \
    ENABLE_ID                                               \
    virtual coreVector3 GetColor ()const = 0;               \
    virtual coreVector3 GetColor2()const = 0;               \
    virtual coreVector2 GetIcon  ()const = 0;
#define ASSIGN_ID_EX(i,n,c,d,o)                             \
    ASSIGN_ID(i, n)                                         \
    static constexpr const coreVector3 Color  = c;          \
    static constexpr const coreVector3 Color2 = d;          \
    static constexpr const coreVector2 Icon   = o;          \
    inline coreVector3 GetColor ()const final {return c;}   \
    inline coreVector3 GetColor2()const final {return d;}   \
    inline coreVector2 GetIcon  ()const final {return o;}
#define ENABLE_EXTRA                                        \
    virtual const coreChar* GetExtra()const = 0;
#define ASSIGN_EXTRA(e)                                     \
    static constexpr const coreChar* Extra = e;             \
    inline const coreChar* GetExtra()const final {return e;}

// 
#define LIST_KEY (CORE_MEMORY_SHARED)

// 
#define HIDDEN_POS (coreVector2(1000.0f,1000.0f))

// 
#define ADD_BIT_EX(o,n)    {ADD_BIT   ((o)[(n) / (sizeof((o)[0]) * 8u)], (n) % (sizeof((o)[0]) * 8u)) ASSERT((n) < ARRAY_SIZE(o) * (sizeof((o)[0]) * 8u))}
#define REMOVE_BIT_EX(o,n) {REMOVE_BIT((o)[(n) / (sizeof((o)[0]) * 8u)], (n) % (sizeof((o)[0]) * 8u)) ASSERT((n) < ARRAY_SIZE(o) * (sizeof((o)[0]) * 8u))}
#define HAS_BIT_EX(o,n)    (HAS_BIT   ((o)[(n) / (sizeof((o)[0]) * 8u)], (n) % (sizeof((o)[0]) * 8u)))


// ****************************************************************
// 
constexpr FUNC_CONST coreUint8 GetSystemOsIndex()
{
#if defined(_CORE_WINDOWS_)
    const coreUint8 A = 1u;
#elif defined(_CORE_LINUX_)
    const coreUint8 A = 2u;
#elif defined(_CORE_MACOS_)
    const coreUint8 A = 3u;
#elif defined(_CORE_ANDROID_)
    const coreUint8 A = 4u;
#elif defined(_CORE_IOS_)
    const coreUint8 A = 5u;
#elif defined(_CORE_EMSCRIPTEN_)
    const coreUint8 A = 6u;
#elif defined(_CORE_SWITCH_)   // #
    const coreUint8 A = 7u;
#endif

#if defined(_CORE_64BIT_)
    const coreUint8 B = 0u;
#else
    const coreUint8 B = 1u;
#endif

    return (A) + (B << 4u);
}


// ****************************************************************
// 
inline coreUint8 GetSystemCpuIndex()
{
    static const coreUint8 s_iCpuIndex = []()
    {
        const coreChar* pcVendor = coreData::StrToLower(coreData::SystemCpuVendor());
        const coreChar* pcBrand  = coreData::StrToLower(coreData::SystemCpuBrand());

        if(std::strstr(pcBrand,  "amd"))   return 1u;
        if(std::strstr(pcBrand,  "intel")) return 2u;
        if(std::strstr(pcVendor, "apple")) return 3u;

        return 0xFFu;
    }();

    return s_iCpuIndex;
}


// ****************************************************************
// 
inline coreUint8 GetSystemGpuIndex()
{
    static const coreUint8 s_iGpuIndex = []()
    {
        const coreChar* pcVendor = coreData::StrToLower(PRINT("%s", glGetString(GL_VENDOR)));   // to handle NULL

        if(std::strstr(pcVendor, "amd"))    return 1u;
        if(std::strstr(pcVendor, "nvidia")) return 2u;
        if(std::strstr(pcVendor, "intel"))  return 3u;
        if(std::strstr(pcVendor, "apple"))  return 4u;

        return 0xFFu;
    }();

    return s_iGpuIndex;
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat FmodRange(const coreFloat x, const coreFloat a, const coreFloat b)
{
    return FMODR(x, a, b);
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat CeilFactor(const coreFloat fValue, const coreFloat fFactor)
{
    return CEIL(fValue * fFactor) * RCP(fFactor);   // different to engine function
}

inline FUNC_CONST coreFloat FloorFactor(const coreFloat fValue, const coreFloat fFactor)
{
    return FLOOR(fValue * fFactor) * RCP(fFactor);   // different to engine function
}

inline FUNC_CONST coreFloat RoundFactor(const coreFloat fValue, const coreFloat fFactor)
{
    return ROUND(fValue * fFactor) * RCP(fFactor);   // different to engine function
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat TriangleWave(const coreFloat x)
{
    const coreFloat A = FRACT(x);
    return MIN(A, 1.0f - A) * 2.0f;
}


// ****************************************************************
// angle difference helper-function
inline FUNC_CONST coreFloat AngleDiff(const coreFloat x, const coreFloat y)
{
    return FmodRange(x - y, -PI, PI);
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat AnglePos(const coreFloat x)
{
    return FmodRange(x, 0.0f*PI, 2.0f*PI);
}


// ****************************************************************
// 
inline FUNC_CONST coreFloat AngleLerp(const coreFloat x, const coreFloat y, const coreFloat s)
{
    return x + AngleDiff(y, x) * s;
}

inline FUNC_CONST coreVector2 AngleLerpDir(const coreVector2 x, const coreVector2 y, const coreFloat s)
{
    return coreVector2::Direction(AngleLerp(x.Angle(), y.Angle(), s));
}


// ****************************************************************
// value range helper-functions
template <typename T, typename S, typename R> constexpr FUNC_LOCAL coreBool InBetween(const T& x, const S& a, const R& b)
{
    ASSERT(a <= b)
    return (x >= a) && (x < b);   // [a,b)
}

template <typename T, typename S, typename R> constexpr FUNC_LOCAL coreInt32 InBetweenExt(const T& x, const S& a, const R& b)
{
    return (a <= b) ? (((x >= a) && (x < b)) ?  1 : 0) :   // [a,b)
                      (((x >= b) && (x < a)) ? -1 : 0);    // [b,a)
}


// ****************************************************************
// 
template <typename T> inline FUNC_LOCAL T LerpSmoothRev(const T& x, const T& y, const coreFloat s)
{
    return (s >= 0.5f) ? LERPB(y, (x + y) * 0.5f, 2.0f - s * 2.0f) :
                         LERPB(x, (x + y) * 0.5f,        s * 2.0f);
}


// ****************************************************************
// ternary interpolation helper-function
template <typename T, typename S, typename R> constexpr FUNC_LOCAL T LerpTernary(const T& x, const S& y, const R& z, const coreFloat s)
{
    return (s >= 0.5f) ? LERP(y, z, s * 2.0f - 1.0f) :
                         LERP(x, y, s * 2.0f);
}


// ****************************************************************
// 
template <typename T, typename S, typename R> inline FUNC_LOCAL T LerpPara(const T& x, const S& y, const R& z, const coreFloat s)
{
    const T A = (x - z);
    return LERPB(x, y + A * 0.5f, s * 2.0f) - (A * s);
}


// ****************************************************************
// 
inline FUNC_PURE coreFloat FrictionFactor(const coreFloat fStrength)
{
    return POW(1.0f - fStrength * (1.0f / FRAMERATE_MIN), TIME * FRAMERATE_MIN);   // TODO 1: still an issue with step size, you need curve increment, but at the location where you use the value [RP]
}


// ****************************************************************
// 
inline FUNC_PURE coreFloat SmoothAimAngle(const coreFloat vOldAngle, const coreFloat vNewAngle, const coreFloat fStrength)
{
    return AngleLerp(vOldAngle, vNewAngle, 1.0f - FrictionFactor(fStrength));
}

inline FUNC_PURE coreVector2 SmoothAim(const coreVector2 vOldDir, const coreVector2 vNewDir, const coreFloat fStrength)
{
    ASSERT(vOldDir.IsNormalized() && vNewDir.IsNormalized())
    return coreVector2::Direction(SmoothAimAngle(vOldDir.Angle(), vNewDir.Angle(), fStrength));
} // TODO 1: towards the end of the curve it is so slow that it never hits its target


// ****************************************************************
// 
constexpr FUNC_CONST coreFloat SmoothTowards(const coreFloat fDistance, const coreFloat fThreshold)
{
    ASSERT((fDistance >= 0.0f) && (fThreshold > 0.0f))
    return (fDistance >= fThreshold) ? 1.0f : (fDistance * RCP(fThreshold));   // TODO 1: not frame rate independent (within the threshold), the higher the FPS, the slower (smoother) the movement is within the threshold [RP]
}


// ****************************************************************
// direction quantization and packing helper-functions
inline FUNC_CONST coreUint8 PackDirection(const coreVector2 vDirection)
{
    return vDirection.PackWay8();
}

constexpr FUNC_CONST coreVector2 UnpackDirection(const coreUint8 iPack)
{
    return coreVector2::UnpackWay8(iPack);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreUint8 StepInvertedX(const coreUint8 iPack)
{
    // 7 0 1
    // 6 8 2
    // 5 4 3
    switch(iPack)
    {
    default: UNREACHABLE
    case 0u: return 0u;
    case 1u: return 7u;
    case 2u: return 6u;
    case 3u: return 5u;
    case 4u: return 4u;
    case 5u: return 3u;
    case 6u: return 2u;
    case 7u: return 1u;
    case 8u: return 8u;
    }
}


// ****************************************************************
// 
constexpr FUNC_CONST coreVector2 StepRotated45(const coreUint8 iStep)
{
    return coreVector2::StepRotated45(iStep);
}

constexpr FUNC_CONST coreVector2 StepRotated45X(const coreUint8 iStep)
{
    return coreVector2::StepRotated45X(iStep);
}

constexpr FUNC_CONST coreVector2 StepRotated90(const coreUint8 iStep)
{
    return coreVector2::StepRotated90(iStep);
}

constexpr FUNC_CONST coreVector2 StepRotated90X(const coreUint8 iStep)
{
    return coreVector2::StepRotated90X(iStep);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreVector2 MapStepRotated45(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotated45(iStep);
}

constexpr FUNC_CONST coreVector2 MapStepRotated90(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotated90(iStep);
}

constexpr FUNC_CONST coreVector2 MapStepRotated90X(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotated90X(iStep);
}

constexpr FUNC_CONST coreVector2 MapStepRotatedInv45(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotatedInv45(iStep);
}

constexpr FUNC_CONST coreVector2 MapStepRotatedInv90(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotatedInv90(iStep);
}

constexpr FUNC_CONST coreVector2 MapStepRotatedInv90X(const coreVector2 vDirection, const coreUint8 iStep)
{
    return vDirection.MapStepRotatedInv90X(iStep);
}


// ****************************************************************
// 
inline FUNC_CONST coreBool IsHorizontal(const coreVector2 v)
{
    return v.IsHorizontal();
}

inline FUNC_CONST coreVector2 AlongCross(const coreVector2 v)
{
    return v.AlongWay4();
}

inline FUNC_CONST coreVector2 AlongCrossNormal(const coreVector2 v)
{
    return v.AlongWay4Normal();
}

inline FUNC_CONST coreVector2 AlongCrossX(const coreVector2 v)
{
    return v.AlongWay4X();
}

inline FUNC_CONST coreVector2 AlongCrossXNormal(const coreVector2 v)
{
    return v.AlongWay4XNormal();
}

inline FUNC_CONST coreVector2 AlongStar(const coreVector2 v)
{
    return v.AlongWay8();
}

inline FUNC_CONST coreVector2 AlongStarNormal(const coreVector2 v)
{
    return v.AlongWay8Normal();
}


// ****************************************************************
// 
constexpr FUNC_CONST coreVector2 MapToAxis(const coreVector2 vVector, const coreVector2 vAxis)
{
    return vVector.MapToAxis(vAxis);
}

constexpr FUNC_CONST coreVector2 MapToAxisInv(const coreVector2 vVector, const coreVector2 vAxis)
{
    return vVector.MapToAxisInv(vAxis);
}

constexpr FUNC_CONST coreVector3 MapToAxis(const coreVector3 vVector, const coreVector2 vAxis)
{
    return coreVector3(vVector.xy().MapToAxis(vAxis), vVector.z);
}

constexpr FUNC_CONST coreVector3 MapToAxisInv(const coreVector3 vVector, const coreVector2 vAxis)
{
    return coreVector3(vVector.xy().MapToAxisInv(vAxis), vVector.z);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreVector3 OriRoundDir(const coreVector2 vOrientation, const coreVector2 vDirection)
{
    ASSERT(vOrientation.IsNormalized() && vDirection.IsNormalized())
    return coreVector3(-vOrientation.x * vDirection.y,
                        vOrientation.x * vDirection.x,
                        vOrientation.y);
}


// ****************************************************************
// 
inline FUNC_NOALIAS coreBool RayIntersection(const coreVector2 vPos1, const coreVector2 vDir1, const coreVector2 vPos2, const coreVector2 vDir2, coreVector2* OUTPUT pvIntersection)
{
    return coreVector2::Intersect(vPos1, vDir1, vPos2, vDir2, pvIntersection);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreBool SameDirection(const coreVector2 v1, const coreVector2 v2)
{
    return coreVector2::SameDir(v1, v2);
}

constexpr FUNC_CONST coreBool SameDirection90(const coreVector2 v1, const coreVector2 v2)
{
    return coreVector2::SameDir90(v1, v2);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreFloat DelayTime(const coreFloat fTime, const coreFloat fOffset, const coreFloat fLength)
{
    return MIN(fTime, fOffset) + MAX0(fTime - fOffset - fLength);
}


// ****************************************************************
// 
constexpr FUNC_CONST coreFloat MaxAspectRatio(const coreVector2 vVector)
{
    return (vVector.Max() * RCP(vVector.Min()));
}


// ****************************************************************
// 
inline FUNC_LOCAL coreVector2 GetTranslation(const coreObject2D& oObject)
{
    return oObject.GetScreenPosition();
}

inline FUNC_PURE coreVector2 GetTranslationArea(const coreObject2D& oObject)
{
    return GetTranslation(oObject) * RCP(Core::System->GetResolution().Min());
}

inline FUNC_PURE coreVector2 GetTranslationScreen(const coreObject2D& oObject)
{
    return GetTranslation(oObject) / Core::System->GetResolution();
}


// ****************************************************************
// execute function without frame time
template <typename F> FORCE_INLINE void Timeless(F&& nFunction)   // []() -> void
{
    // store current frame time
    const coreFloat fSave = TIME;

    // modify global state and execute function
    c_cast<coreFloat&>(TIME) = 0.0f;
    nFunction();
    c_cast<coreFloat&>(TIME) = fSave;
}


#endif // _P1_GUARD_UTILITIES_H_