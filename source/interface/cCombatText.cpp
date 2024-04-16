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
    for(coreUintW i = 0u;                  i < COMBAT_LABELS_SMALL; ++i) m_aLabel[i].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    for(coreUintW i = COMBAT_LABELS_SMALL; i < COMBAT_LABELS;       ++i) m_aLabel[i].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);

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
        if(!fTimer) this->__RemoveOrder(&oLabel);

        // 
        const coreVector2 vPosition = coreVector2(oLabel.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - fTimer));
        const coreVector2 vCenter   = cCombatText::__RestrictCenter(vPosition, oLabel.GetCenter());
        const coreFloat   fAlpha    = LERPH3(0.0f, 1.0f, MIN((fTimer)        *  6.0f, 1.0f)) * MENU_INSIDE_ALPHA;
        const coreFloat   fScale    = LERPB (0.5f, 1.0f, MIN((1.0f - fTimer) * 10.0f, 1.0f));

        // update label object
        oLabel.SetPosition(vPosition);
        oLabel.SetCenter  (vCenter);
        oLabel.SetAlpha   (fAlpha);
        oLabel.SetScale   (fScale);
        oLabel.Move();
    }

    // 
    if(m_fBadgeTimer)
    {
        // 
        m_fBadgeTimer.UpdateMax(-0.8f, 0.0f);

        // 
        const coreVector2 vPosition  = coreVector2(m_BadgeIcon.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - m_fBadgeTimer));
        const coreVector2 vDirection = coreVector2::Direction(LERPB(0.5f*PI, 2.0f*PI, MIN((1.0f - m_fBadgeTimer) * 1.5f, 1.0f)));
        const coreVector2 vCenter    = cCombatText::__RestrictCenter(vPosition, m_BadgeIcon.GetCenter());
        const coreFloat   fAlpha     = LERPH3(0.0f, 1.0f, MIN(m_fBadgeTimer * 8.0f, 1.0f)) * MENU_INSIDE_ALPHA;

        // 
        m_BadgeIcon.SetPosition (vPosition);
        m_BadgeIcon.SetDirection(vDirection);
        m_BadgeIcon.SetCenter   (vCenter);
        m_BadgeIcon.SetAlpha    (fAlpha);
        m_BadgeIcon.Move();

        // 
        m_BadgeLabel.SetPosition(m_BadgeIcon.GetPosition());
        m_BadgeLabel.SetCenter  (m_BadgeIcon.GetCenter  ());
        m_BadgeLabel.SetAlpha   (m_BadgeIcon.GetAlpha   ());
        m_BadgeLabel.Move();
    }
}


// ****************************************************************
// 
void cCombatText::DrawScore(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__DrawLabel(PRINT("+%d", iValue), vPosition, bBig, coreVector3(1.0f,1.0f,1.0f), 0u);
}

void cCombatText::DrawExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__DrawLabel(PRINT("+%d", iValue), vPosition, bBig, COLOR_MENU_YELLOW, 1u);
}

void cCombatText::DrawChain(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    this->__DrawLabel(PRINT("+%d", iValue), vPosition, true, COLOR_MENU_BLUE, 2u);
}


// ****************************************************************
// 
void cCombatText::DrawBadge(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    const coreVector2 vOnScreen = MapToAxis(g_pForeground->Project2D(vPosition), g_vHudDirection);

    // 
    m_BadgeIcon.SetCenter(vOnScreen);
    m_BadgeIcon.SetAlpha (0.0f);

    // 
    m_BadgeLabel.SetText  (coreData::ToChars(iValue));
    m_BadgeLabel.SetCenter(vOnScreen);
    m_BadgeLabel.SetAlpha (0.0f);

    // 
    m_BadgeLabel.RetrieveDesiredSize([this](const coreVector2 vSize)
    {
        const coreFloat fOffset = (vSize.x - m_BadgeIcon.GetSize().x) * -0.5f;
        m_BadgeIcon.SetPosition(coreVector2(fOffset, 0.0f));
    });

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
void cCombatText::__DrawLabel(const coreChar* pcText, const coreVector3 vPosition, const coreBool bBig, const coreVector3 vColor, const coreUint8 iType)
{
    ASSERT(pcText)

    // 
    const coreVector2 vOnScreen = MapToAxis(g_pForeground->Project2D(vPosition), g_vHudDirection);

    // 
    const coreUintW iFrom = bBig ? COMBAT_LABELS_SMALL : 0u;
    const coreUintW iTo   = bBig ? COMBAT_LABELS       : COMBAT_LABELS_SMALL;

    // 
    for(coreUintW i = iFrom; i < iTo; ++i)
    {
        if(m_afTimer[i]) continue;

        // init label object
        m_aLabel[i].SetText  (pcText);
        m_aLabel[i].SetCenter(vOnScreen);
        m_aLabel[i].SetColor4(coreVector4(vColor, 0.0f));

        // 
        m_afTimer[i] = 1.0f;
        m_aiType [i] = iType;

        // 
        this->__AddOrder(&m_aLabel[i]);

        return;
    }

    // 
    WARN_IF(true) {}
}


// ****************************************************************
// 
void cCombatText::__AddOrder(cGuiLabel* pLabel)
{
    ASSERT(pLabel && (m_iOrderNum < COMBAT_LABELS))

    // 
    m_apOrder[m_iOrderNum] = pLabel;
    m_iOrderNum += 1u;
}


// ****************************************************************
// 
void cCombatText::__RemoveOrder(cGuiLabel* pLabel)
{
    ASSERT(pLabel && (m_iOrderNum > 0u))

    // 
    for(coreUintW i = 0u, ie = m_iOrderNum; i < ie; ++i)
    {
        if(m_apOrder[i] == pLabel)
        {
            // 
            std::memmove(m_apOrder + i, m_apOrder + i + 1u, (m_iOrderNum - i - 1u) * sizeof(cGuiLabel*));
            m_iOrderNum -= 1u;

            return;
        }
    }

    // 
    WARN_IF(true) {}
}


// ****************************************************************
// 
coreVector2 cCombatText::__RestrictCenter(const coreVector2 vPosition, const coreVector2 vCenter)
{
    return coreVector2(CLAMP(vCenter.x, -COMBAT_BORDER - vPosition.x, COMBAT_BORDER - vPosition.x),
                       CLAMP(vCenter.y, -COMBAT_BORDER - vPosition.y, COMBAT_BORDER - vPosition.y));
}