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

// TODO 3: allow localized text with standard numbers
// TODO 5: different labels and animations (size, solid background, 000000 -> 123456, ZSDK -> TEST)
// TODO 1: text should be moved into screen if outside (in realtime, due to labels moving each other)
// TODO 1: badge text should also move with other text, or be above


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS_SMALL (16u)                                       // 
#define COMBAT_LABELS_BIG   (4u)                                        // 
#define COMBAT_LABELS       (COMBAT_LABELS_SMALL + COMBAT_LABELS_BIG)   // total number of label objects


// ****************************************************************
// combat text class
class cCombatText final
{
private:
    cGuiLabel m_aLabel [COMBAT_LABELS];    // label objects to display combat text
    coreFlow  m_afTimer[COMBAT_LABELS];    // animation timers
    coreUint8 m_aiType [COMBAT_LABELS];    // 

    cGuiLabel* m_apOrder[COMBAT_LABELS];   // 
    coreUint8  m_iOrderNum;                // 

    cGuiObject m_BadgeIcon;                // 
    cGuiLabel  m_BadgeLabel;               // 
    coreFlow   m_fBadgeTimer;              // 


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // 
    void AddScore(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig);
    void AddExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig);
    void AddChain(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig);
    void AddBadge(const coreUint32 iValue, const coreVector3 vPosition);

    // 
    void UpdateLayout();

    // reset the combat text
    void Reset();


private:
    // 
    void __AddLabel(const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor, const coreUint8 iType);

    // 
    void __AddOrder   (cGuiLabel* pLabel);
    void __RemoveOrder(cGuiLabel* pLabel);
};


#endif // _P1_GUARD_COMBATTEXT_H_