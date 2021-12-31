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


// ****************************************************************
// shield definitions
#define SHIELD_SHIELDS (16u)   // 


// ****************************************************************
// shield manager class
class cShieldManager final
{
private:
    coreObject3D  m_aShield[SHIELD_SHIELDS];   // 
    coreBatchList m_ShieldList;                // 

    cEnemy*   m_apOwner  [SHIELD_SHIELDS];     // 
    coreUint8 m_aiElement[SHIELD_SHIELDS];     // 
    coreInt16 m_aiHealth [SHIELD_SHIELDS];     // 
    coreFlow  m_afExtent [SHIELD_SHIELDS];     // 

    coreFlow m_fAnimation;                     // 


public:
    cShieldManager()noexcept;
    ~cShieldManager();

    DISABLE_COPY(cShieldManager)

    // render and move the shield manager
    void Render();
    void Move();

    // 
    void AbsorbDamage(cEnemy* pEnemy, coreInt32* OUTPUT piDamage, const coreUint8 iElement);

    // 
    void ClearShields(const coreBool bAnimated);

    // 
    void BindEnemy  (cEnemy* pEnemy, const coreUint8 iElement, const coreInt16 iHealth = 0);
    void UnbindEnemy(cEnemy* pEnemy);
};


#endif // _P1_GUARD_SHIELD_H_