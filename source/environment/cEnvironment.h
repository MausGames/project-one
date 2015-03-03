//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENVIRONMENT_H_
#define _P1_GUARD_ENVIRONMENT_H_

// TODO: render depth-quads or use scissor test on unused background areas (transition!)
// TODO: cache environment resources on loading (currently 100s of resource lookups)
// TODO: merge stone diff and norm textures (own shader ?)


// ****************************************************************
// environment distribution values
#define GRASS_STONES_1_NUM    (512u)
#define GRASS_STONES_2_NUM    (768u)
#define GRASS_STONES_RESERVE  (256u)
#define GRASS_REEDS_NUM       (2048u)
#define GRASS_REEDS_1_RESERVE (512u)
#define GRASS_REEDS_2_RESERVE (128u)
#define GRASS_FLOWERS_NUM     (2028u)
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
    static bool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq);
    static bool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq);


private:
    // render and move routines for derived classes
    virtual void __RenderOwn() {}
    virtual void __MoveOwn()   {}
};


// ****************************************************************
// environment class
class cEnvironment final : public coreResourceRelation
{
private:
    cBackground* m_pBackground;       // current background instance (should never be NULL)
    cBackground* m_pOldBackground;    // previous background instance (may be NULL)

    coreFrameBuffer m_iFrameBuffer;   // environment frame buffer used for transition mixing (only texture)
    coreObject2D    m_MixObject;      // fullscreen object for transition mixing
    coreTimer       m_Transition;     // background-transition timer

    coreVector2 m_avDirection[2];     // background direction (0 = current value, 1 = target value)
    coreVector2 m_avSide     [2];     // background position offset
    float       m_afSpeed    [2];     // movement speed

    float m_fFlyOffset;               // global fly offset (directly accessed by background objects)
    float m_fSideOffset;              // global side offset
    coreVector3 m_vCameraPos;         // moved camera position
    coreVector3 m_vLightDir;          // rotated light direction

    bool m_bActive;                   // enables the environment (only for first background-transition on intro)


public:
    cEnvironment()noexcept;
    ~cEnvironment();

    DISABLE_COPY(cEnvironment)

    // render and move the environment
    void Render();
    void Move();

    // control active background
    void ChangeBackground(const coreByte& iID);
    inline cBackground*     GetBackground   ()const {return m_pBackground;}
    inline cBackground*     GetOldBackground()const {return m_pOldBackground;}
    inline const coreTimer& GetTransition   ()const {return m_Transition;}

    // retrieve safe height value
    float RetrieveSafeHeight(const coreVector2& vPosition);

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_Transition.GetStatus() ? &m_iFrameBuffer : m_pBackground->GetResolvedTexture();}

    // set target transformation properties
    inline void SetTargetDirection(const coreVector2& vDirection) {m_avDirection[1] = vDirection; ASSERT(vDirection.IsNormalized())}
    inline void SetTargetSide     (const coreVector2& vSide)      {m_avSide     [1] = vSide;}
    inline void SetTargetSpeed    (const float&       fSpeed)     {m_afSpeed    [1] = fSpeed;}

    // get current transformation properties
    inline const coreVector2& GetDirection()const {return m_avDirection[0];}
    inline const coreVector2& GetSide     ()const {return m_avSide     [0];}
    inline const float&       GetSpeed    ()const {return m_afSpeed    [0];}

    // get offset values
    inline const float&       GetFlyOffset ()const {return m_fFlyOffset;}
    inline const float&       GetSideOffset()const {return m_fSideOffset;}
    inline const coreVector3& GetCameraPos ()const {return m_vCameraPos;}
    inline const coreVector3& GetLightDir  ()const {return m_vLightDir;}

    // enable the environment
    inline void Activate() {m_bActive = true;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
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


#endif // _P1_GUARD_ENVIRONMENT_H_