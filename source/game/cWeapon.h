//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WEAPON_H_
#define _P1_GUARD_WEAPON_H_

// TODO: weapons may have to share their cooldown, otherwise you can cheat (not relevant on single-weapon setup)


// ****************************************************************
// weapon definitions
#define WEAPON_MODES         (2u)   // 
#define WEAPON_TESLA_TARGETS (3u)   // 


// ****************************************************************
// weapon interface
class INTERFACE cWeapon
{
protected:
    coreTimer m_CooldownTimer;    // controls the cooldown between two successive primary shots
    coreUint8 m_iLastStatus;      // last shoot status (to determine trigger and release)

    cPlayer* m_pOwner;            // associated owner of the weapon (and bullets)


public:
    constexpr_func cWeapon()noexcept;
    virtual ~cWeapon() {}

    DISABLE_COPY(cWeapon)
    ENABLE_ID

    // update the weapon
    coreBool Update(const coreUint8& iShootStatus);

    // 
    void Render();
    void RenderOverlay();

    // set object properties
    inline void SetOwner(cPlayer* pOwner) {m_pOwner = pOwner;}

    // get object properties
    inline cPlayer* GetOwner()const {return m_pOwner;}


private:
    // own routines for derived classes
    virtual void __UpdateOwn       (const coreUint8& iShootStatus) {}
    virtual void __TriggerOwn      (const coreUint8& iMode)        {}
    virtual void __ReleaseOwn      (const coreUint8& iMode)        {}
    virtual void __ShootOwn        ()                              {}
    virtual void __RenderOwn       ()                              {}
    virtual void __RenderOverlayOwn()                              {}
};


// ****************************************************************
// empty weapon class
class cNoWeapon final : public cWeapon
{
public:
    cNoWeapon()noexcept {}

    DISABLE_COPY(cNoWeapon)
    ASSIGN_ID(0, "Nothing")
};


// ****************************************************************
// ray weapon class
class cRayWeapon final : public cWeapon
{
private:
    coreSoundPtr m_pBulletSound;   // 
    coreSoundPtr m_pRocketSound;   // 
    coreSoundPtr m_pMineSound;     // 


public:
    cRayWeapon()noexcept;

    DISABLE_COPY(cRayWeapon)
    ASSIGN_ID(1, "Laser Gun")


private:
    // execute own routines
    void __TriggerOwn(const coreUint8& iMode)override;
    void __ShootOwn  ()override;
};


// ****************************************************************
// pulse weapon class
class cPulseWeapon final : public cWeapon
{
private:
    coreFloat m_fCharge;           // 
    coreUint8 m_iCombo;            // 

    coreSoundPtr m_pBulletSound;   // 


public:
    cPulseWeapon()noexcept;

    DISABLE_COPY(cPulseWeapon)
    ASSIGN_ID(2, "Pulse Cannon")


private:
    // execute own routines
    void __UpdateOwn(const coreUint8& iShootStatus)override;
    void __ShootOwn ()override;
};


// ****************************************************************
// wave weapon class
class cWaveWeapon final : public cWeapon
{
private:
    coreBool m_bSide;              // 

    coreSoundPtr m_pBulletSound;   // 
    coreSoundPtr m_pChangeSound;   // 

    coreLabel m_Arrow;             // 


public:
    cWaveWeapon()noexcept;

    DISABLE_COPY(cWaveWeapon)
    ASSIGN_ID(3, "Wave Mortar")


private:
    // execute own routines
    void __TriggerOwn      (const coreUint8& iMode)override;
    void __ShootOwn        ()override;
    void __RenderOverlayOwn()override;
};


// ****************************************************************
// tesla weapon class
class cTeslaWeapon final : public cWeapon
{
private:
    coreInt8  m_iShotType;         // 
    coreUint8 m_iStrikeType;       // 
    coreFloat m_fStrikeOffset;     // 

    coreSoundPtr m_pBulletSound;   // 


public:
    cTeslaWeapon()noexcept;

    DISABLE_COPY(cTeslaWeapon)
    ASSIGN_ID(4, "Tesla Rifle")


private:
    // execute own routines
    void __TriggerOwn(const coreUint8& iMode)override;
    void __ShootOwn  ()override;
};


// ****************************************************************
// antimatter weapon class
class cAntiWeapon final : public cWeapon
{
public:
    cAntiWeapon()noexcept {}

    DISABLE_COPY(cAntiWeapon)
    ASSIGN_ID(5, "Antimatter")
};


// ****************************************************************
// constructor
constexpr_func cWeapon::cWeapon()noexcept
: m_CooldownTimer (coreTimer(1.0f, 1.0f, 0u))
, m_iLastStatus   (0u)
, m_pOwner        (NULL)
{
}


#endif // _P1_GUARD_WEAPON_H_