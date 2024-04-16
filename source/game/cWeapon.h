///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WEAPON_H_
#define _P1_GUARD_WEAPON_H_

// TODO 3: weapons may have to share their cooldown (not relevant on single-weapon setup)
// TODO 1: sound and visual effect for charging pulse


// ****************************************************************
// weapon definitions
#define WEAPON_MODES         (1u)     // 
#define WEAPON_PULSE_CHARGE  (4.0f)   // 
#define WEAPON_TESLA_TARGETS (1u)     // 


// ****************************************************************
// weapon interface
class INTERFACE cWeapon
{
protected:
    coreTimer m_CooldownTimer;    // controls the cooldown between two successive primary shots
    coreUint8 m_iLastStatus;      // last shoot status (to determine trigger and release)

    coreUint8 m_iBurst;           // 

    cPlayer* m_pOwner;            // associated owner of the weapon (and bullets)


public:
    constexpr cWeapon()noexcept;
    virtual ~cWeapon() = default;

    DISABLE_COPY(cWeapon)
    ENABLE_ID

    // update the weapon
    coreBool Update(const coreUint8 iShootStatus, const coreFloat fShootSpeed);

    // 
    void Render();
    void RenderAfter();
    void Move();

    // 
    void Prefetch();

    // set object properties
    inline void SetCooldownTime(const coreFloat fTime) {m_CooldownTimer.SetValue(fTime);}
    inline void SetOwner       (cPlayer* pOwner)       {m_pOwner = pOwner;}

    // get object properties
    inline  coreBool         GetCooldown     ()const {return m_CooldownTimer.GetStatus();}
    inline  coreFloat        GetCooldownTime ()const {return m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL);}
    inline  coreFloat        GetCooldownSpeed()const {return m_CooldownTimer.GetSpeed();}
    inline  const coreUint8& GetLastStatus   ()const {return m_iLastStatus;}
    inline  const coreUint8& GetBurst        ()const {return m_iBurst;}
    inline  cPlayer*         GetOwner        ()const {return m_pOwner;}
    virtual coreVector3      GetColorEnergy  ()const {return coreVector3(0.5f,0.5f,0.5f);}
    virtual coreVector3      GetColorShip    ()const {return coreVector3(0.5f,0.5f,0.5f);}
    virtual coreUint8        GetElement      ()const {return ELEMENT_NEUTRAL;}


protected:
    // 
    coreBool    _IsOwnerDarkShading()const;
    coreBool    _IsOwnerRainbow    ()const;
    coreVector3 _GetEnergyColor    ()const;
    template <typename T> FORCE_INLINE T* _MakeWhite(T* OUTPUT ptBullet) {this->_TrackBullet(); if(this->_IsOwnerRainbow()) ptBullet->SetColor3(this->_GetEnergyColor()); else ptBullet->MakeWhite(); return ptBullet;}

    // 
    void _TrackBullet();


private:
    // own routines for derived classes
    virtual void __UpdateOwn     (const coreUint8 iShootStatus, const coreFloat fShootSpeed) {}
    virtual void __TriggerOwn    (const coreUint8 iMode)                                     {}
    virtual void __ReleaseOwn    (const coreUint8 iMode)                                     {}
    virtual void __ShootOwn      ()                                                          {}
    virtual void __RenderOwn     ()                                                          {}
    virtual void __RenderAfterOwn()                                                          {}
    virtual void __MoveOwn       ()                                                          {}
    virtual void __PrefetchOwn   ()                                                          {}
};


// ****************************************************************
// empty weapon class
class cNoWeapon final : public cWeapon
{
public:
    cNoWeapon() = default;

    DISABLE_COPY(cNoWeapon)
    ASSIGN_ID(0, "Nothing")
};


// ****************************************************************
// ray weapon class
class cRayWeapon final : public cWeapon
{
private:
    coreObject3D m_aMuzzle[2];     // 
    coreFlow     m_fMuzzleTime;    // 
    coreFlow     m_fMuzzleAnim;    // 
    coreFlow     m_fMuzzleScale;   // 


public:
    cRayWeapon()noexcept;
    ~cRayWeapon()final;

    DISABLE_COPY(cRayWeapon)
    ASSIGN_ID(1, "Laser Gun")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_YELLOW;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_YELLOW;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_YELLOW;}


private:
    // execute own routines
    void __UpdateOwn  (const coreUint8 iShootStatus, const coreFloat fShootSpeed)final;
    void __TriggerOwn (const coreUint8 iMode)final;
    void __ReleaseOwn (const coreUint8 iMode)final;
    void __ShootOwn   ()final;
    void __RenderOwn  ()final;
    void __MoveOwn    ()final;
    void __PrefetchOwn()final;
};


// ****************************************************************
// pulse weapon class
class cPulseWeapon final : public cWeapon
{
private:
    coreFlow  m_fCharge;           // 
    coreUint8 m_iShotType;         // 

    coreObject3D m_Charge;         // 
    coreObject3D m_ChargeWave;     // 

    coreObject3D m_Muzzle;         // 
    coreFlow     m_fMuzzleTime;    // 
    coreFlow     m_fMuzzleAnim;    // 
    coreFlow     m_fMuzzleScale;   // 

    coreFlow m_fWave;              // 
    coreFlow m_fAnimation;         // 


public:
    cPulseWeapon()noexcept;
    ~cPulseWeapon()final;

    DISABLE_COPY(cPulseWeapon)
    ASSIGN_ID(2, "Pulse Cannon")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_PURPLE;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_PURPLE;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_PURPLE;}


private:
    // execute own routines
    void __UpdateOwn     (const coreUint8 iShootStatus, const coreFloat fShootSpeed)final;
    void __ReleaseOwn    (const coreUint8 iMode)final;
    void __ShootOwn      ()final;
    void __RenderOwn     ()final;
    void __RenderAfterOwn()final;
    void __MoveOwn       ()final;
    void __PrefetchOwn   ()final;
};


// ****************************************************************
// wave weapon class
class cWaveWeapon final : public cWeapon
{
public:
    cWaveWeapon()noexcept;

    DISABLE_COPY(cWaveWeapon)
    ASSIGN_ID(3, "Wave Mortar")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_GREEN;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_GREEN;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_GREEN;}


private:
    // execute own routines
    void __ReleaseOwn (const coreUint8 iMode)final;
    void __ShootOwn   ()final;
    void __PrefetchOwn()final;
};


// ****************************************************************
// tesla weapon class
class cTeslaWeapon final : public cWeapon
{
private:
    coreUint8 m_iStrikeType;     // 
    coreFloat m_fStrikeOffset;   // 


public:
    cTeslaWeapon()noexcept;

    DISABLE_COPY(cTeslaWeapon)
    ASSIGN_ID(4, "Tesla Rifle")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_BLUE;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_BLUE;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_BLUE;}


private:
    // execute own routines
    void __TriggerOwn (const coreUint8 iMode)final;
    void __ReleaseOwn (const coreUint8 iMode)final;
    void __ShootOwn   ()final;
    void __PrefetchOwn()final;
};


// ****************************************************************
// antimatter weapon class
class cAntiWeapon final : public cWeapon
{
public:
    cAntiWeapon() = default;

    DISABLE_COPY(cAntiWeapon)
    ASSIGN_ID(5, "Antimatter")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_RED;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_RED;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_RED;}
};


// ****************************************************************
// enemy weapon class
class cEnemyWeapon final : public cWeapon
{
public:
    cEnemyWeapon()noexcept;

    DISABLE_COPY(cEnemyWeapon)
    ASSIGN_ID(10, "Enemy Weapon")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_MAGENTA;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_MAGENTA;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_MAGENTA;}


private:
    // execute own routines
    void __ShootOwn   ()final;
    void __PrefetchOwn()final;
};


// ****************************************************************
// final weapon class
class cFinalWeapon final : public cWeapon
{
public:
    cFinalWeapon()noexcept;

    DISABLE_COPY(cFinalWeapon)
    ASSIGN_ID(11, "Final Weapon")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_WHITE;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_WHITE;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_WHITE;}


private:
    // execute own routines
    void __ShootOwn   ()final;
    void __PrefetchOwn()final;
};


// ****************************************************************
// constructor
constexpr cWeapon::cWeapon()noexcept
: m_CooldownTimer (coreTimer(1.0f, 1.0f, 0u))
, m_iLastStatus   (0u)
, m_iBurst        (0u)
, m_pOwner        (NULL)
{
}


#endif // _P1_GUARD_WEAPON_H_