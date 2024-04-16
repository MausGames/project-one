///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SPECIALEFFECTS_H_
#define _P1_GUARD_SPECIALEFFECTS_H_

// TODO 5: glowing waves
// TODO 3: particles should not overdraw outlines (at least color-particles)
// TODO 3: make lightning owner-sticky with position-offset
// TODO 3: don't invoke special-effects out of view (though consider effect-radius)
// TODO 3: adjust rumble to be not toooo strong (and not be annoying)
// TODO 4: remove effect_energy_ring_program if not required anymore (+file +define)
// TODO 4: think about merging *Color and *Dark functions, as they are mostly identical
// TODO 3: sinus shake
// TODO 3: discard every X particle (create min 1) on low quality ?
// TODO 1: separate particles for background rendering (automatic selection, based on start-position ? would be easier to expose)
// TODO 3: extend particle system to allow custom move algorithms
// TODO 3: options to completely disable certain sound effects: turn, player shooting
// TODO 1: 3d-sound option should also affect looping sounds in other classes


// ****************************************************************
// special-effects definitions
#define SPECIAL_LIGHTNINGS       (32u)       // number of lightning sprites
#define SPECIAL_GUSTS            (16u)       // 
#define SPECIAL_BLASTS           (8u)        // number of energy-blasts
#define SPECIAL_EXPLOSION        (8u)        // 
#define SPECIAL_ICONS            (PLAYERS)   // 

#define SPECIAL_LIGHTNING_RESIZE (0.66f)     // 
#define SPECIAL_LIGHTNING_CUTOUT (0.5f)      // 

#define SPECIAL_DEPTH(x)         (((x).z < -1.0f) ? 1u : 0u)

#define SPECIAL_SPLASH_TINY      (25.0f), (13u)
#define SPECIAL_SPLASH_SMALL     (50.0f), (25u)
#define SPECIAL_SPLASH_BIG      (100.0f), (50u)
#define SPECIAL_BLOW_TINY        (25.0f), (13u)
#define SPECIAL_BLOW_SMALL       (50.0f), (25u)
#define SPECIAL_BLOW_BIG        (100.0f), (50u)
#define SPECIAL_CHARGE_SMALL     (30.0f), (25u)
#define SPECIAL_CHARGE_BIG       (50.0f), (50u)
#define SPECIAL_WHIRL_SMALL      (20.0f), (20u)
#define SPECIAL_WHIRL_BIG        (35.0f), (40u)
#define SPECIAL_EXPLOSION_SMALL  (10.0f), (40u)
#define SPECIAL_EXPLOSION_BIG    (20.0f), (80u)
#define SPECIAL_LIGHTNING_SMALL  (12.0f)
#define SPECIAL_LIGHTNING_BIG    (27.0f)
#define SPECIAL_BLAST_SMALL       (2.0f), (3.5f)
#define SPECIAL_BLAST_BIG         (4.0f), (3.5f)
#define SPECIAL_RUMBLE_DEFAULT    (0.4f), (120u)
#define SPECIAL_SHAKE_TINY        (0.4f)
#define SPECIAL_SHAKE_SMALL       (0.6f)
#define SPECIAL_SHAKE_BIG         (1.2f)

#define SPECIAL_RELATIVE            (coreVector3(0.0f,0.0f,0.0f))
#define SPECIAL_SOUND_MEDAL(x)      (eSoundEffect(SOUND_MEDAL_BRONZE + ((x) - MEDAL_BRONZE)))
#define SPECIAL_SOUND_PROGRESS(x,y) (LERP(0.7f, 1.3f, STEP(1.0f, I_TO_F(y), I_TO_F(x))))

enum eSoundEffect : coreUint8
{
    SOUND_PLAYER_EXPLOSION,
    SOUND_PLAYER_FEEL,
    SOUND_PLAYER_TURN,
    SOUND_PLAYER_INTERRUPT,
    SOUND_PLAYER_REPAIR,

    SOUND_ENEMY_EXPLOSION_01,
    SOUND_ENEMY_EXPLOSION_02,
    SOUND_ENEMY_EXPLOSION_03,
    SOUND_ENEMY_EXPLOSION_04,
    SOUND_ENEMY_EXPLOSION_05,
    SOUND_ENEMY_EXPLOSION_06,
    SOUND_ENEMY_EXPLOSION_07,
    SOUND_ENEMY_EXPLOSION_08,
    SOUND_ENEMY_EXPLOSION_09,
    SOUND_ENEMY_EXPLOSION_10,
    SOUND_ENEMY_EXPLOSION_11,

    SOUND_WEAPON_RAY,
    SOUND_WEAPON_ENEMY,

    SOUND_BULLET_HIT,
    SOUND_BULLET_REFLECT,

    SOUND_SHIELD_HIT,
    SOUND_SHIELD_DESTROY,

    SOUND_MEDAL_BRONZE,
    SOUND_MEDAL_SILVER,
    SOUND_MEDAL_GOLD,
    SOUND_MEDAL_PLATINUM,
    SOUND_MEDAL_DARK,

    SOUND_BADGE,

    SOUND_HELPER,

    SOUND_FRAGMENT_HELPER,
    SOUND_FRAGMENT_APPEAR,
    SOUND_FRAGMENT_COLLECT,

    SOUND_ITEM_COLLECT,

    SOUND_SUMMARY_TEXT,
    SOUND_SUMMARY_SCORE,
    SOUND_SUMMARY_MEDAL,
    SOUND_SUMMARY_PERFECT,
    SOUND_SUMMARY_RECORD,

    SOUND_CONTINUE_TICK,
    SOUND_CONTINUE_ACCEPT,

    SOUND_MENU_START,
    SOUND_MENU_MSGBOX_SHOW,
    SOUND_MENU_MSGBOX_YES,
    SOUND_MENU_MSGBOX_NO,
    SOUND_MENU_BUTTON_PRESS,
    SOUND_MENU_SWITCH_ENABLED,
    SOUND_MENU_SWITCH_DISABLED,
    SOUND_MENU_CHANGE_BUTTON,
    SOUND_MENU_CHANGE_TAB,
    SOUND_MENU_CHANGE_LINE,
    SOUND_MENU_SCROLL,
    SOUND_MENU_SUB_IN,
    SOUND_MENU_SUB_OUT,

    SOUND_EFFECT_DUST,
    SOUND_EFFECT_ERROR,
    SOUND_EFFECT_FIRE_START,
    SOUND_EFFECT_SHAKE,   // bomb, laser
    SOUND_EFFECT_SWORD_01,    
    SOUND_EFFECT_SWORD_02,    
    SOUND_EFFECT_WOOSH,

    SOUND_PLACEHOLDER,

    SOUND_MAX
};


// ****************************************************************
// special-effects class
class cSpecialEffects final
{
private:
    // 
    struct cSoundData final
    {
        coreVector3 vPosition;   // 
        coreUint16  iCount;      // 
    };


private:
    coreParticleSystem m_aParticleColor[2];                 // color particle system
    coreParticleSystem m_aParticleDark [2];                 // dark particle system
    coreParticleSystem m_aParticleSmoke[2];                 // smoke particle system
    coreParticleSystem m_aParticleFire [2];                 // fire particle system
    coreParticleSystem m_aParticleDot  [2];                 // 
    // TODO 1: leviathan laser wird nicht gut von bottom partikel gezeichnet, vielleicht deaktivierbar machen ?

    coreObject3D  m_aLightning      [SPECIAL_LIGHTNINGS];   // 
    coreObject3D* m_apLightningOwner[SPECIAL_LIGHTNINGS];   // owner of a lightning sprite (to move together)
    coreBatchList m_LightningList;                          // 
    coreUintW     m_iCurLightning;                          // 

    coreObject3D  m_aGust     [SPECIAL_GUSTS];              // 
    coreFlow      m_afGustTime[SPECIAL_GUSTS];              // 
    coreFloat     m_afGustSide[SPECIAL_GUSTS];              // 
    coreFloat     m_fGustAngle;                             // 
    coreFlow      m_fGustSpawn;                             // 
    coreBatchList m_GustList;                               // 
    coreUintW     m_iCurGust;                               // 

    coreObject3D m_aBlast[SPECIAL_BLASTS];                  // 
    coreModelPtr m_apBlastModel[3];                         // 
    coreUintW    m_iCurBlast;                               // 

    coreObject3D m_aExplosionBody[SPECIAL_EXPLOSION];       // 
    coreObject3D m_aExplosionWave[SPECIAL_EXPLOSION];       // 
    coreUintW    m_iCurExplosion;                           // 

    coreSoundPtr m_apSound[SOUND_MAX];                      // 
    coreUint64   m_iSoundGuard;                             // (to reduce multiple same sound-effects within one frame) 
    coreList<cSoundData> m_aSoundData;

    coreTimer m_ShakeTimer;                                 // 
    coreFloat m_fShakeStrength;                             // current shake strength (decreasing)
    coreUint8 m_iShakeCount;                                // 

    coreFloat m_fFreezeTime;                                // 
    coreFloat m_fSlowTime;                                  // 
    coreFloat m_fSlowStrength;                              // 

    cIcon m_aIcon[SPECIAL_ICONS];                           // 

    coreUint16 m_iEffectFrame;                              // 
    coreUint8  m_iEffectCount;                              // 
    coreUint8  m_iBreakupCount;                             // 

    coreBool m_bActive;                                     // 


public:
    cSpecialEffects()noexcept;

    DISABLE_COPY(cSpecialEffects)

    // render and move special-effects
    void Render();
    void RenderBottom();
    void Move();
    void MoveAlways();

    // 
    void Update();

    // 
    inline const coreBool& IsActive()const {return m_bActive;}
    // EXPOSE particle classes for own effects at certain locations

    // create centered particle splash
    void CreateSplashColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor, const coreBool bDeep = false, const coreBool bLock = false);
    void CreateSplashDark (const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum,                           const coreBool bDeep = false, const coreBool bLock = false);
    void CreateSplashSmoke(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);
    void CreateSplashFire (const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);
    void CreateSplashDot  (const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);

    // create directional particle blow
    void CreateBlowColor(const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);
    void CreateBlowDark (const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum);

    // 
    void CreateChargeColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);
    void CreateChargeDark (const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum);

    // 
    void CreateWhirlColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor);
    void CreateWhirlDark (const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum);

    // 
    void CreateBreakupColor(const coreObject3D* pObject, const coreFloat fScale, const coreUintW iStep, const coreVector3 vColor);
    void CreateBreakupDark (const coreObject3D* pObject, const coreFloat fScale, const coreUintW iStep);

    // 
    coreFloat CreateLightning(const coreVector2 vPosFrom, const coreVector2 vPosTo,                              const coreFloat fWidth, const coreVector3 vColor, const coreVector2 vTexSizeFactor, const coreFloat fTexOffset);
    void      CreateLightning(coreObject3D*     pOwner,   const coreVector2 vDirection, const coreFloat fLength, const coreFloat fWidth, const coreVector3 vColor, const coreVector2 vTexSizeFactor, const coreFloat fTexOffset);

    // 
    void CreateGust(const coreFloat fFrequency, const coreFloat fAngle);

    // 
    void CreateBlastSphere(const coreVector3 vPosition,                               const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor);
    void CreateBlastCube  (const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor);
    void CreateBlastTetra (const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor);

    // 
    void CreateExplosion(const coreVector3 vPosition);

    // 
    void PlaySound(const coreVector3 vPosition, const coreFloat fVolume, const coreFloat fPitch, const eSoundEffect eSoundIndex);
    void StopSound(const eSoundEffect eSoundIndex);

    // 
    void RumblePlayer(const cPlayer* pPlayer, const coreFloat fStrength, const coreUint32 iLengthMs);

    // 
    void ShakeScreen(const coreFloat fStrength);
    inline const coreFloat& GetShakeStrength()const {return m_fShakeStrength;}

    // 
    void FreezeScreen(const coreFloat fTime);
    inline const coreFloat& GetFreezeTime()const {return m_fFreezeTime;}

    // 
    void SlowScreen(const coreFloat fTime);
    inline const coreFloat& GetSlowTime    ()const {return m_fSlowTime;}
    inline const coreFloat& GetSlowStrength()const {return m_fSlowStrength;}

    // 
    inline const coreTexturePtr& GetIconTexture(const coreUintW iIndex) {ASSERT(iIndex < SPECIAL_ICONS) return m_aIcon[iIndex].GetFrameBuffer()->GetColorTarget(0u).pTexture;}

    // 
    void ClearAll();

    // 
    void MacroExplosionColorSmall        (const coreVector3 vPosition, const coreVector3 vColor);
    void MacroExplosionColorBig          (const coreVector3 vPosition, const coreVector3 vColor);
    void MacroExplosionDarkSmall         (const coreVector3 vPosition);
    void MacroExplosionDarkBig           (const coreVector3 vPosition);
    void MacroExplosionPhysicalColorSmall(const coreVector3 vPosition, const coreVector3 vColor);
    void MacroExplosionPhysicalColorBig  (const coreVector3 vPosition, const coreVector3 vColor);
    void MacroExplosionPhysicalDarkSmall (const coreVector3 vPosition);
    void MacroExplosionPhysicalDarkBig   (const coreVector3 vPosition);
    void MacroEruptionColorSmall         (const coreVector3 vPosition, const coreVector2 vDirection, const coreVector3 vColor);
    void MacroEruptionColorBig           (const coreVector3 vPosition, const coreVector2 vDirection, const coreVector3 vColor);
    void MacroEruptionDarkSmall          (const coreVector3 vPosition, const coreVector2 vDirection);
    void MacroEruptionDarkBig            (const coreVector3 vPosition, const coreVector2 vDirection);
    void MacroDestructionColor           (const coreObject3D* pObject, const coreVector3 vColor);
    void MacroDestructionDark            (const coreObject3D* pObject);


private:
    // 
    coreFloat   __GetEffectBase (const coreBool bLock);
    coreVector2 __GetBreakupSide();
};


#endif // _P1_GUARD_SPECIALEFFECTS_H_