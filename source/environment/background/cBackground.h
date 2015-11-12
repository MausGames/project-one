//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BACKGROUND_H_
#define _P1_GUARD_BACKGROUND_H_

// TODO: cache environment resources on loading (currently 100s of resource lookups)
// TODO: merge stone diff and norm textures (own shader ?)
// TODO: clouds on grass background need no separate heap allocation! (beware of _FillInfinite and destructor delete)
// TODO: added object gets shadow-shader
// TODO: overlay on cloud background creates redundant overdraw
// TODO: decals are not affected by shadow or underlying diffuse value (depth-only from water (centralize away) ? -> no shadow, maybe render both (color(shadow, no textures)+depth) single-sampled)


// ****************************************************************
// background definitions
#define BACKGROUND_OBJECT_RANGE (80.0f)   // default (+/-) Y-range where objects on ground are considered visible


// ****************************************************************
// background distribution values
#define GRASS_STONE_NUM      (1536u)
#define GRASS_STONE_RESERVE  (256u)
#define GRASS_REED_NUM       (3072u)
#define GRASS_REED_1_RESERVE (512u)
#define GRASS_REED_2_RESERVE (128u)
#define GRASS_FLOWER_NUM     (2048u)
#define GRASS_FLOWER_RESERVE (1024u)
#define GRASS_LEAF_NUM       (2048u)
#define GRASS_LEAF_RESERVE   (1024u)
#define GRASS_CLOUD_NUM      (64u)
#define GRASS_CLOUD_RESERVE  (76u)   // # tested
#define GRASS_CLOUD_DENSITY  (10u)

#define SEA_STONE_NUM        (1536u)  
#define SEA_STONE_RESERVE    (256u)  
#define SEA_WEED_NUM         (3072u)  
#define SEA_WEED_RESERVE     (512u)  
#define SEA_ANIMAL_NUM       (1536u)  
#define SEA_ANIMAL_1_RESERVE (256u)  
#define SEA_ANIMAL_2_RESERVE (256u)  
// bubbles ?    


// ****************************************************************
// background interface
class INTERFACE cBackground
{
protected:
    coreFrameBuffer m_FrameBuffer;                         // background frame buffer (intern, multisampled)
    coreFrameBuffer m_ResolvedTexture;                     // resolved texture

    cOutdoor* m_pOutdoor;                                  // outdoor-surface object (optional)
    cWater*   m_pWater;                                    // water-surface object (optional)

    std::vector<coreBatchList*> m_apGroundObjectList;      // persistent objects connected to the ground
    std::vector<coreBatchList*> m_apDecalObjectList;       // persistent transparent objects connected to the ground
    std::vector<coreBatchList*> m_apAirObjectList;         // persistent objects floating in the air

    std::vector<coreObject3D*>            m_apAddObject;   // temporary additional objects
    coreLookup<coreUint8, coreBatchList*> m_apAddList;     // optimized lists for temporary additional objects


public:
    cBackground()noexcept;
    virtual ~cBackground();

    DISABLE_COPY(cBackground)
    ENABLE_ID

    // render and move the background
    void Render();
    void Move();

    // manage additional objects
    void AddObject   (coreObject3D* pObject, const coreVector3& vRelativePos);
    void AddObject   (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint8& iListIndex);
    void AddList     (const coreUint8& iListIndex, const coreUint32& iCapacity, const coreChar* pcProgramInstancedName);
    void ShoveObjects(const coreFloat& fOffset);
    void ClearObjects();

    // access frame buffer
    inline coreFrameBuffer* GetResolvedTexture() {return &m_ResolvedTexture;}

    // access background components
    inline cOutdoor*                    GetOutdoor         ()const {return m_pOutdoor;}
    inline cWater*                      GetWater           ()const {return m_pWater;}
    inline std::vector<coreBatchList*>* GetGroundObjectList()      {return &m_apGroundObjectList;}
    inline std::vector<coreBatchList*>* GetDecalObjectList ()      {return &m_apDecalObjectList;}
    inline std::vector<coreBatchList*>* GetAirObjectList   ()      {return &m_apAirObjectList;}


protected:
    // create infinite looking object list
    static void _FillInfinite(coreBatchList* OUTPUT pObjectList);

    // check for intersection with other objects
    static coreBool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat& fDistanceSq);
    static coreBool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat& fDistanceSq);


private:
    // own routines for derived classes
    virtual void __RenderOwn() {}
    virtual void __MoveOwn  () {}
};


// ****************************************************************
// empty background class
class cNoBackground final : public cBackground
{
public:
    cNoBackground()noexcept {}

    DISABLE_COPY(cNoBackground)
    ASSIGN_ID(0, "Nothing")


private:
    // execute own routines
    inline void __RenderOwn()override {m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}
};


// ****************************************************************
// grass background class
class cGrassBackground : public cBackground
{
private:
    coreSoundPtr m_pNatureSound;   // nature sound-effect

    coreFlow  m_fLeafTime;         // 
    coreUintW m_iLeafNum;          // 


public:
    explicit cGrassBackground(const coreUint8& iCloudDensity = 1u)noexcept;
    virtual ~cGrassBackground();

    DISABLE_COPY(cGrassBackground)
    ASSIGN_ID(11, "Grass")


protected:
    // execute own routines
    void __MoveOwn()override;
};


// ****************************************************************
// cloudy grass background class
class cCloudBackground final : public cGrassBackground
{
private:
    coreObject2D m_Overlay;       // 

    coreSoundPtr m_pWindSound;    // wind sound-effect
    coreFloat    m_fWindVolume;   // dedicated volume (to adjust with cloud density)


public:
    cCloudBackground()noexcept;
    ~cCloudBackground();

    DISABLE_COPY(cCloudBackground)
    ASSIGN_ID(1, "Cloud")

    // 
    void SetCloudAlpha  (const coreFloat& fAlpha);
    void SetOverlayAlpha(const coreFloat& fAlpha);
    void ReduceClouds();


private:
    // execute own routines
    void __RenderOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// sea background class
class cSeaBackground final : public cBackground
{
private:
    coreFlow m_fWaveTime;   // 


public:
    cSeaBackground()noexcept;

    DISABLE_COPY(cSeaBackground)
    ASSIGN_ID(2, "Sea")


protected:
    // execute own routines
    void __RenderOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// desert background class
class cDesertBackground final : public cBackground
{
public:
    cDesertBackground()noexcept;

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID(3, "Desert")
};


// ****************************************************************
// space background class
class cSpaceBackground final : public cBackground
{
public:
    cSpaceBackground()noexcept;

    DISABLE_COPY(cSpaceBackground)
    ASSIGN_ID(4, "Space")
};


// ****************************************************************
// volcano background class
class cVolcanoBackground final : public cBackground
{
private:
    cLava m_Lava;   // 


public:
    cVolcanoBackground()noexcept;

    DISABLE_COPY(cVolcanoBackground)
    ASSIGN_ID(5, "Volcano")


protected:
    // execute own routines
    void __RenderOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// snow background class
class cSnowBackground final : public cBackground
{
public:
    cSnowBackground()noexcept;

    DISABLE_COPY(cSnowBackground)
    ASSIGN_ID(6, "Snow")
};


// ****************************************************************
// moss background class
class cMossBackground final : public cBackground
{
public:
    cMossBackground()noexcept;

    DISABLE_COPY(cMossBackground)
    ASSIGN_ID(7, "Moss")
};


#endif // _P1_GUARD_BACKGROUND_H_