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
// construct player view
void cInterface::sPlayerView::Construct(const coreUintW iIndex)
{
    // create view objects
    for(coreUintW i = 0u; i < INTERFACE_LIVES; ++i)
    {
        aLife[i].DefineTexture(0u, g_pSpecialEffects->GetIconTexture(iIndex));
        aLife[i].DefineProgram("default_2d_program");
        aLife[i].SetTexSize   (ICON_TEXSIZE);
        aLife[i].SetTexOffset (ICON_TEXOFFSET * I_TO_F(i + 1u));
    }

    aShieldBar[0].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[0].DefineProgram("default_2d_program");
    aShieldBar[0].SetSize      (coreVector2(3.5f,0.45f) * 0.07f);

    aShieldBar[1].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[1].DefineProgram("default_2d_program");
    aShieldBar[1].SetSize      (aShieldBar[0].GetSize() - coreVector2(0.01f,0.01f));

    aShieldBar[2].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[2].DefineProgram("default_2d_program");
    aShieldBar[2].SetSize      (aShieldBar[1].GetSize());

    oShieldValue.Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    oShieldValue.SetColor3(COLOR_MENU_INSIDE);

    oScore.Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    oScore.SetColor3(COLOR_MENU_INSIDE);

    oCooldownBar.DefineProgram("menu_color_program");

    oComboValue.Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    oComboValue.SetColor3(COLOR_MENU_INSIDE);

    oChainValue.Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    oChainValue.SetColor3(COLOR_MENU_INSIDE);

    oImmune.Construct      (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    oImmune.SetColor3      (COLOR_MENU_INSIDE);
    oImmune.SetTextLanguage("IMMUNE");

    // 
    fShieldBump = 0.0f;
    iShieldOld  = 0;
    fImmuneTime = 0.0f;
    fSpin       = 0.0f;
    fSpin2      = 0.0f;
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreUint8 iNumViews)noexcept
: m_iNumViews        (iNumViews)
, m_fBossSpin        (0.0f)
, m_fGoalBump        (0.0f)
, m_iGoalOld         (MEDAL_NONE)
, m_afHelperBump     {}
, m_iHelperState     (0u)
, m_afBadgeBump      {}
, m_iBadgeState      (0u)
, m_fBannerStart     (INTERFACE_INVALID_START)
, m_fBannerDuration  (0.0f)
, m_fBannerSpeed     (0.0f)
, m_iBannerType      (0u)
, m_bBannerEigengrau (false)
, m_fStoryStart      (INTERFACE_INVALID_START)
, m_fFragmentStart   (INTERFACE_INVALID_START)
, m_iFragmentNew     (0u)
, m_iFragmentState   (0u)
, m_fAlertStart      (INTERFACE_INVALID_START)
, m_fAnimation       (0.0f)
, m_fRotation        (0.0f)
, m_fShake           (0.0f)
, m_bVisible         (false)
, m_fAlphaAll        (0.0f)
, m_fAlphaBoss       (0.0f)
, m_fAlphaWave       (0.0f)
, m_fAlphaSegment    (0.0f)
, m_fAlphaTurf       (0.0f)
, m_fAlphaGoal       (0.0f)
, m_fAlphaBadge      (0.0f)
, m_fAlphaFragment   (0.0f)
, m_bFakeEnd         (false)
, m_bBossChange      (false)
{
    ASSERT((m_iNumViews > 0) && (m_iNumViews <= INTERFACE_VIEWS))

    // construct player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        m_aView[i].Construct(i);

    // create interface objects
    m_aBossHealthBar[0].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].SetSize      (coreVector2(14.0f,0.5f) * 0.07f);
    m_aBossHealthBar[0].SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_aBossHealthBar[1].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize() - coreVector2(0.01f,0.01f));

    m_aBossHealthBar[2].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[2].DefineProgram("default_2d_program");
    m_aBossHealthBar[2].SetSize      (m_aBossHealthBar[1].GetSize());

    m_BossHealthValue.Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_BossHealthValue.SetColor3(COLOR_MENU_INSIDE);

    m_aBossTime[0].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aBossTime[0].SetColor3(COLOR_MENU_INSIDE);

    m_aBossTime[1].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aBossTime[1].SetColor3(COLOR_MENU_INSIDE);

    m_aBossTime[2].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aBossTime[2].SetColor3(COLOR_MENU_INSIDE * 0.75f);

    m_aWaveTime[0].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aWaveTime[0].SetColor3(COLOR_MENU_INSIDE);

    m_aWaveTime[1].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aWaveTime[1].SetColor3(COLOR_MENU_INSIDE);

    m_aWaveTime[2].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aWaveTime[2].SetColor3(COLOR_MENU_INSIDE * 0.75f);

    m_SegmentName.Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_SegmentName.SetColor3(COLOR_MENU_INSIDE);

    m_SegmentBest.Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_SegmentBest.SetColor3(COLOR_MENU_INSIDE * 0.5f);

    m_aTurfBar[0].DefineTexture(0u, "menu_detail_02.png");
    m_aTurfBar[0].DefineProgram("default_2d_program");
    m_aTurfBar[0].SetSize      (coreVector2(7.0f,0.5f) * 0.07f);
    m_aTurfBar[0].SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_aTurfBar[1].DefineTexture(0u, "menu_detail_02.png");
    m_aTurfBar[1].DefineProgram("default_2d_program");
    m_aTurfBar[1].SetSize      (m_aTurfBar[0].GetSize() - coreVector2(0.01f,0.01f));

    m_aTurfBar[2].DefineTexture(0u, "menu_detail_02.png");
    m_aTurfBar[2].DefineProgram("default_2d_program");
    m_aTurfBar[2].SetSize      (m_aTurfBar[1].GetSize());

    m_TurfValue.Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_TurfValue.SetColor3(COLOR_MENU_INSIDE);

    m_GoalMedal.DefineTexture(0u, "menu_medal.png");
    m_GoalMedal.DefineProgram("default_2d_program");
    m_GoalMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_GoalTime.Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_GoalTime.SetColor3(COLOR_MENU_INSIDE * 0.75f);

    for(coreUintW i = 0u; i < INTERFACE_HELPERS; ++i)
    {
        m_aHelper[i].DefineTexture(0u, "menu_helper.png");
        m_aHelper[i].DefineProgram("menu_helper_program");
        m_aHelper[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aHelper[i].SetTexOffset (coreVector2(0.25f,0.25f) * coreVector2(I_TO_F(i % 4u), I_TO_F(i / 4u)));

        m_aHelperWave[i].DefineTexture(0u, m_aHelper[i].GetTexture(0u));
        m_aHelperWave[i].DefineProgram(m_aHelper[i].GetProgram  ());
        m_aHelperWave[i].SetTexSize   (m_aHelper[i].GetTexSize  ());
        m_aHelperWave[i].SetTexOffset (m_aHelper[i].GetTexOffset());
    }

    for(coreUintW i = 0u; i < INTERFACE_BADGES; ++i)
    {
        m_aBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aBadge[i].DefineProgram("default_2d_program");
        m_aBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aBadgeWave[i].DefineProgram("menu_single_program");
    }

    m_BannerBar.DefineTexture(0u, "menu_detail_04.png");
    m_BannerBar.DefineTexture(1u, "menu_background_black.png");
    m_BannerBar.DefineProgram("menu_animate_program");

    m_BannerShadow.DefineTexture(0u, "effect_headlight_point.png");
    m_BannerShadow.DefineProgram("menu_single_program");
    m_BannerShadow.SetPosition  (coreVector2(0.0f,0.0f));
    m_BannerShadow.SetSize      (coreVector2(1.0f,0.4f) * 0.75f);
    m_BannerShadow.SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBannerIcon); ++i)
    {
        m_aBannerIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aBannerIcon[i].DefineProgram("menu_helper_program");
        m_aBannerIcon[i].SetPosition  (coreVector2(0.0f, 0.0f));
        m_aBannerIcon[i].SetCenter    (coreVector2(0.0f, 0.02f));
        m_aBannerIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
    }

    m_aBannerText[0].Construct    (MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
    m_aBannerText[0].SetColor3    (COLOR_MENU_INSIDE * 0.75f);
    m_aBannerText[1].Construct    (MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
    m_aBannerText[1].SetColor3    (COLOR_MENU_INSIDE * 0.75f);
    m_aBannerText[2].DefineTexture(2u, "menu_background_black.png");
    m_aBannerText[3].DefineTexture(2u, "menu_background_black.png");

    m_aStoryText[0].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aStoryText[0].SetColor3(COLOR_MENU_INSIDE);
    m_aStoryText[1].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aStoryText[1].SetColor3(COLOR_MENU_INSIDE);

    m_aDialogText[0].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aDialogText[0].SetColor3(COLOR_MENU_INSIDE);
    m_aDialogText[1].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aDialogText[1].SetColor3(COLOR_MENU_INSIDE);

    for(coreUintW i = 0u; i < INTERFACE_FRAGMENTS; ++i)
    {
        const coreVector2 vDir  = FRAGMENT_DIRECTION;
        const coreVector2 vSize = g_aFragmentData[i].vSize * INTERFACE_FRAGMENT_SCALE * ((i == 8u) ? 1.1f : 1.0f);

        m_aFragment[i].DefineTexture(0u, PRINT("menu_fragment_%02zu.png", g_aFragmentData[i].iIndex));
        m_aFragment[i].DefineTexture(1u, "menu_background_black.png");
        m_aFragment[i].DefineProgram("menu_fragment_program");
        m_aFragment[i].SetSize      (vSize);
        m_aFragment[i].SetDirection (vDir);
        m_aFragment[i].SetColor3    (g_aFragmentData[i].vColor);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFragmentTable); ++i)
    {
        m_aFragmentTable[i].DefineTexture(0u, "menu_fragment_plate.png");
        m_aFragmentTable[i].DefineProgram("menu_single_program");
        m_aFragmentTable[i].SetPosition  (coreVector2(0.0f,0.0f));
        m_aFragmentTable[i].SetSize      (coreVector2(1.0f,1.0f) * INTERFACE_FRAGMENT_SCALE);
        m_aFragmentTable[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
        m_aFragmentTable[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.0f);
    }

    m_FragmentShadow.DefineTexture(0u, "effect_headlight_point.png");
    m_FragmentShadow.DefineProgram("menu_single_program");
    m_FragmentShadow.SetPosition(coreVector2(0.0f,0.0f));
    m_FragmentShadow.SetSize    (coreVector2(1.0f,1.0f) * 0.75f);
    m_FragmentShadow.SetColor3  (coreVector3(1.0f,1.0f,1.0f));

    m_Medal.DefineTexture(0u, "menu_medal.png");
    m_Medal.DefineProgram("default_2d_program");
    m_Medal.SetTexSize   (coreVector2(0.25f,0.25f));

    // 
    m_pAlertSound = Core::Manager::Resource->Get<coreSound>("effect_alert.wav");

    // 
    this->ChangeLanguage(Core::Language);
}


// ****************************************************************
// destructor
cInterface::~cInterface()
{
    if(m_pAlertSound->EnableRef(this))
    {
        m_pAlertSound->Stop();
    }
}


// ****************************************************************
// render the interface
void cInterface::Render()
{
    if(this->IsBannerActive())
    {
        // 
        cMenu::UpdateAnimateProgram(&m_BannerBar);

        // render banner
        m_BannerBar     .Render();
        m_aBannerIcon[0].Render();
        m_aBannerIcon[1].Render();
        m_BannerShadow  .Render();
        m_aBannerText[0].Render();
        m_aBannerText[1].Render();
        m_aBannerText[2].Render();
        m_aBannerText[3].Render();

        // 
        m_Medal.Render();
    }

    if(this->IsFragmentActive())
    {
        // 
        m_FragmentShadow   .Render();
        m_aFragmentTable[0].Render();
        m_aFragmentTable[1].Render();
        for(coreUintW i = 0u; i < INTERFACE_FRAGMENTS; ++i) m_aFragment[i].Render();
    }

    if(m_fAlphaAll)
    {
        // render player images
        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)                                                 m_aView[i].oCooldownBar .Render();   // # first
        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i) for(coreUintW j = 0u; j < INTERFACE_LIVES; ++j) m_aView[i].aLife     [j].Render();
        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i) for(coreUintW j = 0u; j < 3u;              ++j) m_aView[i].aShieldBar[j].Render();

        if(m_fAlphaBoss)
        {
            // render boss images
            m_aBossHealthBar[0].Render();
            m_aBossHealthBar[1].Render();
            m_aBossHealthBar[2].Render();
        }

        if(m_fAlphaTurf)
        {
            // 
            m_aTurfBar[0].Render();
            m_aTurfBar[1].Render();
            m_aTurfBar[2].Render();
        }

        // 
        m_GoalMedal.Render();

        // 
        if(m_fAlphaBoss)
        {
            for(coreUintW i = 0u; i < INTERFACE_HELPERS; ++i) m_aHelperWave[i].Render();
            for(coreUintW i = 0u; i < INTERFACE_HELPERS; ++i) m_aHelper    [i].Render();
        }

        // 
        if(m_fAlphaWave)
        {
            for(coreUintW i = 0u; i < INTERFACE_BADGES; ++i) m_aBadgeWave[i].Render();
            for(coreUintW i = 0u; i < INTERFACE_BADGES; ++i) m_aBadge    [i].Render();
        }

        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        {
            // render player labels
            m_aView[i].oShieldValue.Render();
            m_aView[i].oScore      .Render();
            m_aView[i].oComboValue .Render();
            m_aView[i].oChainValue .Render();
            m_aView[i].oImmune     .Render();
        }

        if(m_fAlphaBoss)
        {
            // render boss labels
            m_BossHealthValue.Render();
            m_aBossTime[0]   .Render();
            m_aBossTime[1]   .Render();
            m_aBossTime[2]   .Render();
        }

        if(m_fAlphaWave)
        {
            // render wave labels
            m_aWaveTime[0].Render();
            m_aWaveTime[1].Render();
            m_aWaveTime[2].Render();
        }

        // 
        m_SegmentName.Render();
        m_SegmentBest.Render();

        if(m_fAlphaTurf)
        {
            // 
            m_TurfValue.Render();
        }

        // 
        m_GoalTime.Render();
    }

    if(this->IsStoryActive())
    {
        // render story
        m_aStoryText[0].Render();
        m_aStoryText[1].Render();
    }

    // 
    m_aDialogText[0].Render();
    m_aDialogText[1].Render();
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();

    // 
    m_fAnimation.UpdateMod(6.0f*PI, 2.0f*PI);
    const coreFloat fDanger = g_CurConfig.Graphics.iFlash ? (0.5f + 0.5f * SIN(m_fAnimation)) : 0.0f;

    // 
    m_fRotation.UpdateMod(1.0f, 2.0f*PI);

    // 
    const coreFloat fAlphaPlayerFull  = BLENDH3(m_fAlphaAll) * MENU_INSIDE_ALPHA;
    const coreFloat fAlphaBossFull    = fAlphaPlayerFull * BLENDH3(m_fAlphaBoss);
    const coreFloat fAlphaWaveFull    = fAlphaPlayerFull * BLENDH3(m_fAlphaWave);
    const coreFloat fAlphaSegmentFull = fAlphaPlayerFull * BLENDH3(m_fAlphaSegment);
    const coreFloat fAlphaTurfFull    = fAlphaPlayerFull * BLENDH3(m_fAlphaTurf);
    
    this->UpdateSpacing();   // TODO 1: sollte nicht ständig aufgerufen werden
    
    
    const coreVector2 vFlip = (g_CurConfig.Game.iHudType == 2u) ? (cInterface::__IsFlipped() ? coreVector2(-1.0f,1.0f) : coreVector2(1.0f,-1.0f)) : coreVector2(1.0f,1.0f);
    const coreVector2 vAdd  = coreVector2((vFlip.x < 0.0f) ? 0.02f : 0.0f, (vFlip.y < 0.0f) ? 0.02f : 0.0f);
    
    const auto nFlipFunc = [&](const coreVector2 vBasePos, const coreObject2D* pObject)
    {
        const coreVector2 vAlign = pObject->GetAlignment();
        return coreVector2(vAlign.x ? ((ABS(vBasePos.x) + vAdd.x) * vAlign.x) : vBasePos.x,
                           vAlign.y ? ((ABS(vBasePos.y) + vAdd.y) * vAlign.y) : vBasePos.y);
    };

    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView   = m_aView[i];
        cPlayer*     pPlayer = g_pGame->GetPlayer(i);

        if(pPlayer->HasStatus(PLAYER_STATUS_SHIELDED))
        {
            if(oView.iShieldOld != pPlayer->GetCurShield()) oView.fShieldBump = 1.0f;
            oView.iShieldOld = pPlayer->GetCurShield();

            oView.fShieldBump.UpdateMax(-3.0f, 0.0f);
            const coreFloat fBump = BLENDBR(oView.fShieldBump);

            // 
            if(m_fAlphaAll) oView.fSpin2.UpdateMin(1.0f, 1.0f);
                       else oView.fSpin2 = 0.0f;

            // set shield bar size
            const coreFloat fBase    = pPlayer->GetCurShieldPct();
            const coreFloat fPercent = fBase * oView.fSpin2;
            const coreFloat fWidth   = oView.aShieldBar[0].GetSize().x - 0.01f;
            oView.aShieldBar[1].SetSize     (coreVector2(fPercent * fWidth, oView.aShieldBar[1].GetSize().y));
            oView.aShieldBar[1].SetTexSize  (coreVector2(fPercent, 1.0f));
            oView.aShieldBar[1].SetTexOffset(coreVector2((oView.aShieldBar[1].GetAlignment().x < 0.0f) ? (1.0f-fPercent) : 0.0f, 0.0f));

            const coreFloat fRev = 1.0f - fPercent;
            oView.aShieldBar[2].SetPosition (coreVector2(fWidth * (fPercent) * oView.aShieldBar[2].GetAlignment().x + oView.aShieldBar[1].GetPosition().x, oView.aShieldBar[2].GetPosition().y));
            oView.aShieldBar[2].SetSize     (coreVector2(fWidth *  fRev,                                                                                   oView.aShieldBar[2].GetSize    ().y));
            oView.aShieldBar[2].SetTexSize  (coreVector2(fRev, 1.0f));
            oView.aShieldBar[2].SetTexOffset(coreVector2((oView.aShieldBar[2].GetAlignment().x < 0.0f) ? 0.0f : fPercent, 1.0f));

            // set shield bar color
            const coreVector3 vColor = LERP((pPlayer->GetEnergyColor() * 2.3f) * 0.95f * 0.95f + ((fBase <= 0.2f) ? (fDanger * 0.5f) : 0.0f), coreVector3(1.0f,1.0f,1.0f), fBump);
            oView.aShieldBar[1].SetColor3(vColor * 1.0f);
            oView.aShieldBar[2].SetColor3(vColor * 0.3f);

            oView.aShieldBar[0].SetColor3(LERP(coreVector3(0.0f,0.0f,0.0f), coreVector3(1.0f,1.0f,1.0f) * 0.6f, fBump));

            // display shield value
            oView.oShieldValue.SetText(PRINT("%.0f", I_TO_F(pPlayer->GetCurShield()) * oView.fSpin2));
        }

        // 
        const coreFloat fCurHealth = I_TO_F(pPlayer->GetCurHealth());
             if(!m_fAlphaAll)             oView.fSpin = 0.0f;
        else if(fCurHealth > oView.fSpin) oView.fSpin.UpdateMin( 8.0f, fCurHealth);
        else if(fCurHealth < oView.fSpin) oView.fSpin.UpdateMax(-8.0f, fCurHealth);

        // 
        for(coreUintW j = 0u; j < INTERFACE_LIVES; ++j)
        {
            // 
            //const coreVector2 vSide = coreVector2(i ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vPos  = coreVector2(0.007f + I_TO_F(j) * 0.055f, 0.005f);
            const coreVector2 vSize = coreVector2(1.0f,1.0f) * 0.058f;

            // 
            const coreFloat   fCurSpin = CLAMP01(oView.fSpin - I_TO_F(j));
            const coreVector2 vNewSize = vSize * (1.5f - 0.5f * fCurSpin);
            const coreVector2 vNewPos  = nFlipFunc(vPos, &oView.aLife[j]) + 0.5f * (vSize - vNewSize) * oView.aLife[j].GetAlignment();
            const coreVector2 vNewDir  = coreVector2::Direction(m_fRotation + (0.8f*PI) * (I_TO_F(j) / I_TO_F(INTERFACE_LIVES)));

            // 
            oView.aLife[j].SetPosition (vNewPos);
            oView.aLife[j].SetSize     (vNewSize);
            oView.aLife[j].SetDirection(vNewDir);
            oView.aLife[j].SetAlpha    (fAlphaPlayerFull * fCurSpin);
            oView.aLife[j].SetEnabled  (fCurSpin ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            oView.aLife[j].Move();
        }

        const cScoreTable* pScoreTable = pPlayer->GetScoreTable();

        // display score
        oView.oScore.SetText(PRINT("%07u", pScoreTable->GetScoreTotal()));

        const coreUint32 iModifier  = pScoreTable->GetModifier();
        const coreBool   bShowValue = (iModifier > (pScoreTable->HasOverride() ? 0u : 10u));
        const coreBool   bShowBar   = bShowValue && (!pScoreTable->HasOverride() || pScoreTable->GetCurChain());

        // 
        oView.oCooldownBar.SetSize  (coreVector2(bShowBar ? (0.15f * MIN(pScoreTable->GetCooldown() * 1.1f, 1.0f)) : 0.0f, 0.013f));
        oView.oCooldownBar.SetColor3((pScoreTable->GetCooldown() > 0.5f) ? pPlayer->GetMenuColor() : COLOR_MENU_RED);

        // 
        oView.oComboValue.SetText(bShowValue ? PRINT("x%u.%u", iModifier / 10u, iModifier % 10u) : "");
        oView.oChainValue.SetText(pScoreTable->GetCurChain() ? PRINT("+%u", pScoreTable->GetCurChain()) : "");

        // 
        oView.fImmuneTime.UpdateMax(-1.0f, 0.0f);
        const coreFloat fImmuneValue = BLENDB(1.0f - oView.fImmuneTime);

        // 
        oView.oImmune.SetPosition(MapToAxis(GetTranslationArea(oView.aLife[2]), g_vHudDirection) + coreVector2(0.0f, 0.05f * fImmuneValue));
        oView.oImmune.SetEnabled (oView.fImmuneTime ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        // set player transparency
        oView.aShieldBar[0].SetAlpha(fAlphaPlayerFull);
        oView.aShieldBar[1].SetAlpha(fAlphaPlayerFull);
        oView.aShieldBar[2].SetAlpha(fAlphaPlayerFull);
        oView.oShieldValue .SetAlpha(fAlphaPlayerFull);
        oView.oScore       .SetAlpha(fAlphaPlayerFull);
        oView.oCooldownBar .SetAlpha(fAlphaPlayerFull);
        oView.oComboValue  .SetAlpha(fAlphaPlayerFull);
        oView.oChainValue  .SetAlpha(fAlphaPlayerFull);
        oView.oImmune      .SetAlpha(fAlphaPlayerFull * (1.0f - fImmuneValue));

        // move player
        oView.oShieldValue .Move();
        oView.aShieldBar[0].Move();
        oView.aShieldBar[1].Move();
        oView.aShieldBar[2].Move();
        oView.oScore       .Move();
        oView.oCooldownBar .Move();
        oView.oComboValue  .Move();
        oView.oChainValue  .Move();
        oView.oImmune      .Move();
    }

    // check for active boss
    const cBoss* pBoss = g_pGame->GetCurMission()->GetCurBoss();
    if(pBoss)
    {
        // 
        if(m_fAlphaBoss) m_fBossSpin.UpdateMin(1.0f / (INTERFACE_BANNER_DURATION_BOSS - INTERFACE_BOSS_DELAY), 1.0f);
                    else m_fBossSpin = 0.0f;

        // set health bar size
        const coreFloat fBase    = pBoss->GetCurHealthPct();
        const coreFloat fPercent = fBase * m_fBossSpin;
        const coreFloat fWidth   = m_aBossHealthBar[0].GetSize().x - 0.01f;
        m_aBossHealthBar[1].SetPosition(coreVector2(fWidth * (fPercent-1.0f) * 0.5f, m_aBossHealthBar[1].GetPosition().y));
        m_aBossHealthBar[1].SetSize    (coreVector2(fWidth *  fPercent,              m_aBossHealthBar[1].GetSize    ().y));
        m_aBossHealthBar[1].SetTexSize (coreVector2(fPercent, 1.0f));

        const coreFloat fRev = 1.0f - fPercent;
        m_aBossHealthBar[2].SetPosition (coreVector2(fWidth * fPercent * 0.5f, m_aBossHealthBar[2].GetPosition().y));
        m_aBossHealthBar[2].SetSize     (coreVector2(fWidth * fRev,            m_aBossHealthBar[2].GetSize    ().y));
        m_aBossHealthBar[2].SetTexSize  (coreVector2(fRev,     1.0f));
        m_aBossHealthBar[2].SetTexOffset(coreVector2(fPercent, 1.0f));

        const coreVector3 vColor = LERP(coreVector3(1.0f,1.0f,1.0f), pBoss->GetColor(), cShip::TransformColorFactor(fBase)) + ((fBase <= 0.2f) ? (fDanger * 0.5f) : 0.0f);
        m_aBossHealthBar[1].SetColor3(vColor * 1.0f);
        m_aBossHealthBar[2].SetColor3(vColor * 0.3f);

        // display health value
        m_BossHealthValue.SetText(PRINT("%.0f%%", fPercent ? FLOOR(LERP(1.0f, 100.0f, fPercent)) : 0.0f));
    }

    coreBool bTurf = false;
    if(g_pGame->GetCurMission()->GetID() == cAterMission::ID)
    {
        const cTurf* pTurf = d_cast<cAterMission*>(g_pGame->GetCurMission())->GetTurf();

        bTurf = pTurf->IsActive();
        if(bTurf)
        {
            // 
            const coreFloat fPercent = pTurf->CalcPercent();
            const coreFloat fWidth   = m_aTurfBar[0].GetSize().x - 0.01f;
            m_aTurfBar[1].SetPosition(coreVector2(fWidth * (fPercent-1.0f) * 0.5f, m_aTurfBar[1].GetPosition().y));
            m_aTurfBar[1].SetSize    (coreVector2(fWidth *  fPercent,              m_aTurfBar[1].GetSize    ().y));
            m_aTurfBar[1].SetTexSize (coreVector2(fPercent, 1.0f));

            const coreFloat fRev = 1.0f - fPercent;
            m_aTurfBar[2].SetPosition(coreVector2(fWidth * fPercent * 0.5f, m_aTurfBar[2].GetPosition().y));
            m_aTurfBar[2].SetSize    (coreVector2(fWidth * fRev,            m_aTurfBar[2].GetSize    ().y));
            m_aTurfBar[2].SetTexSize (coreVector2(fRev, 1.0f));
            m_aTurfBar[2].SetTexOffset (coreVector2(fPercent, 1.0f));

            const coreVector3 vColor = coreVector3(1.0f,1.0f,1.0f);// + ((fPercent <= 0.4f) ? (fDanger * 0.5f) : 0.0f);
            m_aTurfBar[1].SetColor3(vColor * 1.0f);
            m_aTurfBar[2].SetColor3(vColor * 0.3f);

            // 
            m_TurfValue.SetText(PRINT("%.0f%%", fPercent ? FLOOR(LERP(1.0f, 100.0f, fPercent)) : 0.0f));
        }
    }

    // display time
    const coreFloat fTime = g_pGame->GetTimeTable()->GetTimeSegmentSafe();
    if(fTime)
    {
        m_aBossTime[0].SetText(PRINT("%.0f.", FLOOR(      fTime)));
        m_aBossTime[1].SetText(PRINT("%.0f",  FLOOR(FRACT(fTime)*10.0f)));
        m_aWaveTime[0].SetText(m_aBossTime[0].GetText());
        m_aWaveTime[1].SetText(m_aBossTime[1].GetText());
    }
    else
    {
        if(!fAlphaBossFull && !fAlphaWaveFull)
        {
            m_aBossTime[0].SetText("0.");
            m_aBossTime[1].SetText("0");
            m_aBossTime[2].SetText(" +0");

            m_aWaveTime[0].SetText("0.");
            m_aWaveTime[1].SetText("0");
            m_aWaveTime[2].SetText(" +0");
        }
    }

    // 
    const coreInt32 iShift = g_pGame->GetTimeTable()->GetShiftSegmentSafe();
    if(fTime)
    {
        m_aBossTime[2].SetText(PRINT(" %+d", iShift));
        m_aWaveTime[2].SetText(m_aBossTime[2].GetText());
    }
    
    
    if(((pBoss ? fAlphaBossFull : fAlphaWaveFull) || m_bFakeEnd) && !m_bBossChange)
    {
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            const auto&     oStats     = g_pSave->GetHeader().aaaaaLocalStatsSegment[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()][iMissionIndex][iSegmentIndex];
            const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
            const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

            m_SegmentBest.SetText(fBestTime ? PRINT("%.1f %+d", fBestTime, iBestShift) : "");
            
            m_fAlphaSegment.UpdateMin(2.0f, 1.0f);
        }
    }
    else
    {
        m_fAlphaSegment.UpdateMax(-2.0f, 0.0f);
    }

    // adjust time position (# only required if alignment is centered)    
    m_aBossTime[0].RetrieveDesiredSize([this](const coreVector2 vSize)
    {
        coreFloat fWidth = 0.0f;
        m_aBossTime[2].RetrieveDesiredSize([&](const coreVector2 vSize) {fWidth = vSize.x;});

        const coreFloat fPos = (vSize.x - 0.015f - fWidth) * 0.5f;
        m_aBossTime[0].SetPosition(coreVector2(fPos,          m_aBossTime[0].GetPosition().y));
        m_aBossTime[1].SetPosition(coreVector2(fPos - 0.004f, m_aBossTime[1].GetPosition().y));
        m_aBossTime[2].SetPosition(coreVector2(fPos + 0.015f, m_aBossTime[2].GetPosition().y));
    });
    m_aWaveTime[0].RetrieveDesiredSize([this](const coreVector2 vSize)
    {
        coreFloat fWidth = 0.0f;
        m_aWaveTime[2].RetrieveDesiredSize([&](const coreVector2 vSize) {fWidth = vSize.x;});

        const coreFloat fPos = (vSize.x - 0.015f - fWidth) * 0.5f;
        m_aWaveTime[0].SetPosition(coreVector2(fPos,          m_aWaveTime[0].GetPosition().y));
        m_aWaveTime[1].SetPosition(coreVector2(fPos - 0.004f, m_aWaveTime[1].GetPosition().y));
        m_aWaveTime[2].SetPosition(coreVector2(fPos + 0.015f, m_aWaveTime[2].GetPosition().y));
    });

    // set boss transparency
    m_aBossHealthBar[0].SetAlpha(fAlphaBossFull);
    m_aBossHealthBar[1].SetAlpha(fAlphaBossFull);
    m_aBossHealthBar[2].SetAlpha(fAlphaBossFull);
    m_BossHealthValue  .SetAlpha(fAlphaBossFull);
    m_aBossTime[0]     .SetAlpha(fAlphaBossFull);
    m_aBossTime[1]     .SetAlpha(fAlphaBossFull);
    m_aBossTime[2]     .SetAlpha(fAlphaBossFull);

    // move boss
    m_aBossHealthBar[0].Move();
    m_aBossHealthBar[1].Move();
    m_aBossHealthBar[2].Move();
    m_BossHealthValue  .Move();
    m_aBossTime[0]     .Move();
    m_aBossTime[1]     .Move();
    m_aBossTime[2]     .Move();

    // set wave transparency
    m_aWaveTime[0].SetAlpha(fAlphaWaveFull);
    m_aWaveTime[1].SetAlpha(fAlphaWaveFull);
    m_aWaveTime[2].SetAlpha(fAlphaWaveFull);

    // move wave
    m_aWaveTime[0].Move();
    m_aWaveTime[1].Move();
    m_aWaveTime[2].Move();

    // 
    m_SegmentName.SetAlpha(fAlphaSegmentFull);
    m_SegmentBest.SetAlpha(fAlphaSegmentFull);

    // 
    m_SegmentName.Move();
    m_SegmentBest.Move();

    // 
    m_aTurfBar[0].SetAlpha(fAlphaTurfFull);
    m_aTurfBar[1].SetAlpha(fAlphaTurfFull);
    m_aTurfBar[2].SetAlpha(fAlphaTurfFull);
    m_TurfValue  .SetAlpha(fAlphaTurfFull);

    // 
    m_aTurfBar[0].Move();
    m_aTurfBar[1].Move();
    m_aTurfBar[2].Move();
    m_TurfValue  .Move();
    
    
    
    

    const coreFloat* pfMedalGoal  = g_pGame->GetCurMission()->GetMedalGoal();
    const coreFloat  fTimeShifted = g_pGame->GetTimeTable ()->GetTimeShiftedSegmentSafe();
    if(pfMedalGoal && fTime)
    {
        const coreUint8 iNewMedal = cGame::CalcMedal(fTimeShifted, pfMedalGoal);

        if(m_iGoalOld != iNewMedal) m_fGoalBump = 1.0f;
        m_iGoalOld = iNewMedal;

        cMenu::ApplyMedalTexture(&m_GoalMedal, iNewMedal, pBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);
        m_GoalTime.SetText(PRINT("%03.0f", CEIL(cGame::CalcMedalTime(fTimeShifted, pfMedalGoal))));

        m_fAlphaGoal = 1.0f;
    }
    else
    {
        m_iGoalOld = MEDAL_NONE;
        m_fAlphaGoal.UpdateMax(-2.0f, 0.0f);
    }

    if(fTime)
    {
        m_fGoalBump.UpdateMax(-4.0f, 0.0f);
        m_GoalMedal.SetSize(coreVector2(1.0f,1.0f) * LERPBR(0.04f, 0.06f, m_fGoalBump));

        const coreObject2D* pBase = pBoss ? &m_aBossTime[0] : &m_aWaveTime[0];
        const coreVector2   vPos  = coreVector2(-0.12f, pBase->GetPosition().y);

        m_GoalTime .SetPosition(vPos);
        m_GoalMedal.SetPosition(vPos + coreVector2(-0.06f,0.0f) - m_GoalMedal.GetAlignment() * (m_GoalMedal.GetSize() - coreVector2(0.04f, pBase->GetSize().y)) * 0.5f);
    }
    
    
    
    // 
    const coreFloat fAlphaGoalFull = MAX(fAlphaWaveFull, fAlphaBossFull) * BLENDH3(m_fAlphaGoal);
    m_GoalMedal.SetAlpha(fAlphaGoalFull);
    m_GoalTime .SetAlpha(fAlphaGoalFull);

    // 
    m_GoalMedal.Move();
    m_GoalTime .Move();
    
    

    for(coreUintW i = 0u; i < INTERFACE_HELPERS; ++i)
    {
        const coreBool bState = (pBoss && HAS_BIT(pBoss->GetHelperHit(), i));
        
        coreVector3 vColor;
        switch(i)
        {
        default: ASSERT(false)
        case 0u: vColor = COLOR_MENU_YELLOW;  break;
        case 1u: vColor = COLOR_MENU_ORANGE;  break;
        case 2u: vColor = COLOR_MENU_RED;     break;
        case 3u: vColor = COLOR_MENU_MAGENTA; break;
        case 4u: vColor = COLOR_MENU_PURPLE;  break;
        case 5u: vColor = COLOR_MENU_BLUE;    break;
        case 6u: vColor = COLOR_MENU_CYAN;    break;
        case 7u: vColor = COLOR_MENU_GREEN;   break;
        }
        
        if(bState && !HAS_BIT(m_iHelperState, i))
        {
            m_afHelperBump[i] = 1.0f;
        }
        if(iSegmentIndex != MISSION_NO_SEGMENT) SET_BIT(m_iHelperState, i, bState)
        const coreBool bRealState = HAS_BIT(m_iHelperState, i);
        
        m_afHelperBump[i].UpdateMax(-1.5f, 0.0f);
        
        const coreFloat fBump = BLENDBR(m_afHelperBump[i]);
        

        coreVector2 vBasePos = coreVector2(-0.013f - 0.02f * SIN(fBump * (1.0f*PI)), m_aHelper[i].GetAlignment().y ? (0.011f + I_TO_F(INTERFACE_HELPERS - i - 1u) * 0.06f) : ((I_TO_F(INTERFACE_HELPERS - i - 1u) - I_TO_F(INTERFACE_HELPERS - 1u) * 0.5f) * 0.06f));
        
        if(!cInterface::__IsFlipped() && m_aHelper[i].GetAlignment().y) vBasePos = vBasePos.yx();
        
        // 
        m_aHelper[i].SetPosition (nFlipFunc(vBasePos, &m_aHelper[i]));
        m_aHelper[i].SetSize     (coreVector2(1.0f,1.0f) * 0.045f);
        m_aHelper[i].SetDirection(coreVector2::Direction((bRealState ? (m_fRotation + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS))) : 0.0f) + fBump * (-1.0f*PI)));
        m_aHelper[i].SetColor3   (bRealState ? vColor : coreVector3(1.0f,1.0f,1.0f));
        m_aHelper[i].SetAlpha    (fAlphaBossFull * (bRealState ? 1.0f : 0.5f));
        m_aHelper[i].Move();

        // 
        m_aHelperWave[i].SetPosition (m_aHelper[i].GetPosition() - m_aHelperWave[i].GetAlignment() * m_aHelper[i].GetSize() * (bRealState ? (0.2f + fBump) : 0.0f) * 0.5f);
        m_aHelperWave[i].SetSize     (m_aHelper[i].GetSize() * (1.2f + fBump));
        m_aHelperWave[i].SetDirection(coreVector2::Direction(m_fRotation * -0.5f + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS)) + fBump * (1.0f*PI)));
        m_aHelperWave[i].SetColor3   (m_aHelper[i].GetColor3());
        m_aHelperWave[i].SetAlpha    (fAlphaBossFull * (bRealState ? (0.5f * (1.0f - fBump)) : 0.0f));
        m_aHelperWave[i].Move();

        // 
        if(pBoss)
        {
            const coreBool bVisible = (pBoss->GetID() != cIntroBoss::ID) && (pBoss->GetID() != cProjectOneBoss::ID) && (pBoss->GetID() != cEigengrauBoss::ID);
            m_aHelper    [i].SetEnabled(bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aHelperWave[i].SetEnabled(bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        }
    }
    
    if(m_fAlphaWave >= 1.0f)   // # not full
    {
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            m_fAlphaBadge.UpdateMin(2.0f, 1.0f);
        }
    }
    else
    {
        m_fAlphaBadge.UpdateMax(-2.0f, 0.0f);
    }
    const coreFloat fAlphaBadgeFull = fAlphaPlayerFull * BLENDH3(m_fAlphaBadge);
    
    for(coreUintW i = 0u; i < INTERFACE_BADGES; ++i)
    {
        const coreBool bState = (iSegmentIndex != MISSION_NO_SEGMENT) && g_pGame->GetPlayer(0u)->GetDataTable()->GetBadge(i, iMissionIndex, iSegmentIndex);
        
        if(bState && !HAS_BIT(m_iBadgeState, i))
        {
            m_afBadgeBump[i] = 1.0f;
        }
        if((iSegmentIndex != MISSION_NO_SEGMENT) && !MISSION_SEGMENT_IS_BOSS(iSegmentIndex)) SET_BIT(m_iBadgeState, i, bState)
        const coreBool bRealState = HAS_BIT(m_iBadgeState, i);
        
        m_afBadgeBump[i].UpdateMax(-1.0f, 0.0f);
        
        const coreFloat fBump = BLENDBR(m_afBadgeBump[i]);
        
        
        const coreBool bIntro = (g_pGame->GetCurMission()->GetID() == cIntroMission::ID);
        const coreUintW iNum = bIntro ? 1u : INTERFACE_BADGES;
        
        const coreFloat fOffset = m_aBadge[i].GetAlignment().x ? ((0.007f + I_TO_F((m_aBadge[i].GetAlignment().x > 0.0f) ? i : (iNum - i - 1u)) * 0.06f) * SIGN(m_aBadge[i].GetAlignment().x)) : ((I_TO_F(i) - I_TO_F(iNum - 1u) * 0.5f) * 0.06f);
        
        const coreVector2 vNewSize = coreVector2(1.0f,1.0f) * LERP(0.063f, 0.095f, fBump);
        
        m_aBadge[i].SetPosition (nFlipFunc(coreVector2(fOffset, 0.005f), &m_aBadge[i]) - m_aBadge[i].GetAlignment() * (vNewSize - 0.063f) * 0.5f);
        m_aBadge[i].SetSize     (vNewSize);
        m_aBadge[i].SetDirection(coreVector2::Direction(m_fRotation + (2.0f*PI) * fBump + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_BADGES))));
        m_aBadge[i].SetTexOffset(coreVector2(bRealState ? 0.0f : 0.5f, 0.0f));
        m_aBadge[i].SetAlpha    (fAlphaBadgeFull * (bRealState ? 1.0f : 0.5f));
        m_aBadge[i].Move();
        
        const coreFloat fScale = 0.6f + 0.2f * SIN(m_fRotation * 4.0f);

        m_aBadgeWave[i].SetPosition(m_aBadge[i].GetPosition() - m_aBadgeWave[i].GetAlignment() * vNewSize * ((0.7f - 1.0f) * 0.5f));
        m_aBadgeWave[i].SetSize    (m_aBadge[i].GetSize() * 0.7f);
        m_aBadgeWave[i].SetAlpha   (m_aBadge[i].GetAlpha() * (bRealState ? (1.0f * (1.0f - fBump) * fScale) : 0.0f));
        m_aBadgeWave[i].Move();

        
        // 
        m_aBadge    [i].SetEnabled((!bIntro || bRealState) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aBadgeWave[i].SetEnabled((!bIntro || bRealState) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // check for active banner
    const coreFloat fBanner = g_pGame->GetTimeTable()->GetTimeEvent() - m_fBannerStart;
    if((fBanner <= m_fBannerDuration) && (fBanner >= 0.0f))
    {
        // calculate visibility and animation value
        const coreFloat fVisibility = MIN(fBanner, m_fBannerDuration - fBanner, RCP(m_fBannerSpeed)) * m_fBannerSpeed;
        const coreFloat fAnimation  = BLENDB(MIN1(fBanner / INTERFACE_BANNER_ANIMATION)) * INTERFACE_BANNER_ANIMATION;

        // slash banner bar across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
        const coreBool bLeftRight = (fBanner < (m_fBannerDuration * 0.5f)) ? false : true;
        m_BannerBar.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_BannerBar.GetDirection().yx());
        m_BannerBar.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

        // animate banner bar
        m_BannerBar.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.26f));
        m_BannerBar.SetTexSize  (coreVector2(fVisibility, 1.0f));
        m_BannerBar.SetTexOffset(coreVector2(1.0f,1.0f) * (fBanner * 0.15f));

        // animate banner text
        const coreFloat fTextOffset = (fAnimation + 2.0f) * 0.034f;
        m_aBannerText[0].SetPosition(coreVector2( fTextOffset,  0.029f));
        m_aBannerText[1].SetPosition(coreVector2(-fTextOffset, -0.029f));

        // 
        if(m_iBannerType == INTERFACE_BANNER_TYPE_ALERT)
        {
            m_aBannerText[3].SetColor3(COLOR_MENU_INSIDE * LERP(MENU_LIGHT_ACTIVE, MENU_LIGHT_IDLE, fDanger));

            m_fShake.Update(30.0f);
            if(m_fShake >= 1.0f)
            {
                m_fShake = FRACT(m_fShake);
                m_aBannerText[3].SetPosition(coreVector2::Rand(1.0f) * (g_CurConfig.Graphics.iShake ? 0.0016f : 0.0f));   // only on/off
            }
        }

        // 
        if((m_iBannerType == INTERFACE_BANNER_TYPE_MISSION) || (m_iBannerType == INTERFACE_BANNER_TYPE_BOSS))
        {
            m_aBannerText[2].SetTexOffset(bLeftRight ? coreVector2(-1.0f,0.0f) : coreVector2(1.0f,0.0f));
            m_aBannerText[3].SetTexOffset(bLeftRight ? coreVector2(-1.0f,0.0f) : coreVector2(1.0f,0.0f));
        }

        // 
        m_aBannerIcon[0].SetSize     (coreVector2(1.0f,1.0f) * 0.45f * LERPB(0.85f, 1.0f, fVisibility));
        m_aBannerIcon[1].SetSize     (coreVector2(1.0f,1.0f) * 0.35f * LERPB(0.85f, 1.0f, fVisibility));
        m_aBannerIcon[0].SetDirection(coreVector2::Direction(fBanner *  (0.2f*PI)));
        m_aBannerIcon[1].SetDirection(coreVector2::Direction(fBanner * (-0.1f*PI)));

        // 
        m_Medal.SetPosition(coreVector2(0.0f, LERPB(-0.07f, -0.04f, MIN1(fBanner * 0.8f))));
        m_Medal.SetSize    (coreVector2(0.135f,0.135f) * LERP(1.5f, 1.0f, MIN1(fBanner * 10.0f)));

        // set banner transparency
        const coreFloat fBannerAlpha = BLENDH3(fVisibility) * MENU_INSIDE_ALPHA;
        m_BannerShadow  .SetAlpha(fBannerAlpha * 0.7f);
        m_aBannerIcon[0].SetAlpha(fBannerAlpha * 0.6f);
        m_aBannerIcon[1].SetAlpha(fBannerAlpha * 0.8f);
        m_aBannerText[0].SetAlpha(fBannerAlpha * 0.2f);
        m_aBannerText[1].SetAlpha(fBannerAlpha * 0.2f);
        m_aBannerText[2].SetAlpha(fBannerAlpha);   // TODO 1: MENU_INSIDE_ALPHA is falsch für BOSS und MISSION banner animation
        m_aBannerText[3].SetAlpha(fBannerAlpha);   // TODO 1: - || -
        m_Medal         .SetAlpha(fBannerAlpha);
        

        if(m_bBannerEigengrau)
        {
            m_aBannerText[2].RetrieveDesiredSize([&](const coreVector2 vSize1)
            {
                m_aBannerText[3].RetrieveDesiredSize([&](const coreVector2 vSize2)
                {
                    const coreFloat fDiff = vSize1.x - vSize2.x;

                    m_aBannerText[2].SetPosition(coreVector2(fDiff * 0.5f + 0.002f, m_aBannerText[2].GetPosition().y));
                    m_aBannerText[3].SetPosition(coreVector2(fDiff * 0.5f - 0.002f, m_aBannerText[3].GetPosition().y));

                    const coreFloat fValue = BLENDH3(fVisibility) * (vSize1.x + vSize2.x);
                    if(bLeftRight)
                    {
                        m_aBannerText[2].SetAlpha(STEP(vSize2.x, vSize1.x + vSize2.x, fValue));
                        m_aBannerText[3].SetAlpha(STEP(0.0f,     vSize2.x + 0.09f,    fValue));
                    }
                    else
                    {
                        m_aBannerText[2].SetAlpha(STEP(0.0f,             vSize1.x,            fValue));
                        m_aBannerText[3].SetAlpha(STEP(vSize1.x - 0.09f, vSize1.x + vSize2.x, fValue));
                    }
                });
            });
        }

        // move banner
        m_BannerBar     .Move();
        m_BannerShadow  .Move();
        m_aBannerIcon[0].Move();
        m_aBannerIcon[1].Move();
        m_aBannerText[0].Move();
        m_aBannerText[1].Move();
        m_aBannerText[2].Move();
        m_aBannerText[3].Move();
        m_Medal         .Move();
    }

    // check for active story
    const coreFloat fStory = g_pGame->GetTimeTable()->GetTimeEvent() - m_fStoryStart;
    if((fStory <= INTERFACE_STORY_DURATION) && (fStory >= 0.0f))
    {
        // 
        const coreFloat fVisibility = MIN(fStory, INTERFACE_STORY_DURATION - fStory, 1.0f / INTERFACE_STORY_SPEED) * INTERFACE_STORY_SPEED;

        // set story transparency
        const coreFloat fStoryAlpha = BLENDH3(fVisibility) * MENU_INSIDE_ALPHA;
        m_aStoryText[0].SetAlpha(fStoryAlpha);
        m_aStoryText[1].SetAlpha(fStoryAlpha);

        // move story
        m_aStoryText[0].Move();
        m_aStoryText[1].Move();
    }

    // 
    m_aDialogText[0].Move();
    m_aDialogText[1].Move();

    // 
    const coreFloat fFragment = g_pGame->GetTimeTable()->GetTimeEvent() - m_fFragmentStart;
    if((fFragment <= m_fFragmentDuration) && (fFragment >= 0.0f))
    {
        // 
        const coreBool bShow = (m_iFragmentNew == INTERFACE_FRAGMENT_TYPE_SHOW);

        // 
        const coreFloat fVisibility = bShow ? 1.0f : (MIN(fFragment, m_fFragmentDuration - fFragment, 1.0f / INTERFACE_FRAGMENT_SPEED) * INTERFACE_FRAGMENT_SPEED);
        const coreFloat fAnimation  = STEPBR(1.0f, 2.0f, fFragment);

        // 
        const coreFloat fFragmentAlpha = BLENDH3(fVisibility) * m_fAlphaFragment;   // no inside-alpha
        const coreFloat fOffset        = coreFloat(Core::System->GetTotalTime()) * 0.1f;

        if(!bShow)
        {
            if((m_iFragmentState == 0u) && (fFragment >= 2.0f))
            {
                m_iFragmentState = 1u;

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_FRAGMENT_IMPACT);
            }

            const coreVector2 vDir = FRAGMENT_DIRECTION;
            const coreVector2 vPos = MapToAxisInv(FRAGMENT_POSITION(m_iFragmentNew), vDir) * INTERFACE_FRAGMENT_SCALE;

            m_aFragment[m_iFragmentNew].SetPosition(vPos + ((m_iFragmentNew == 8u) ? coreVector2(0.0f,1.0f) : vPos.Normalized()) * LERP(1.5f, 0.0f, fAnimation) + m_aFragmentTable[0].GetPosition());

            g_pPostProcessing->SetChroma(fFragmentAlpha);
            g_pGame->SetMusicVolume(LERP(1.0f, 0.2f, fVisibility));
        }

        if(fFragment > INTERFACE_FRAGMENT_DURATION)
        {
            for(coreUintW i = 0u; i < INTERFACE_FRAGMENTS - 1u; ++i)
            {
                const coreVector2 vDir = FRAGMENT_DIRECTION;
                const coreVector2 vPos = MapToAxisInv(FRAGMENT_POSITION(i), vDir) * INTERFACE_FRAGMENT_SCALE;

                const coreFloat fTime2 = CLAMP01(fFragment - INTERFACE_FRAGMENT_DURATION - I_TO_F(i) * 0.3f);

                m_aFragment[i].SetPosition(vPos + ((i == 8u) ? coreVector2(0.0f,1.0f) : vPos.Normalized()) * LERPBR(0.0f, 1.5f, fTime2) + m_aFragmentTable[0].GetPosition());
            }

            m_aFragment[8].SetDirection(coreVector2::Direction(STEPBR(INTERFACE_FRAGMENT_DURATION, INTERFACE_FRAGMENT_DURATION_EXT, fFragment) * (4.0f*PI) - (0.25f*PI)));
        }

        // 
        for(coreUintW i = 0u; i < INTERFACE_FRAGMENTS; ++i)
        {
            m_aFragment[i].SetTexOffset(coreVector2(0.0f, fOffset));
            m_aFragment[i].SetAlpha    (fFragmentAlpha);
            m_aFragment[i].Move();
        }

        // 
        const coreFloat fFlash = (m_iFragmentState == 1u) ? (1.0f - STEPB(2.0f, 3.0f, fFragment)) : 0.0f;
        m_aFragmentTable[1].SetSize(m_aFragmentTable[0].GetSize() * LERP(1.3f, 1.0f, fFlash));

        // 
        m_aFragmentTable[0].SetAlpha(fFragmentAlpha * 0.7f);
        m_aFragmentTable[1].SetAlpha(fFragmentAlpha * fFlash);
        m_FragmentShadow   .SetAlpha(fFragmentAlpha * 0.7f);

        // 
        m_aFragmentTable[0].Move();
        m_aFragmentTable[1].Move();
        m_FragmentShadow   .Move();
    }

    // 
    const coreFloat fAlert = g_pGame->GetTimeTable()->GetTimeEvent() - m_fAlertStart;
    if(m_pAlertSound->EnableRef(this))
    {
        m_pAlertSound->SetVolume(BLENDH3(CLAMP01((4.5f - fAlert) * 4.0f)));
        if(fAlert > 4.5f) m_pAlertSound->Stop();
    }

    // smoothly toggle interface visibility (after forwarding, to allow overriding)
    if(m_bVisible)
         m_fAlphaAll.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaAll.UpdateMax(-2.0f, 0.0f);

    // smoothly toggle boss data visibility
    if(pBoss && (m_iBannerType == INTERFACE_BANNER_TYPE_BOSS) && (fBanner >= INTERFACE_BOSS_DELAY))
         m_fAlphaBoss.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaBoss.UpdateMax(-2.0f, 0.0f);

    // 
    if(!pBoss)
         m_fAlphaWave.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaWave.UpdateMax(-2.0f, 0.0f);

    // 
    if(bTurf)
         m_fAlphaTurf.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaTurf.UpdateMax(-2.0f, 0.0f);
}


// ****************************************************************
// show mission banner
void cInterface::ShowMission(const coreChar* pcMain, const coreChar* pcSub)
{
    // 
    this->__PrepareBanner();

    // set banner text
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // save animation properties
    m_fBannerStart    = g_pGame->GetTimeTable()->GetTimeEvent() + 1.95f;
    m_fBannerDuration = INTERFACE_BANNER_DURATION_MISSION;
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED_OPEN;
    m_iBannerType     = INTERFACE_BANNER_TYPE_MISSION;

    {
        // realign objects as mission banner
        m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_3,  MENU_OUTLINE_SMALL);
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
        m_aBannerText[2].DefineProgram("menu_swipe_program");
        m_aBannerText[3].DefineProgram("menu_swipe_program");

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.06f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

        m_aBannerText[2].SetCenter(coreVector2(0.0f,0.02f));
        m_aBannerText[3].SetCenter(m_aBannerText[2].GetCenter());

        m_aBannerText[2].SetColor3(COLOR_MENU_INSIDE);
        m_aBannerText[3].SetColor3(g_pEnvironment->GetBackground()->GetColor());
    }

    // 
    m_aBannerIcon[0].SetColor3   (g_pEnvironment->GetBackground()->GetColor() * 0.8f);
    m_aBannerIcon[0].SetTexOffset(g_pEnvironment->GetBackground()->GetIcon ());
    m_aBannerIcon[1].SetColor3   (g_pEnvironment->GetBackground()->GetColor() * 0.8f);
    m_aBannerIcon[1].SetTexOffset(g_pEnvironment->GetBackground()->GetIcon ());

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerIcon[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerIcon[1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[3].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}

void cInterface::ShowMission(const cMission* pMission)
{
    ASSERT(pMission && (pMission == g_pGame->GetCurMission()))

    // show default mission banner
    this->ShowMission(pMission->GetName(), PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(g_pGame->GetCurMissionIndex())));
}


// ****************************************************************
// show boss banner
void cInterface::ShowBoss(const coreChar* pcMain, const coreChar* pcSub)
{
    // 
    this->__PrepareBanner();
    
    
    m_bBannerEigengrau = (pcSub[0] == '\0');

    // set banner text
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // save animation properties
    m_fBannerStart    = g_pGame->GetTimeTable()->GetTimeEvent();
    m_fBannerDuration = m_bBannerEigengrau ? 5.0f : INTERFACE_BANNER_DURATION_BOSS;
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED_OPEN;
    m_iBannerType     = INTERFACE_BANNER_TYPE_BOSS;

    {
        // realign objects as boss banner
        m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_3,  MENU_OUTLINE_SMALL);
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
        m_aBannerText[2].DefineProgram("menu_swipe_program");
        m_aBannerText[3].DefineProgram("menu_swipe_program");

        if(m_bBannerEigengrau)
        {
            m_aBannerText[2].SetPosition(coreVector2(0.0f,0.0f));
            m_aBannerText[3].SetPosition(coreVector2(0.0f,0.0f));

            m_aBannerText[2].SetCenter(coreVector2(0.0f,0.0f));
            m_aBannerText[3].SetCenter(coreVector2(0.0f,0.0f));

            m_aBannerText[2].SetColor3(COLOR_MENU_INSIDE);
            m_aBannerText[3].SetColor3(COLOR_MENU_INSIDE * 0.6f);
            
            
            m_aBannerText[2].SetAlignment(coreVector2(-1.0f,0.0f));
            m_aBannerText[3].SetAlignment(coreVector2( 1.0f,0.0f));
            m_aBannerText[2].Construct(MENU_FONT_STANDARD_5,  MENU_OUTLINE_SMALL);
            m_aBannerText[2].DefineProgram("menu_swipe_program");
            m_aBannerText[2].SetText("EIGEN");
            m_aBannerText[3].SetText("GRAU");

            m_BannerShadow.SetCenter(coreVector2(0.0f,0.0f));
        }
        else
        {
            m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.06f));
            m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

            m_aBannerText[2].SetCenter(coreVector2(0.0f,0.02f));
            m_aBannerText[3].SetCenter(m_aBannerText[2].GetCenter());

            m_aBannerText[2].SetColor3(COLOR_MENU_INSIDE);
            m_aBannerText[3].SetColor3(g_pEnvironment->GetBackground()->GetColor());
        }
    }

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerIcon[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerIcon[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[3].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_SegmentName.SetText(pcMain);
    m_sSegmentString = "";
}

void cInterface::ShowBoss(const cBoss* pBoss, const coreBool bSilent)
{
    ASSERT(pBoss)
    
    if(pBoss->GetID() == cProjectOneBoss::ID)
    {
        coreUint8 iFragments = 0u;
        for(coreUintW i = 0u; i < FRAGMENTS - 1u; ++i)
        {
            if(g_pSave->GetHeader().oProgress.aiFragment[i])
                iFragments += 1u;
        }

        if(iFragments < 8u)
        {
            this->ShowBoss(pBoss->GetName(), Core::Language->GetString("BOSS_TITLE_HIDDEN"));
            return;
        }
    }

    // show default boss banner
    this->ShowBoss(pBoss->GetName(), Core::Language->GetString(PRINT("BOSS_TITLE_%04d", pBoss->GetID())));

    if(bSilent)
    {
        // 
        m_fBannerStart = g_pGame->GetTimeTable()->GetTimeEvent() - INTERFACE_BOSS_DELAY;

        // 
        m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerIcon[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerIcon[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aBannerText[3].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cInterface::ShowWave(const coreChar* pcName)
{
    // 
    m_SegmentName.SetText(PRINT("%s %s", coreData::StrUpper(Core::Language->GetString("MISSION")), pcName));
    m_sSegmentString = pcName;
}


// ****************************************************************
// 
void cInterface::ShowScore(const coreChar* pcMain, const coreChar* pcSub, const coreUint8 iMedal, const coreUint8 iMedalType)
{
    // 
    this->__PrepareBanner();

    // set banner text
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // save animation properties
    m_fBannerStart    = g_pGame->GetTimeTable()->GetTimeEvent() + 0.1f;
    m_fBannerDuration = INTERFACE_BANNER_DURATION_SCORE;
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED;
    m_iBannerType     = INTERFACE_BANNER_TYPE_SCORE;

    {
        // realign objects as score banner
        m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_2,  MENU_OUTLINE_SMALL);
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.032f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

        m_BannerBar     .SetDirection(coreVector2(-1.0f,0.0f));

        m_BannerBar     .SetCenter(coreVector2(0.0f,0.0f));
        m_aBannerText[2].SetCenter(coreVector2(0.0f,0.08f));
        m_aBannerText[3].SetCenter(m_aBannerText[2].GetCenter());

        m_aBannerText[2].SetColor3(g_pEnvironment->GetBackground()->GetColor());
        m_aBannerText[3].SetColor3(COLOR_MENU_INSIDE);
    }

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerIcon[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerIcon[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[3].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    ASSERT(iMedal != MEDAL_NONE)
    cMenu::ApplyMedalTexture(&m_Medal, iMedal, iMedalType);
}

void cInterface::ShowScore(const coreUint32 iScore, const coreUint8 iMedal, const coreUint8 iMedalType)
{
    // show default score banner
    this->ShowScore(coreData::ToChars(iScore), Core::Language->GetString("BONUS_TIME"), iMedal, iMedalType);
}


// ****************************************************************
// 
void cInterface::ShowAlert()
{
    // 
    this->__PrepareBanner();

    // set banner text
    m_aBannerText[0].SetText("WARNING");
    m_aBannerText[1].SetText("WARNING");
    m_aBannerText[3].SetText("WARNING");

    // save animation properties
    m_fBannerStart    = g_pGame->GetTimeTable()->GetTimeEvent();
    m_fBannerDuration = INTERFACE_BANNER_DURATION_ALERT;
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED;
    m_iBannerType     = INTERFACE_BANNER_TYPE_ALERT;

    {
        // realign objects as alert banner
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);

        m_aBannerText[3].SetPosition(coreVector2(0.0f,0.0f));

        m_BannerBar     .SetDirection(coreVector2(0.0f,1.0f));
        m_aBannerText[0].SetDirection(m_BannerBar.GetDirection());
        m_aBannerText[1].SetDirection(m_BannerBar.GetDirection());

        m_BannerBar     .SetCenter(coreVector2(0.0f,0.1f));
        m_aBannerText[0].SetCenter(m_BannerBar.GetCenter());
        m_aBannerText[1].SetCenter(m_BannerBar.GetCenter());
        m_aBannerText[3].SetCenter(m_BannerBar.GetCenter());
    }

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerIcon[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerIcon[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[3].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_pAlertSound->PlayRelative(this, 1.0f, 1.0f, true, SOUND_EFFECT);
    m_fAlertStart = g_pGame->GetTimeTable()->GetTimeEvent();
}


// ****************************************************************
// check for active banner
coreBool cInterface::IsBannerActive()const
{
    // compare with game-time offset
    return ((g_pGame->GetTimeTable()->GetTimeEvent() - m_fBannerStart) <= m_fBannerDuration);
}


// ****************************************************************
// 
void cInterface::ShowStory(const coreChar* pcRow1, const coreChar* pcRow2)
{
    ASSERT(pcRow1 && pcRow2)
    const coreFloat fHeight = (pcRow2[0] ? 0.02f : 0.0f);

    // 
    m_aStoryText[0].SetText(pcRow1);
    m_aStoryText[0].SetPosition(coreVector2(0.0f, fHeight));

    // 
    m_aStoryText[1].SetText(pcRow2);
    m_aStoryText[1].SetPosition(coreVector2(0.0f, -fHeight));

    // 
    m_fStoryStart = g_pGame->GetTimeTable()->GetTimeEvent();
}


// ****************************************************************
// 
coreBool cInterface::IsStoryActive()const
{
    // 
    return ((g_pGame->GetTimeTable()->GetTimeEvent() - m_fStoryStart) <= INTERFACE_STORY_DURATION);
}


// ****************************************************************
// 
void cInterface::ShowFragment(const coreUint8 iNewIndex, const coreUint32 iOldBits)
{
    if(iNewIndex == INTERFACE_FRAGMENT_TYPE_HIDE)
    {
        m_fFragmentStart = INTERFACE_INVALID_START;
        return;
    }

    for(coreUintW i = 0u; i < INTERFACE_FRAGMENTS; ++i)
    {
        const coreVector2 vDir = FRAGMENT_DIRECTION;
        const coreVector2 vPos = MapToAxisInv(FRAGMENT_POSITION(i), vDir) * INTERFACE_FRAGMENT_SCALE;

        m_aFragment[i].SetPosition(vPos + m_aFragmentTable[0].GetPosition());
        m_aFragment[i].SetEnabled (((iNewIndex == i) || HAS_BIT(iOldBits, i)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_fFragmentStart    = g_pGame->GetTimeTable()->GetTimeEvent();
    m_fFragmentDuration = (iNewIndex == INTERFACE_FRAGMENT_TYPE_SHOW) ? 3600.0f : ((iNewIndex == 8u) ? INTERFACE_FRAGMENT_DURATION_EXT : INTERFACE_FRAGMENT_DURATION);
    m_iFragmentNew      = iNewIndex;
    m_iFragmentState    = 0u;

    // 
    m_fAlphaFragment = 1.0f;
}

void cInterface::ShowFragment(const coreUint8 iNewIndex)
{
    // 
    coreUint32 iOldBits = 0u;
    for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i)
    {
        if(g_pSave->GetHeader().oProgress.aiFragment[i])
            ADD_BIT(iOldBits, i)
    }

    // 
    this->ShowFragment(iNewIndex, iOldBits);
}


// ****************************************************************
// 
coreBool cInterface::IsFragmentActive()const
{
    // 
    return ((g_pGame->GetTimeTable()->GetTimeEvent() - m_fFragmentStart) <= m_fFragmentDuration);
}


// ****************************************************************
// 
void cInterface::UpdateLayout()
{   
    const coreVector2 vCenter = coreVector2(0.5f,0.5f) - (g_CurConfig.Game.iHudType ? coreVector2(1.0f,1.0f) : Core::System->GetResolution().yx().LowRatio()) * 0.005f;
    // TODO 1: option 0.5f-0.45f, handle aspect ratio (for outside) (also up-down)
    
    const coreVector2 vFlip = (g_CurConfig.Game.iHudType == 2u) ? (cInterface::__IsFlipped() ? coreVector2(-1.0f,1.0f) : coreVector2(1.0f,-1.0f)) : coreVector2(1.0f,1.0f);
    const coreVector2 vAdd  = coreVector2((vFlip.x < 0.0f) ? 0.02f : 0.0f, (vFlip.y < 0.0f) ? 0.02f : 0.0f);

    // 
    const coreObject2DStyle eStyle  = CORE_OBJECT2D_STYLE_VIEWDIR | (g_CurConfig.Game.iHudType ? CORE_OBJECT2D_STYLE_ALTCENTER : CORE_OBJECT2D_STYLE_NOTHING);
    const coreVector2       vTop    = coreVector2(0.0f,  vCenter.y);
    const coreVector2       vBottom = coreVector2(0.0f, -vCenter.y);
    const coreVector2       vRight  = coreVector2(vCenter.x, 0.0f);

    // 
    const auto nUpdateFunc = [&](coreObject2D* OUTPUT pObject, const coreVector2 vCenter, const coreVector2 vAlignment)
    {
        pObject->SetCenter   (vCenter);
        pObject->SetAlignment(vAlignment);
        pObject->SetStyle    (eStyle);
        
        
        const coreVector2 vBasePos = pObject->GetPosition();
        const coreVector2 vAlign   = vAlignment;
        pObject->SetPosition(coreVector2(vCenter.x ? ((ABS(vBasePos.x) + vAdd.x) * vAlign.x) : vBasePos.x,
                                         vCenter.y ? ((ABS(vBasePos.y) + vAdd.y) * vAlign.y) : vBasePos.y));
    };

    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView = m_aView[i];

        // 
        const coreVector2 vSide = coreVector2(i ? vCenter.x : -vCenter.x, 0.0f);
        
        
        const coreVector2 vSide2 = coreVector2(i ? -1.0f : 1.0f, 1.0f);
        
        oView.aShieldBar[0].SetPosition(coreVector2(0.025f,0.06f) * vSide2);
        oView.aShieldBar[1].SetPosition(oView.aShieldBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f * vSide2);
        oView.aShieldBar[2].SetPosition(oView.aShieldBar[1].GetPosition());
        oView.oShieldValue .SetPosition(oView.aShieldBar[0].GetPosition() + coreVector2(0.035f,0.01f) * vSide2);
        oView.oScore       .SetPosition(coreVector2(0.01f,0.0f) * vSide2);
        oView.oCooldownBar .SetPosition(oView.oScore.GetPosition() + coreVector2(0.003f,-0.105f) * vSide2);
        oView.oComboValue  .SetPosition(oView.oScore.GetPosition() + coreVector2(0.003f,-0.045f) * vSide2);
        oView.oChainValue  .SetPosition(oView.oScore.GetPosition() + coreVector2(0.003f,-0.13f) * vSide2);
    

        // 
        for(coreUintW j = 0u; j < INTERFACE_LIVES; ++j) nUpdateFunc(&oView.aLife[j], vBottom + vSide, coreVector2(1.0f,1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.aShieldBar[0], vBottom + vSide, coreVector2(1.0f, 1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.aShieldBar[1], vBottom + vSide, coreVector2(1.0f, 1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.aShieldBar[2], vBottom + vSide, coreVector2(1.0f, 1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.oShieldValue,  vBottom + vSide, coreVector2(1.0f, 1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.oScore,        vTop    + vSide, coreVector2(1.0f,-1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.oCooldownBar,  vTop    + vSide, coreVector2(1.0f,-1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.oComboValue,   vTop    + vSide, coreVector2(1.0f,-1.0f) * vFlip * vSide2);
        nUpdateFunc(&oView.oChainValue,   vTop    + vSide, coreVector2(1.0f,-1.0f) * vFlip * vSide2);
        
    }
    
    m_aBossHealthBar[0].SetPosition(coreVector2(0.0f,-0.005f));
    m_aBossHealthBar[1].SetPosition(m_aBossHealthBar[0].GetPosition() + coreVector2(0.00f,-0.01f) * 0.5f);
    m_aBossHealthBar[2].SetPosition(m_aBossHealthBar[1].GetPosition());
    m_BossHealthValue  .SetPosition(m_aBossHealthBar[0].GetPosition() + coreVector2(0.12f,-0.011f));
    m_aBossTime[0]     .SetPosition(m_aBossHealthBar[0].GetPosition() + coreVector2(0.0f,-0.011f));
    m_aBossTime[1]     .SetPosition(m_aBossTime[0].GetPosition());
    m_aBossTime[2]     .SetPosition(m_aBossTime[0].GetPosition() + coreVector2(0.06f,0.0f));
    m_aWaveTime[0]     .SetPosition(coreVector2(0.0f,-0.005f));
    m_aWaveTime[1]     .SetPosition(m_aWaveTime[0].GetPosition());
    m_aWaveTime[2]     .SetPosition(m_aWaveTime[0].GetPosition() + coreVector2(0.06f,0.0f));
    m_SegmentName      .SetPosition(coreVector2(-0.01f,-0.005f));
    m_SegmentBest      .SetPosition(coreVector2(-0.01f,-0.055f));
    m_aTurfBar[0]      .SetPosition(coreVector2(0.0f,0.005f));
    m_aTurfBar[1]      .SetPosition(m_aTurfBar[0].GetPosition() + coreVector2(0.00f,0.01f) * 0.5f);
    m_aTurfBar[2]      .SetPosition(m_aTurfBar[1].GetPosition());
    m_TurfValue        .SetPosition(m_aTurfBar[0].GetPosition() + coreVector2(0.0f,0.011f));
    

    // 
    nUpdateFunc(&m_aBossHealthBar[0], vTop, coreVector2( 0.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aBossHealthBar[1], vTop, coreVector2( 0.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aBossHealthBar[2], vTop, coreVector2( 0.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_BossHealthValue,   vTop, coreVector2( 1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aBossTime[0],      vTop, coreVector2(-1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aBossTime[1],      vTop, coreVector2( 1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aBossTime[2],      vTop, coreVector2( 1.0f,-1.0f * vFlip.y));

    // 
    nUpdateFunc(&m_aWaveTime[0], vTop, coreVector2(-1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aWaveTime[1], vTop, coreVector2( 1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_aWaveTime[2], vTop, coreVector2( 1.0f,-1.0f * vFlip.y));
    
    // 
    nUpdateFunc(&m_SegmentName, vCenter, coreVector2(-1.0f,-1.0f) * vFlip);
    nUpdateFunc(&m_SegmentBest, vCenter, coreVector2(-1.0f,-1.0f) * vFlip);

    // 
    nUpdateFunc(&m_aTurfBar[0], vBottom, coreVector2( 0.0f, 1.0f * vFlip.y));
    nUpdateFunc(&m_aTurfBar[1], vBottom, coreVector2( 0.0f, 1.0f * vFlip.y));
    nUpdateFunc(&m_aTurfBar[2], vBottom, coreVector2( 0.0f, 1.0f * vFlip.y));
    nUpdateFunc(&m_TurfValue,   vBottom, coreVector2( 0.0f, 1.0f * vFlip.y));

    // 
    nUpdateFunc(&m_GoalMedal, vTop, coreVector2(-1.0f,-1.0f * vFlip.y));
    nUpdateFunc(&m_GoalTime,  vTop, coreVector2(-1.0f,-1.0f * vFlip.y));

    
    const coreBool bMulti = g_pGame->IsMulti();
    
    // 
    for(coreUintW j = 0u; j < INTERFACE_BADGES;  ++j) nUpdateFunc(&m_aBadge     [j], bMulti ? vBottom : vCenter.InvertedY(), (bMulti ? (coreVector2( 0.0f,1.0f) * vFlip) : (coreVector2(-1.0f,1.0f) * vFlip)));     
    for(coreUintW j = 0u; j < INTERFACE_BADGES;  ++j) nUpdateFunc(&m_aBadgeWave [j], bMulti ? vBottom : vCenter.InvertedY(), (bMulti ? (coreVector2( 0.0f,1.0f) * vFlip) : (coreVector2(-1.0f,1.0f) * vFlip)));     
    for(coreUintW j = 0u; j < INTERFACE_HELPERS; ++j) nUpdateFunc(&m_aHelper    [j], bMulti ? vRight  : vCenter.InvertedY(), (bMulti ? coreVector2(-1.0f,0.0f) : coreVector2(-1.0f,1.0f)) * vFlip);     
    for(coreUintW j = 0u; j < INTERFACE_HELPERS; ++j) nUpdateFunc(&m_aHelperWave[j], bMulti ? vRight  : vCenter.InvertedY(), (bMulti ? coreVector2(-1.0f,0.0f) : coreVector2(-1.0f,1.0f)) * vFlip);     
    
    
    
    
    
}


// ****************************************************************
// 
void cInterface::UpdateSpacing()
{
    
    const coreVector2 vFlip = (g_CurConfig.Game.iHudType == 2u) ? (cInterface::__IsFlipped() ? coreVector2(-1.0f,1.0f) : coreVector2(1.0f,-1.0f)) : coreVector2(1.0f,1.0f);
    const coreVector2 vAdd  = coreVector2((vFlip.x < 0.0f) ? 0.02f : 0.0f, (vFlip.y < 0.0f) ? 0.02f : 0.0f);
    
    const coreFloat fShift = -vAdd.y + LERPS(-0.005f, -0.04f, ((g_CurConfig.Game.iHudType == 1u) || !cInterface::__IsFlipped()) ? m_fAlphaBoss.ToFloat() : 0.0f);
    
    const auto nSetSpacingFunc = [&](coreObject2D* OUTPUT pObject, const coreFloat fOffset)
    {
        pObject->SetPosition(coreVector2(pObject->GetPosition().x, (fShift + fOffset) * vFlip.y));
    };

    // 
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView = m_aView[i];

        // 
        nSetSpacingFunc(&oView.oScore,        0.0f);
        nSetSpacingFunc(&oView.oCooldownBar, -0.11f);
        nSetSpacingFunc(&oView.oComboValue,  -0.045f);
        nSetSpacingFunc(&oView.oChainValue,  -0.135f);
    }

    // 
    nSetSpacingFunc(&m_SegmentName,  0.0f);
    nSetSpacingFunc(&m_SegmentBest, -0.05f);
}


// ****************************************************************
// 
void cInterface::MoveTimeless()
{
    if(!STATIC_ISVALID(g_pGame)) return;

    Timeless([this]()
    {
        this->Move();
    });
}


// ****************************************************************
// 
void cInterface::UpdateEnabled()
{
    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView   = m_aView[i];
        cPlayer*     pPlayer = g_pGame->GetPlayer(i);

        if(pPlayer->HasStatus(PLAYER_STATUS_SHIELDED))
        {
            // 
            oView.aShieldBar[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            oView.aShieldBar[1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            oView.aShieldBar[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            oView.oShieldValue .SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }
        else
        {
            // 
            oView.aShieldBar[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            oView.aShieldBar[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            oView.aShieldBar[2].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            oView.oShieldValue .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }

    // 
    m_SegmentName.SetEnabled(g_pGame->IsMulti() ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    m_SegmentBest.SetEnabled(g_pGame->IsMulti() ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cInterface::__Update()
{
    //m_aBannerText[2].SetText(pcSub);
    
    if(!m_sSegmentString.empty())
    {
        m_SegmentName.SetText(PRINT("%s %s", coreData::StrUpper(Core::Language->GetString("MISSION")), m_sSegmentString.c_str()));
    }
}


// ****************************************************************
// 
void cInterface::__PrepareBanner()
{
    // hide banner initially (to prevent flickering)
    m_BannerShadow  .SetAlpha(0.0f);
    m_aBannerIcon[0].SetAlpha(0.0f);
    m_aBannerIcon[1].SetAlpha(0.0f);
    m_aBannerText[0].SetAlpha(0.0f);
    m_aBannerText[1].SetAlpha(0.0f);
    m_aBannerText[2].SetAlpha(0.0f);
    m_aBannerText[3].SetAlpha(0.0f);

    // also hide banner bar (but has no transparency)
    m_BannerBar.SetSize(coreVector2(0.0f,0.0f));
    m_BannerBar.Move();
    
    m_bBannerEigengrau = false;
    m_aBannerText[2].SetAlignment(coreVector2(0.0f,0.0f));
    m_aBannerText[3].SetAlignment(coreVector2(0.0f,0.0f));
    m_BannerShadow.SetCenter    (coreVector2(0.0f,0.02f));
}