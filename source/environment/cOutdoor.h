//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTDOOR_H_
#define _P1_GUARD_OUTDOOR_H_

// TODO: use triangle-strip with primitive-restart ?


// ****************************************************************
// outdoor definitions
#define OUTDOOR_VIEW           (31)                                              // visible rows
#define OUTDOOR_DETAIL         (6.0f)                                            // size of a block

#define OUTDOOR_WIDTH          (31)                                              // vertices per row
#define OUTDOOR_HEIGHT         (160)                                             // vertices per column
#define OUTDOOR_HEIGHT_FULL    (OUTDOOR_HEIGHT + OUTDOOR_VIEW)                   // with double vertices

#define OUTDOOR_BLOCKS_X       (OUTDOOR_WIDTH-1)                                 // blocks per row
#define OUTDOOR_BLOCKS_Y       (OUTDOOR_HEIGHT_FULL-1)                           // blocks per column
#define OUTDOOR_BLOCKS         (OUTDOOR_BLOCKS_X * OUTDOOR_BLOCKS_Y)             // number of all blocks

#define OUTDOOR_PER_INDICES    (6)                                               // indices per block
#define OUTDOOR_TOTAL_VERTICES (OUTDOOR_WIDTH * OUTDOOR_HEIGHT_FULL)             // total number of vertices
#define OUTDOOR_TOTAL_INDICES  (OUTDOOR_PER_INDICES * OUTDOOR_BLOCKS)            // total number of indices

#define OUTDOOR_RANGE (OUTDOOR_BLOCKS_X * OUTDOOR_PER_INDICES * OUTDOOR_VIEW)    // vertices to draw at once


// ****************************************************************
// outdoor-surface class
class cOutdoor final : public coreObject3D
{
private:
    float m_afHeight[OUTDOOR_TOTAL_VERTICES];   // height data for height calculations

    coreUint m_iRenderOffset;                   // current render offset
    float    m_fFlyOffset;                      // current fly offset

    coreByte m_iAlgorithm;                      // geometry algorithm ID
    float    m_fGrade;                          // randomness grade

    cShadow m_ShadowMap;                        // shadow map object


public:
    cOutdoor(const char* pcTextureTop, const char* pcTextureBottom, const coreByte& iAlgorithm, const float& fGrade)noexcept;
    ~cOutdoor();

    // render and move the outdoor-surface
    void Render()override;
    void Move  ()override;

    // load outdoor geometry
    void LoadGeometry(const coreByte& iAlgorithm, const float& fGrade);

    // retrieve height value
    float RetrieveHeight(const coreVector2& vPosition);

    // access shadow map object
    inline cShadow* GetShadowMap() {return &m_ShadowMap;}

    // set object properties
    void SetFlyOffset(const float& fFlyOffset);

    // get object properties
    inline const coreUint& GetRenderOffset()const {return m_iRenderOffset;}
    inline const float&    GetFlyOffset   ()const {return m_fFlyOffset;}


private:
    DISABLE_COPY(cOutdoor)
};


#endif // _P1_GUARD_OUTDOOR_H_