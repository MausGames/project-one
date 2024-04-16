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
// TODO: if possible, reuse the same text


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS (16u)//(8u)   // number of label objects


// ****************************************************************
// combat text class
class cCombatText final
{
private:
    coreLabel m_aLabel [COMBAT_LABELS];   // label objects to display combat text
    coreFlow  m_afTimer[COMBAT_LABELS];   // animation timers

    coreUintW m_iCurLabel;                // current label object

    coreObject2D m_BadgeIcon;             // 
    coreLabel    m_BadgeLabel;            // 
    coreFlow     m_fBadgeTimer;           // 


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // add new active label object
    void AddText (const coreChar* pcText, const coreVector3& vPosition, const coreVector3& vColor);
    void AddValue(const coreInt32 iValue, const coreVector3& vPosition, const coreVector3& vColor);

    // 
    void AddBadge(const coreUint32 iValue, const coreVector3& vPosition);

    // reset the combat text
    void Reset();
};


#endif // _P1_GUARD_COMBATTEXT_H_