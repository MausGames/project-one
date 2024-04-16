///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_COMBATTEXT_H_
#define _P1_GUARD_COMBATTEXT_H_

// TODO: allow localized text with standard numbers
// TODO: different labels and animations (size, solid background, 000000 -> 123456, ZSDK -> TEST)


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS (8u)   // number of label objects


// ****************************************************************
// combat text class
class cCombatText final
{
private:
    coreLabel m_aLabel [COMBAT_LABELS];   // label objects to display combat text
    coreFlow  m_afTimer[COMBAT_LABELS];   // animation timers

    coreUintW m_iCurLabel;                // current label object


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // add new active label object
    void        AddText (const coreChar*  pcText, const coreVector3& vPosition, const coreVector3& vColor);
    inline void AddBonus(const coreUint32 iValue, const coreVector3& vPosition) {if(iValue) this->AddText(PRINT("%u", iValue), vPosition, COLOR_MENU_BLUE);}

    // reset the combat text
    void Reset();
};


#endif // _P1_GUARD_COMBATTEXT_H_