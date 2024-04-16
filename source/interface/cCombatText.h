///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_COMBATTEXT_H_
#define _P1_GUARD_COMBATTEXT_H_

// TODO 3: allow localized text with standard numbers
// TODO 5: different labels and animations (size, solid background, 000000 -> 123456, ZSDK -> TEST)
// TODO 4: calling functions to display text is not consistent, chain is displayed by table, but (regular) score, extra and badge individually alongside adding score to table
// TODO 2: use oldest active label instead of nothing, if all labels are busy (or dynamically allocated fallback)
// TODO 3: RestrictCenter should also use size
// TODO 4: use index instead of pointer for order-list ?
// TODO 2: also invert with mirror mode (like interface, maybe move outside ?) (even more, like summary and continue menu) (should it be a 3rd option ? Off, Light, Full)


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS_SMALL (32u)                                       // 
#define COMBAT_LABELS_BIG   (4u)                                        // 
#define COMBAT_LABELS       (COMBAT_LABELS_SMALL + COMBAT_LABELS_BIG)   // total number of label objects
#define COMBAT_BORDER       (0.47f)                                     // 


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
    
    coreUint8 m_iLastLabel;


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // 
    void DrawScore(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig);
    void DrawExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig);
    void DrawChain(const coreUint32 iValue, const coreVector3 vPosition);
    void DrawShift(const coreUint32 iValue, const coreVector3 vPosition);
    void DrawBadge(const coreUint32 iValue, const coreVector3 vPosition);

    // 
    void UpdateLayout();

    // reset the combat text
    void Reset();


private:
    // 
    void __DrawLabel(const coreChar* pcText, const coreVector3 vPosition, const coreBool bBig, const coreVector3 vColor, const coreUint8 iType);

    // 
    void __AddOrder   (cGuiLabel* pLabel);
    void __RemoveOrder(cGuiLabel* pLabel);

    // 
    static coreVector2 __TransformPosition(const coreVector3 vPosition);

    // 
    static coreVector2 __RestrictCenter(const coreVector2 vPosition, const coreVector2 vCenter);
};


#endif // _P1_GUARD_COMBATTEXT_H_