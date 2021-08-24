///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WATER_H_
#define _P1_GUARD_WATER_H_

// TODO 5: reduce reflection-framebuffer without reflection enabled
// TODO 5: alpha mapping water (no distortion) for very bad hardware
// TODO 3: water-surface clipping for refraction
// TODO 5: flying should create strong/fast water ripples
// TODO 3: specular contribution should be reduced by shadow (object shadow and environment darkening)
// TODO 3: remove sqrt/coreUnpackNormalMap in rainy shader (pre-processing like in outdoor)
// TODO 3: do not load default water shader and texture if not required (maybe create water-interface, NormalWater, Clean, Fresh)
// TODO 3: glBindTexture has to reset internal texture-cache (or implement method to change in texture)
// TODO 3: stencil or early depth to not evaluate land areas


// ****************************************************************
// water definitions
#define WATER_HEIGHT       (-20.0f)   // default water-surface z-position
#define WATER_SIZE         (200.0f)   // absolute size of the water-surface
#define WATER_SKY_SIZE     (3.0f)     // texture size of the sky-plane
#define WATER_SCALE_FACTOR (0.5f)     // frame buffer resolution factor (reflection and depth)

#define RAIN_DROPS         (20u)      // 
#define RAIN_DROP_SPEED    (1.15f)    // 
#define RAIN_DROP_WIDTH    (0.2f)     // 
#define RAIN_SCALE_FACTOR  (0.5f)     // frame buffer resolution factor


// ****************************************************************
// water-surface class
class cWater : public coreObject3D
{
protected:
    coreFlow  m_fAnimation;             // water animation value
    coreFloat m_fFlyOffset;             // current fly offset

    coreFrameBuffer m_Reflection;       // reflection frame buffer (reduced resolution)
    coreFrameBuffer m_Refraction;       // refraction frame buffer
    coreFrameBuffer m_Depth;            // depth frame buffer (reduced resolution)

    coreFullscreen m_Sky;               // sky-plane as reflection background
    coreProgramPtr m_apSkyProgram[2];   // 


public:
    explicit cWater(const coreHashString& sSkyTexture)noexcept;
    virtual ~cWater()override;

    DISABLE_COPY(cWater)

    // render and move the water-surface
    void Render(coreFrameBuffer* pBackground);
    void Move()final;

    // update water reflection and depth
    void UpdateReflection();
    void UpdateDepth(cOutdoor* pOutdoor, const coreList<coreBatchList*>& apGroundObjectList);

    // set object properties
    inline void SetFlyOffset(const coreFloat fFlyOffset) {m_fFlyOffset = fFlyOffset;}

    // get object properties
    inline const coreFloat& GetFlyOffset ()const {return m_fFlyOffset;}
    inline coreFrameBuffer* GetReflection()      {return &m_Reflection;}
    inline coreFrameBuffer* GetRefraction()      {return &m_Refraction;}
    inline coreFrameBuffer* GetDepth     ()      {return &m_Depth;}


private:
    // own routines for derived classes
    virtual void __RenderOwn() {}
    virtual void __MoveOwn  () {}
    virtual void __UpdateOwn() {}

    // hide default render functions
    inline void Render(const coreProgramPtr& pProgram)final {ASSERT(false)}
    inline void Render()final                               {ASSERT(false)}
};


// ****************************************************************
// underwater-surface class
class cUnderWater final : public cWater
{
public:
    cUnderWater()noexcept;

    DISABLE_COPY(cUnderWater)
};


// ****************************************************************
// ice-surface class
class cIceWater final : public cWater
{
private:
    coreObject3D m_Ice;   // 


public:
    explicit cIceWater(const coreHashString& sSkyTexture)noexcept;

    DISABLE_COPY(cIceWater)


private:
    // 
    void __RenderOwn()final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// rain-surface class
class cRainWater final : public cWater
{
private:
    coreFrameBuffer m_WaveMap;           // 
    coreFullscreen  m_WaveInjection;     // 

    coreObject3D  m_aDrop[RAIN_DROPS];   // 
    coreBatchList m_DropList;            // 
    coreUintW     m_iCurDrop;            // 

    coreFlow m_fFallDelay;               // 


public:
    explicit cRainWater(const coreHashString& sSkyTexture)noexcept;
    ~cRainWater()final;

    DISABLE_COPY(cRainWater)


private:
    // 
    void __UpdateOwn()final;
};


// ****************************************************************
// lava-surface class
class cLava final : public coreObject3D
{
private:
    coreFlow  m_fAnimation;   // lava animation value
    coreFloat m_fFlyOffset;   // current fly offset


public:
    cLava()noexcept;

    DISABLE_COPY(cLava)

    // render and move the lava-surface
    void Render()final;
    void Move  ()final;

    // set object properties
    inline void SetFlyOffset(const coreFloat fFlyOffset) {m_fFlyOffset = fFlyOffset;}

    // get object properties
    inline const coreFloat& GetFlyOffset()const {return m_fFlyOffset;}
};


#endif // _P1_GUARD_WATER_H_