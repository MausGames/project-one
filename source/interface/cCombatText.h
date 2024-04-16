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
// TODO: text should be moved into screen if outside (in realtime, due to labels moving each other)
// TODO: badge text should also move with other text
// TODO: moving text needs to stay sharp, either adjust engine non-rectified text, or increase size and scale down


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS (16u)   // number of label objects


// ****************************************************************
// combat text class
class cCombatText final
{
private:
    cGuiLabel m_aLabel [COMBAT_LABELS];   // label objects to display combat text
    coreFlow  m_afTimer[COMBAT_LABELS];   // animation timers
    coreUint8 m_aiType [COMBAT_LABELS];   // 

    coreUintW m_iCurLabel;                // current label object

    cGuiObject m_BadgeIcon;               // 
    cGuiLabel  m_BadgeLabel;              // 
    coreFlow   m_fBadgeTimer;             // 


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // add new active label object
    void AddText (const coreChar* pcText, const coreVector3& vPosition, const coreVector3& vColor, const coreUint8 iType);
    void AddScore(const coreInt32 iValue, const coreVector3& vPosition);

    // 
    void AddBadge(const coreUint32 iValue, const coreVector3& vPosition);

    // 
    void UpdateLayout();

    // reset the combat text
    void Reset();
};


#endif // _P1_GUARD_COMBATTEXT_H_