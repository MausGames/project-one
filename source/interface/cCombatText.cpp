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
: m_afTime        {}
, m_aiType        {}
, m_apOrder       {}
, m_iOrderNum     (0u)
, m_afMarkerAlpha {}
, m_iMarkerCover  (0u)
, m_iMarkerState  (0u)
, m_fBadgeTime    (0.0f)
, m_fTrophyTime   (0.0f)
, m_iLastScore    (0u)
, m_vOldDirection (coreVector2(0.0f,1.0f))
, m_fOldSide      (1.0f)
, m_bVisible      (false)
, m_fAlpha        (0.0f)
{
    // create label objects
    for(coreUintW i = 0u;                  i < COMBAT_LABELS_SMALL; ++i) {m_aLabel[i].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL); m_aLabel[i].PregenerateTexture("00000");}
    for(coreUintW i = COMBAT_LABELS_SMALL; i < COMBAT_LABELS;       ++i) {m_aLabel[i].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL); m_aLabel[i].PregenerateTexture("00000");}

    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        // 
        m_aMarker[i].Construct (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
        m_aMarker[i].SetAlpha  (0.0f);
        m_aMarker[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aMarker[i].SetRectify(false);   // # might move around with target

        // 
        m_aMarkerBack[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aMarkerBack[i].DefineProgram("menu_single_program");
        m_aMarkerBack[i].SetColor4    (coreVector4(0.0f,0.0f,0.0f,0.0f));
        m_aMarkerBack[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_BadgeIcon.DefineTexture(0u, "menu_badge.png");
    m_BadgeIcon.DefineProgram("default_2d_program");
    m_BadgeIcon.SetTexSize   (coreVector2(0.5f,1.0f));

    // 
    m_BadgeBack.DefineTexture(0u, "effect_headlight_point.png");
    m_BadgeBack.DefineProgram("menu_single_program");
    m_BadgeBack.SetSize      (coreVector2(1.0f,1.0f) * 0.1f);
    m_BadgeBack.SetColor4    (coreVector4(0.0f,0.0f,0.0f,0.0f));

    // 
    m_BadgeLabel.Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    m_BadgeLabel.SetColor3(COLOR_MENU_INSIDE);

    // 
    m_TrophyIcon.DefineTexture(0u, "menu_trophy.png");
    m_TrophyIcon.DefineProgram("default_2d_program");
    m_TrophyIcon.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.95f);

    // 
    m_TrophyBack.DefineTexture(0u, "effect_headlight_point.png");
    m_TrophyBack.DefineProgram("menu_single_program");
    m_TrophyBack.SetSize      (coreVector2(1.0f,1.0f) * 0.1f);
    m_TrophyBack.SetColor4    (coreVector4(0.0f,0.0f,0.0f,0.0f));
    
    
    
    // 
    //m_vOldDirection = g_pPostProcessing->GetDirection();
    m_fOldSide      = (g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f;
    
    
    const coreVector2 vGame  = g_pPostProcessing->GetDirection();
    const coreVector2 vHud   = g_vHudDirection;
    m_vOldDirection = MapToAxisInv(vGame, vHud);
    ASSERT(m_vOldDirection.IsNormalized())
}


// ****************************************************************
// render the combat text
void cCombatText::Render()
{
    // 
    const coreVector2 vCorner = coreVector2(0.5f,0.5f) * (g_vGameResolution * Core::Graphics->GetViewResolution().zw());
    if(!g_bTiltMode) Core::Graphics->StartScissorTest(-vCorner, vCorner);
    {
        // render active label objects
        for(coreUintW i = 0u, ie = m_iOrderNum; i < ie; ++i)
            m_apOrder[i]->Render();

        // 
        for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i) m_aMarkerBack[i].Render();
        for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i) m_aMarker    [i].Render();

        // 
        if(m_fBadgeTime)
        {
            m_BadgeBack .Render();
            m_BadgeIcon .Render();
            m_BadgeLabel.Render();
        }

        // 
        if(m_fTrophyTime)
        {
            m_TrophyBack.Render();
            m_TrophyIcon.Render();
        }
    }
    if(!g_bTiltMode) Core::Graphics->EndScissorTest();
}


// ****************************************************************
// move the combat text
void cCombatText::Move()
{
    if(SPECIAL_FROZEN) return;

    // 
    const coreFloat fAlphaFull = BLENDH3(m_fAlpha) * MENU_INSIDE_ALPHA;

    // 
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        cGuiLabel& oLabel = m_aLabel[i];
        coreFlow&  fTime  = m_afTime[i];
        if(!fTime) continue;

        // 
        const coreUint8 iType = m_aiType[i];

        // update animation timer
        fTime.UpdateMax(-1.0f, 0.0f);
        if(!fTime) this->__RemoveOrder(&oLabel);
        
        coreVector2 vPosition;
        coreFloat   fAlpha;
        coreVector2 vScale;
        
        if(iType == COMBAT_TYPE_SHIFT)
        {
            vPosition = coreVector2(oLabel.GetPosition().x, SIN(fTime * (-16.0f*PI)) * LERP(0.027f, 0.0f, MIN1((1.0f - fTime) * 2.0f)));
            fAlpha    = LERPH3(0.0f, 1.0f, MIN1((fTime)        *  6.0f)) * fAlphaFull;
            vScale    = LERPB (0.5f, 1.0f, MIN1((1.0f - fTime) * 10.0f)) * coreVector2(1.0f,1.0f);
        }
        else if((iType == COMBAT_TYPE_EXTRA) || (iType == COMBAT_TYPE_CHAIN))
        {
            vPosition = coreVector2(LERPB(-0.025f, 0.025f, 1.0f - fTime), oLabel.GetPosition().y);
            fAlpha    = LERPH3(0.0f, 1.0f, MIN1((fTime)        *  6.0f)) * fAlphaFull;
            
            const coreFloat fScale = LERPB(0.5f, 1.0f, MIN1((1.0f - fTime) * 3.0f));
            vScale = coreVector2(1.0f, fScale);
        }
        else
        {
            vPosition = coreVector2(oLabel.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - fTime));
            fAlpha    = LERPH3(0.0f, 1.0f, MIN1((fTime)        *  6.0f)) * fAlphaFull;
            vScale    = LERPB (0.5f, 1.0f, MIN1((1.0f - fTime) * 10.0f)) * coreVector2(1.0f,1.0f);
        }

        oLabel.SetPosition(vPosition);
        oLabel.SetAlpha   (fAlpha * (((m_aiType[i] != COMBAT_TYPE_SCORE) || (m_iLastScore == i)) ? 1.0f : 0.6f));
        oLabel.SetScale   (vScale);
        
        
        // move overlaying label objects away from each other
        for(coreUintW j = i+1u; j < COMBAT_LABELS; ++j)
        {
            if(!m_afTime[j]) continue;

            // 
            if((m_aiType[i] == COMBAT_TYPE_COUNTDOWN) && (m_aiType[j] == COMBAT_TYPE_COUNTDOWN)) continue;

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

                // 
                if(m_aiType[i] != COMBAT_TYPE_SCORE) A.SetCenter(A.GetCenter() + coreVector2(0.0f, fOffset));
                if(m_aiType[j] != COMBAT_TYPE_SCORE) B.SetCenter(B.GetCenter() - coreVector2(0.0f, fOffset));
            }
        }

        // 
        oLabel.RetrieveDesiredSize([=, this](const coreVector2 vSize)
        {
            m_aLabel[i].SetCenter(cCombatText::__RestrictCenter(vPosition, vSize, m_aLabel[i].GetCenter()));
        });

        // 
        oLabel.Move();
    }

    // 
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        cGuiLabel&  oMarker = m_aMarker    [i];
        cGuiObject& oBack   = m_aMarkerBack[i];
        if(!oMarker.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(!HAS_BIT(m_iMarkerState, i))
        {
            // 
            m_afMarkerAlpha[i].UpdateMax(-5.0f, 0.0f);
            if(!m_afMarkerAlpha[i])
            {
                // 
                oMarker.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                oBack  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            }
        }

        // 
        const coreFloat fCover = HAS_BIT(m_iMarkerCover, i) ? g_pGame->GetInterface()->CalcGameCover(&oMarker, coreVector2(1.6f,1.6f), false) : 1.0f;

        // 
        oMarker.SetAlpha(m_afMarkerAlpha[i] * fCover);
        oMarker.SetScale(coreVector2(1.0f,1.0f) * LERPBR(1.2f, 1.0f, m_afMarkerAlpha[i]));
        oMarker.Move();

        // 
        oBack.SetAlpha(oMarker.GetAlpha());
        oBack.Move();
    }

    // 
    m_iMarkerState = 0u;

    // 
    if(m_fBadgeTime)
    {
        // 
        m_fBadgeTime.UpdateMax(-0.8f, 0.0f);

        // 
        const coreVector2 vPosition  = coreVector2(m_BadgeIcon.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - m_fBadgeTime));
        const coreVector2 vSize      = coreVector2(1.0f,1.0f) * 0.12f * LERPB(0.5f, 1.0f, MIN1((1.0f - m_fBadgeTime) * 10.0f));
        const coreVector2 vDirection = coreVector2::Direction(LERPB(-0.5f*PI, -2.0f*PI, MIN1((1.0f - m_fBadgeTime) * 1.5f)));
        const coreVector2 vCenter    = cCombatText::__RestrictCenter(vPosition, m_BadgeIcon.GetSize(), m_BadgeIcon.GetCenter());
        const coreFloat   fAlpha     = BLENDH3(MIN1(m_fBadgeTime * 8.0f)) * fAlphaFull;

        // 
        m_BadgeIcon.SetPosition (vPosition);
        m_BadgeIcon.SetSize     (vSize);
        m_BadgeIcon.SetDirection(vDirection);
        m_BadgeIcon.SetCenter   (vCenter);
        m_BadgeIcon.SetAlpha    (fAlpha);
        m_BadgeIcon.Move();

        // 
        m_BadgeBack.SetPosition(m_BadgeIcon.GetPosition());
        m_BadgeBack.SetCenter  (m_BadgeIcon.GetCenter  ());
        m_BadgeBack.SetAlpha   (m_BadgeIcon.GetAlpha   ());
        m_BadgeBack.Move();

        // 
        m_BadgeLabel.SetPosition(m_BadgeIcon.GetPosition());
        m_BadgeLabel.SetCenter  (m_BadgeIcon.GetCenter  ());
        m_BadgeLabel.SetAlpha   (m_BadgeIcon.GetAlpha   ());
        m_BadgeLabel.Move();
    }

    // 
    if(m_fTrophyTime)
    {
        // 
        m_fTrophyTime.UpdateMax(-0.8f, 0.0f);

        // 
        const coreVector2 vPosition = coreVector2(m_TrophyIcon.GetPosition().x, LERPB(0.0f, 0.05f, 1.0f - m_fTrophyTime));
        const coreVector2 vSize     = coreVector2(1.0f,1.0f) * 0.12f * LERPB (0.5f, 1.0f, MIN1((1.0f - m_fTrophyTime) * 10.0f));
        const coreVector2 vDirection = coreVector2::Direction(LERPB(1.0f*PI, -2.0f*PI, MIN1((1.0f - m_fTrophyTime) * 1.5f)));
        const coreVector2 vCenter   = cCombatText::__RestrictCenter(vPosition, m_TrophyIcon.GetSize(), m_TrophyIcon.GetCenter());
        const coreFloat   fAlpha    = BLENDH3(MIN1(m_fTrophyTime * 8.0f)) * fAlphaFull;

        // 
        m_TrophyIcon.SetPosition (vPosition);
        m_TrophyIcon.SetSize     (vSize);
        m_TrophyIcon.SetDirection(vDirection);
        m_TrophyIcon.SetCenter   (vCenter);
        m_TrophyIcon.SetAlpha    (fAlpha);
        m_TrophyIcon.Move();

        // 
        m_TrophyBack.SetPosition(m_TrophyIcon.GetPosition());
        m_TrophyBack.SetCenter  (m_TrophyIcon.GetCenter  ());
        m_TrophyBack.SetAlpha   (m_TrophyIcon.GetAlpha   ());
        m_TrophyBack.Move();
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
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 0u))
    {
        // 
        this->__DrawLabel(coreData::ToChars(iValue), vPosition, bBig, COLOR_MENU_INSIDE, COMBAT_TYPE_SCORE);
    }
}

void cCombatText::DrawExtra(const coreUint32 iValue, const coreVector3 vPosition, const coreBool bBig)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 0u))
    {
        // 
        this->__DrawLabel(coreData::ToChars(iValue), vPosition, bBig, COLOR_MENU_GREEN, COMBAT_TYPE_EXTRA);
    }
}

void cCombatText::DrawChain(const coreUint32 iValue, const coreVector3 vPosition)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 0u))
    {
        // 
        this->__DrawLabel(coreData::ToChars(iValue), vPosition, true, COLOR_MENU_BLUE, COMBAT_TYPE_CHAIN);
    }
}

void cCombatText::DrawShift(const coreUint32 iValue, const coreVector3 vPosition)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 0u))
    {
        // 
        this->__DrawLabel(PRINT("+%u", iValue), vPosition, true, COLOR_MENU_RED, COMBAT_TYPE_SHIFT);
    }
}

void cCombatText::DrawProgress(const coreUint32 iCurrent, const coreUint32 iTotal, const coreVector3 vPosition)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 1u))
    {
        // 
        this->__DrawLabel(PRINT("%u/%u", iCurrent, iTotal), vPosition, true, COLOR_MENU_YELLOW, COMBAT_TYPE_PROGRESS);
    }
}

void cCombatText::DrawCountdown(const coreUint32 iCurrent, const coreUint32 iTotal, const coreVector3 vPosition)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 1u))
    {
        // 
        this->__DrawLabel(coreData::ToChars(iTotal - iCurrent), vPosition, true, COLOR_MENU_YELLOW, COMBAT_TYPE_COUNTDOWN);
    }
}

void cCombatText::DrawText(const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor)
{
    if(HAS_BIT(g_CurConfig.Game.iCombatText, 1u))
    {
        // 
        this->__DrawLabel(pcText, vPosition, true, vColor, COMBAT_TYPE_TEXT);
    }
}


// ****************************************************************
// 
void cCombatText::DrawBadge(const coreUint32 iValue, const coreVector3 vPosition)
{
    // 
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);

    // 
    m_BadgeIcon.SetCenter   (vOnScreen);
    m_BadgeIcon.SetAlpha    (0.0f);
    m_BadgeIcon.SetTexOffset(coreVector2(iValue ? 0.0f : 0.5f, 0.0f));

    // 
    m_BadgeBack.SetCenter(vOnScreen);
    m_BadgeBack.SetAlpha (0.0f);

    // 
    m_BadgeLabel.SetText  ("");//(HAS_BIT(g_CurConfig.Game.iCombatText, 2u) && iValue) ? PRINT("-%u", iValue) : "");
    m_BadgeLabel.SetCenter(vOnScreen);
    m_BadgeLabel.SetAlpha (0.0f);

    // 
    ASSERT(!m_fBadgeTime)
    m_fBadgeTime = 1.0f;
}


// ****************************************************************
// 
void cCombatText::DrawTrophy(const coreVector3 vPosition)
{
    // 
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);

    // 
    m_TrophyIcon.SetCenter(vOnScreen);
    m_TrophyIcon.SetAlpha (0.0f);

    // 
    m_TrophyBack.SetCenter(vOnScreen);
    m_TrophyBack.SetAlpha (0.0f);

    // 
    ASSERT(!m_fTrophyTime)
    m_fTrophyTime = 1.0f;
}


// ****************************************************************
// 
void cCombatText::AttachMarker(const coreUintW iIndex, const coreChar* pcText, const coreVector3 vPosition, const coreVector3 vColor, const coreBool bCover)
{
    ASSERT(iIndex < COMBAT_MARKERS)

    if(!HAS_BIT(g_CurConfig.Game.iCombatText, 3u)) return;

    // 
    const coreVector2 vOnScreen = cCombatText::__TransformPosition(vPosition);

    // 
    m_aMarker[iIndex].SetText   (pcText);
    m_aMarker[iIndex].SetCenter (vOnScreen);
    m_aMarker[iIndex].SetColor4 (coreVector4(vColor, 1.0f));
    m_aMarker[iIndex].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_aMarkerBack[iIndex].SetSize   (coreVector2(0.07f,0.07f));
    m_aMarkerBack[iIndex].SetCenter (vOnScreen);
    m_aMarkerBack[iIndex].SetAlpha  (1.0f);
    m_aMarkerBack[iIndex].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    m_afMarkerAlpha[iIndex] = 1.0f;
    
    // 
    SET_BIT(m_iMarkerCover, iIndex, bCover)
    STATIC_ASSERT(COMBAT_MARKERS <= sizeof(m_iMarkerCover)*8u)

    // 
    ADD_BIT(m_iMarkerState, iIndex)
    STATIC_ASSERT(COMBAT_MARKERS <= sizeof(m_iMarkerState)*8u)
}


// ****************************************************************
// 
void cCombatText::UpdateLayout()
{
    // 
    const coreFloat   fSide  = (g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f;
    const coreVector2 vFinal = CalcFinalDirection();
    
    
    const auto nTransformFunc = [&](coreObject2D* OUTPUT pObject)
    {
        coreVector2 vCurCenter = pObject->GetCenter();

        if(m_fOldSide != fSide)
        {
            vCurCenter.arr(IsHorizontal(m_vOldDirection) ? 1u : 0u) *= -1.0f;
        }

        vCurCenter = MapToAxisInv(MapToAxis(vCurCenter, m_vOldDirection), vFinal);

        pObject->SetCenter(vCurCenter);
        pObject->Move();
    };

    // 
    for(coreUintW i = 0u; i < COMBAT_LABELS; ++i)
    {
        nTransformFunc(&m_aLabel[i]);
    }

    // 
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        nTransformFunc(&m_aMarker[i]);
        nTransformFunc(&m_aMarkerBack[i]);
    }

    // 
    nTransformFunc(&m_BadgeIcon);
    nTransformFunc(&m_BadgeBack);
    nTransformFunc(&m_BadgeLabel);

    // 
    nTransformFunc(&m_TrophyIcon);
    nTransformFunc(&m_TrophyBack);

    // 
    m_vOldDirection = vFinal;
    m_fOldSide      = fSide;
}


// ****************************************************************
// reset the combat text
void cCombatText::Reset()
{
    // stop all animation timers
    std::memset(m_afTime, 0, sizeof(m_afTime));

    // 
    m_iOrderNum = 0u;

    // 
    for(coreUintW i = 0u; i < COMBAT_MARKERS; ++i)
    {
        m_aMarker    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aMarkerBack[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_fBadgeTime  = 0.0f;
    m_fTrophyTime = 0.0f;
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
        if(m_afTime[i]) continue;

        // init label object
        m_aLabel[i].SetText    (pcText);
        m_aLabel[i].SetPosition(coreVector2(0.0f,0.0f));
        m_aLabel[i].SetCenter  (vOnScreen);
        m_aLabel[i].SetColor4  (coreVector4(vColor, 0.0f));
        m_aLabel[i].SetScale   (coreVector2(1.0f,1.0f));
        
        
        //m_aLabel[i].SetColor4(coreVector4(COLOR_MENU_WHITE, 0.0f));// [A1]

        // 
        m_afTime[i] = 1.0f;
        m_aiType[i] = iType;

        // 
        this->__AddOrder(&m_aLabel[i]);

        // 
        if(iType == COMBAT_TYPE_SCORE) m_iLastScore = i;

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
    const coreFloat   fSide  = (g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f;
    const coreVector2 vFinal = CalcFinalDirection();

    // 
    return MapToAxisInv(g_pForeground->Project2D(vPosition - coreVector3(Core::Graphics->GetCamPosition().xy(), 0.0f)) * coreVector2(fSide, 1.0f), vFinal);
}


// ****************************************************************
// 
coreVector2 cCombatText::__RestrictCenter(const coreVector2 vPosition, const coreVector2 vSize, const coreVector2 vCenter)
{
    if(g_bTiltMode)
    {
        const coreVector2 vRealBorder = Core::System->GetResolution().HighRatio() * 0.5f - (0.5f - COMBAT_BORDER);

        return coreVector2(CLAMP(vCenter.x, -vRealBorder.x - vPosition.x + vSize.x * 0.5f, vRealBorder.x - vPosition.x - vSize.x * 0.5f),
                           CLAMP(vCenter.y, -vRealBorder.y - vPosition.y + vSize.y * 0.5f, vRealBorder.y - vPosition.y - vSize.y * 0.5f));
    }

    return coreVector2(CLAMP(vCenter.x, -COMBAT_BORDER - vPosition.x + vSize.x * 0.5f, COMBAT_BORDER - vPosition.x - vSize.x * 0.5f),
                       CLAMP(vCenter.y, -COMBAT_BORDER - vPosition.y + vSize.y * 0.5f, COMBAT_BORDER - vPosition.y - vSize.y * 0.5f));
}