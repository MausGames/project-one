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
cCombatText::cCombatText()noexcept
: m_afTimer   {}
, m_iCurLabel (0u)
{
    // create label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        m_aLabel[i].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // render active label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        if(m_afTimer[i]) m_aLabel[i].Render();
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // move active label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        cGuiLabel& oLabel = m_aLabel [i];
        coreFlow&  fTimer = m_afTimer[i];
        if(!fTimer) continue;

        // update animation timer
        fTimer.UpdateMax(-0.8f, 0.0f);

        // update label object
        oLabel.SetPosition(coreVector2(0.0f, 0.03f * (1.0f - fTimer)));
        oLabel.SetAlpha   (MIN(fTimer * 5.0f, 1.0f));
        oLabel.Move();
    }
}


// ****************************************************************
// add new active label object
void cCombatText::AddText(const coreChar* pcText, const coreVector3& vPosition, const coreVector3& vColor)
{
    ASSERT(pcText)

    // 
    if(++m_iCurLabel >= COMBAT_LABELS) m_iCurLabel = 0u;
    cGuiLabel& oLabel = m_aLabel [m_iCurLabel];
    coreFlow&  fTimer = m_afTimer[m_iCurLabel];

    // init label object
    oLabel.SetText  (pcText);
    oLabel.SetCenter(MapToAxis(g_pForeground->Project2D(vPosition), g_vHudDirection));
    oLabel.SetColor4(coreVector4(vColor, 0.0f));

    // start animation timer
    ASSERT(!fTimer)
    fTimer = 1.0f;
}


// ****************************************************************
// 
void cCombatText::UpdateLayout()
{
    // TODO 1: rotate position if game rotates
    // TODO 1: rotate direction (without position) if interface rotates
}


// ****************************************************************
// reset the combat text
void cCombatText::Reset()
{
    // stop all animation timers
    std::memset(m_afTimer, 0, sizeof(m_afTimer));
}