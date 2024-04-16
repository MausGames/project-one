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

// TODO: merge stone diff and norm textures (own shader ?)
// TODO: added object gets shadow-shader
// TODO: no blitting on disabled anti-aliasing (low-optimizations on other components)
// TODO: optimize density to never try to draw on 0.0f 
// TODO: make grass leafs same color as other plants
// TODO: make wind-sound (sand) depend on speed
// TODO: check if alL _RESERVES are correct
// TODO: reduce object-buffer sizes, not all are drawn at once anyway, also allocate only once
// TODO: positions in separate list (when iterating through lambda)
// TODO: provide own memory pool for temporary additional objects (remove MANAGED_), also WindScreen
// TODO: expose pool-allocator for additional objects (AddList), also WindScreen
// TODO: all environment sound effects should fade in transition
// TODO: popping artifacts with shadow in sea-background (configurable view-range ? per list ? auto per height ?)
// TODO: calls to pList->MoveNormal(); may be redundant
// TODO: remove texture-sampling from lightning effect in moss
// TODO: stomach should not create all vertices
// TODO: EnableShadowRead only if appropriate ground objects would be rendered (IsInstanced)
// TODO: use scissor for transition, to reduce overhead


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
#define SEA_ANIMAL_2_RESERVE  (36u)
#define SEA_ALGAE_NUM         (2048u)
#define SEA_ALGAE_RESERVE     (512u)

#define DESERT_STONE_NUM      (1536u)
#define DESERT_STONE_RESERVE  (192u)
#define DESERT_SAND_NUM       (7u)

#define SPACE_METEOR_NUM      (1536u)
#define SPACE_METEOR_RESERVE  (1824u)   // # exact 

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
    coreFrameBuffer m_FrameBuffer;                      // background frame buffer (multisampled)
    coreFrameBuffer m_ResolvedTexture;                  // resolved texture

    cOutdoor* m_pOutdoor;                               // outdoor-surface object (optional)
    cWater*   m_pWater;                                 // water-surface object (optional)

    std::vector<coreBatchList*> m_apGroundObjectList;   // persistent objects connected to the ground
    std::vector<coreBatchList*> m_apDecalObjectList;    // persistent transparent objects connected to the ground
    std::vector<coreBatchList*> m_apAirObjectList;      // persistent objects floating in the air

    coreLookupStr<coreBatchList*> m_apGroundAddList;    // temporary objects connected to the ground
    coreLookupStr<coreBatchList*> m_apDecalAddList;     // temporary transparent objects connected to the ground
    coreLookupStr<coreBatchList*> m_apAirAddList;       // temporary objects floating in the air

    coreLookup<const coreBatchList*, std::vector<coreUint16>> m_aaiBaseHeight;   // 
    coreLookup<const coreBatchList*, std::vector<coreUint32>> m_aaiBaseNormal;   // 

    static coreMemoryPool s_MemoryPool;                 // 


public:
    cBackground()noexcept;
    virtual ~cBackground();

    DISABLE_COPY(cBackground)
    ENABLE_ID

    // render and move the background
    void Render();
    void Move();

    // manage temporary objects
    void AddGround(coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void AddDecal (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void AddAir   (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
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
    static FUNC_PURE coreBool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq);
    static FUNC_PURE coreBool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq);


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
    ASSIGN_ID(0, "Nothing")


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
    coreFlow m_fWaveTime;         // 

    coreSoundPtr m_pUnderSound;   // 


public:
    cSeaBackground()noexcept;
    ~cSeaBackground()final;

    DISABLE_COPY(cSeaBackground)
    ASSIGN_ID(2, "Sea")


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
    coreFullscreen m_Sand;             // 
    coreVector2    m_vSandDirection;   // 
    coreFlow       m_fSandWave;        // 

    coreSoundPtr m_pWindSound;         // wind sound-effect


public:
    cDesertBackground()noexcept;
    ~cDesertBackground()final;

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID(3, "Desert")

    // 
    inline void SetSandDirection(const coreVector2& vDirection) {m_vSandDirection = vDirection; ASSERT(vDirection.IsNormalized())}


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// space background class
class cSpaceBackground final : public cBackground
{
public:
    cSpaceBackground()noexcept;

    DISABLE_COPY(cSpaceBackground)
    ASSIGN_ID(4, "Space")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// volcano background class
class cVolcanoBackground final : public cBackground
{
private:
    cLava m_Lava;                           // 

    //coreObject3D       m_aSmokeObject[2];   // 
    //coreParticleEffect m_aSmokeEffect[2];   // 

    coreSoundPtr m_pLavaSound;              // 

    coreParticleSystem m_Smoke;
    std::vector<coreParticleEffect> m_aSmokeEffect;

    coreFlow  m_fSparkTime;         // 
    coreUintW m_iSparkNum;          // 


public:
    cVolcanoBackground()noexcept;
    ~cVolcanoBackground()final;

    DISABLE_COPY(cVolcanoBackground)
    ASSIGN_ID(5, "Volcano")


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
    coreFullscreen m_Snow;             // 
    coreVector2    m_vSnowDirection;   // 
    coreFlow       m_fSnowWave;        // 


public:
    cSnowBackground()noexcept;

    DISABLE_COPY(cSnowBackground)
    ASSIGN_ID(6, "Snow")

    // 
    inline void SetSnowDirection(const coreVector2& vDirection) {m_vSnowDirection = vDirection; ASSERT(vDirection.IsNormalized())}


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
    coreVector2    m_vRainDirection;    // 

    coreFullscreen m_Lightning;         // 
    coreFlow       m_fLightningDelay;   // 
    coreTimer      m_LightningTicker;   // 

    coreSoundPtr m_apThunder[3];        // 
    coreFlow     m_fThunderDelay;       // 
    coreUint8    m_iThunderIndex;       // 

    coreSoundPtr m_pRainSound;          // 


public:
    cMossBackground()noexcept;
    ~cMossBackground()final;

    DISABLE_COPY(cMossBackground)
    ASSIGN_ID(7, "Moss")

    // 
    inline void SetRainDirection(const coreVector2& vDirection) {m_vRainDirection = vDirection; ASSERT(vDirection.IsNormalized())}


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
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
// 
class cStomachBackground final : public cBackground
{
private:
    cHeadlight m_Headlight;   // 


public:
    cStomachBackground()noexcept;

    DISABLE_COPY(cStomachBackground)
    ASSIGN_ID(51, "Stomach")


private:
    // execute own routines
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
    void __UpdateOwn     ()final;
};


// ****************************************************************
// 
class cCaveBackground final : public cBackground
{
private:
    cHeadlight m_Headlight;   // 


public:
    cCaveBackground()noexcept;

    DISABLE_COPY(cCaveBackground)
    ASSIGN_ID(52, "Cave")


private:
    // execute own routines
    void __RenderOwnAfter()final;
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
    ASSIGN_ID(99, "Cloud")


private:
    // execute own routines
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
};


#endif // _P1_GUARD_BACKGROUND_H_