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
// TODO 4: remove render functions if not required
// TODO 1: sound and visual effect for charging pulse


// ****************************************************************
// weapon definitions
#define WEAPON_MODES         (1u)   // 
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
    constexpr cWeapon()noexcept;
    virtual ~cWeapon() = default;

    DISABLE_COPY(cWeapon)
    ENABLE_ID

    // update the weapon
    coreBool Update(const coreUint8 iShootStatus, const coreFloat fShootSpeed);

    // 
    void Render();

    // set object properties
    inline void SetOwner(cPlayer* pOwner) {m_pOwner = pOwner;}

    // get object properties
    inline  coreBool    GetCooldown   ()const {return m_CooldownTimer.GetStatus();}
    inline  cPlayer*    GetOwner      ()const {return m_pOwner;}
    virtual coreVector3 GetColorEnergy()const {return coreVector3(0.5f,0.5f,0.5f);}
    virtual coreVector3 GetColorShip  ()const {return coreVector3(0.5f,0.5f,0.5f);}
    virtual coreUint8   GetElement    ()const {return ELEMENT_NEUTRAL;}


protected:
    // 
    coreBool _IsOwnerDarkShading()const;
    template <typename T> FORCE_INLINE T* _MakeWhite(T* OUTPUT ptBullet)const {if(this->_IsOwnerDarkShading()) ptBullet->MakeWhite(); return ptBullet;}


private:
    // own routines for derived classes
    virtual void __UpdateOwn (const coreUint8 iShootStatus, const coreFloat fShootSpeed) {}
    virtual void __TriggerOwn(const coreUint8 iMode)                                     {}
    virtual void __ReleaseOwn(const coreUint8 iMode)                                     {}
    virtual void __ShootOwn  ()                                                          {}
    virtual void __RenderOwn ()                                                          {}
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
    coreBool m_bBurst;             // 

    coreSoundPtr m_pBulletSound;   // 
    coreSoundPtr m_pRocketSound;   // 
    coreSoundPtr m_pMineSound;     // 


public:
    cRayWeapon()noexcept;

    DISABLE_COPY(cRayWeapon)
    ASSIGN_ID(1, "Laser Gun")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_YELLOW;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_YELLOW;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_YELLOW;}


private:
    // execute own routines
    void __TriggerOwn(const coreUint8 iMode)final;
    void __ReleaseOwn(const coreUint8 iMode)final;
    void __ShootOwn  ()final;
};


// ****************************************************************
// pulse weapon class
class cPulseWeapon final : public cWeapon
{
private:
    coreFlow m_fCharge;            // 

    coreSoundPtr m_pBulletSound;   // 


public:
    cPulseWeapon()noexcept;

    DISABLE_COPY(cPulseWeapon)
    ASSIGN_ID(2, "Pulse Cannon")

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_PURPLE;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_PURPLE;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_PURPLE;}


private:
    // execute own routines
    void __UpdateOwn(const coreUint8 iShootStatus, const coreFloat fShootSpeed)final;
    void __ShootOwn ()final;
};


// ****************************************************************
// wave weapon class
class cWaveWeapon final : public cWeapon
{
private:
    coreSoundPtr m_pBulletSound;    // 


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
    void __ShootOwn()final;
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

    // get object properties
    inline coreVector3 GetColorEnergy()const final {return COLOR_ENERGY_BLUE;}
    inline coreVector3 GetColorShip  ()const final {return COLOR_SHIP_BLUE;}
    inline coreUint8   GetElement    ()const final {return ELEMENT_BLUE;}


private:
    // execute own routines
    void __TriggerOwn(const coreUint8 iMode)final;
    void __ShootOwn  ()final;
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
    void __ShootOwn()final;
};


// ****************************************************************
// constructor
constexpr cWeapon::cWeapon()noexcept
: m_CooldownTimer (coreTimer(1.0f, 1.0f, 0u))
, m_iLastStatus   (0u)
, m_pOwner        (NULL)
{
}


#endif // _P1_GUARD_WEAPON_H_