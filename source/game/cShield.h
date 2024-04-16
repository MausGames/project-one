///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIELD_H_
#define _P1_GUARD_SHIELD_H_

// TODO 1: bullets should reflect from shield, not from enemy (but not every invincible enemy has a shield)


// ****************************************************************
// shield definitions
#define SHIELD_SHIELDS           (32u)   // 
#define SHIELD_EFFECT_INVINCIBLE (0u)    // 
#define SHIELD_EFFECT_DAMAGING   (1u)    // 
#define SHIELD_EFFECT_BASE       (2u)    // 
#define SHIELD_EFFECTS           (3u)    // 


// ****************************************************************
// shield effect class
class cShieldEffect final
{
private:
    coreObject3D  m_aShield[SHIELD_SHIELDS];   // 
    coreBatchList m_ShieldList;                // 

    const cEnemy* m_apOwner[SHIELD_SHIELDS];   // 

    coreUintW m_iCurShield;                    // 

    coreFloat m_fScale;                        // 
    coreFloat m_fTexScale;                     // 
    coreFloat m_fTexSpeed;                     // 

    coreFlow m_fAnimation;                     // 


public:
    cShieldEffect()noexcept;
    ~cShieldEffect();

    DISABLE_COPY(cShieldEffect)

    // 
    void Construct(const coreHashString& sModelName, const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName, const coreFloat fScale, const coreFloat fTexScale, const coreVector3 vColor);

    // render and move the shield effect
    void Render();
    void Move();

    // 
    void ClearShields(const coreBool bAnimated);

    // 
    template <typename F> void ForEachShield(F&& nFunction);   // [](coreObject3D* OUTPUT pShield) -> void

    // 
    void SetActive(const coreBool bActive);

    // 
    void BindEnemy  (cEnemy* pEnemy);
    void UnbindEnemy(cEnemy* pEnemy);
};


// ****************************************************************
// shield manager class
class cShieldManager final
{
private:
    cShieldEffect m_aShieldEffect[SHIELD_EFFECTS];   // 


public:
    cShieldManager()noexcept;

    DISABLE_COPY(cShieldManager)

    // render and move the shield manager
    void Render();
    void Move();

    // 
    void ClearShields(const coreBool bAnimated);

    // 
    void SetActive(const coreBool bActive);

    // 
    inline cShieldEffect* GetEffect(const coreUintW iEffect) {ASSERT(iEffect < SHIELD_EFFECTS) return &m_aShieldEffect[iEffect];}
};


// ****************************************************************
// 
template <typename F> void cShieldEffect::ForEachShield(F&& nFunction)
{
    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        coreObject3D* pShield = &m_aShield[i];
        if(!pShield->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        nFunction(pShield);
    }
}


#endif // _P1_GUARD_SHIELD_H_