///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cCombatText::cCombatText()noexcept
: m_afTimer      {}
, m_aiType       {}
, m_apOrder      {}
, m_iOrderNum    (0u)
, m_iMarkerState (0u)
, m_fBadgeTimer  (0.0f)
, m_bVisible     (false)
, m_fAlpha       (0.0f)

, m_iLastLabel (0u)
{
    // create label objects
    for(coreUintW i = 0u;                  i < COMBAT_LABELS_SMALL; ++i) m_aLabel[i].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    for(coreUintW i = COMBAT_LABELS_SMALL; i < COMBAT_LABELS;       ++i) m_aLabel[i].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    
    
    
    
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        m_aMarker[i].Construct (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
        m_aMarker[i].SetAlpha  (0.0f);
        m_aMarker[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aMarker[i].SetRectify(false);
    }
    
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        m_aMarkerBack[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aMarkerBack[i].DefineProgram("menu_single_program");
        m_aMarkerBack[i].SetColor4    (coreVector4(0.0f,0.0f,0.0f,0.0f));
        m_aMarkerBack[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }


    // 
    m_BadgeIcon.DefineTexture(0u, "menu_star.png");
    m_BadgeIcon.DefineProgram("default_2d_program");
    m_BadgeIcon.SetSize      (coreVector2( 1.0f,1.0f) * 0.11f);
    //m_BadgeIcon.SetAlignment (coreVector2(-1.0f,0.0f));
    m_BadgeIcon.SetTexSize   (coreVector2( 0.5f,1.0f));

    // 
    m_BadgeLabel.Construct   (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    //m_BadgeLabel.SetAlignment(coreVector2(1.0f,0.0f));
    m_BadgeLabel.SetColor3   (COLOR_MENU_INSIDE);
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // 
    const coreVector2 vCorner = coreVector2(0.5f,0.5f) * (g_vGameResolution / Core::Graphics->GetViewResolution().xy());
    if(!g_bTiltMode) Core::Graphics->StartScissorTest(-vCorner, vCorner);
    {
        // render active label objects
        for(coreUintW i = 0u, ie = m_iOrderNum; i < ie; ++i)
            m_apOrder[i]->Render();
        
        
        
        for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
            m_aMarkerBack[i].Render();
        
        for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
            m_aMarker[i].Render();
    
        // 
        if(m_fBadgeTimer)
        {
            m_BadgeIcon .Render();
            m_BadgeLabel.Render();
        }
    }
    if(!g_bTiltMode) Core::Graphics->EndScissorTest();
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    // 
    const coreFloat fAlphaFull = BLENDH3(m_fAlpha) * MENU_INSIDE_ALPHA;

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
        //const coreVector2 vCenter   = cCombatText::__RestrictCenter(vPosition, coreVector2(0.0f,0.0f), oLabel.GetCenter());
        const coreFloat   fAlpha    = LERPH3(0.0f, 1.0f, MIN((fTimer)        *  6.0f, 1.0f)) * fAlphaFull;
        const coreFloat   fScale    = LERPB (0.5f, 1.0f, MIN((1.0f - fTimer) * 10.0f, 1.0f));

        // update label object
        oLabel.SetPosition(vPosition);
        //oLabel.SetCenter  (vCenter);
        oLabel.SetAlpha   (fAlpha * (((m_aiType[i] != 0u) || (m_iLastLabel == i)) ? 1.0f : 0.6f));
        oLabel.SetScale   (fScale);
        //oLabel.Move();

        oLabel.RetrieveDesiredSize([=, this](const coreVector2 vSize)
        {
            ASSERT(Core::System->GetMainThread() == SDL_ThreadID())

            const coreVector2 vCenter = cCombatText::__RestrictCenter(vPosition, vSize, m_aLabel[i].GetCenter());

            m_aLabel[i].SetCenter(vCenter);
            m_aLabel[i].Move();
        });
    }
    
    
    
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        cGuiLabel&  oMarker = m_aMarker    [i];
        cGuiObject& oBack   = m_aMarkerBack[i];
        if(!oMarker.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
        
        if(!HAS_BIT(m_iMarkerState, i))
        {
            oMarker.SetAlpha(MAX0(oMarker.GetAlpha() - 5.0f * TIME));
            if(!oMarker.GetAlpha()) oMarker.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }

        oMarker.SetScale(LERPBR(1.2f, 1.0f, oMarker.GetAlpha()));
        oMarker.Move();

        oBack.SetAlpha(oMarker.GetAlpha());
        oBack.Move();
    }


    m_iMarkerState = 0u;

    // 
    if(m_fBadgeTimer)
    {
        // 
        m_fBadgeTimer.UpdateMax(-0.8f, 0.0f);

        // 
        const coreVector2 vPosition  = coreVector2(m_BadgeIcon.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - m_fBadgeTimer));
        const coreVector2 vDirection = coreVector2::Direction(LERPB(0.5f*PI, 2.0f*PI, MIN1((1.0f - m_fBadgeTimer) * 1.5f)));
        const coreVector2 vCenter    = cCombatText::__RestrictCenter(vPosition, m_BadgeIcon.GetSize(), m_BadgeIcon.GetCenter());
        const coreFloat   fAlpha     = BLENDH3(MIN1(m_fBadgeTimer * 8.0f)) * fAlphaFull;

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

    // smoothly toggle combat text visibility (after forwarding, to allow overriding)
    if(m_bVisible)
         m_fAlpha.UpdateMin( 2.0f, 1.0f);
    else m_fAlpha.UpdateMax(-2.0f, 0.0f);
}


// ****************************************************************
// 
void cCombatText::DrawScore(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__DrawLabel(PRINT("%u", iValue), vPosition, bBig, COLOR_MENU_INSIDE, 0u);
}

void cCombatText::DrawExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    // 
    this->__DrawLabel(PRINT("%u", iValue), vPosition, bBig, COLOR_MENU_YELLOW, 1u);
}

void cCombatText::DrawChain(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    this->__DrawLabel(PRINT("%u", iValue), vPosition, true, COLOR_MENU_BLUE, 2u);
}

void cCombatText::DrawShift(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    this->__DrawLabel(PRINT("+%u", iValue), vPosition, true, COLOR_MENU_RED, 3u);
}

void cCombatText::DrawProgress(const coreUint32 iCurrent, const coreUint32 iTotal, const coreVector3 vPosition)
{
    // 
    this->__DrawLabel(PRINT("%u/%u", iCurrent, iTotal), vPosition, true, COLOR_MENU_INSIDE, 4u);
}

void cCombatText::DrawText(const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor)
{
    // 
    this->__DrawLabel(pcText, vPosition, true, vColor, 5u);
}


// ****************************************************************
// 
void cCombatText::DrawBadge(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);

    // 
    m_BadgeIcon.SetCenter(vOnScreen);
    m_BadgeIcon.SetAlpha (0.0f);

    // 
    m_BadgeLabel.SetText  (PRINT("-%u", iValue));
    m_BadgeLabel.SetCenter(vOnScreen);
    m_BadgeLabel.SetAlpha (0.0f);

    // 
    //m_BadgeLabel.RetrieveDesiredSize([this](const coreVector2 vSize)
    //{
    //    const coreFloat fOffset = (vSize.x - m_BadgeIcon.GetSize().x) * -0.5f;
    //    m_BadgeIcon.SetPosition(coreVector2(fOffset, 0.0f));
    //});

    // 
    ASSERT(!m_fBadgeTimer)
    m_fBadgeTimer = 1.0f;
}


// ****************************************************************
// 
void cCombatText::AttachMarker(const coreUintW iIndex, const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor)
{
    ASSERT(iIndex < COMBAT_MARKERS)
    
    
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);
    
    m_aMarker[iIndex].SetText  (pcText);
    m_aMarker[iIndex].SetCenter(vOnScreen);
    m_aMarker[iIndex].SetColor4(coreVector4(vColor, 1.0f));
    m_aMarker[iIndex].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    m_aMarkerBack[iIndex].SetSize   (coreVector2(0.07f,0.07f));
    m_aMarkerBack[iIndex].SetCenter (vOnScreen);
    m_aMarkerBack[iIndex].SetAlpha  (1.0f);
    m_aMarkerBack[iIndex].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    ADD_BIT(m_iMarkerState, iIndex)
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
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);

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
        
        m_iLastLabel = i;

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
coreVector2 cCombatText::__TransformPosition(const coreVector3 vPosition)
{
    // 
    const coreFloat fSide = (g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f;

    // 
    const coreVector2 vGame  = g_pPostProcessing->GetDirection();
    const coreVector2 vHud   = g_vHudDirection;
    const coreVector2 vFinal = MapToAxisInv(vGame, vHud);
    ASSERT(vFinal.IsNormalized())

    // 
    return MapToAxisInv(g_pForeground->Project2D(vPosition    - coreVector3(Core::Graphics->GetCamPosition().xy(), 0.0f)) * coreVector2(fSide, 1.0f), vFinal);
}


// ****************************************************************
// 
coreVector2 cCombatText::__RestrictCenter(const coreVector2 vPosition, const coreVector2 vSize, const coreVector2 vCenter)
{
    if(g_bTiltMode)
    {
        const coreVector2 vRealBorder = Core::System->GetResolution().MaxRatio() * 0.5f - (0.5f - COMBAT_BORDER);

        return coreVector2(CLAMP(vCenter.x, -vRealBorder.x - vPosition.x + vSize.x * 0.5f, vRealBorder.x - vPosition.x - vSize.x * 0.5f),
                           CLAMP(vCenter.y, -vRealBorder.y - vPosition.y + vSize.y * 0.5f, vRealBorder.y - vPosition.y - vSize.y * 0.5f));
    }

    return coreVector2(CLAMP(vCenter.x, -COMBAT_BORDER - vPosition.x + vSize.x * 0.5f, COMBAT_BORDER - vPosition.x - vSize.x * 0.5f),
                       CLAMP(vCenter.y, -COMBAT_BORDER - vPosition.y + vSize.y * 0.5f, COMBAT_BORDER - vPosition.y - vSize.y * 0.5f));
}