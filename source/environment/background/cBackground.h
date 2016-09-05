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

// TODO: merge stone diff and norm textures (own shader ?)
// TODO: added object gets shadow-shader
// TODO: expose pool-allocator for additional objects (AddList)
// TODO: no blitting on disabled anti-aliasing ( low-optimizations on other components)
// TODO: optimize density to never try to draw on 0.0f
// TODO: make grass leafs same color as other plants
// TODO: make wind-sound (sand) depend on speed


// ****************************************************************
// background definitions
#define BACKGROUND_OBJECT_RANGE (80.0f)   // default (+/-) Y-range where objects on ground are considered visible

#define __BACKGROUND_SCANLINE(x,i,n) (coreVector2((x) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(n)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2u)) * OUTDOOR_DETAIL)


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

#define SEA_STONE_NUM        (1536u)
#define SEA_STONE_RESERVE    (256u)
#define SEA_WEED_NUM         (3072u)
#define SEA_WEED_RESERVE     (512u)
#define SEA_ANIMAL_NUM       (1536u)
#define SEA_ANIMAL_1_RESERVE (256u)
#define SEA_ANIMAL_2_RESERVE (256u)
#define SEA_ALGAE_NUM        (2048u)
#define SEA_ALGAE_RESERVE    (1024u)

#define DESERT_STONE_NUM     (1536u)
#define DESERT_STONE_RESERVE (256u)
#define DESERT_SAND_NUM      (3u)

#define MOSS_RAIN_NUM        (6u)
#define MOSS_CLOUD_NUM       (64u)
#define MOSS_CLOUD_RESERVE   (76u)   // # tested

#define CLOUD_CLOUD_NUM      (576u)
#define CLOUD_CLOUD_RESERVE  (684u)   // # tested


// ****************************************************************
// background interface
class INTERFACE cBackground
{
protected:
    coreFrameBuffer m_FrameBuffer;                         // background frame buffer (multisampled)
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
    void AddObject   (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint8 iListIndex);
    void AddList     (const coreUint8 iListIndex, const coreUint32 iCapacity, const coreChar* pcProgramInstancedName);
    void ShoveObjects(const coreFloat fOffset);
    void ClearObjects();

    // 
    void SetGroundDensity(const coreUintW iIndex, const coreFloat fDensity);
    void SetDecalDensity (const coreUintW iIndex, const coreFloat fDensity);
    void SetAirDensity   (const coreUintW iIndex, const coreFloat fDensity);

    // access frame buffer
    inline coreFrameBuffer* GetResolvedTexture() {return &m_ResolvedTexture;}

    // access background components
    inline cOutdoor*                    GetOutdoor         ()const {return m_pOutdoor;}
    inline cWater*                      GetWater           ()const {return m_pWater;}
    inline std::vector<coreBatchList*>* GetGroundObjectList()      {return &m_apGroundObjectList;}
    inline std::vector<coreBatchList*>* GetDecalObjectList ()      {return &m_apDecalObjectList;}
    inline std::vector<coreBatchList*>* GetAirObjectList   ()      {return &m_apAirObjectList;}


protected:
    // 
    static void _FillInfinite   (coreBatchList* OUTPUT pObjectList);
    static void _SortBackToFront(coreBatchList* OUTPUT pObjectList);

    // check for intersection with other objects
    static coreBool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq);
    static coreBool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq);


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
    cNoBackground() = default;

    DISABLE_COPY(cNoBackground)
    ASSIGN_ID(0, "Nothing")


private:
    // execute own routines
    inline void __RenderOwn()final {m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}
};


// ****************************************************************
// grass background class
class cGrassBackground final : public cBackground
{
private:
    coreSoundPtr m_pNatureSound;   // nature sound-effect

    coreFlow  m_fLeafTime;         // 
    coreUintW m_iLeafNum;          // 


public:
    cGrassBackground()noexcept;
    ~cGrassBackground()final;

    DISABLE_COPY(cGrassBackground)
    ASSIGN_ID(1, "Grass")


protected:
    // execute own routines
    void __MoveOwn()final;
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
    void __RenderOwn()final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// desert background class
class cDesertBackground final : public cBackground
{
private:
    coreSoundPtr m_pWindSound;   // wind sound-effect


public:
    cDesertBackground()noexcept;
    ~cDesertBackground()final;

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID(3, "Desert")


private:
    // execute own routines
    void __MoveOwn()final;
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
    void __RenderOwn()final;
    void __MoveOwn  ()final;
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


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// dark background class
class cDarkBackground final : public cBackground
{
public:
    cDarkBackground()noexcept;

    DISABLE_COPY(cDarkBackground)
    ASSIGN_ID(8, "Dark")
};


// ****************************************************************
// cloud background class
class cCloudBackground final : public cBackground
{
private:
    coreObject2D m_Cover;        // 
    coreFlow     m_fOffset;      // 

    coreSoundPtr m_pWindSound;   // wind sound-effect


public:
    cCloudBackground()noexcept;
    ~cCloudBackground()final;

    DISABLE_COPY(cCloudBackground)
    ASSIGN_ID(99, "Cloud")


private:
    // execute own routines
    void __RenderOwn()final;
    void __MoveOwn  ()final;
};


#endif // _P1_GUARD_BACKGROUND_H_