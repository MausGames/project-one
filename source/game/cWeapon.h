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


// ****************************************************************
// weapon interface
class INTERFACE cWeapon
{
protected:
    coreTimer    m_CooldownTimer;   // controls the cooldown between two successive shots
    coreSoundPtr m_pShootSound;     // shooting sound-effect

    cPlayer*  m_pOwner;             // associated owner of the weapon (and bullets)
    coreUint8 m_iLevel;             // current weapon level

    coreBool m_bLastStatus;         // last shoot status (to determine trigger and release)


public:
    constexpr_func cWeapon()noexcept;
    virtual ~cWeapon() {}

    DISABLE_COPY(cWeapon)
    ENABLE_ID

    // update the weapon
    coreBool Update(const coreBool& bShootStatus);

    // set object properties
    inline void SetOwner(cPlayer*         pOwner) {m_pOwner = pOwner;}
    inline void SetLevel(const coreUint8& iLevel) {m_iLevel = iLevel;}

    // get object properties
    inline cPlayer*         GetOwner()const {return m_pOwner;}
    inline const coreUint8& GetLevel()const {return m_iLevel;}


private:
    // own routines for derived classes
    virtual void __TriggerOwn() {}
    virtual void __ReleaseOwn() {}
    virtual void __ShootOwn  () {}
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
public:
    cRayWeapon()noexcept;

    DISABLE_COPY(cRayWeapon)
    ASSIGN_ID(1, "Laser Gun")


private:
    //shoot with the ray weapon
    void __ShootOwn()override;
};


// ****************************************************************
// pulse weapon class
class cPulseWeapon final : public cWeapon
{
private:
    coreUint8 m_iCharge;   // 
    coreBool  m_bSide;     // 


public:
    cPulseWeapon()noexcept;

    DISABLE_COPY(cPulseWeapon)
    ASSIGN_ID(2, "Pulse Cannon")


private:
    //shoot with the pulse weapon
    void __ShootOwn()override;
};


// ****************************************************************
// wave weapon class
class cWaveWeapon final : public cWeapon
{
public:
    cWaveWeapon()noexcept {}

    DISABLE_COPY(cWaveWeapon)
    ASSIGN_ID(3, "Wave Mortar")
};


// ****************************************************************
// tesla weapon class
class cTeslaWeapon final : public cWeapon
{
public:
    cTeslaWeapon()noexcept {}

    DISABLE_COPY(cTeslaWeapon)
    ASSIGN_ID(4, "Tesla Rifle")
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
: m_CooldownTimer (coreTimer(1.0f, 1.0f, 1u))
, m_pOwner        (NULL)
, m_iLevel        (0u)
, m_bLastStatus   (false)
{
}


#endif // _P1_GUARD_WEAPON_H_