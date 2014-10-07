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


// ****************************************************************
// water definitions
#define WATER_HEIGHT (-20.0f)


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

    // render and move the water-surface
    virtual void Render(coreFrameBuffer* pBackground);
    virtual void Move()override;

    // update water reflection map
    void UpdateReflection();

    // set object properties
    void SetFlyOffset(const float& fFlyOffset);

    // get object properties
    inline const float&     GetFlyOffset ()const {return m_fFlyOffset;}
    inline coreFrameBuffer* GetReflection()      {return &m_iAboveReflection;}
    inline coreFrameBuffer* GetRefraction()      {return &m_iBelowRefraction;}


private:
    DISABLE_COPY(cWater)
     
    // hide default render function
    inline void Render()override {ASSERT(false)}
};


// ****************************************************************
// 
class cIce final : public cWater
{
private:


public:


};


// ****************************************************************
// 
class cRain final : public cWater
{
private:
    coreFrameBuffer* m_pRainBuffer;               // 
    //coreContainer<coreSprite>* m_paRainSprite;  // 
    coreTimer* m_pRainTimer;                      // 


public:

    // 
    void UpdateRain();
};


#endif // _P1_GUARD_WATER_H_