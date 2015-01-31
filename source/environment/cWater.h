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


// ****************************************************************
// water definitions
#define WATER_HEIGHT   (-20.0f)   // default water-surface z-position
#define WATER_SKY_SIZE (3.0f)     // texture size of the sky-plane


// ****************************************************************
// water-surface class
class cWater : public coreObject3D
{
protected:
    coreFlow m_fAnimation;                // water animation value
    float    m_fFlyOffset;                // current fly offset

    coreFrameBuffer m_iAboveReflection;   // reflection frame buffer
    coreFrameBuffer m_iBelowRefraction;   // refraction frame buffer with geometric depth

    coreObject2D m_Sky;                   // sky-plane as reflection background


public:
    cWater()noexcept;
    virtual ~cWater();

    DISABLE_COPY(cWater)

    // render and move the water-surface
    virtual void Render(coreFrameBuffer* pBackground);
    virtual void Move()override;

    // update water reflection and depth map
    void UpdateReflection();
    void UpdateDepth(cOutdoor* pOutdoor, const std::vector<coreBatchList*>& apGroundObjectList);

    // set object properties
    void SetFlyOffset(const float& fFlyOffset);

    // get object properties
    inline const float&     GetFlyOffset ()const {return m_fFlyOffset;}
    inline coreFrameBuffer* GetReflection()      {return &m_iAboveReflection;}
    inline coreFrameBuffer* GetRefraction()      {return &m_iBelowRefraction;}


private:
    // hide default render functions
    inline void Render(const coreProgramPtr& pProgram)override {ASSERT(false)}
    inline void Render()override                               {ASSERT(false)}
};


// ****************************************************************
// ice-surface class
class cIce final : public cWater
{
public:
    cIce()noexcept {}

    DISABLE_COPY(cIce)
};


// ****************************************************************
// rain-surface class
class cRain final : public cWater
{
public:
    cRain()noexcept {}

    DISABLE_COPY(cRain)
};


#endif // _P1_GUARD_WATER_H_