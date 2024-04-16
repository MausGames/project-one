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
, m_aiType      {}
, m_apOrder     {}
, m_iOrderNum   (0u)
, m_fBadgeTimer (0.0f)
{
    // create label objects
    for(coreUintW i = 0u; i < COMBAT_LABELS_SMALL; ++i)
    {
        m_aLabel[i].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    }
    for(coreUintW i = COMBAT_LABELS_SMALL; i < COMBAT_LABELS; ++i)
    {
        m_aLabel[i].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    }

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
    // render active label objects
    for(coreUintW i = 0u, ie = m_iOrderNum; i < ie; ++i)
        m_apOrder[i]->Render();

    // 
    if(m_fBadgeTimer)
    {
        m_BadgeLabel.Render();
        m_BadgeIcon .Render();
    }
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // 
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        cGuiLabel& oLabel = m_aLabel [i];
        coreFlow&  fTimer = m_afTimer[i];
        if(!fTimer) continue;

        // update animation timer
        fTimer.UpdateMax(-1.0f, 0.0f);
        if(!fTimer)
        {
            this->__RemoveOrder(&oLabel);
        }

        // update label object
        oLabel.SetPosition(coreVector2(0.0f, LERPB(0.0f, 0.05f, 1.0f - fTimer)));
        oLabel.SetAlpha   (LERPH3(0.0f, 1.0f, MIN((fTimer)        *  6.0f, 1.0f)) * MENU_INSIDE_ALPHA);
        oLabel.SetScale   (LERPB (0.5f, 1.0f, MIN((1.0f - fTimer) * 10.0f, 1.0f)));
        
        
        
        const coreVector4 vRange = coreVector4(-0.47f - m_aLabel[i].GetPosition().x, 0.47f - m_aLabel[i].GetPosition().x,
                                               -0.47f - m_aLabel[i].GetPosition().y, 0.47f - m_aLabel[i].GetPosition().y);
        m_aLabel[i].SetCenter(coreVector2(CLAMP(m_aLabel[i].GetCenter().x, vRange.x, vRange.y), CLAMP(m_aLabel[i].GetCenter().y, vRange.z, vRange.w)));
        
        m_aLabel[i].Move();
    }

    // 
    if(m_fBadgeTimer)
    {
        // 
        m_fBadgeTimer.UpdateMax(-0.8f, 0.0f);

        // 
        m_BadgeIcon.SetPosition(coreVector2(0.0f, LERPB(0.0f, 0.05f, 1.0f - m_fBadgeTimer)));
        m_BadgeIcon.SetDirection(coreVector2::Direction(LERPB(0.5f*PI, 2.0f*PI, MIN((1.0f - m_fBadgeTimer) * 1.5f, 1.0f))));
        m_BadgeIcon.SetAlpha   (LERPH3(0.0f, 1.0f, MIN(m_fBadgeTimer * 8.0f, 1.0f)) * MENU_INSIDE_ALPHA);
        
        
        const coreVector4 vRange = coreVector4(-0.47f - m_BadgeIcon.GetPosition().x, 0.47f - m_BadgeIcon.GetPosition().x,
                                               -0.47f - m_BadgeIcon.GetPosition().y, 0.47f - m_BadgeIcon.GetPosition().y);
        m_BadgeIcon.SetCenter(coreVector2(CLAMP(m_BadgeIcon.GetCenter().x, vRange.x, vRange.y), CLAMP(m_BadgeIcon.GetCenter().y, vRange.z, vRange.w)));
        
        m_BadgeIcon.Move();

        // 
        m_BadgeLabel.SetPosition(m_BadgeIcon.GetPosition());
        m_BadgeLabel.SetCenter(m_BadgeIcon.GetCenter());
        m_BadgeLabel.SetAlpha   (m_BadgeIcon.GetAlpha   ());
        m_BadgeLabel.Move();
    }

#if 0
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
                const coreFloat fOffset = (vRange.y - ABS(vDiff.y)) * SIGN(vDiff.y) * 0.5f * (TIME * 15.0f);

                //     if(m_iCurLabel == i) B.SetCenter(B.GetCenter() - coreVector2(0.0f, fOffset));
                //else if(m_iCurLabel == j) A.SetCenter(A.GetCenter() + coreVector2(0.0f, fOffset));
                //else
                {
                    // 
                    A.SetCenter(A.GetCenter() + coreVector2(0.0f, fOffset));
                    B.SetCenter(B.GetCenter() - coreVector2(0.0f, fOffset));
                }
            }
        }
    }
#endif
}


// ****************************************************************
// 
void cCombatText::AddScore(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__AddLabel(PRINT("+%d", iValue), vPosition, coreVector3(1.0f,1.0f,1.0f), bBig ? 1u : 0u);
}

void cCombatText::AddExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__AddLabel(PRINT("+%d", iValue), vPosition, COLOR_MENU_YELLOW, 1u);
}

void cCombatText::AddChain(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__AddLabel(PRINT("+%d", iValue), vPosition, COLOR_MENU_BLUE, 2u);
}


// ****************************************************************
// 
void cCombatText::AddBadge(const coreUint32 iValue, const coreVector3 vPosition)
{
    // TODO 1: combat text outside needs to be handled (maybe in move)
    
    const coreVector2 vOnScreen = MapToAxis(g_pForeground->Project2D(vPosition), g_vHudDirection);

    // 
    m_BadgeIcon.SetCenter(vOnScreen);

    // 
    m_BadgeLabel.SetText  (coreData::ToChars(iValue));
    m_BadgeLabel.SetCenter(vOnScreen);

    // 
    ASSERT(!m_fBadgeTimer)
    m_fBadgeTimer = 1.0f;
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

    // 
    m_fBadgeTimer = 0.0f;
}


// ****************************************************************
// 
void cCombatText::__AddLabel(const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor, const coreUint8 iType)
{
    ASSERT(pcText)
    
    // TODO 1: combat text outside needs to be handled (maybe in move -> because of "away from each other")
    
    const coreVector2 vOnScreen = MapToAxis(g_pForeground->Project2D(vPosition), g_vHudDirection);//.Processed(CLAMP, -0.45f, 0.45f);
    
    const coreUintW iFrom = iType ? COMBAT_LABELS_SMALL : 0u;
    const coreUintW iTo   = iType ? COMBAT_LABELS : COMBAT_LABELS_SMALL;
    
    
    for(coreUintW i = iFrom; i < iTo; ++i)
    {
        cGuiLabel& oLabel = m_aLabel [i];
        coreFlow&  fTimer = m_afTimer[i];
        if(fTimer) continue;

        // init label object
        oLabel.SetText  (pcText);
        oLabel.SetCenter(vOnScreen);
        oLabel.SetColor4(coreVector4(vColor, 0.0f));

        // start animation timer
        ASSERT(!fTimer)
        fTimer = 1.0f;
        
        this->__AddOrder(&oLabel);

        break;
    }
    

    // 
    //if(++m_iCurLabel >= COMBAT_LABELS) m_iCurLabel = 0u;
    //cGuiLabel& oLabel = m_aLabel [m_iCurLabel];
    //coreFlow&  fTimer = m_afTimer[m_iCurLabel];
}


// ****************************************************************
// 
void cCombatText::__AddOrder(cGuiLabel* pLabel)
{
    ASSERT(pLabel)

    m_apOrder[m_iOrderNum++] = pLabel;
}


// ****************************************************************
// 
void cCombatText::__RemoveOrder(cGuiLabel* pLabel)
{
    ASSERT(pLabel)

    for(coreUintW i = 0u, ie = m_iOrderNum; i < ie; ++i)
    {
        if(m_apOrder[i] == pLabel)
        {
            std::memmove(m_apOrder + i, m_apOrder + i + 1u, sizeof(cGuiLabel*) * (m_iOrderNum - i - 1u));
            m_iOrderNum -= 1u;

            return;
        }
    }

    WARN_IF(true) {}
}