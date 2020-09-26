///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cBridgeMenu::cBridgeMenu()noexcept
: coreMenu        (SURFACE_BRIDGE_MAX, SURFACE_BRIDGE_DEFAULT)
, m_fReturnTimer  (0.0f)
, m_iReturnTarget (0u)
, m_bReturnState  (false)
{
}


// ****************************************************************
// move the bridge menu
void cBridgeMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_BRIDGE_DEFAULT:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(CLAMP(1.0f - m_fReturnTimer, 0.0f, 1.0f));

                if(m_fReturnTimer >= 1.5f)
                {
                    // 
                    m_iStatus = 1;

                    // 
                    m_bReturnState = false;

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID,           ENVIRONMENT_MIX_FADE,    0.0f);
                    g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 0.75f, coreVector2(1.0f,0.0f));

                    // 
                    g_pPostProcessing->SetSaturationAll(1.0f);
                    g_pPostProcessing->SetValueAll     (1.0f);

                    // prevent flickering (# render function)
                    g_pGlow->Clear();
                    g_pSpecialEffects->ClearAll();
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}


// ****************************************************************
// 
void cBridgeMenu::ReturnMenu(const coreUint8 iTarget)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_fReturnTimer  = 0.0f;
    m_iReturnTarget = iTarget;
    m_bReturnState  = true;

    // 
    g_pGame->GetInterface()->SetVisible(false);
    g_pGame->GetInterface()->Reset();

    // 
    this->SetAlpha(0.0f);
    this->ChangeSurface(SURFACE_BRIDGE_DEFAULT, 0.0f);
}