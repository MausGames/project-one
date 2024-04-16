///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BACKGROUND_H_
#define _P1_GUARD_BACKGROUND_H_

// TODO 5: merge stone diff and norm textures (own shader ?)
// TODO 5: added object gets shadow-shader
// TODO 3: reduce object-buffer sizes, not all are drawn at once anyway, also allocate only once
// TODO 3: positions in separate list (when iterating through lambda)
// TODO 3: provide own memory pool for temporary additional objects (remove MANAGED_), also WindScreen
// TODO 3: expose pool-allocator for additional objects (AddList), also WindScreen
// TODO 2: configurable view-range ? per list ? auto per height ?
// TODO 3: calls to pList->MoveNormal(); (sub-classes) are required to invoke buffer-updates, can they be reduced ?
// TODO 3: stomach should not create all vertices
// TODO 3: EnableShadowRead only if appropriate ground objects would be rendered (IsInstanced)
// TODO 5: grass+blood and sand+snow (norm) textures are duplicated (especially normal maps), proxies are not possible, because files are loaded directly
// TODO 5: change snow texture to little flakes (broken quads)
// TODO 3: in Add functions change sListKey to a combination of resource identifiers
// TODO 3: adding temporary objects should cache resources
// TODO 3: remove draw-call for volcano smoke base-objects
// TODO 3: adding temporary objects can/should implicitly rotate the position ?
// TODO 3: use __Reset in water class, instead of __OwnInit
// TODO 3: tropfen einschlag bei moss
// TODO 3: there is no shadow on the lava (add shadow to lava or blend on edges ?)
// TODO 3: if dark-background blocks are only using 1 channel, only use single-channel texture, but requires special shader (merge with normal map then ?)
// TODO 4: move base-sound into base class ?
// TODO 3: caustics for sea background (Worley noise)
// TODO 3: vielleicht visual height + Z reinrechnen für visibility calculations (dann kann große view range reduziert werden)
// TODO 3: background object replication bug (wo objekte zu nah zusammen sind an der replikations-linie) (bei fly offset 145 in center), fällt fast nicht auf, nur bei sting-only snow background
// TODO 2: bei background-density interpolation, wenn infinity-jump durchgeführt wird, poppen objekte noch immer rein/raus
// TODO 3: shadow-flickering on 45 degree pyramids on intel and amd slightly


// ****************************************************************
// background definitions
#define BACKGROUND_OBJECT_RANGE  (95.0f)   // default Y-range where objects on ground are considered visible (+/-)
#define BACKGROUND_ADD_EXTENSION (1.5f)    // multiplier with Y-range where temporary additional objects are kept alive

#define __BACKGROUND_SCANLINE(x,i,n) (coreVector2((x) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i) / I_TO_F(n)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW / 2u)) * OUTDOOR_DETAIL)


// ****************************************************************
// background distribution values
#define GRASS_STONE_NUM         (3072u)
#define GRASS_STONE_RESERVE     (384u)
#define GRASS_PLANT_NUM         (3072u)
#define GRASS_PLANT_1_RESERVE   (1024u)
#define GRASS_PLANT_2_RESERVE   (384u)
#define GRASS_SHIP_NUM          (1024u)
#define GRASS_SHIP_RESERVE      (64u)
#define GRASS_FLOWER_NUM        (2048u)
#define GRASS_FLOWER_RESERVE    (1024u)
#define GRASS_LEAF_NUM          (2048u)
#define GRASS_LEAF_RESERVE      (512u)
#define GRASS_CLOUD_1_NUM       (64u)
#define GRASS_CLOUD_1_RESERVE   (76u)   // # exact
#define GRASS_CLOUD_2_NUM       (64u * 6u)
#define GRASS_CLOUD_2_RESERVE   (76u * 6u)   // # exact

#define SEA_CORAL_NUM           (2048u)
#define SEA_CORAL_1_RESERVE     (256u)
#define SEA_CORAL_2_RESERVE     (256u)
#define SEA_CORAL_3_RESERVE     (64u)
#define SEA_WEED_NUM            (3072u)
#define SEA_WEED_RESERVE        (3072u)
#define SEA_ANIMAL_NUM          (1536u)
#define SEA_ANIMAL_1_RESERVE    (256u)
#define SEA_ANIMAL_2_RESERVE    (64u)
#define SEA_ALGAE_NUM           (512u)
#define SEA_ALGAE_RESERVE       (768u)

#define DESERT_PILLAR_NUM       (1536u)
#define DESERT_PILLAR_RESERVE   (64u)
#define DESERT_STONE_NUM        (1536u)
#define DESERT_STONE_RESERVE    (192u)
#define DESERT_PLANT_NUM        (3072u)
#define DESERT_PLANT_RESERVE    (768u)
#define DESERT_CACTUS_NUM       (3072u)
#define DESERT_CACTUS_RESERVE   (256u)
#define DESERT_SAND_NUM         (7u)

#define SPACE_METEOR_NUM        (1536u)
#define SPACE_METEOR_RESERVE    (1024u + 64u)
#define SPACE_NEBULA_NUM        (4u)

#define VOLCANO_SMOKE_NUM       (512u)
#define VOLCANO_SMOKE_RESERVE   (64u)
#define VOLCANO_PYRAMID_NUM     (768u)
#define VOLCANO_PYRAMID_RESERVE (128u)
#define VOLCANO_PLANT_NUM       (3072u)
#define VOLCANO_PLANT_1_RESERVE (256u)
#define VOLCANO_PLANT_2_RESERVE (128u)
#define VOLCANO_SPARK_NUM       (512u)
#define VOLCANO_SPARK_RESERVE   (768u)
#define VOLCANO_CLOUD_NUM       (64u)
#define VOLCANO_CLOUD_RESERVE   (76u)   // # exact

#define SNOW_PLANT_NUM          (3072u)
#define SNOW_PLANT_RESERVE      (512u)
#define SNOW_STING_1_NUM        (3072u)
#define SNOW_STING_2_NUM        (3072u)
#define SNOW_STING_1_RESERVE    (768u)
#define SNOW_STING_2_RESERVE    (768u)
#define SNOW_CLOUD_1_NUM        (128u)
#define SNOW_CLOUD_1_RESERVE    (152u)   // # exact
#define SNOW_CLOUD_2_NUM        (64u)
#define SNOW_CLOUD_2_RESERVE    (76u)   // # exact
#define SNOW_SNOW_NUM           (12u)
#define SNOW_SNOW_NUM_LOW       (8u)

#define MOSS_TREE_NUM           (1536u)
#define MOSS_TREE_1_RESERVE     (128u)
#define MOSS_TREE_2_RESERVE     (64u)
#define MOSS_TREE_3_RESERVE     (32u)
#define MOSS_GRAVE_NUM          (1536u)
#define MOSS_GRAVE_RESERVE      (384u)
#define MOSS_CLOUD_NUM          (64u)
#define MOSS_CLOUD_RESERVE      (76u)   // # exact
#define MOSS_RAIN_NUM           (6u)

#define DARK_DETAIL             (10.0f)
#define DARK_BLOCKS_X           (12u)
#define DARK_BLOCKS_Y           (13u)
#define DARK_BLOCKS             (DARK_BLOCKS_X * DARK_BLOCKS_Y)
#define DARK_HEIGHT             (WATER_HEIGHT)
#define DARK_SPEED              (OUTDOOR_DETAIL)
#define DARK_COLOR_DEFAULT      (coreVector3(1.0f,1.0f,1.0f) * 0.75f)

#define STOMACH_CLOUD_NUM       (256u)
#define STOMACH_CLOUD_RESERVE   (304u)   // # exact

#define CLOUD_CLOUD_NUM         (128u * 9u)
#define CLOUD_CLOUD_RESERVE     (152u * 9u)   // # exact
#define CLOUD_RAIN_NUM          (6u)


// ****************************************************************
// background interface
class INTERFACE cBackground : public coreResourceRelation
{
private:
    // 
    template <typename T> using uDataMap = coreMap<const coreBatchList*, coreList<T>>;


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

    coreList<coreBatchList*> m_apWaterRefList;       // 

    uDataMap<coreUint16> m_aaiBaseHeight;            // 
    uDataMap<coreUint32> m_aaiBaseNormal;            // 

    coreBool m_bEmpty;                               // 
    coreBool m_bFresh;

    static coreMemoryPool s_MemoryPool;              // 

    static coreRand       s_Rand;                    // 
    // TODO 3: rand für background, outdoor (andere location für dieses coreRand object?) (oder eigentlich nur ein stack-object in Outdoor und Background)


public:
    cBackground(const coreBool bEmpty)noexcept;
    virtual ~cBackground()override;

    DISABLE_COPY(cBackground)
    ENABLE_ID_EX

    // 
    void Exit();

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
    void SetGroundDensity(const coreUintW iIndex, const coreFloat fDensity, const coreBool bForce = false);
    void SetDecalDensity (const coreUintW iIndex, const coreFloat fDensity, const coreBool bForce = false);
    void SetAirDensity   (const coreUintW iIndex, const coreFloat fDensity, const coreBool bForce = false);

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
    static void _BindSorted     (coreBatchList* OUTPUT pObjectList, coreObject3D* pObject);

    // check for intersection with other objects
    static FUNC_PURE coreBool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2 vNewPos, const coreFloat fDistanceSq);
    static FUNC_PURE coreBool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2 vNewPos, const coreFloat fDistanceSq);
    static FUNC_PURE coreBool _CheckIntersectionQuick3(const coreBatchList* pObjectList, const coreVector3 vNewPos, const coreFloat fDistanceSq);


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // reshape with the resource manager
    void __Reshape()final;

    // own routines for derived classes
    virtual void __InitOwn        () {}
    virtual void __ExitOwn        () {}
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
    // 
    static constexpr const coreVector3 Color  = coreVector3(0.5f,0.5f,0.5f);
    static constexpr const coreVector3 Color2 = coreVector3(0.4f,0.4f,0.4f);
    static constexpr const coreVector2 Icon   = coreVector2(-1.0f,-1.0f);


private:
    coreVector3 m_vColor;    // 
    coreVector3 m_vColor2;   // 


public:
    cNoBackground()noexcept : cBackground (true), m_vColor (Color), m_vColor2 (Color2) {}

    DISABLE_COPY(cNoBackground)
    ASSIGN_ID(0, "Nothing")

    // 
    inline void SetColor(const coreVector3 vColor, const coreVector3 vColor2) {m_vColor = vColor; m_vColor2 = vColor2;}

    // 
    inline coreVector3 GetColor ()const final {return m_vColor;}
    inline coreVector3 GetColor2()const final {return m_vColor2;}
    inline coreVector2 GetIcon  ()const final {return Icon;}
};


// ****************************************************************
// grass background class
class cGrassBackground final : public cBackground
{
private:
    coreFlow  m_fLeafTime;       // 
    coreUintW m_iLeafNum;        // 

    coreFullscreen m_Cover;       // 
    coreFlow       m_fOffset;     // 

    coreSoundPtr m_pBaseSound;   // base sound-effect
    //coreMusicPlayer m_Music;


public:
    cGrassBackground()noexcept;
    ~cGrassBackground()final;

    DISABLE_COPY(cGrassBackground)
    ASSIGN_ID_EX(1, "Grass", COLOR_MENU_GREEN, COLOR_MENU_GREEN, coreVector2(0.75f,0.25f))

    // 
    inline void SetCoverAlpha(const coreFloat fAlpha) {m_Cover.SetAlpha(fAlpha);}


protected:
    // execute own routines
    void __InitOwn        ()final;
    void __ExitOwn        ()final;
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
    
    
    void __UpdateOwn()final;
};


// ****************************************************************
// sea background class
class cSeaBackground final : public cBackground
{
private:
    coreFlow m_fWaveTime;        // 

    coreFlow  m_fAlgaeTime;      // 
    coreUintW m_iAlgaeNum;       // 
    
    coreFlow  m_fOverdriveTime;   // 
    coreUint8 m_vOverdriveCount;   // 
    coreBool  m_bOverdrive;      // 

    coreSoundPtr m_pBaseSound;   // base sound-effect
    //coreMusicPlayer m_Music;


public:
    cSeaBackground()noexcept;
    ~cSeaBackground()final;

    DISABLE_COPY(cSeaBackground)
    ASSIGN_ID_EX(2, "Sea", COLOR_MENU_CYAN, COLOR_MENU_CYAN, coreVector2(0.5f,0.25f))

    // 
    inline void SetOverdrive(const coreBool bOrverdrive) {m_bOverdrive = bOrverdrive;}


protected:
    // execute own routines
    void __InitOwn  ()final;
    void __ExitOwn  ()final;
    void __MoveOwn  ()final;
    void __UpdateOwn()final;

    void __CreateOverdrive(const coreVector3 vIntersect);
};


// ****************************************************************
// desert background class
class cDesertBackground final : public cBackground
{
private:
    coreFullscreen m_Sand;        // 
    coreVector2    m_vSandMove;   // 
    coreFlow       m_fSandWave;   // 

    coreFullscreen m_Veil;        // 
    
    coreVector3 m_avTrailHit[2];   // 
    coreFlow    m_fTrailBlend;
    coreBool    m_bTrail;
    
    coreVector2 m_vGroundPos;

    coreSoundPtr m_pBaseSound;    // base sound-effect


public:
    cDesertBackground()noexcept;
    ~cDesertBackground()final;

    DISABLE_COPY(cDesertBackground)
    ASSIGN_ID_EX(3, "Desert", COLOR_MENU_YELLOW, COLOR_MENU_YELLOW, coreVector2(0.0f,0.0f))

    // 
    inline void SetSandMove (const coreVector2 vMove)  {m_vSandMove = vMove;}
    inline void SetVeilAlpha(const coreFloat   fAlpha) {m_Veil.SetAlpha(fAlpha * 0.8f);}
    inline void SetTrail    (const coreBool    bTrail) {m_bTrail    = bTrail;}

    // 
    inline const coreVector2& GetSandMove()const {return m_vSandMove;}


private:
    // execute own routines
    void __InitOwn       ()final;
    void __ExitOwn       ()final;
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;

    // 
    void __CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect);
};


// ****************************************************************
// space background class
class cSpaceBackground final : public cBackground
{
private:
    coreObject2D m_Cover;         // (# not fullscreen) 
    coreObject2D m_Cover2;        // (# not fullscreen) 
    coreVector2  m_vCoverDir;     // 
    coreFloat    m_fCoverScale;   // 

    coreFloat  m_fMeteorSpeed;    // 
    coreUint16 m_iCopyLower;      // 
    coreUint16 m_iCopyUpper;      // 

    coreObject2D m_Nebula;        // (# not fullscreen) 
    coreVector2  m_vNebulaMove;   // 

    coreSoundPtr m_pBaseSound;    // base sound-effect


public:
    cSpaceBackground()noexcept;
    ~cSpaceBackground()final;

    DISABLE_COPY(cSpaceBackground)
    ASSIGN_ID_EX(4, "Space", COLOR_MENU_MAGENTA, COLOR_MENU_MAGENTA, coreVector2(0.75f,0.0f))

    // 
    inline void SetCoverColor (const coreVector3 vColor) {m_Cover.SetColor3(LERP(vColor, coreVector3(1.0f,1.0f,1.0f), 0.35f) * 1.3f); m_Cover2.SetColor3(LERP(vColor, coreVector3(1.0f,1.0f,1.0f), 0.15f) * 1.3f);}
    inline void SetCoverDir   (const coreVector2 vDir)   {m_vCoverDir    = vDir; ASSERT(vDir.IsNormalized())}
    inline void SetCoverScale (const coreFloat   fScale) {m_fCoverScale  = fScale;}
    inline void SetMeteorSpeed(const coreFloat   fSpeed) {m_fMeteorSpeed = fSpeed;}

    // 
    inline coreBatchList* GetMeteorList()const {return m_apGroundObjectList[0];}


private:
    // execute own routines
    void __InitOwn        ()final;
    void __ExitOwn        ()final;
    void __RenderOwnBefore()final;
    void __RenderOwnAfter ()final;
    void __MoveOwn        ()final;
};


// ****************************************************************
// volcano background class
class cVolcanoBackground final : public cBackground
{
private:
    cLava m_Lava;                           // 

    coreParticleSystem m_Smoke;
    coreList<coreParticleEffect> m_aSmokeEffect;

    coreFlow  m_fSparkTime;         // 
    coreUintW m_iSparkNum;          // 

    coreSoundPtr m_pBaseSound;   // base sound-effect


public:
    cVolcanoBackground()noexcept;
    ~cVolcanoBackground()final;

    DISABLE_COPY(cVolcanoBackground)
    ASSIGN_ID_EX(5, "Volcano", COLOR_MENU_ORANGE, COLOR_MENU_ORANGE, coreVector2(0.25f,0.0f))


protected:
    // execute own routines
    void __InitOwn        ()final;
    void __ExitOwn        ()final;
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

    coreSoundPtr m_pBaseSound;    // base sound-effect


public:
    cSnowBackground()noexcept;
    ~cSnowBackground()final;

    DISABLE_COPY(cSnowBackground)
    ASSIGN_ID_EX(6, "Snow", COLOR_MENU_BLUE, COLOR_MENU_BLUE, coreVector2(0.25f,0.25f))

    // 
    inline void SetSnowMove(const coreVector2 vMove) {m_vSnowMove = vMove;}


private:
    // execute own routines
    void __InitOwn       ()final;
    void __ExitOwn       ()final;
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
    coreFlow       m_fLightningFlash;   // 
    coreTimer      m_LightningTicker;   // 

    coreSoundPtr m_apThunder[3];        // 
    coreFlow     m_fThunderDelay;       // 
    coreUint8    m_iThunderIndex;       // 

    coreSoundPtr m_pBaseSound;          // base sound-effect

    cHeadlight m_Headlight;             // 

    coreBool m_bEnableLightning;        // 


public:
    cMossBackground()noexcept;
    ~cMossBackground()final;

    DISABLE_COPY(cMossBackground)
    ASSIGN_ID_EX(7, "Moss", COLOR_MENU_RED, COLOR_MENU_RED, coreVector2(0.5f,0.0f))

    // 
    inline void FlashLightning() {m_fLightningFlash = 1.0f; this->__UpdateLightning();}

    // 
    inline void SetRainMove       (const coreVector2 vMove)   {m_vRainMove        = vMove;}
    inline void SetEnableLightning(const coreBool    bEnable) {m_bEnableLightning = bEnable;}
    inline void SetEnableHeadlight(const coreBool    bEnable) {m_Headlight.SetAlpha(bEnable ? 1.0f : 0.0f);}

    // 
    inline cHeadlight* GetHeadlight() {return &m_Headlight;}

    // 
    inline const coreFloat& GetLightningAlpha ()const {return m_Lightning.GetAlpha();}
    inline const coreBool&  GetLightningStatus()const {return m_LightningTicker.GetStatus();}


private:
    // execute own routines
    void __InitOwn       ()final;
    void __ExitOwn       ()final;
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
    void __UpdateOwn     ()final;

    // 
    void __UpdateLightning();
};


// ****************************************************************
// dark background class
class cDarkBackground final : public cBackground
{
public:
    // 
    static constexpr const coreVector3 Color  = coreVector3(0.5f,0.5f,0.5f);
    static constexpr const coreVector3 Color2 = coreVector3(0.4f,0.4f,0.4f);
    static constexpr const coreVector2 Icon   = coreVector2(0.0f,0.5f);


private:
    coreBatchList m_Block;                    // 
    coreObject3D  m_aBlockRaw[DARK_BLOCKS];   // 

    coreFloat m_afStartHeight[DARK_BLOCKS];   // 

    coreFlow m_fFlyOffset;                    // 

    coreUintW m_iIndexOffset;                 // 

    coreFlow m_fAppear;                       // 
    coreFlow m_fDissolve;                     // 
    coreFlow m_afFade[DARK_BLOCKS];           // 

    coreVector3 m_vColor;                     // 
    coreVector3 m_vColor2;                    // 

    coreSoundPtr m_pBaseSound;                // base sound-effect

    coreFullscreen m_Lightning;               // 
    coreFlow       m_fLightningFlash;         // 

    cHeadlight m_Headlight;                   // 


public:
    cDarkBackground()noexcept;
    ~cDarkBackground()final;

    DISABLE_COPY(cDarkBackground)
    ASSIGN_ID(8, "Dark")

    // 
    void Appear();
    void Dissolve();
    inline coreBool IsDissolved()const {return (m_fDissolve >= 10.0f);}

    // 
    inline void FlashLightning() {m_fLightningFlash = 1.0f; this->__UpdateLightning();}

    // 
    inline void SetColor      (const coreVector3 vColor, const coreVector3 vColor2) {m_vColor = vColor; m_vColor2 = vColor2;}
    inline void SetBlockHeight(const coreUintW   iIndex, const coreFloat   fHeight) {ASSERT(iIndex < DARK_BLOCKS) m_aBlockRaw[iIndex].SetPosition(coreVector3(m_aBlockRaw[iIndex].GetPosition().xy(), DARK_HEIGHT + fHeight));}
    inline void SetBlockColor (const coreUintW   iIndex, const coreVector3 vColor)  {ASSERT(iIndex < DARK_BLOCKS) m_aBlockRaw[iIndex].SetColor3  (vColor * m_aBlockRaw[iIndex].GetAlpha());}

    // 
    inline coreVector3      GetColor            ()const final                 {return m_vColor;}
    inline coreVector3      GetColor2           ()const final                 {return m_vColor2;}
    inline coreVector2      GetIcon             ()const final                 {return cDarkBackground::Icon;}
    inline coreVector2      GetBlockPosition    (const coreUintW iIndex)const {ASSERT(iIndex < DARK_BLOCKS) return m_aBlockRaw[iIndex].GetPosition().xy() - cDarkBackground::__GetCameraPos();}
    inline coreVector2      GetBlockPositionNorm(const coreUintW iIndex)const {return this->GetBlockPosition(iIndex) / (coreVector2(I_TO_F(DARK_BLOCKS_X - 1u), I_TO_F(DARK_BLOCKS_Y - 1u)) / 2.0f * DARK_DETAIL);}
    inline const coreFloat& GetBlockHeight      (const coreUintW iIndex)const {ASSERT(iIndex < DARK_BLOCKS) return m_aBlockRaw[iIndex].GetPosition().z;}
    inline coreVector3      GetBlockColor       (const coreUintW iIndex)const {ASSERT(iIndex < DARK_BLOCKS) return m_aBlockRaw[iIndex].GetColor3() / m_aBlockRaw[iIndex].GetAlpha();}
    inline const coreFloat& GetStartHeight      (const coreUintW iIndex)const {ASSERT(iIndex < DARK_BLOCKS) return m_afStartHeight[iIndex];}
    inline coreUintW        GetRelativeIndex    (const coreUintW iIndex)const {return (iIndex + m_iIndexOffset) % DARK_BLOCKS;}

    // 
    inline cHeadlight* GetHeadlight() {return &m_Headlight;}


private:
    // execute own routines
    void __InitOwn        ()final;
    void __ExitOwn        ()final;
    void __RenderOwnBefore()final;
    void __RenderOwnAfter ()final;
    void __MoveOwn        ()final;
    void __UpdateOwn      ()final;

    // 
    void __UpdateLightning();

    // 
    static coreVector2 __GetCameraPos();
};


// ****************************************************************
// 
class cStomachBackground final : public cBackground
{
private:
    cHeadlight m_Headlight;      // 

    coreSoundPtr m_pBaseSound;   // base sound-effect


public:
    cStomachBackground()noexcept;
    ~cStomachBackground()final;

    DISABLE_COPY(cStomachBackground)
    ASSIGN_ID_EX(51, "Stomach", COLOR_MENU_RED, COLOR_MENU_RED, coreVector2(-1.0f,-1.0f))

    // 
    inline cHeadlight* GetHeadlight() {return &m_Headlight;}


private:
    // execute own routines
    void __InitOwn       ()final;
    void __ExitOwn       ()final;
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
    void __UpdateOwn     ()final;
};


// ****************************************************************
// cloud background class
class cCloudBackground final : public cBackground
{
private:
    coreFullscreen m_Rain;        // 
    coreVector2    m_vRainMove;   // 

    coreFullscreen m_Cover;       // 
    coreFlow       m_fOffset;     // 

    coreSoundPtr m_pBaseSound;    // base sound-effect


public:
    cCloudBackground()noexcept;
    ~cCloudBackground()final;

    DISABLE_COPY(cCloudBackground)
    ASSIGN_ID_EX(99, "Cloud", COLOR_MENU_PURPLE, COLOR_MENU_PURPLE, coreVector2(0.0f,0.25f))


private:
    // execute own routines
    void __InitOwn        ()final;
    void __ExitOwn        ()final;
    void __RenderOwnBefore()final;
    void __RenderOwnAfter ()final;
    void __MoveOwn        ()final;
};


#endif // _P1_GUARD_BACKGROUND_H_