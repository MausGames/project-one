//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WATER_H_
#define _P1_GUARD_WATER_H_

// TODO: reduce reflection-framebuffer without reflection enabled
// TODO: water-surface clipping for refraction
// TODO: flying creates strong/fast water ripples
// TODO: alpha mapping water (no distortion) for very bad hardware


// ****************************************************************
// water definitions
#define WATER_HEIGHT       (-20.0f)   // default water-surface z-position
#define WATER_SIZE         (200.0f)   // absolute size of the water-surface
#define WATER_SKY_SIZE     (3.0f)     // texture size of the sky-plane
#define WATER_SCALE_FACTOR (0.5f)     // frame buffer resolution factor (reflection and depth)


// ****************************************************************
// water-surface class
class cWater : public coreObject3D
{
protected:
    coreFlow  m_fAnimation;         // water animation value
    coreFloat m_fFlyOffset;         // current fly offset

    coreFrameBuffer m_Reflection;   // reflection frame buffer (reduced resolution)
    coreFrameBuffer m_Refraction;   // refraction frame buffer
    coreFrameBuffer m_Depth;        // depth frame buffer (reduced resolution)

    coreObject2D m_Sky;             // sky-plane as reflection background


public:
    cWater()noexcept;
    virtual ~cWater()override;

    DISABLE_COPY(cWater)

    // render and move the water-surface
    virtual void Render(coreFrameBuffer* pBackground);
    virtual void Move()override;

    // update water reflection and depth
    void UpdateReflection();
    void UpdateDepth(cOutdoor* pOutdoor, const std::vector<coreBatchList*>& apGroundObjectList);

    // set object properties
    void SetFlyOffset(const coreFloat fFlyOffset);

    // get object properties
    inline const coreFloat& GetFlyOffset ()const {return m_fFlyOffset;}
    inline coreFrameBuffer* GetReflection()      {return &m_Reflection;}
    inline coreFrameBuffer* GetRefraction()      {return &m_Refraction;}
    inline coreFrameBuffer* GetDepth     ()      {return &m_Depth;}


private:
    // hide default render functions
    inline void Render(const coreProgramPtr& pProgram)override {ASSERT(false)}
    inline void Render()override                               {ASSERT(false)}
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
public:
    cIceWater()noexcept;

    DISABLE_COPY(cIceWater)
};


// ****************************************************************
// rain-surface class
class cRainWater final : public cWater
{
public:
    cRainWater() = default;

    DISABLE_COPY(cRainWater)
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
    void Render()override;
    void Move  ()override;

    // set object properties
    inline void SetFlyOffset(const coreFloat fFlyOffset) {m_fFlyOffset = fFlyOffset;}

    // get object properties
    inline const coreFloat& GetFlyOffset()const {return m_fFlyOffset;}
};


#endif // _P1_GUARD_WATER_H_