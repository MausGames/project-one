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
    coreSoundPtr m_pShootSound;     // 

    coreByte m_iLevel;              // 
    cPlayer* m_pOwner;              // 

    bool m_bLastStatus;             // 


public:
    constexpr_func cWeapon()noexcept;
    virtual ~cWeapon() {}

    DISABLE_COPY(cWeapon)
    ENABLE_ID

    // 
    bool Update(const bool& bShootStatus);

    // set object properties
    inline void SetLevel(const coreByte& iLevel) {m_iLevel = iLevel;}
    inline void SetOwner(cPlayer*        pOwner) {m_pOwner = pOwner;}

    // get object properties
    inline const coreByte& GetLevel()const {return m_iLevel;}


private:
    // 
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
    ASSIGN_ID(0, "NoWeapon")
};


// ****************************************************************
// ray weapon class
class cRayWeapon final : public cWeapon
{
private:
    bool m_bSpread;   // 


public:
    cRayWeapon()noexcept;

    DISABLE_COPY(cRayWeapon)
    ASSIGN_ID(1, "RayWeapon")


private:
    // 
    void __ReleaseOwn()override;
    void __ShootOwn  ()override;
};


// ****************************************************************
// pulse weapon class
class cPulseWeapon final : public cWeapon
{
public:
    cPulseWeapon()noexcept {}

    DISABLE_COPY(cPulseWeapon)
    ASSIGN_ID(2, "PulseWeapon")
};


// ****************************************************************
// wave weapon class
class cWaveWeapon final : public cWeapon
{
public:
    cWaveWeapon()noexcept {}

    DISABLE_COPY(cWaveWeapon)
    ASSIGN_ID(3, "WaveWeapon")
};


// ****************************************************************
// tesla weapon class
class cTeslaWeapon final : public cWeapon
{
public:
    cTeslaWeapon()noexcept {}

    DISABLE_COPY(cTeslaWeapon)
    ASSIGN_ID(4, "TeslaWeapon")
};


// ****************************************************************
// antimatter weapon class
class cAntiWeapon final : public cWeapon
{
public:
    cAntiWeapon()noexcept {}

    DISABLE_COPY(cAntiWeapon)
    ASSIGN_ID(5, "AntiWeapon")
};


// ****************************************************************
// constructor
constexpr_func cWeapon::cWeapon()noexcept
: m_CooldownTimer (coreTimer(1.0f, 1.0f, 1))
, m_iLevel        (0)
, m_pOwner        (NULL)
, m_bLastStatus   (false)
{
}


#endif // _P1_GUARD_WEAPON_H_