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


// ****************************************************************
// background distribution values
#define GRASS_STONES_1_NUM    (512u)
#define GRASS_STONES_2_NUM    (768u)
#define GRASS_STONES_RESERVE  (256u)
#define GRASS_REEDS_NUM       (2048u)
#define GRASS_REEDS_1_RESERVE (512u)
#define GRASS_REEDS_2_RESERVE (128u)
#define GRASS_FLOWERS_NUM     (2048u)
#define GRASS_FLOWERS_RESERVE (256u)
#define GRASS_CLOUDS_NUM      (96u)
#define GRASS_CLOUDS_RESERVE  (115u)   // # tested


// ****************************************************************
// background interface
class INTERFACE cBackground
{
protected:
    coreFrameBuffer m_iFrameBuffer;                     // background frame buffer (intern, multisampled)
    coreFrameBuffer m_iResolvedTexture;                 // resolved texture

    cOutdoor* m_pOutdoor;                               // outdoor-surface object (optional)
    cWater*   m_pWater;                                 // water-surface object (optional)

    std::vector<coreBatchList*> m_apGroundObjectList;   // persistent objects connected to the ground
    std::vector<coreBatchList*> m_apDecalObjectList;    // persistent transparent objects connected to the ground
    std::vector<coreBatchList*> m_apAirObjectList;      // persistent objects floating in the air

    std::vector<coreObject3D*> m_apAddObject;           // temporary additional objects


public:
    cBackground()noexcept;
    virtual ~cBackground();

    DISABLE_COPY(cBackground)
    ENABLE_ID

    // render and move the background
    void Render();
    void Move();

    // manage additional objects
    void AddObject(coreObject3D* pObject, const coreVector3& vRelativePos);
    void ClearObjects();

    // access frame buffer
    inline coreFrameBuffer* GetResolvedTexture() {return &m_iResolvedTexture;}

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
    // render the empty background
    inline void __RenderOwn()override {m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}
};


// ****************************************************************
// grass background class
class cGrassBackground final : public cBackground
{
private:
    coreSoundPtr m_pNatureSound;   // nature sound-effect


public:
    cGrassBackground()noexcept;
    ~cGrassBackground();

    DISABLE_COPY(cGrassBackground)
    ASSIGN_ID(1, "Grass")


private:
    // move the grass background
    void __MoveOwn()override;
};


// ****************************************************************
// sea background class
class cSeaBackground final : public cBackground
{
public:
    cSeaBackground()noexcept;

    DISABLE_COPY(cSeaBackground)
    ASSIGN_ID(2, "Sea")
};


// ****************************************************************
// desert background class
class cDesertBackground final : public cBackground
{
public:
    cDesertBackground()noexcept {}

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID(3, "Desert")
};


// ****************************************************************
// space background class
class cSpaceBackground final : public cBackground
{
public:
    cSpaceBackground()noexcept {}

    DISABLE_COPY(cSpaceBackground)
    ASSIGN_ID(4, "Space")
};


// ****************************************************************
// volcano background class
class cVolcanoBackground final : public cBackground
{
public:
    cVolcanoBackground()noexcept {}

    DISABLE_COPY(cVolcanoBackground)
    ASSIGN_ID(5, "Volcano")
};


// ****************************************************************
// snow background class
class cSnowBackground final : public cBackground
{
public:
    cSnowBackground()noexcept {}

    DISABLE_COPY(cSnowBackground)
    ASSIGN_ID(6, "Snow")
};


// ****************************************************************
// moss background class
class cMossBackground final : public cBackground
{
public:
    cMossBackground()noexcept {}

    DISABLE_COPY(cMossBackground)
    ASSIGN_ID(7, "Moss")
};


#endif // _P1_GUARD_BACKGROUND_H_