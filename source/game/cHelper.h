///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_HELPER_H_
#define _P1_GUARD_HELPER_H_

// TODO: make sure positions are reset to (1000.0f,1000.0f) between stages, not not cause accidental popping


// ****************************************************************
// helper definitions
enum eHelperStatus : coreUint8
{
    HELPER_STATUS_DEAD = 0x01u   // completely removed from the game
};


// ****************************************************************
// helper entity
class cHelper final : public cShip
{
private:
    coreUint8 m_iElement;              // 

    static coreFlow    s_fAnimation;   // 
    static coreVector2 s_vDirection;   // 


public:
    cHelper()noexcept;
    ~cHelper()final;

    DISABLE_COPY(cHelper)

    // configure the helper
    void Configure(const coreUint8 iElement);

    // render and move the helper
    void Render()final;
    void Move  ()final;

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // 
    static void GlobalUpdate();
    static void GlobalReset();
};


#endif // _P1_GUARD_HELPER_H_