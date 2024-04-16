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
: m_afTimer     {}
, m_iCurLabel   (0u)
, m_fBadgeTimer (0.0f)
{
    // create label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        m_aLabel[i].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);

    // 
    m_BadgeIcon.DefineTexture(0u, "menu_star.png");
    m_BadgeIcon.DefineProgram("default_2d_program");
    m_BadgeIcon.SetSize      (coreVector2( 1.0f,1.0f) * 0.06f);
    m_BadgeIcon.SetAlignment (coreVector2(-1.0f,0.0f));

    // 
    m_BadgeLabel.Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_BadgeLabel.SetAlignment(coreVector2(1.0f,0.0f));
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // 
    if(m_fBadgeTimer)
    {
        m_BadgeIcon .Render();
        m_BadgeLabel.Render();
    }

    // render active label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        if(m_afTimer[i]) m_aLabel[i].Render();
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // 
    if(m_fBadgeTimer)
    {
        // 
        m_fBadgeTimer.UpdateMax(-0.8f, 0.0f);

        // 
        m_BadgeIcon.SetPosition(coreVector2(0.0f, 0.03f * (1.0f - m_fBadgeTimer)));
        m_BadgeIcon.SetAlpha   (MIN(m_fBadgeTimer * 5.0f, 1.0f));
        m_BadgeIcon.Move();

        // 
        m_BadgeLabel.SetPosition(m_BadgeIcon.GetPosition());
        m_BadgeLabel.SetAlpha   (m_BadgeIcon.GetAlpha   ());
        m_BadgeLabel.Move();
    }

    // 
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
    }

    // move overlaying label objects away from each other
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        if(!m_afTimer[i]) continue;

        for(coreUintW j = i+1u; j < COMBAT_LABELS; ++j)
        {
            if(!m_afTimer[j]) continue;

            cGuiLabel& A = m_aLabel[i];
            cGuiLabel& B = m_aLabel[j];

            // 
            const coreVector2 vRange = (A.GetSize()     + B.GetSize()) * 0.5f;
            const coreVector2 vDiff  = (A.GetCenter()   - B.GetCenter()) +
                                       (A.GetPosition() - B.GetPosition());

            // 
            if((vRange.x > ABS(vDiff.x)) &&
               (vRange.y > ABS(vDiff.y)))
            {
                // 
                const coreFloat fOffset = (vRange.x - ABS(vDiff.x)) * SIGN(vDiff.x) * 0.5f;

                // 
                A.SetCenter(A.GetCenter() + coreVector2(fOffset, 0.0f));
                B.SetCenter(B.GetCenter() - coreVector2(fOffset, 0.0f));
            }
        }
    }

    // move active label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
        if(m_afTimer[i]) m_aLabel[i].Move();
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
    oLabel.SetCenter(MapToAxis(g_pForeground->Project2D(vPosition).Processed(CLAMP, -0.4f, 0.4f), g_vHudDirection));
    oLabel.SetColor4(coreVector4(vColor, 0.0f));

    // start animation timer
    ASSERT(!fTimer)
    fTimer = 1.0f;
}

void cCombatText::AddValue(const coreInt32 iValue, const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    this->AddText(PRINT("%d", iValue), vPosition, vColor);
}


// ****************************************************************
// 
void cCombatText::AddBadge(const coreUint32 iValue, const coreVector3& vPosition)
{
    const coreVector2 vOnScreen = MapToAxis(g_pForeground->Project2D(vPosition).Processed(CLAMP, -0.4f, 0.4f), g_vHudDirection);

    // 
    m_BadgeIcon.SetCenter(vOnScreen);

    // 
    m_BadgeLabel.SetText  (PRINT("%d", iValue));
    m_BadgeLabel.SetCenter(vOnScreen);

    // 
    ASSERT(!m_fBadgeTimer)
    m_fBadgeTimer = 1.0f;
}


// ****************************************************************
// 
void cCombatText::UpdateLayout()
{
    // TODO: rotate position if game rotates
    // TODO: rotate direction (without position) if interface rotates
}


// ****************************************************************
// reset the combat text
void cCombatText::Reset()
{
    // stop all animation timers
    std::memset(m_afTimer, 0, sizeof(m_afTimer));
    
    
    m_fBadgeTimer = 0.0f;
}