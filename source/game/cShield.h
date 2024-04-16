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

// TODO 1: better would be a shield which is only visible on bullet-hits (and tighter, maybe around silhouette)


// ****************************************************************
// shield definitions
#define SHIELD_EFFECT_INVINCIBLE (0u)   // 
#define SHIELD_EFFECT_DAMAGING   (1u)   // 
#define SHIELD_EFFECTS           (2u)   // 

#define SHIELD_SHIELDS (128u)   // 


// ****************************************************************
// shield effect class
class cShieldEffect final
{
private:
    coreObject3D  m_aShield[SHIELD_SHIELDS];   // 
    coreBatchList m_ShieldList;                // 

    cEnemy* m_apOwner[SHIELD_SHIELDS];         // 

    coreFlow m_fAnimation;                     // 


public:
    cShieldEffect()noexcept;
    ~cShieldEffect();

    DISABLE_COPY(cShieldEffect)

    // 
    void Construct(const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName, const coreVector3 vColor);

    // render and move the shield effect
    void Render();
    void Move();

    // 
    void ClearShields(const coreBool bAnimated);

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
    inline cShieldEffect* GetEffect(const coreUintW iEffect) {ASSERT(iEffect < SHIELD_EFFECTS) return &m_aShieldEffect[iEffect];}
};


#endif // _P1_GUARD_SHIELD_H_