///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_NEWINDICATOR_H_
#define _P1_GUARD_NEWINDICATOR_H_


// ****************************************************************
// 
class cNewIndicator final : public cGuiObject
{
private:
    cGuiLabel m_Text;     // 

    coreFlow  m_fFade;    // 
    coreUint8 m_iIndex;   // 


public:
    cNewIndicator()noexcept;

    DISABLE_COPY(cNewIndicator)

    // 
    void Render()final;
    void Move  ()final;

    // 
    void Ressolve();

    // 
    inline void SetIndex(const coreUint8 iIndex) {m_iIndex = iIndex;}
};


#endif // _P1_GUARD_NEWINDICATOR_H_