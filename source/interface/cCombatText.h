//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_COMBATTEXT_H_
#define _P1_GUARD_COMBATTEXT_H_

// TODO: maybe get rid of the smooth text-overlap resolve
// TODO: allow localized text with standard numbers


// ****************************************************************
// combat text definitions
#define COMBAT_LABELS (8u)   // number of label objects


// ****************************************************************
// combat text class
class cCombatText final
{
private:
    // text data structure
    struct sData final
    {
        std::string sText;       // text to display
        coreVector2 vPosition;   // screen position (center)
        coreUint32  iColor;      // packed RGBA color-value
    };


private:
    coreLabel m_aLabel [COMBAT_LABELS];   // label objects to display combat text
    coreFloat m_afTimer[COMBAT_LABELS];   // animation timers

    coreUintW m_iCurLabel;                // current label object

    std::list<sData> m_aData;             // text data list


public:
    cCombatText()noexcept;

    DISABLE_COPY(cCombatText)

    // render and move the combat text
    void Render();
    void Move();

    // add new combat text
    void        AddText  (const coreChar*  pcText, const coreVector3& vPosition, const coreVector3& vColor);
    inline void AddDamage(const coreInt32  iValue, const coreVector3& vPosition) {if(g_CurConfig.Game.Combat.iDamage && iValue) this->AddText(PRINT("%d",  iValue), vPosition, COLOR_MENU_RED);}
    inline void AddChain (const coreUint32 iValue, const coreVector3& vPosition) {if(g_CurConfig.Game.Combat.iChain  && iValue) this->AddText(PRINT("+%u", iValue), vPosition, COLOR_MENU_BLUE);}
    inline void AddCombo (const coreUint32 iValue, const coreVector3& vPosition) {if(g_CurConfig.Game.Combat.iCombo  && iValue) this->AddText(PRINT("x%u", iValue), vPosition, COLOR_MENU_PURPLE);}

     // reset all active label objects
    void Reset();
};


#endif // _P1_GUARD_COMBATTEXT_H_