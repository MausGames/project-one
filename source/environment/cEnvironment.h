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

// TODO: affect decals by early depth-pass
// TODO: render depth-quads on unused background areas (transition!)


// ****************************************************************
// environment definitions
#define GRASS_STONES_1_NUM    (512u)
#define GRASS_STONES_2_NUM    (768u)
#define GRASS_STONES_1_SIZE   (2.2f)
#define GRASS_STONES_2_SIZE   (2.0f)
#define GRASS_STONES_RESERVE  (256u)
#define GRASS_REEDS_NUM       (2048u)
#define GRASS_REEDS_1_RESERVE (512u)
#define GRASS_REEDS_2_RESERVE (128u)
#define GRASS_FLOWERS_NUM     (768u)
#define GRASS_FLOWERS_RESERVE (256u)
#define GRASS_CLOUDS_NUM      (96u)
#define GRASS_CLOUDS_RESERVE  (115u)   // tested


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
    static void _FillInfinite(coreBatchList* pObjectList);

    // check for intersection with other objects
    static bool _CheckIntersection     (const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq);
    static bool _CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq);


private:
    DISABLE_COPY(cBackground)

    // render and move routine for derived classes
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


public:
    cEnvironment()noexcept;
    ~cEnvironment();

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


private:
    DISABLE_COPY(cEnvironment)

    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


// ****************************************************************
// empty background class
class cEmptyBackground final : public cBackground
{
public:
    cEmptyBackground()noexcept {}
    ~cEmptyBackground()        {}

    ASSIGN_ID(0, "Empty")


private:
    // render the empty background
    inline void __RenderOwn()override {glClearColor(0.0f, 0.0f, 0.0f, 0.0f); m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR); glClearColor(0.5f, 0.5f, 0.5f, 0.0f);}
};


// ****************************************************************
// grass background class
class cGrass final : public cBackground
{
private:
    coreSoundPtr m_pNatureSound;   // nature sound-effect


public:
    cGrass()noexcept;
    ~cGrass();

    ASSIGN_ID(1, "Grass")


private:
    // move the grass background
    void __MoveOwn()override;
};



#if 0

// ****************************************************************
// Weltraum
#define NUM_METEORS 62
class cSpace : public cBackground
{
private:
    coreSprite* m_pBackground;               // Hintergrund des Weltraums
    coreSprite* m_pStar;                     // Sterne des Weltraums

    float m_fOffset;                         // Versatz des Hintergrunds

    coreVector3 m_avColor[2];                // Farb-Werte des Hintergrunds
    float m_fFactor;                         // Misch-Faktor

    coreSprite* m_pOutdoorWrap;              // Tiefenüberzeichner für Außen-Bereiche

    coreObject3D* m_apMeteor[NUM_METEORS];   // Meteoriten-Objekte
    coreInstanceList* m_apMeteorList[2];     // Instanzierte Liste für die Meteoriten

    float m_fAngle;                          // Rotation der Meteoriten
    float m_fSpeed;                          // Bewegungsgeschwindigkeit der Meteoriten
    coreVector2 m_vMeteorDir;                // Flugrichtung der Meteoriten

    BOOL m_bUseMeteor;                       // Meteoriten-Animation verwenden
    BOOL m_bUseOutdoor;                      // Außengebiet verwenden


public:
    cSpace(const BYTE &Type)noexcept;
    ~cSpace();

    // Weltraum zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


// ****************************************************************
// Meeres-Gebiet
class cSea : public cBackground
{
private:
    coreTexture* m_pWindCircle;                  // Textur für Wind-Ausschnitte
    vector<coreVector3> m_avCloudPos2;          // Gespeicherte Position der Wolken-Sprites
    // seetang, seesterne, muscheln, verschwommen

public:
    cSea()noexcept;
    ~cSea();

    // Meeres-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


// ****************************************************************
// Vulkan-Gebiet
class cLava : public cBackground
{
private:
    cMagma* m_pMagma;             // Magma-Oberfläche

    coreSprite* m_apWind[3];     // Wind-Animation
    coreVector2 m_avOffset[3];   // Versatz des Windes
    float m_fWave;                 // Aktuelle Schwingung
    // rauch mit schein, lava, kleine feuer, funken

public:
    cLava()noexcept;
    ~cLava();

    // Vulkan-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


// ****************************************************************
// Wüsten-Gebiet
class cDesert : public cBackground
{
private:
    coreSprite* m_apWind[2];        // Wind-Animation
    coreVector2 m_avOffset[2];        // Versatz des Windes
    coreVector2 m_avDirection[2];   // Wind-Richtung
    // sandsturm, zerstörte teile, knochen?

public:
    cDesert()noexcept;
    ~cDesert();

    // Wüsten-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


// ****************************************************************
// Schnee-Gebiet
class cSnow : public cBackground
{
private:
    cWater* m_pWater;             // Wasser-Oberfläche

    coreSprite* m_apWind[5];     // Wind-Animation
    coreVector2 m_avOffset[5];   // Versatz des Windes
    // zersplittertes eis am wasser, schnee, dicke wolken

public:
    cSnow()noexcept;
    ~cSnow();

    // Schnee-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


// ****************************************************************
// Felsen-Gebiet
class cRock : public cBackground
{
private:
    coreSprite* m_pBlack;          // Verdunklung

    coreFrameBuffer* m_pLight;      // Licht-Textur
    coreObject2D* m_apSpot[15];   // Licht-Objekte
    BYTE m_NumSpots;              // Aktive Licht-Objekte über 4

    coreSprite* m_apWind[2];      // Wind-Animation
    coreVector2 m_avOffset[2];    // Versatz des Windes

    float m_fBaseDarkness;          // Basis-Dunkelheit
    float m_fTrueDarkness;          // Echte Dunkelheit


public:
    cRock()noexcept;
    ~cRock();

    // Felsen-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();

    // Basis-Dunkelheit setzen
    inline void SetBaseDarkness(const float &fValue) {m_fBaseDarkness = fValue;}
};


// ****************************************************************
// Mars-Gebiet
class cMars : public cBackground
{
public:
    cMars()noexcept;
    ~cMars();

    // Mars-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;
};


// ****************************************************************
// Moos-Gebiet
class cMoss : public cBackground
{
private:
    cWater* m_pWater;             // Wasser-Oberfläche

    coreSprite* m_pBlack;         // Verdunklung
    coreSprite* m_pSpark;         // Blitzleuchten
    coreTimer* m_apThunder[3];   // Donner-Zähler

    coreSprite* m_apWind[4];     // Wind-Animation
    coreVector2 m_avOffset[4];   // Versatz des Windes
    // umliegendes holz, tote bäume, seeblätter

public:
    cMoss()noexcept;
    ~cMoss();

    // Mars-Gebiet zeichnen und berechnen
    void Render()override;
    void Move  ()override;

    // Übergeordnete Animation zeichnen
    void RenderOverlay();
};


#endif


#endif // _P1_GUARD_ENVIRONMENT_H_