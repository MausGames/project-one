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
// construct player view
void cInterface::sPlayerView::Construct(const coreUintW& iIndex)
{
    // create view objects
    m_aHealthBar[0].DefineProgram("default_2d_program");
    m_aHealthBar[0].DefineTexture(0u, "menu_detail_01.png");
    m_aHealthBar[0].SetPosition  (coreVector2( 0.0f, 0.005f));
    m_aHealthBar[0].SetSize      (coreVector2( 4.0f, 0.4f) * 0.07f);
    m_aHealthBar[0].SetCenter    (coreVector2(-0.5f,-0.5f) * g_vMenuCenter);
    m_aHealthBar[0].SetAlignment (coreVector2( 1.0f, 1.0f));

    m_aHealthBar[1].DefineProgram("default_2d_program");
    m_aHealthBar[1].DefineTexture(0u, "menu_detail_01.png");
    m_aHealthBar[1].SetPosition  (m_aHealthBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f);
    m_aHealthBar[1].SetSize      (m_aHealthBar[0].GetSize()     - coreVector2(0.01f,0.01f));
    m_aHealthBar[1].SetCenter    (m_aHealthBar[0].GetCenter());
    m_aHealthBar[1].SetAlignment (m_aHealthBar[0].GetAlignment());

    m_aHealthValue[0].Construct      (MENU_FONT_SMALL, 0u);
    m_aHealthValue[0].SetPosition    (m_aHealthBar[0].GetPosition() + coreVector2(0.035f,0.01f));
    m_aHealthValue[0].SetCenter      (m_aHealthBar[0].GetCenter());
    m_aHealthValue[0].SetAlignment   (m_aHealthBar[0].GetAlignment());
    m_aHealthValue[0].SetTextLanguage("ARMOR");

    m_aHealthValue[1].Construct   (MENU_FONT_SMALL, 10u);
    m_aHealthValue[1].SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(m_aHealthBar[0].GetSize().x - 0.035f, 0.01f));
    m_aHealthValue[1].SetCenter   (m_aHealthBar[0].GetCenter());
    m_aHealthValue[1].SetAlignment(coreVector2(-1.0f,1.0f));

    m_Score.Construct   (MENU_FONT_MEDIUM_2, 7u);
    m_Score.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.017f,0.035f));
    m_Score.SetCenter   (m_aHealthBar[0].GetCenter());
    m_Score.SetAlignment(m_aHealthBar[0].GetAlignment());

    m_Combo.Construct   (MENU_FONT_SMALL, 6u);
    m_Combo.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(m_aHealthBar[0].GetSize().x - 0.017f, 0.038f));
    m_Combo.SetCenter   (m_aHealthBar[0].GetCenter());
    m_Combo.SetAlignment(coreVector2(-1.0f,1.0f));

    m_ChainBar.DefineProgram("default_2d_program");
    m_ChainBar.DefineTexture(0u, "default_white.png");
    m_ChainBar.SetPosition  (m_Score.GetPosition() + coreVector2(0.001f,0.055f));
    m_ChainBar.SetCenter    (m_Score.GetCenter());
    m_ChainBar.SetAlignment (m_Score.GetAlignment());

    m_ChainValue.Construct   (MENU_FONT_SMALL, 8u);
    m_ChainValue.SetPosition (m_Score.GetPosition() + coreVector2(0.001f,0.03f));
    m_ChainValue.SetCenter   (m_Score.GetCenter());
    m_ChainValue.SetAlignment(m_Score.GetAlignment());
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreUint8& iNumViews)noexcept
: m_iNumViews    (iNumViews)
, m_fBannerStart (-100.0f)
, m_bBannerType  (false)
, m_fAlphaAll    (0.0f)
, m_fAlphaBoss   (0.0f)
, m_bVisible     (true)
{
    ASSERT(m_iNumViews <= INTERFACE_VIEWS)

    // construct player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        m_aView[i].Construct(i);

    // create interface objects
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].DefineTexture(0u, "menu_detail_02.png");
    m_aBossHealthBar[0].SetPosition  (coreVector2( 0.0f,-0.005f));
    m_aBossHealthBar[0].SetSize      (coreVector2( 8.0f, 0.4f) * 0.07f);
    m_aBossHealthBar[0].SetCenter    (coreVector2(-0.5f, 0.5f) * g_vMenuCenter);
    m_aBossHealthBar[0].SetAlignment (coreVector2( 1.0f,-1.0f));

    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].DefineTexture(0u, "menu_detail_02.png");
    m_aBossHealthBar[1].SetPosition  (m_aBossHealthBar[0].GetPosition() + coreVector2(0.01f,-0.01f) * 0.5f);
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize()     - coreVector2(0.01f, 0.01f));
    m_aBossHealthBar[1].SetCenter    (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthBar[1].SetAlignment (m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthValue[0].Construct   (MENU_FONT_SMALL, 7u);
    m_aBossHealthValue[0].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(0.05f,-0.01f));
    m_aBossHealthValue[0].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[0].SetAlignment(m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthValue[1].Construct   (MENU_FONT_SMALL, 14u);
    m_aBossHealthValue[1].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(m_aBossHealthBar[0].GetSize().x - 0.05f, -0.01f));
    m_aBossHealthValue[1].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[1].SetAlignment(coreVector2(-1.0f,-1.0f));

    m_BossName.Construct   (MENU_FONT_MEDIUM_2, 0u);
    m_BossName.SetPosition (coreVector2(-0.009f,-0.004f));
    m_BossName.SetCenter   (coreVector2( 0.5f,   0.5f) * g_vMenuCenter);
    m_BossName.SetAlignment(coreVector2(-1.0f,  -1.0f));

    m_aBossTime[0].Construct   (MENU_FONT_MEDIUM_2, 5u);
    m_aBossTime[0].SetPosition (m_BossName.GetPosition() + coreVector2(-0.023f,-0.025f));
    m_aBossTime[0].SetCenter   (m_BossName.GetCenter());
    m_aBossTime[0].SetAlignment(m_BossName.GetAlignment());

    m_aBossTime[1].Construct   (MENU_FONT_MEDIUM_2, 2u);
    m_aBossTime[1].SetPosition (m_BossName.GetPosition() + coreVector2(0.0f,-0.025f));
    m_aBossTime[1].SetCenter   (m_BossName.GetCenter());
    m_aBossTime[1].SetAlignment(m_BossName.GetAlignment());

    m_BannerBar.DefineProgram("menu_animate_program");
    m_BannerBar.DefineTexture(0u, "menu_detail_03.png");
    m_BannerBar.DefineTexture(1u, "menu_background_black.png");

    m_aBannerText[0].Construct(MENU_FONT_BIG_7,    0u);
    m_aBannerText[1].Construct(MENU_FONT_BIG_7,    0u);
    m_aBannerText[2].Construct(MENU_FONT_MEDIUM_2, 0u);
    m_aBannerText[0].SetColor3(coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);
    m_aBannerText[1].SetColor3(coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);
    m_aBannerText[2].SetColor3(coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);
    m_aBannerText[3].SetColor3(coreVector3(1.00f,1.00f,1.00f) * MENU_CONTRAST_WHITE);

    // 
    this->UpdateLayout();
}


// ****************************************************************
// render the interface
void cInterface::Render()
{
    if(m_fAlphaAll)
    {
        // loop through all player views
        for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        {
            sPlayerView& oView = m_aView[i];

            // render player
            oView.m_aHealthBar[0]  .Render();
            oView.m_aHealthBar[1]  .Render();
            oView.m_aHealthValue[0].Render();
            oView.m_aHealthValue[1].Render();
            oView.m_Score          .Render();
            oView.m_Combo          .Render();
            oView.m_ChainValue     .Render();   // # swapped
            oView.m_ChainBar       .Render();   // # swapped
        }

        if(m_fAlphaBoss)
        {
            // render boss
            m_aBossHealthBar[0]  .Render();
            m_aBossHealthBar[1]  .Render();
            m_aBossHealthValue[0].Render();
            m_aBossHealthValue[1].Render();
            m_aBossTime[0]       .Render();
            m_aBossTime[1]       .Render();
            m_BossName           .Render();
        }
    }

    if(this->IsBannerActive())
    {
        // render banner
        m_BannerBar     .Render();
        m_aBannerText[0].Render();
        m_aBannerText[1].Render();
        m_aBannerText[2].Render();
        m_aBannerText[3].Render();
    }
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    // loop through all player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
    {
        sPlayerView& oView   = m_aView[i];
        cPlayer*     pPlayer = g_pGame->GetPlayer(i);

        // set health bar size
        const coreFloat fPercent = pPlayer->GetCurHealthPct();
        oView.m_aHealthBar[1].SetSize   ((oView.m_aHealthBar[0].GetSize() - coreVector2(0.01f,0.01f)) * coreVector2(fPercent, 1.0f));
        oView.m_aHealthBar[1].SetTexSize(coreVector2(fPercent, 1.0f));

        // set health bar color
        const coreVector3 vColor = cMenu::HealthColor(fPercent);
        oView.m_aHealthBar[0].SetColor3(vColor * 0.2f);
        oView.m_aHealthBar[1].SetColor3(vColor * 0.9f);

        // set chain bar size and color
        oView.m_ChainBar.SetSize  (coreVector2(0.05f * MIN(pPlayer->GetChainCooldown()*1.1f, 1.0f), 0.005f));
        oView.m_ChainBar.SetColor3(cMenu::ChainColor(pPlayer->GetChainCooldown()));

        // display player values
        oView.m_aHealthValue[1].SetText(PRINT("%d / %d", pPlayer->GetCurHealth(), pPlayer->GetMaxHealth()));
        oView.m_Score          .SetText(PRINT("%06d",    pPlayer->GetScoreMission()));
        oView.m_Combo          .SetText(PRINT("x%.1f",   pPlayer->GetCurCombo()));
        oView.m_ChainValue     .SetText(pPlayer->GetCurChain() ? PRINT("+%d", pPlayer->GetCurChain()) : "");

        // set player transparency
        oView.m_aHealthBar[0]  .SetAlpha(m_fAlphaAll);
        oView.m_aHealthBar[1]  .SetAlpha(m_fAlphaAll);
        oView.m_aHealthValue[0].SetAlpha(m_fAlphaAll);
        oView.m_aHealthValue[1].SetAlpha(m_fAlphaAll);
        oView.m_Score          .SetAlpha(m_fAlphaAll);
        oView.m_Combo          .SetAlpha(m_fAlphaAll);
        oView.m_ChainBar       .SetAlpha(m_fAlphaAll);
        oView.m_ChainValue     .SetAlpha(m_fAlphaAll);

        // move player
        oView.m_aHealthBar[0]  .Move();
        oView.m_aHealthBar[1]  .Move();
        oView.m_aHealthValue[0].Move();
        oView.m_aHealthValue[1].Move();
        oView.m_Score          .Move();
        oView.m_Combo          .Move();
        oView.m_ChainBar       .Move();
        oView.m_ChainValue     .Move();
    }

    // check for active boss
    cBoss* pBoss = g_pGame->GetMission()->GetCurBoss();
    if(pBoss)
    {
        // set health bar size
        const coreFloat fPercent = pBoss->GetCurHealthPct();
        m_aBossHealthBar[1].SetSize   ((m_aBossHealthBar[0].GetSize() - coreVector2(0.01f,0.01f)) * coreVector2(fPercent, 1.0f));
        m_aBossHealthBar[1].SetTexSize(coreVector2(fPercent, 1.0f));

        // set health bar color
        const coreVector3 vColor = cMenu::HealthColor(fPercent * 0.5f);
        m_aBossHealthBar[0].SetColor3(vColor * 0.2f);
        m_aBossHealthBar[1].SetColor3(vColor * 0.9f);

        // display boss values
        m_aBossHealthValue[0].SetText(PRINT("[%.0f%%]", fPercent * 100.0f));
        m_aBossHealthValue[1].SetText(PRINT("%d / %d",  pBoss->GetCurHealth(), pBoss->GetMaxHealth()));

        // display boss time
        const coreFloat fTimeBoss = MAX(g_pGame->GetTimeBoss(g_pGame->GetMission()->GetCurBossIndex()), 0.0f);
        m_aBossTime[0].SetText(PRINT("%.0f.", FLOOR(      fTimeBoss)));
        m_aBossTime[1].SetText(PRINT("%.0f",  FLOOR(FRACT(fTimeBoss)*10.0f)));

        // set boss transparency
        const coreFloat fAlphaBossFull = m_fAlphaAll * m_fAlphaBoss;
        m_aBossHealthBar[0]  .SetAlpha(fAlphaBossFull);
        m_aBossHealthBar[1]  .SetAlpha(fAlphaBossFull);
        m_aBossHealthValue[0].SetAlpha(fAlphaBossFull);
        m_aBossHealthValue[1].SetAlpha(fAlphaBossFull);
        m_aBossTime[0]       .SetAlpha(fAlphaBossFull);
        m_aBossTime[1]       .SetAlpha(fAlphaBossFull);
        m_BossName           .SetAlpha(fAlphaBossFull);

        // move boss
        m_aBossHealthBar[0]  .Move();
        m_aBossHealthBar[1]  .Move();
        m_aBossHealthValue[0].Move();
        m_aBossHealthValue[1].Move();
        m_aBossTime[0]       .Move();
        m_aBossTime[1]       .Move();
        m_BossName           .Move();
    }

    // check for active banner
    const coreFloat fBanner = g_pGame->GetTimeMission() - m_fBannerStart;
    if((fBanner <= INTERFACE_BANNER_DURATION) && (fBanner >= 0.0f))
    {
        // calculate visibility and animation value
        const coreFloat fVisibility = MIN(fBanner, INTERFACE_BANNER_DURATION - fBanner, 1.0f / INTERFACE_BANNER_SPEED) * INTERFACE_BANNER_SPEED;
        const coreFloat fAnimation  = LERPB(0.0f, INTERFACE_BANNER_ANIMATION, fBanner / INTERFACE_BANNER_ANIMATION);

        // slash banner across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
        const coreBool bLeftRight = (fBanner < (INTERFACE_BANNER_DURATION * 0.5f)) ? false : true;
        m_BannerBar.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_BannerBar.GetDirection().yx());
        m_BannerBar.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

        // animate banner bar
        m_BannerBar.SetSize     (coreVector2(fVisibility, 1.0f) * (coreVector2(4.0f, m_bBannerType ? 0.9f : 0.8f) * 0.25f));
        m_BannerBar.SetTexSize  (coreVector2(fVisibility, 1.0f));
        m_BannerBar.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

        // animate banner text
        const coreFloat fTextOffset = (fAnimation + 3.0f) * 0.01f;
        m_aBannerText[0].SetPosition(m_bBannerType ? coreVector2(-0.0155f,  fTextOffset) : coreVector2( fTextOffset,  0.019f));
        m_aBannerText[1].SetPosition(m_bBannerType ? coreVector2( 0.0155f, -fTextOffset) : coreVector2(-fTextOffset, -0.012f));

        // set banner transparency
        m_aBannerText[0].SetAlpha(fVisibility * 0.2f);
        m_aBannerText[1].SetAlpha(fVisibility * 0.2f);
        m_aBannerText[2].SetAlpha(fVisibility);
        m_aBannerText[3].SetAlpha(fVisibility);

        // move banner
        m_BannerBar     .Move();
        m_aBannerText[0].Move();
        m_aBannerText[1].Move();
        m_aBannerText[2].Move();
        m_aBannerText[3].Move();
    }

    // smoothly toggle interface visibility (after forwarding, can be overloaded)
    if(m_bVisible)
         {if(m_fAlphaAll < 1.0f) m_fAlphaAll = MIN(m_fAlphaAll + Core::System->GetTime(), 1.0f);}
    else {if(m_fAlphaAll > 0.0f) m_fAlphaAll = MAX(m_fAlphaAll - Core::System->GetTime(), 0.0f);}

    // smoothly toggle boss data visibility
    if(pBoss && (m_bBannerType == INTERFACE_BANNER_TYPE_BOSS) && (fBanner >= INTERFACE_BOSS_DELAY))
         {if(m_fAlphaBoss < 1.0f) m_fAlphaBoss = MIN(m_fAlphaBoss + Core::System->GetTime(), 1.0f);}
    else {if(m_fAlphaBoss > 0.0f) m_fAlphaBoss = MAX(m_fAlphaBoss - Core::System->GetTime(), 0.0f);}
}


// ****************************************************************
// show boss banner
void cInterface::ShowBoss(const coreChar* pcMain, const coreChar* pcSub)
{
    // set boss name
    m_BossName.SetText(pcMain);

    // set banner text
    m_aBannerText[0].SetText(pcMain);
    m_aBannerText[1].SetText(pcMain);
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // hide banner initially (to prevent flickering)
    m_BannerBar     .SetSize (coreVector2(0.0f,0.0f));
    m_aBannerText[0].SetAlpha(0.0f);
    m_aBannerText[1].SetAlpha(0.0f);
    m_aBannerText[2].SetAlpha(0.0f);
    m_aBannerText[3].SetAlpha(0.0f);

    // save animation properties
    m_fBannerStart = g_pGame->GetTimeMission();
    m_bBannerType  = INTERFACE_BANNER_TYPE_BOSS;
    {
        // and realign objects as boss banner
        m_aBannerText[3].Construct(MENU_FONT_BIG_4, 0u);

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.027f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.012f));

        m_BannerBar     .SetDirection(coreVector2(0.0f,1.0f));
        m_aBannerText[0].SetDirection(m_BannerBar.GetDirection());
        m_aBannerText[1].SetDirection(m_BannerBar.GetDirection());

        m_BannerBar     .SetCenter(coreVector2(0.0f,0.1f) * g_vMenuCenter);
        m_aBannerText[0].SetCenter(m_BannerBar.GetCenter());
        m_aBannerText[1].SetCenter(m_BannerBar.GetCenter());
        m_aBannerText[2].SetCenter(m_BannerBar.GetCenter());
        m_aBannerText[3].SetCenter(m_BannerBar.GetCenter());
    }
}

void cInterface::ShowBoss(const cBoss* pBoss)
{
    // show default boss banner
    this->ShowBoss(pBoss->GetName(), Core::Language->GetString(PRINT("BOSS_TITLE_%04d", pBoss->GetID())));
}


// ****************************************************************
// show mission banner
void cInterface::ShowMission(const coreChar* pcMain, const coreChar* pcSub)
{
    // set banner text
    m_aBannerText[0].SetText(pcMain);
    m_aBannerText[1].SetText(pcMain);
    m_aBannerText[2].SetText(pcSub);
    m_aBannerText[3].SetText(pcMain);

    // hide banner initially (to prevent flickering)
    m_BannerBar     .SetSize (coreVector2(0.0f,0.0f));
    m_aBannerText[0].SetAlpha(0.0f);
    m_aBannerText[1].SetAlpha(0.0f);
    m_aBannerText[2].SetAlpha(0.0f);
    m_aBannerText[3].SetAlpha(0.0f);

    // save animation properties
    m_fBannerStart = -1.0f;
    m_bBannerType  = INTERFACE_BANNER_TYPE_MISSION;
    {
        // and realign objects as mission banner
        m_aBannerText[3].Construct(MENU_FONT_MEDIUM_3, 0u);

        m_aBannerText[2].SetPosition(coreVector2(0.0f, 0.025f));
        m_aBannerText[3].SetPosition(coreVector2(0.0f,-0.01f));

        m_BannerBar     .SetDirection(coreVector2(-1.0f,0.0f));
        m_aBannerText[0].SetDirection(m_BannerBar.GetDirection());
        m_aBannerText[1].SetDirection(m_BannerBar.GetDirection());

        m_BannerBar     .SetCenter(coreVector2(0.3f,0.0f)  * g_vMenuCenter);
        m_aBannerText[0].SetCenter(coreVector2(0.3f,0.02f) * g_vMenuCenter);
        m_aBannerText[1].SetCenter(m_aBannerText[0].GetCenter());
        m_aBannerText[2].SetCenter(m_aBannerText[0].GetCenter());
        m_aBannerText[3].SetCenter(m_aBannerText[0].GetCenter());
    }
}

void cInterface::ShowMission(const cMission* pMission)
{
    // show default mission banner
    this->ShowMission(pMission->GetName(), PRINT("%s %d", Core::Language->GetString("STAGE"), pMission->GetID()));
}


// ****************************************************************
// check for active banner
coreBool cInterface::IsBannerActive()const
{
    // compare with mission-time offset
    return ((g_pGame->GetTimeMission() - m_fBannerStart) <= INTERFACE_BANNER_DURATION) ? true : false;
}


// ****************************************************************
// 
void cInterface::UpdateLayout()
{return;
/*
        for(coreUintW i = 0u; i < INTERFACE_VIEWS; ++i)
        {
            sPlayerView& oView = m_aView[i];


            oView.m_aHealthBar[0]  .SetCenter(coreVector2(-0.5f,-0.5f));
            oView.m_aHealthBar[1]  .SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_aHealthValue[0].SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_aHealthValue[1].SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_Score          .SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_Combo          .SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_ChainBar       .SetCenter(oView.m_aHealthBar[0].GetCenter());
            oView.m_ChainValue     .SetCenter(oView.m_aHealthBar[0].GetCenter());

            m_aBossHealthBar[0]  .SetCenter(coreVector2(-0.5f,0.5f));
            m_aBossHealthBar[1]  .SetCenter(m_aBossHealthBar[0].GetCenter());
            m_aBossHealthValue[0].SetCenter(m_aBossHealthBar[0].GetCenter());
            m_aBossHealthValue[1].SetCenter(m_aBossHealthBar[0].GetCenter());

            m_aBossTime[0].SetCenter(coreVector2(0.5f,0.5f));
            m_aBossTime[1].SetCenter(m_aBossTime[0].GetCenter());
            m_BossName    .SetCenter(m_aBossTime[0].GetCenter());


            m_aBossHealthBar[0].SetDirection(coreVector2(1.0f,0.0f));
            m_aBossHealthBar[1].SetDirection(m_aBossHealthBar[0].GetDirection());
            */
        //    oView.m_aHealthBar[0]  .SetDirection(coreVector2(1.0f,0.0f));
        //    oView.m_aHealthBar[1]  .SetDirection(oView.m_aHealthBar[0].GetDirection());
        //
        //
        //
        //    oView.m_aHealthBar[0].SetPosition  (coreVector2( 0.005f, 0.0f));
        //    oView.m_aHealthBar[0].SetSize      (coreVector2( 4.0f, 0.4f) * 0.07f);
        //    oView.m_aHealthBar[0].SetCenter    (coreVector2(-0.5f,-0.5f) * g_vMenuCenter);
        //    oView.m_aHealthBar[0].SetAlignment (coreVector2( 1.0f, 1.0f));
        //
        //    oView.m_aHealthBar[1].SetPosition  (oView.m_aHealthBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f);
        //    oView.m_aHealthBar[1].SetSize      (oView.m_aHealthBar[0].GetSize()     - coreVector2(0.01f,0.01f));
        //    oView.m_aHealthBar[1].SetCenter    (oView.m_aHealthBar[0].GetCenter());
        //    oView.m_aHealthBar[1].SetAlignment (oView.m_aHealthBar[0].GetAlignment());
        //
        //
        //    oView.m_aHealthValue[0].SetPosition    (oView.m_aHealthBar[0].GetPosition() + coreVector2(0.015f,0.03f));
        //    oView.m_aHealthValue[0].SetCenter      (oView.m_aHealthBar[0].GetCenter());
        //    oView.m_aHealthValue[0].SetAlignment   (oView.m_aHealthBar[0].GetAlignment());
        //
        //    oView.m_aHealthValue[1].SetPosition (oView.m_aHealthBar[0].GetPosition() + coreVector2(0.015f,0.01f));
        //    oView.m_aHealthValue[1].SetCenter   (oView.m_aHealthBar[0].GetCenter());
        //    oView.m_aHealthValue[1].SetAlignment(oView.m_aHealthBar[0].GetAlignment());
        //
        //        //oView.m_Score.Construct   (MENU_FONT_SMALL, 7u);
        //    oView.m_Score.SetPosition (coreVector2(-0.02f,0.3f));
        //    oView.m_Score.SetCenter   (oView.m_aHealthBar[0].GetCenter());
        //    oView.m_Score.SetAlignment(coreVector2(-1.0f,1.0f));
        //
        //    oView.m_Combo.SetPosition (oView.m_aHealthBar[0].GetPosition() + coreVector2(oView.m_aHealthBar[0].GetSize().x-0.017f,0.08f+0.003f));
        //    oView.m_Combo.SetCenter   (oView.m_aHealthBar[0].GetCenter());
        //    oView.m_Combo.SetAlignment(coreVector2(-1.0f,1.0f));
        //
        //    oView.m_Chain.SetPosition (oView.m_Score.GetPosition() + coreVector2(0.001f,0.03f));
        //    oView.m_Chain.SetCenter   (oView.m_Score.GetCenter());
        //    oView.m_Chain.SetAlignment(coreVector2(1.0f,1.0f));
        //
        //
        //
        //        oView.m_aHealthBar[0]  .SetAlignment(oView.m_aHealthBar[0]  .GetAlignment().InvertedX());
        //        oView.m_aHealthBar[0]  .SetPosition (oView.m_aHealthBar[0]  .GetPosition ().InvertedX() + coreVector2(-0.015f,0.0f));
        //        oView.m_aHealthBar[1]  .SetAlignment(oView.m_aHealthBar[1]  .GetAlignment().InvertedX());
        //        oView.m_aHealthBar[1]  .SetPosition (oView.m_aHealthBar[1]  .GetPosition ().InvertedX() + coreVector2(-0.015f,0.0f));
        //        oView.m_aHealthValue[0].SetAlignment(oView.m_aHealthValue[0].GetAlignment().InvertedX());
        //        oView.m_aHealthValue[0].SetPosition (oView.m_aHealthValue[0].GetPosition ().InvertedX() + coreVector2(-0.01f,0.0f));
        //        oView.m_aHealthValue[1].SetAlignment(oView.m_aHealthValue[1].GetAlignment().InvertedX());
        //        oView.m_aHealthValue[1].SetPosition (oView.m_aHealthValue[1].GetPosition ().InvertedX() + coreVector2(-0.01f,0.0f));
        //
      //  }

}