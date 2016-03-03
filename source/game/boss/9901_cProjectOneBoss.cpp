//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// counter identifier


// ****************************************************************
// constructor
cProjectOneBoss::cProjectOneBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_projectone.md3");
    this->DefineModelLow ("ship_projectone.md3");


    this->DefineTexture(0u, "ship_player.png");


    // 
    this->SetSize(coreVector3(1.2f,1.2f,1.2f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY);


    g_pGlow->BindObject(this);


    this->DefineModelHigh("effect_shield.md3");
    this->DefineModelLow ("effect_shield.md3");
    this->DefineTexture  (0u, "effect_particle_128.png");
    this->DefineProgram  ("effect_shield_program");
    this->SetSize(coreVector3(5.0f,5.0f,5.0f));
    this->SetBaseColor(COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cProjectOneBoss::__ResurrectOwn()
{

}


// ****************************************************************
// 
void cProjectOneBoss::__KillOwn(const coreBool bAnimated)
{

}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOwn()
{
    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            // 
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.0f), fTime);

            // 
            if(PHASE_SUB(0.85f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            // 
            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            m_iPhase = 10u;
        });
    }


}