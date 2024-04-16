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
// construct player view
void cInterface::sPlayerView::Construct(const coreUintW iIndex)
{
    // 
    const coreVector2 vSide = coreVector2(iIndex ? -1.0f : 1.0f, 1.0f);

    // create view objects
    for(coreUintW i = 0u; i < INTERFACE_LIVES; ++i)
    {
        aLife[i].DefineTexture(0u, iIndex ? "ship_icon_02.png" : "ship_icon_01.png");
        aLife[i].DefineProgram("default_2d_program");
        aLife[i].SetAlignment (coreVector2(1.0f,1.0f) * vSide);
    }

    aShieldBar[0].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[0].DefineProgram("default_2d_program");
    aShieldBar[0].SetPosition  (coreVector2(0.02f,0.06f) * vSide);
    aShieldBar[0].SetSize      (coreVector2(3.5f, 0.45f) * 0.07f);
    aShieldBar[0].SetAlignment (coreVector2(1.0f, 1.0f)  * vSide);
    aShieldBar[0].SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    aShieldBar[1].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[1].DefineProgram("default_2d_program");
    aShieldBar[1].SetPosition  (aShieldBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f * vSide);
    aShieldBar[1].SetSize      (aShieldBar[0].GetSize()     - coreVector2(0.01f,0.01f));
    aShieldBar[1].SetAlignment (aShieldBar[0].GetAlignment());

    aShieldBar[2].DefineTexture(0u, "menu_detail_01.png");
    aShieldBar[2].DefineProgram("default_2d_program");
    aShieldBar[2].SetPosition  (aShieldBar[1].GetPosition());
    aShieldBar[2].SetSize      (aShieldBar[1].GetSize());
    aShieldBar[2].SetAlignment (aShieldBar[1].GetAlignment());

    oShieldValue.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    oShieldValue.SetPosition (aShieldBar[0].GetPosition() + coreVector2(0.035f, 0.01f) * vSide);
    oShieldValue.SetAlignment(coreVector2(1.0f,1.0f) * vSide);
    oShieldValue.SetColor3   (COLOR_MENU_INSIDE);

    oScoreTotal.Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    oScoreTotal.SetPosition (coreVector2(0.01f,-0.035f) * vSide);
    oScoreTotal.SetAlignment(coreVector2(1.0f, -1.0f)   * vSide);
    oScoreTotal.SetColor3   (COLOR_MENU_INSIDE);

    oScoreMission.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    oScoreMission.SetPosition (oScoreTotal.GetPosition() + coreVector2(0.01f,-0.035f) * vSide);
    oScoreMission.SetAlignment(oScoreTotal.GetAlignment());
    oScoreMission.SetColor3   (COLOR_MENU_INSIDE);

    oCooldownBar.DefineTexture(0u, "default_white.png");
    oCooldownBar.DefineProgram("default_2d_program");
    oCooldownBar.SetPosition  (oScoreTotal.GetPosition() + coreVector2(0.003f,-0.105f) * vSide);
    oCooldownBar.SetAlignment (oScoreTotal.GetAlignment());

    oComboValue.Construct   (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    oComboValue.SetPosition (oScoreTotal.GetPosition() + coreVector2(0.003f,-0.045f) * vSide);
    oComboValue.SetAlignment(oScoreTotal.GetAlignment());
    oComboValue.SetColor3   (COLOR_MENU_INSIDE);

    oChainValue.Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    oChainValue.SetPosition (oScoreTotal.GetPosition() + coreVector2(0.003f,-0.13f) * vSide);
    oChainValue.SetAlignment(oScoreTotal.GetAlignment());
    oChainValue.SetColor3   (COLOR_MENU_INSIDE);

    // 
    fSpin  = 0.0f;
    fSpin2 = 0.0f;
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreUint8 iNumViews)noexcept
: m_iNumViews       (iNumViews)
, m_fBossSpin       (0.0f)
, m_fBannerStart    (-FLT_MAX)
, m_fBannerDuration (0.0f)
, m_fBannerSpeed    (0.0f)
, m_iBannerType     (0u)
, m_fStoryStart     (-FLT_MAX)
, m_fAnimation      (0.0f)
, m_bVisible        (false)
, m_fAlphaAll       (0.0f)
, m_fAlphaBoss      (0.0f)
{
    ASSERT((m_iNumViews > 0) && (m_iNumViews <= INTERFACE_VIEWS))

    // construct player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        m_aView[i].Construct(i);

    // create interface objects
    m_aBossHealthBar[0].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].SetPosition  (coreVector2(0.0f,-0.005f));
    m_aBossHealthBar[0].SetSize      (coreVector2(14.0f,0.5f) * 0.07f);
    m_aBossHealthBar[0].SetAlignment (coreVector2(0.0f,-1.0f));
    m_aBossHealthBar[0].SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_aBossHealthBar[1].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].SetPosition  (m_aBossHealthBar[0].GetPosition() + coreVector2(0.00f,-0.01f) * 0.5f);
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize()     - coreVector2(0.01f, 0.01f));
    m_aBossHealthBar[1].SetAlignment (m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthBar[2].DefineTexture(0u, "menu_detail_03.png");
    m_aBossHealthBar[2].DefineProgram("default_2d_program");
    m_aBossHealthBar[2].SetPosition  (m_aBossHealthBar[1].GetPosition());
    m_aBossHealthBar[2].SetSize      (m_aBossHealthBar[1].GetSize());
    m_aBossHealthBar[2].SetAlignment (m_aBossHealthBar[1].GetAlignment());

    m_BossHealthValue.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_BossHealthValue.SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(-0.1f,-0.011f));
    m_BossHealthValue.SetAlignment(m_aBossHealthBar[0].GetAlignment());
    m_BossHealthValue.SetColor3   (COLOR_MENU_INSIDE);

    m_aBossTime[0].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aBossTime[0].SetPosition (coreVector2( 0.0f, m_BossHealthValue.GetPosition().y));
    m_aBossTime[0].SetAlignment(coreVector2(-1.0f,-1.0f));
    m_aBossTime[0].SetColor3   (COLOR_MENU_INSIDE);

    m_aBossTime[1].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aBossTime[1].SetPosition (m_aBossTime[0].GetPosition());
    m_aBossTime[1].SetAlignment(coreVector2(1.0f,-1.0f));
    m_aBossTime[1].SetColor3   (COLOR_MENU_INSIDE);

    m_WaveName.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_WaveName.SetPosition (coreVector2(0.0f,0.005f));
    m_WaveName.SetAlignment(coreVector2(0.0f,1.0f));
    m_WaveName.SetColor3   (COLOR_MENU_INSIDE);

    m_aWaveTime[0].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aWaveTime[0].SetPosition (coreVector2( 0.0f,-0.005f));
    m_aWaveTime[0].SetAlignment(coreVector2(-1.0f,-1.0f));
    m_aWaveTime[0].SetColor3   (COLOR_MENU_INSIDE);

    m_aWaveTime[1].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aWaveTime[1].SetPosition (m_aWaveTime[0].GetPosition());
    m_aWaveTime[1].SetAlignment(coreVector2(1.0f,-1.0f));
    m_aWaveTime[1].SetColor3   (COLOR_MENU_INSIDE);

    m_BannerBar.DefineTexture(0u, "menu_detail_04.png");
    m_BannerBar.DefineTexture(1u, "menu_background_black.png");
    m_BannerBar.DefineProgram("menu_animate_program");
    
    m_BannerShadow.DefineTexture(0u, "effect_headlight_point.png");
    m_BannerShadow.DefineProgram("menu_single_program");
    m_BannerShadow.SetPosition(coreVector2(0.0f,0.0f));
    m_BannerShadow.SetSize    (coreVector2(0.5f,0.2f));
    m_BannerShadow.SetCenter  (coreVector2(0.0f,0.02f));
    m_BannerShadow.SetColor3  (coreVector3(0.0f,0.0f,0.0f));

    m_aBannerText[0].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
    m_aBannerText[0].SetColor3(COLOR_MENU_INSIDE * 0.75f);
    m_aBannerText[1].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);
    m_aBannerText[1].SetColor3(COLOR_MENU_INSIDE * 0.75f);

    m_aStoryText[0].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aStoryText[0].SetColor3(COLOR_MENU_INSIDE);
    m_aStoryText[1].Construct(MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aStoryText[1].SetColor3(COLOR_MENU_INSIDE);

    m_Medal.DefineTexture(0u, "menu_medal.png");
    m_Medal.DefineProgram("default_2d_program");
    m_Medal.SetTexSize   (coreVector2(0.25f,0.25f));
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
        m_BannerShadow  .Render();
        m_aBannerText[0].Render();
        m_aBannerText[1].Render();
        m_aBannerText[2].Render();
        m_aBannerText[3].Render();

        // 
        m_Medal.Render();
    }

    if(m_fAlphaAll)
    {
        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        {
            // render player images
            for(coreUintW j = 0u; j < INTERFACE_LIVES; ++j) m_aView[i].aLife[j].Render();
            m_aView[i].aShieldBar[0].Render();
            m_aView[i].aShieldBar[1].Render();
            m_aView[i].aShieldBar[2].Render();
            m_aView[i].oCooldownBar .Render();
        }

        if(m_fAlphaBoss)
        {
            // render boss images
            m_aBossHealthBar[0].Render();
            m_aBossHealthBar[1].Render();
            m_aBossHealthBar[2].Render();
        }

        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        {
            // render player labels
            m_aView[i].oShieldValue .Render();
            m_aView[i].oScoreTotal  .Render();
            //m_aView[i].oScoreMission.Render();   // TODO 1: still needed ?
            m_aView[i].oComboValue  .Render();
            m_aView[i].oChainValue  .Render();
        }

        if(m_fAlphaBoss)
        {
            // render boss labels
            m_BossHealthValue.Render();
            m_aBossTime[0]   .Render();
            m_aBossTime[1]   .Render();
        }

        if(m_fAlphaBoss != 1.0f)
        {
            // render wave labels
            //m_WaveName    .Render();   // TODO 1: der nervt
            m_aWaveTime[0].Render();
            m_aWaveTime[1].Render();
        }
    }

    if(this->IsStoryActive())
    {
        // render story
        m_aStoryText[0].Render();
        m_aStoryText[1].Render();
    }
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    // 
    m_fAnimation.UpdateMod(6.0f*PI, 2.0f*PI);
    const coreFloat fDanger = 0.5f + 0.5f * SIN(m_fAnimation);

    // 
    const coreFloat fAlphaPlayerFull = BLENDH3(m_fAlphaAll) * MENU_INSIDE_ALPHA;
    const coreFloat fAlphaBossFull   = BLENDH3(m_fAlphaAll) * MENU_INSIDE_ALPHA * BLENDH3(m_fAlphaBoss);
    const coreFloat fAlphaWaveFull   = BLENDH3(m_fAlphaAll) * MENU_INSIDE_ALPHA * BLENDH3(1.0f - m_fAlphaBoss);

    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView   = m_aView[i];
        cPlayer*     pPlayer = g_pGame->GetPlayer(i);

        if(pPlayer->HasStatus(PLAYER_STATUS_SHIELDED))
        {
            // 
            if(m_fAlphaAll) oView.fSpin2.UpdateMin(1.0f, 1.0f);
                       else oView.fSpin2 = 0.0f;

            // set shield bar size
            const coreFloat fPercent = pPlayer->GetCurShieldPct() * oView.fSpin2;
            const coreFloat fWidth   = oView.aShieldBar[0].GetSize().x - 0.01f;
            oView.aShieldBar[1].SetSize     (coreVector2(fPercent * fWidth, oView.aShieldBar[1].GetSize().y));
            oView.aShieldBar[1].SetTexSize  (coreVector2(fPercent, 1.0f));
            oView.aShieldBar[1].SetTexOffset(coreVector2(i ? (1.0f-fPercent) : 0.0f, 0.0f));
            
        const coreFloat fRev = 1.0f - fPercent;
        oView.aShieldBar[2].SetPosition(coreVector2(fWidth * (fPercent) * oView.aShieldBar[2].GetAlignment().x + oView.aShieldBar[1].GetPosition().x, oView.aShieldBar[2].GetPosition().y));
        oView.aShieldBar[2].SetSize    (coreVector2(fWidth *  fRev,              oView.aShieldBar[2].GetSize    ().y));
        oView.aShieldBar[2].SetTexSize (coreVector2(fRev, 1.0f));
        oView.aShieldBar[2].SetTexOffset (coreVector2(i ? 0.0f : fPercent, 1.0f));

            // set shield bar color
            const coreVector3 vColor = (i ? (COLOR_ENERGY_YELLOW * 1.45f) : (COLOR_ENERGY_BLUE * 2.3f)) * 0.95f * 0.95f + ((fPercent <= 0.2f) ? (fDanger * 0.5f) : 0.0f);
            oView.aShieldBar[1].SetColor3(vColor * 1.0f);
            oView.aShieldBar[2].SetColor3(vColor * 0.3f);

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
            const coreVector2 vSide = coreVector2(i ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vPos  = coreVector2(0.007f + I_TO_F(j) * 0.055f, 0.005f);
            const coreVector2 vSize = coreVector2(1.0f,1.0f) * 0.058f;

            // 
            const coreFloat   fCurSpin = CLAMP01(oView.fSpin - I_TO_F(j));
            const coreVector2 vNewSize = vSize * (1.5f - 0.5f * fCurSpin);
            const coreVector2 vNewPos  = vPos + 0.5f * (vSize - vNewSize);

            
            
        oView.aLife[j].SetDirection(coreVector2::Direction(coreFloat(Core::System->GetTotalTime()) + (0.8f*PI) * (I_TO_F(j) / I_TO_F(INTERFACE_LIVES))));
        
            // 
            oView.aLife[j].SetPosition(vNewPos * vSide);
            oView.aLife[j].SetSize    (vNewSize);
            oView.aLife[j].SetAlpha   (fAlphaPlayerFull * fCurSpin);
            oView.aLife[j].SetEnabled (fCurSpin ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            oView.aLife[j].Move();
        }

        const cScoreTable* pScoreTable = pPlayer->GetScoreTable();

        // display score
        oView.oScoreTotal  .SetText(PRINT("%07u", pScoreTable->GetScoreTotal()));
        oView.oScoreMission.SetText(PRINT("%07u", pScoreTable->GetScoreMission(g_pGame->GetCurMissionIndex())));

        const coreBool bCombo = ((pScoreTable->GetModifier() > 1.0f) || m_fAlphaBoss);
        
        // 
        oView.oCooldownBar.SetSize  (coreVector2(bCombo ? (0.15f * MIN(pScoreTable->GetCooldown() * 1.1f, 1.0f)) : 0.0f, 0.013f));
        oView.oCooldownBar.SetColor3((pScoreTable->GetCooldown() > 0.5f) ? COLOR_MENU_BLUE : COLOR_MENU_RED);

        // 
        oView.oComboValue.SetText(bCombo ? PRINT("x%.1f", pScoreTable->GetModifier()) : "");
        oView.oChainValue.SetText(pScoreTable->GetCurChain() ? PRINT("+%u", pScoreTable->GetCurChain()) : "");

        
        
        oView.oScoreTotal.SetPosition(coreVector2(oView.oScoreTotal.GetPosition().x, LERPS(-0.005f, -0.04f, (HAS_FLAG(oView.oScoreTotal.GetStyle(), CORE_OBJECT2D_STYLE_ALTCENTER) || !IsHorizontal(Core::System->GetResolution())) ? m_fAlphaBoss.ToFloat() : 0.0f)));
        oView.oScoreMission.SetPosition(coreVector2(oView.oScoreMission.GetPosition().x, oView.oScoreTotal.GetPosition().y -0.035f));
        oView.oCooldownBar .SetPosition(coreVector2(oView.oCooldownBar .GetPosition().x, oView.oScoreTotal.GetPosition().y -0.11f));
        oView.oComboValue  .SetPosition(coreVector2(oView.oComboValue  .GetPosition().x, oView.oScoreTotal.GetPosition().y -0.045f));
        oView.oChainValue  .SetPosition(coreVector2(oView.oChainValue  .GetPosition().x, oView.oScoreTotal.GetPosition().y -0.135f));
        

        // set player transparency
        oView.aShieldBar[0].SetAlpha(fAlphaPlayerFull);
        oView.aShieldBar[1].SetAlpha(fAlphaPlayerFull);
        oView.aShieldBar[2].SetAlpha(fAlphaPlayerFull);
        oView.oShieldValue .SetAlpha(fAlphaPlayerFull);
        oView.oScoreTotal  .SetAlpha(fAlphaPlayerFull);
        oView.oScoreMission.SetAlpha(fAlphaPlayerFull);
        oView.oCooldownBar .SetAlpha(fAlphaPlayerFull);
        oView.oComboValue  .SetAlpha(fAlphaPlayerFull * (m_fAlphaBoss ? m_fAlphaBoss.ToFloat() : 1.0f));
        oView.oChainValue  .SetAlpha(fAlphaPlayerFull);

        // move player
        oView.oShieldValue .Move();
        oView.aShieldBar[0].Move();
        oView.aShieldBar[1].Move();
        oView.aShieldBar[2].Move();
        oView.oScoreTotal  .Move();
        oView.oScoreMission.Move();
        oView.oCooldownBar .Move();
        oView.oComboValue  .Move();
        oView.oChainValue  .Move();
    }

    // check for active boss
    const cBoss* pBoss = g_pGame->GetCurMission()->GetCurBoss();
    if(pBoss)
    {
        // 
        if(m_fAlphaBoss) m_fBossSpin.UpdateMin(1.0f / (INTERFACE_BANNER_DURATION_BOSS - INTERFACE_BOSS_DELAY), 1.0f);
                    else m_fBossSpin = 0.0f;

        // set health bar size
        const coreFloat fPercent = pBoss->GetCurHealthPct() * m_fBossSpin;
        const coreFloat fWidth   = m_aBossHealthBar[0].GetSize().x - 0.01f;
        m_aBossHealthBar[1].SetPosition(coreVector2(fWidth * (fPercent-1.0f) * 0.5f, m_aBossHealthBar[1].GetPosition().y));
        m_aBossHealthBar[1].SetSize    (coreVector2(fWidth *  fPercent,              m_aBossHealthBar[1].GetSize    ().y));
        m_aBossHealthBar[1].SetTexSize (coreVector2(fPercent, 1.0f));
        
        const coreFloat fRev = 1.0f - fPercent;
        m_aBossHealthBar[2].SetPosition(coreVector2(fWidth * fPercent * 0.5f, m_aBossHealthBar[2].GetPosition().y));
        m_aBossHealthBar[2].SetSize    (coreVector2(fWidth *  fRev,              m_aBossHealthBar[2].GetSize    ().y));
        m_aBossHealthBar[2].SetTexSize (coreVector2(fRev, 1.0f));
        m_aBossHealthBar[2].SetTexOffset (coreVector2(fPercent, 1.0f));
        
        
        //m_aBossHealthBar[1].SetPosition(coreVector2(fWidth * ((1.0f-fPercent)-1.0f) * 0.5f, m_aBossHealthBar[1].GetPosition().y));
        //m_aBossHealthBar[1].SetSize    (coreVector2(fWidth *  (1.0f-fPercent),              m_aBossHealthBar[1].GetSize    ().y));
        //m_aBossHealthBar[1].SetTexSize (coreVector2((1.0f-fPercent), 1.0f));
        
        //m_aBossHealthBar[1].SetPosition(coreVector2(0.0f, m_aBossHealthBar[1].GetPosition().y));
        //m_aBossHealthBar[1].SetTexSize (coreVector2(fPercent, 1.0f));
        //m_aBossHealthBar[1].SetTexOffset (coreVector2((1.0f-fPercent) * 0.5f, 1.0f));

        // set health bar color
        //const coreVector3 vColor = LERP(coreVector3(1.0f,1.0f,1.0f), pBoss->GetColor(), 1.0f);//fPercent);
        //m_aBossHealthBar[0].SetColor3(vColor * 0.2f + 0.5f * ((fPercent <= 0.2f) ? fDanger : 0.0f));
        //m_aBossHealthBar[1].SetColor3(vColor * 0.9f + 0.9f * ((fPercent <= 0.2f) ? fDanger : 0.0f));
        //const coreVector3 vColor = LERP(coreVector3(1.0f,1.0f,1.0f), pBoss->GetColor(), fPercent) + ((fPercent <= 0.2f) ? fDanger : 0.0f);//fPercent);
        //const coreVector3 vColor = LERP(coreVector3(1.0f,1.0f,1.0f), pBoss->GetColor(), CLAMP((fPercent - 0.2f) * (1.0f/0.8f) * (0.8f/0.6f), 0.0f, 1.0f)) + ((fPercent <= 0.2f) ? fDanger : 0.0f);
        const coreVector3 vColor = LERP(coreVector3(1.0f,1.0f,1.0f), pBoss->GetColor(), cShip::CalcColorFactor(pBoss->GetCurHealthPct())) + ((fPercent <= 0.2f) ? (fDanger * 0.5f) : 0.0f);
        m_aBossHealthBar[1].SetColor3(vColor * 1.0f);
        m_aBossHealthBar[2].SetColor3(vColor * 0.3f);

        // display health value
        m_BossHealthValue.SetText(PRINT("%.0f%%", fPercent ? FLOOR(LERP(1.0f, 100.0f, fPercent)) : 0.0f));
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

    // adjust time position (# only required if alignment is centered)
    (m_fAlphaBoss ? m_aBossTime[0] : m_aWaveTime[0]).RetrieveDesiredSize([this](const coreVector2 vSize)
    {
        const coreFloat fPos = (vSize.x - 0.022f) * 0.5f;
        const coreFloat fOff = m_BossHealthValue.GetPosition().x;
        m_aBossTime[0].SetPosition(coreVector2(fPos - fOff,        m_aBossTime[0].GetPosition().y));
        m_aBossTime[1].SetPosition(coreVector2(fPos - fOff - 0.0f, m_aBossTime[1].GetPosition().y));
        m_aWaveTime[0].SetPosition(coreVector2(fPos,               m_aWaveTime[0].GetPosition().y));
        m_aWaveTime[1].SetPosition(coreVector2(fPos        - 0.0f, m_aWaveTime[1].GetPosition().y));
    });

    // set boss transparency
    m_aBossHealthBar[0].SetAlpha(fAlphaBossFull);
    m_aBossHealthBar[1].SetAlpha(fAlphaBossFull);
    m_aBossHealthBar[2].SetAlpha(fAlphaBossFull);
    m_BossHealthValue  .SetAlpha(fAlphaBossFull);
    m_aBossTime[0]     .SetAlpha(fAlphaBossFull);
    m_aBossTime[1]     .SetAlpha(fAlphaBossFull);

    // move boss
    m_aBossHealthBar[0].Move();
    m_aBossHealthBar[1].Move();
    m_aBossHealthBar[2].Move();
    m_BossHealthValue  .Move();
    m_aBossTime[0]     .Move();
    m_aBossTime[1]     .Move();

    // set wave transparency
    m_WaveName    .SetAlpha(fAlphaWaveFull);
    m_aWaveTime[0].SetAlpha(fAlphaWaveFull);
    m_aWaveTime[1].SetAlpha(fAlphaWaveFull);

    // move wave
    m_WaveName    .Move();
    m_aWaveTime[0].Move();
    m_aWaveTime[1].Move();

    // check for active banner
    const coreFloat fBanner = g_pGame->GetTimeTable()->GetTimeEvent() - m_fBannerStart;
    if((fBanner <= m_fBannerDuration) && (fBanner >= 0.0f))
    {
        // calculate visibility and animation value
        const coreFloat fVisibility = MIN(fBanner, m_fBannerDuration - fBanner, RCP(m_fBannerSpeed)) * m_fBannerSpeed;
        const coreFloat fAnimation  = LERPB(0.0f, 1.0f, MIN(fBanner / INTERFACE_BANNER_ANIMATION, 1.0f)) * INTERFACE_BANNER_ANIMATION;

        // slash banner bar across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
        const coreBool bLeftRight = (fBanner < (m_fBannerDuration * 0.5f)) ? false : true;
        m_BannerBar.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_BannerBar.GetDirection().yx());
        m_BannerBar.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

        // animate banner bar
        m_BannerBar.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.26f));
        m_BannerBar.SetTexSize  (coreVector2(fVisibility, 1.0f));
        m_BannerBar.SetTexOffset(coreVector2(1.0f,1.0f) * (fBanner * 0.15f));

        // animate banner text
        const coreFloat fTextOffset = (fAnimation + 2.0f) * 0.024f;
        m_aBannerText[0].SetPosition(coreVector2( fTextOffset,  0.029f));
        m_aBannerText[1].SetPosition(coreVector2(-fTextOffset, -0.029f));
        if(m_iBannerType == INTERFACE_BANNER_TYPE_ALERT) m_aBannerText[3].SetColor3(COLOR_MENU_INSIDE * LERP(MENU_LIGHT_IDLE, MENU_LIGHT_ACTIVE, fDanger));

        // 
        m_Medal.SetPosition(coreVector2(0.0f, fAnimation * 0.006f - 0.07f));
        m_Medal.SetSize    (coreVector2(0.135f,0.135f) * LERP(1.5f, 1.0f, MIN(fBanner * 10.0f, 1.0f)));

        const coreFloat fBannerAlpha = BLENDH3(fVisibility) * MENU_INSIDE_ALPHA;

        // set banner transparency
        m_BannerShadow  .SetAlpha(fBannerAlpha * 0.7f);
        m_aBannerText[0].SetAlpha(fBannerAlpha * 0.2f);
        m_aBannerText[1].SetAlpha(fBannerAlpha * 0.2f);
        m_aBannerText[2].SetAlpha(fBannerAlpha);
        m_aBannerText[3].SetAlpha(fBannerAlpha);
        m_Medal         .SetAlpha(fBannerAlpha);

        // move banner
        m_BannerBar     .Move();
        m_BannerShadow  .Move();
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
        const coreFloat fVisibility = MIN(fStory, INTERFACE_STORY_DURATION - fStory, 1.0f / INTERFACE_STORY_SPEED) * INTERFACE_STORY_SPEED * MENU_INSIDE_ALPHA;

        const coreFloat fStoryAlpha = BLENDH3(fVisibility) * MENU_INSIDE_ALPHA;

        // set story transparency
        m_aStoryText[0].SetAlpha(fStoryAlpha);
        m_aStoryText[1].SetAlpha(fStoryAlpha);

        // move story
        m_aStoryText[0].Move();
        m_aStoryText[1].Move();
    }

    // smoothly toggle interface visibility (after forwarding, to allow overriding)
    if(m_bVisible)
         m_fAlphaAll.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaAll.UpdateMax(-2.0f, 0.0f);

    // smoothly toggle boss data visibility
    if(pBoss && (m_iBannerType == INTERFACE_BANNER_TYPE_BOSS) && (fBanner >= INTERFACE_BOSS_DELAY))
         m_fAlphaBoss.UpdateMin( 2.0f, 1.0f);
    else m_fAlphaBoss.UpdateMax(-2.0f, 0.0f);
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
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED_MISSION;
    m_iBannerType     = INTERFACE_BANNER_TYPE_MISSION;

    {
        // realign objects as mission banner
        m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_3,  MENU_OUTLINE_SMALL);
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.06f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

        m_aBannerText[2].SetCenter(coreVector2(0.0f,0.02f));
        m_aBannerText[3].SetCenter(m_aBannerText[2].GetCenter());

        m_aBannerText[2].SetColor3(COLOR_MENU_INSIDE);
        m_aBannerText[3].SetColor3(g_pEnvironment->GetBackground()->GetColor());
    }

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}

void cInterface::ShowMission(const cMission* pMission)
{
    // show default mission banner
    this->ShowMission(coreData::StrUpper(pMission->GetName()), PRINT("%s %d", Core::Language->GetString("MISSION"), pMission->GetID()));
}


// ****************************************************************
// show boss banner
void cInterface::ShowBoss(const coreChar* pcMain, const coreChar* pcSub)
{
    // 
    this->__PrepareBanner();

    // set banner text
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // save animation properties
    m_fBannerStart    = g_pGame->GetTimeTable()->GetTimeEvent();
    m_fBannerDuration = INTERFACE_BANNER_DURATION_BOSS;
    m_fBannerSpeed    = INTERFACE_BANNER_SPEED_MISSION;   
    m_iBannerType     = INTERFACE_BANNER_TYPE_BOSS;

    {
        // realign objects as boss banner
        m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_3,  MENU_OUTLINE_SMALL);
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL);

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.06f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

        m_aBannerText[2].SetCenter(coreVector2(0.0f,0.02f));
        m_aBannerText[3].SetCenter(m_aBannerText[2].GetCenter());

        m_aBannerText[2].SetColor3(COLOR_MENU_INSIDE);
        m_aBannerText[3].SetColor3(g_pEnvironment->GetBackground()->GetColor());
    }

    // 
    m_BannerBar     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_BannerShadow  .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}

void cInterface::ShowBoss(const cBoss* pBoss)
{
    // show default boss banner
    this->ShowBoss(coreData::StrUpper(pBoss->GetName()), Core::Language->GetString(PRINT("BOSS_TITLE_%04d", pBoss->GetID())));
}


// ****************************************************************
// 
void cInterface::ShowWave(const coreChar* pcName)
{

    //if(pcName && pcName[0]) this->ShowStory(pcName);   
    // TODO 1: story-text font is still dynamic


    m_WaveName.SetText(pcName);
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
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_ALL);

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
    m_aBannerText[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aBannerText[2].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Medal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
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
void cInterface::ChangeBannerText(const coreChar* pcMain, const coreChar* pcSub)
{
    // 
    if(pcMain) m_aBannerText[0].SetText(pcMain);
    if(pcMain) m_aBannerText[1].SetText(pcMain);
    if(pcSub)  m_aBannerText[2].SetText(pcSub);
    if(pcMain) m_aBannerText[3].SetText(pcMain);
}


// ****************************************************************
// 
void cInterface::UpdateLayout()
{
    const coreVector2 vCenter = coreVector2(0.5f,0.5f) - (g_CurConfig.Game.iHudType ? coreVector2(1.0f,1.0f) : (Core::System->GetResolution().yx() * RCP(Core::System->GetResolution().Max()))) * 0.005f;
    // TODO 1: option 0.5f-0.45f, handle aspect ratio (for outside) (also up-down)

    // 
    const coreObject2DStyle eStyle  = CORE_OBJECT2D_STYLE_VIEWDIR | (g_CurConfig.Game.iHudType ? CORE_OBJECT2D_STYLE_ALTCENTER : CORE_OBJECT2D_STYLE_NOTHING);
    const coreVector2       vTop    = coreVector2(0.0f,  vCenter.y);
    const coreVector2       vBottom = coreVector2(0.0f, -vCenter.y);

    // 
    const auto nUpdateFunc = [&](coreObject2D* OUTPUT pObject, const coreVector2 vCenter)
    {
        pObject->SetCenter(vCenter);
        pObject->SetStyle (eStyle);
        pObject->Move();
    };

    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView = m_aView[i];

        // 
        const coreVector2 vSide = coreVector2(i ? vCenter.x : -vCenter.x, 0.0f);

        // 
        for(coreUintW j = 0u; j < INTERFACE_LIVES; ++j) nUpdateFunc(&oView.aLife[j], vBottom + vSide);
        nUpdateFunc(&oView.aShieldBar[0], vBottom + vSide);
        nUpdateFunc(&oView.aShieldBar[1], vBottom + vSide);
        nUpdateFunc(&oView.aShieldBar[2], vBottom + vSide);
        nUpdateFunc(&oView.oShieldValue,  vBottom + vSide);
        nUpdateFunc(&oView.oScoreTotal,   vTop    + vSide);
        nUpdateFunc(&oView.oScoreMission, vTop    + vSide);
        nUpdateFunc(&oView.oCooldownBar,  vTop    + vSide);
        nUpdateFunc(&oView.oComboValue,   vTop    + vSide);
        nUpdateFunc(&oView.oChainValue,   vTop    + vSide);
    }

    // 
    nUpdateFunc(&m_aBossHealthBar[0], vTop);
    nUpdateFunc(&m_aBossHealthBar[1], vTop);
    nUpdateFunc(&m_aBossHealthBar[2], vTop);
    nUpdateFunc(&m_BossHealthValue,   vTop);
    nUpdateFunc(&m_aBossTime[0],      vTop);
    nUpdateFunc(&m_aBossTime[1],      vTop);

    // 
    nUpdateFunc(&m_WaveName,     vBottom);
    nUpdateFunc(&m_aWaveTime[0], vTop);
    nUpdateFunc(&m_aWaveTime[1], vTop);
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
}


// ****************************************************************
// 
void cInterface::__PrepareBanner()
{
    // hide banner initially (to prevent flickering)
    m_BannerBar     .SetSize (coreVector2(0.0f,0.0f));
    m_BannerShadow  .SetAlpha(0.0f);
    m_aBannerText[0].SetAlpha(0.0f);
    m_aBannerText[1].SetAlpha(0.0f);
    m_aBannerText[2].SetAlpha(0.0f);
    m_aBannerText[3].SetAlpha(0.0f);
    
    m_BannerBar.Move();
}