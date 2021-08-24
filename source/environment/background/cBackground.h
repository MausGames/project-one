///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BACKGROUND_H_
#define _P1_GUARD_BACKGROUND_H_

// TODO 5: merge stone diff and norm textures (own shader ?)
// TODO 3: added object gets shadow-shader
// TODO 3: no blitting on disabled anti-aliasing (low-optimizations on other components)
// TODO 3: optimize density to never try to draw on 0.0f
// TODO 3: object-sets with 0% visibility and no active object should be skipped entirely
// TODO 3: make grass leafs same color as other plants
// TODO 3: check if alL _RESERVES are correct
// TODO 3: reduce object-buffer sizes, not all are drawn at once anyway, also allocate only once
// TODO 3: positions in separate list (when iterating through lambda)
// TODO 3: provide own memory pool for temporary additional objects (remove MANAGED_), also WindScreen
// TODO 3: expose pool-allocator for additional objects (AddList), also WindScreen
// TODO 3: all environment sound effects should fade in transition (check at end)
// TODO 2: popping artifacts with shadow in sea-background (configurable view-range ? per list ? auto per height ?)
// TODO 3: calls to pList->MoveNormal(); may be redundant
// TODO 3: remove texture-sampling from lightning effect in moss
// TODO 3: stomach should not create all vertices
// TODO 3: EnableShadowRead only if appropriate ground objects would be rendered (IsInstanced)
// TODO 5: grass and blood textures are duplicated (especially normal maps)
// TODO 1: can thunder effect cause issues for some players ? are there other effects causing issues ? (change effects or add disable option)
// TODO 3: improve snow texture to little flakes (broken quads) 
// TODO 3: in Add functions change sListKey to a combination of resource identifiers
// TODO 3: adding temporary objects should cache resources
// TODO 3: adding temporary objects can/should implicitly rotate the position ?


// ****************************************************************
// background definitions
#define BACKGROUND_OBJECT_RANGE  (95.0f)   // default Y-range where objects on ground are considered visible (+/-)
#define BACKGROUND_ADD_EXTENSION (3.0f)    // multiplier with Y-range where temporary additional objects are kept alive

#define __BACKGROUND_SCANLINE(x,i,n) (coreVector2((x) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i) / I_TO_F(n)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW / 2u)) * OUTDOOR_DETAIL)


// ****************************************************************
// background distribution values
#define GRASS_STONE_NUM       (1536u)
#define GRASS_STONE_RESERVE   (256u)
#define GRASS_REED_NUM        (3072u)
#define GRASS_REED_1_RESERVE  (1024u)
#define GRASS_REED_2_RESERVE  (256u)
#define GRASS_FLOWER_NUM      (2048u)
#define GRASS_FLOWER_RESERVE  (1024u)
#define GRASS_LEAF_NUM        (2048u)
#define GRASS_LEAF_RESERVE    (512u)
#define GRASS_CLOUD_NUM       (64u)
#define GRASS_CLOUD_RESERVE   (76u)   // # exact

#define SEA_STONE_NUM         (1536u)
#define SEA_STONE_RESERVE     (256u)
#define SEA_WEED_NUM          (3072u)
#define SEA_WEED_RESERVE      (3072u)
#define SEA_ANIMAL_NUM        (1536u)
#define SEA_ANIMAL_1_RESERVE  (192u)
#define SEA_ANIMAL_2_RESERVE  (64u)
#define SEA_ALGAE_NUM         (2048u)
#define SEA_ALGAE_RESERVE     (512u)

#define DESERT_STONE_NUM      (1536u)
#define DESERT_STONE_RESERVE  (192u)
#define DESERT_SAND_NUM       (7u)

#define SPACE_METEOR_NUM      (1536u)
#define SPACE_METEOR_RESERVE  (1024u)

#define VOLCANO_SMOKE_NUM     (512u)
#define VOLCANO_SMOKE_RESERVE (64u)
#define VOLCANO_SPARK_NUM     (2048u)
#define VOLCANO_SPARK_RESERVE (1024u)

#define SNOW_STONE_NUM        (1536u)
#define SNOW_STONE_RESERVE    (128u)
#define SNOW_REED_NUM         (3072u)
#define SNOW_REED_RESERVE     (769u)
#define SNOW_SNOW_NUM         (12u)
#define SNOW_CLOUD_NUM        (128u)
#define SNOW_CLOUD_RESERVE    (152u)   // # exact

#define MOSS_RAIN_NUM         (6u)
#define MOSS_CLOUD_NUM        (64u)
#define MOSS_CLOUD_RESERVE    (76u)   // # exact

#define DARK_

#define STOMACH_CLOUD_NUM     (256u)
#define STOMACH_CLOUD_RESERVE (304u)   // # exact

#define CLOUD_CLOUD_NUM       (1024u)
#define CLOUD_CLOUD_RESERVE   (1216u)   // # exact


// ****************************************************************
// background interface
class INTERFACE cBackground
{
protected:
    coreFrameBuffer m_FrameBuffer;                   // background frame buffer (multisampled)
    coreFrameBuffer m_ResolvedTexture;               // resolved texture

    cOutdoor* m_pOutdoor;                            // outdoor-surface object (optional)
    cWater*   m_pWater;                              // water-surface object (optional)

    coreList<coreBatchList*> m_apGroundObjectList;   // persistent objects connected to the ground
    coreList<coreBatchList*> m_apDecalObjectList;    // persistent transparent objects connected to the ground
    coreList<coreBatchList*> m_apAirObjectList;      // persistent objects floating in the air

    coreMapStr<coreBatchList*> m_apGroundAddList;    // temporary objects connected to the ground
    coreMapStr<coreBatchList*> m_apDecalAddList;     // temporary transparent objects connected to the ground
    coreMapStr<coreBatchList*> m_apAirAddList;       // temporary objects floating in the air

    coreMap<const coreBatchList*, coreList<coreUint16>> m_aaiBaseHeight;   // 
    coreMap<const coreBatchList*, coreList<coreUint32>> m_aaiBaseNormal;   // 

    static coreMemoryPool s_MemoryPool;              // 


public:
    cBackground()noexcept;
    virtual ~cBackground();

    DISABLE_COPY(cBackground)
    ENABLE_ID_EX

    // render and move the background
    void Render();
    void Move();

    // manage temporary objects
    void AddGround(coreObject3D* pObject, const coreVector3 vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void AddDecal (coreObject3D* pObject, const coreVector3 vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void AddAir   (coreObject3D* pObject, const coreVector3 vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void ShoveAdds(const coreFloat fOffset);
    void ClearAdds();

    // 
    void SetGroundDensity(const coreUintW iIndex, const coreFloat fDensity);
    void SetDecalDensity (const coreUintW iIndex, const coreFloat fDensity);
    void SetAirDensity   (const coreUintW iIndex, const coreFloat fDensity);

    // access frame buffer
    inline coreFrameBuffer* GetResolvedTexture() {return &m_ResolvedTexture;}

    // access background components
    inline cOutdoor* GetOutdoor()const {return m_pOutdoor;}
    inline cWater*   GetWater  ()const {return m_pWater;}


protected:
    // 
    void _StoreHeight    (const coreBatchList* pObjectList, const coreFloat fHeight);
    void _StoreHeightList(const coreBatchList* pObjectList);
    void _StoreNormalList(const coreBatchList* pObjectList);

    // 
    static void _FillInfinite   (coreBatchList* OUTPUT pObjectList, const coreUintW iReserve);
    static void _SortBackToFront(coreBatchList* OUTPUT pObjectList);

    // check for intersection with other objects
    static FUNC_PURE coreBool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2 vNewPos, const coreFloat fDistanceSq);
    static FUNC_PURE coreBool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2 vNewPos, const coreFloat fDistanceSq);


private:
    // own routines for derived classes
    virtual void __RenderOwnBefore() {}
    virtual void __RenderOwnAfter () {}
    virtual void __MoveOwn        () {}
    virtual void __UpdateOwn      () {}
};


// ****************************************************************
// empty background class
class cNoBackground final : public cBackground
{
public:
    cNoBackground() = default;

    DISABLE_COPY(cNoBackground)
    ASSIGN_ID_EX(0, "Nothing", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    inline void __RenderOwnAfter()final {m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}
};


// ****************************************************************
// grass background class
class cGrassBackground final : public cBackground
{
private:
    coreFlow  m_fLeafTime;         // 
    coreUintW m_iLeafNum;          // 

    coreSoundPtr m_pNatureSound;   // nature sound-effect


public:
    cGrassBackground()noexcept;
    ~cGrassBackground()final;

    DISABLE_COPY(cGrassBackground)
    ASSIGN_ID_EX(1, "Grass", COLOR_MENU_GREEN)


protected:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// sea background class
class cSeaBackground final : public cBackground
{
private:
    coreFlow m_fWaveTime;         // 

    coreSoundPtr m_pUnderSound;   // 


public:
    cSeaBackground()noexcept;
    ~cSeaBackground()final;

    DISABLE_COPY(cSeaBackground)
    ASSIGN_ID_EX(2, "Sea", COLOR_MENU_CYAN)


protected:
    // execute own routines
    void __MoveOwn  ()final;
    void __UpdateOwn()final;
};


// ****************************************************************
// desert background class
class cDesertBackground final : public cBackground
{
private:
    coreFullscreen m_Sand;        // 
    coreVector2    m_vSandMove;   // 
    coreFlow       m_fSandWave;   // 

    coreSoundPtr m_pWindSound;    // wind sound-effect


public:
    cDesertBackground()noexcept;
    ~cDesertBackground()final;

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID_EX(3, "Desert", COLOR_MENU_YELLOW)

    // 
    inline void SetSandMove(const coreVector2 vMove) {m_vSandMove = vMove;}

    // 
    inline const coreVector2& GetSandMove()const {return m_vSandMove;}


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// space background class
class cSpaceBackground final : public cBackground
{
private:
    coreFullscreen m_Cover;       // 
    coreVector2    m_vCoverDir;   // 

    coreFloat  m_fMeteorSpeed;    // 
    coreUint16 m_iCopyLower;      // 
    coreUint16 m_iCopyUpper;      // 


public:
    cSpaceBackground()noexcept;

    DISABLE_COPY(cSpaceBackground)
    ASSIGN_ID_EX(4, "Space", COLOR_MENU_MAGENTA)

    // 
    inline void SetCoverColor (const coreVector3 vColor) {m_Cover.SetColor3(LERP(vColor, coreVector3(1.0f,1.0f,1.0f), 0.5f));}
    inline void SetCoverDir   (const coreVector2 vDir)   {m_vCoverDir    = vDir; ASSERT(vDir.IsNormalized())}
    inline void SetMeteorSpeed(const coreFloat   fSpeed) {m_fMeteorSpeed = fSpeed;}

    // 
    inline coreBatchList* GetMeteorList()const {return m_apGroundObjectList[0];}


private:
    // execute own routines
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
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
    ASSIGN_ID_EX(5, "Volcano", COLOR_MENU_ORANGE)


protected:
    // execute own routines
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
};


// ****************************************************************
// snow background class
class cSnowBackground final : public cBackground
{
private:
    coreFullscreen m_Snow;        // 
    coreVector2    m_vSnowMove;   // 
    coreFlow       m_fSnowWave;   // 


public:
    cSnowBackground()noexcept;

    DISABLE_COPY(cSnowBackground)
    ASSIGN_ID_EX(6, "Snow", COLOR_MENU_BLUE)

    // 
    inline void SetSnowMove(const coreVector2 vMove) {m_vSnowMove = vMove;}


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// moss background class
class cMossBackground final : public cBackground
{
private:
    coreFullscreen m_Rain;              // 
    coreVector2    m_vRainMove;         // 

    coreFullscreen m_Lightning;         // 
    coreFlow       m_fLightningDelay;   // 
    coreTimer      m_LightningTicker;   // 

    coreSoundPtr m_apThunder[3];        // 
    coreFlow     m_fThunderDelay;       // 
    coreUint8    m_iThunderIndex;       // 

    coreSoundPtr m_pRainSound;          // 

    cHeadlight m_Headlight;             // 

    coreBool m_bEnableLightning;        // 
    coreBool m_bEnableHeadlight;        // 


public:
    cMossBackground()noexcept;
    ~cMossBackground()final;

    DISABLE_COPY(cMossBackground)
    ASSIGN_ID_EX(7, "Moss", COLOR_MENU_RED)

    // 
    inline void SetRainMove       (const coreVector2 vMove)   {m_vRainMove        = vMove;}
    inline void SetEnableLightning(const coreBool    bEnable) {m_bEnableLightning = bEnable; m_fLightningDelay = 0.0f;}
    inline void SetEnableHeadlight(const coreBool    bEnable) {m_bEnableHeadlight = bEnable;}

    // 
    inline cHeadlight* GetHeadlight() {return &m_Headlight;}


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
    void __UpdateOwn     ()final;
};


// ****************************************************************
// dark background class
class cDarkBackground final : public cBackground
{
public:
    cDarkBackground()noexcept;

    DISABLE_COPY(cDarkBackground)
    ASSIGN_ID_EX(8, "Dark", coreVector3(0.5f,0.5f,0.5f))
};


// ****************************************************************
// 
class cStomachBackground final : public cBackground
{
private:
    cHeadlight m_Headlight;   // 


public:
    cStomachBackground()noexcept;

    DISABLE_COPY(cStomachBackground)
    ASSIGN_ID_EX(51, "Stomach", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
    void __UpdateOwn     ()final;
};


// ****************************************************************
// cloud background class
class cCloudBackground final : public cBackground
{
private:
    coreFullscreen m_Cover;      // 
    coreFlow       m_fOffset;    // 

    coreSoundPtr m_pWindSound;   // wind sound-effect


public:
    cCloudBackground()noexcept;
    ~cCloudBackground()final;

    DISABLE_COPY(cCloudBackground)
    ASSIGN_ID_EX(99, "Cloud", COLOR_MENU_PURPLE)


private:
    // execute own routines
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
};


#endif // _P1_GUARD_BACKGROUND_H_