///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIELD_H_
#define _P1_GUARD_SHIELD_H_


// ****************************************************************
// shield definitions
#define SHIELD_SHIELDS (64u)   // 


// ****************************************************************
// shield manager class
class cShieldManager final
{
private:
    coreObject3D  m_aShield[SHIELD_SHIELDS];   // 
    coreBatchList m_ShieldList;                // 

    cEnemy*  m_apOwner  [SHIELD_SHIELDS];      // 
    coreFlow m_afExtent [SHIELD_SHIELDS];      // 

    coreFlow m_fAnimation;                     // 


public:
    cShieldManager()noexcept;
    ~cShieldManager();

    DISABLE_COPY(cShieldManager)

    // render and move the shield manager
    void Render();
    void Move();

    // 
    void ClearShields(const coreBool bAnimated);

    // 
    void BindEnemy  (cEnemy* pEnemy);
    void UnbindEnemy(cEnemy* pEnemy);
};


#endif // _P1_GUARD_SHIELD_H_