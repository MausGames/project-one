///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_GUIOBJECT_H_
#define _P1_GUARD_GUIOBJECT_H_


// ****************************************************************
// generic GUI object class
template <typename T> class cGuiObjectGen : public T
{
public:
    cGuiObjectGen()noexcept;
    virtual ~cGuiObjectGen()override = default;

    DISABLE_COPY(cGuiObjectGen)
};


// ****************************************************************
// 
using cGuiObject    = cGuiObjectGen<coreObject2D>;
using cGuiLabel     = cGuiObjectGen<coreLabel>;
using cGuiButton    = cGuiObjectGen<coreButton>;
using cGuiCheckBox  = cGuiObjectGen<coreCheckBox>;
using cGuiTextBox   = cGuiObjectGen<coreTextBox>;
using cGuiSwitchBox = cGuiObjectGen<coreSwitchBoxU8>;


// ****************************************************************
// 
template <typename T> cGuiObjectGen<T>::cGuiObjectGen()noexcept
{
    // 
    this->SetStyle(CORE_OBJECT2D_STYLE_VIEWDIR | CORE_OBJECT2D_STYLE_ALTCENTER);
}


#endif // _P1_GUARD_GUIOBJECT_H_