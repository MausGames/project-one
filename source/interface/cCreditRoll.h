///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CREDITROLL_H_
#define _P1_GUARD_CREDITROLL_H_

// TODO 5: rotate entries and make them rotatable


// ****************************************************************
// credit roll definitions
#define CREDIT_ENTRIES (ARRAY_SIZE(g_aapcCreditEntry))   // 

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_aapcCreditEntry[][2] =
{
    {"Horst Fuchs", "Programmer, Game Designer"}
};


// ****************************************************************
// credit roll class
class cCreditRoll final : public cGuiObject
{
private:
    coreViewBox m_ViewBox;                      // 

    cGuiLabel m_aName       [CREDIT_ENTRIES];   // 
    cGuiLabel m_aDescription[CREDIT_ENTRIES];   // 

    coreFlow m_fOffset;                         // 


public:
    cCreditRoll()noexcept;

    DISABLE_COPY(cCreditRoll)

    // render and move the credit roll
    void Render()final;
    void Move  ()final;

    // 
    void Start();
};


#endif // _P1_GUARD_CREDITROLL_H_