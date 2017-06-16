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
void cInterface::sPlayerView::Construct(const coreUintW iIndex)
{
    // create view objects
    aHealthBar[0].DefineProgram("default_2d_program");
    aHealthBar[0].DefineTexture(0u, "menu_detail_01.png");
    aHealthBar[0].SetPosition  (coreVector2( 0.0f, 0.005f));
    aHealthBar[0].SetSize      (coreVector2( 4.0f, 0.4f) * 0.07f);
    aHealthBar[0].SetCenter    (coreVector2(-0.5f,-0.5f) * g_vMenuCenter);
    aHealthBar[0].SetAlignment (coreVector2( 1.0f, 1.0f));

    aHealthBar[1].DefineProgram("default_2d_program");
    aHealthBar[1].DefineTexture(0u, "menu_detail_01.png");
    aHealthBar[1].SetPosition  (aHealthBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f);
    aHealthBar[1].SetSize      (aHealthBar[0].GetSize()     - coreVector2(0.01f,0.01f));
    aHealthBar[1].SetCenter    (aHealthBar[0].GetCenter());
    aHealthBar[1].SetAlignment (aHealthBar[0].GetAlignment());

    aHealthValue[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL, 0u);
    aHealthValue[0].SetPosition    (aHealthBar[0].GetPosition() + coreVector2(0.035f,0.01f));
    aHealthValue[0].SetCenter      (aHealthBar[0].GetCenter());
    aHealthValue[0].SetAlignment   (aHealthBar[0].GetAlignment());
    aHealthValue[0].SetTextLanguage("ARMOR");

    aHealthValue[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL, 9u);
    aHealthValue[1].SetPosition (aHealthBar[0].GetPosition() + coreVector2(aHealthBar[0].GetSize().x - 0.035f, 0.01f));
    aHealthValue[1].SetCenter   (aHealthBar[0].GetCenter());
    aHealthValue[1].SetAlignment(aHealthBar[0].GetAlignment().InvertedX());

    oScore.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL, 8u);
    oScore.SetPosition (coreVector2( 0.01f,-0.005f));
    oScore.SetCenter   (coreVector2(-0.5f,  0.5f) * g_vMenuCenter);
    oScore.SetAlignment(coreVector2( 1.0f, -1.0f));

    oCombo.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL, 5u);
    oCombo.SetPosition (oScore.GetPosition() + coreVector2(0.002f,-0.03f));
    oCombo.SetCenter   (oScore.GetCenter());
    oCombo.SetAlignment(oScore.GetAlignment());

    oChainBar.DefineProgram("default_2d_program");
    oChainBar.DefineTexture(0u, "default_white.png");
    oChainBar.SetPosition  (oScore.GetPosition() + coreVector2(0.001f,-0.055f));
    oChainBar.SetCenter    (oScore.GetCenter());
    oChainBar.SetAlignment (oScore.GetAlignment());

    oChainValue.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL, 8u);
    oChainValue.SetPosition (oScore.GetPosition() + coreVector2(0.001f,-0.06f));
    oChainValue.SetCenter   (oScore.GetCenter());
    oChainValue.SetAlignment(oScore.GetAlignment());
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreUint8 iNumViews)noexcept
: m_iNumViews    (iNumViews)
, m_fBannerStart (-FLT_MAX)
, m_bBannerType  (false)
, m_fStoryStart  (-FLT_MAX)
, m_bVisible     (false)
, m_fAlphaAll    (0.0f)
, m_fAlphaBoss   (0.0f)
{
    ASSERT(m_iNumViews <= INTERFACE_VIEWS)

    // construct player views
    for(coreUintW i = 0u, ie = m_iNumViews; i < ie; ++i)
        m_aView[i].Construct(i);

    // create interface objects
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].DefineTexture(0u, "menu_detail_02.png");
    m_aBossHealthBar[0].SetPosition  (coreVector2(0.0f,-0.005f));
    m_aBossHealthBar[0].SetSize      (coreVector2(8.0f, 0.4f) * 0.07f);
    m_aBossHealthBar[0].SetCenter    (coreVector2(0.0f, 0.5f) * g_vMenuCenter);
    m_aBossHealthBar[0].SetAlignment (coreVector2(0.0f,-1.0f));

    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].DefineTexture(0u, "menu_detail_02.png");
    m_aBossHealthBar[1].SetPosition  (m_aBossHealthBar[0].GetPosition() + coreVector2(0.00f,-0.01f) * 0.5f);
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize()     - coreVector2(0.01f, 0.01f));
    m_aBossHealthBar[1].SetCenter    (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthBar[1].SetAlignment (m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthValue[0].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL, 6u);
    m_aBossHealthValue[0].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(-(m_aBossHealthBar[0].GetSize().x*0.5f - 0.05f), -0.01f));
    m_aBossHealthValue[0].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[0].SetAlignment(coreVector2(1.0f,-1.0f));

    m_aBossHealthValue[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL, 13u);
    m_aBossHealthValue[1].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2( (m_aBossHealthBar[0].GetSize().x*0.5f - 0.05f), -0.01f));
    m_aBossHealthValue[1].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[1].SetAlignment(coreVector2(-1.0f,-1.0f));

    m_BossName.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL, 0u);
    m_BossName.SetPosition (coreVector2(-0.01f,-0.005f));
    m_BossName.SetCenter   (coreVector2( 0.5f,   0.5f) * g_vMenuCenter);
    m_BossName.SetAlignment(coreVector2(-1.0f,  -1.0f));

    m_aBossTime[0].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL, 5u);
    m_aBossTime[0].SetPosition (m_BossName.GetPosition() + coreVector2(-0.023f,-0.025f));
    m_aBossTime[0].SetCenter   (m_BossName.GetCenter());
    m_aBossTime[0].SetAlignment(m_BossName.GetAlignment());

    m_aBossTime[1].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL, 1u);
    m_aBossTime[1].SetPosition (m_BossName.GetPosition() + coreVector2(0.0f,-0.025f));
    m_aBossTime[1].SetCenter   (m_BossName.GetCenter());
    m_aBossTime[1].SetAlignment(m_BossName.GetAlignment());

    m_BannerBar.DefineProgram("menu_animate_program");
    m_BannerBar.DefineTexture(0u, "menu_detail_03.png");
    m_BannerBar.DefineTexture(1u, "menu_background_black.png");

    m_aBannerText[0].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL, 0u);
    m_aBannerText[1].Construct(MENU_FONT_STANDARD_5, MENU_OUTLINE_SMALL, 0u);
    m_aBannerText[2].Construct(MENU_FONT_DYNAMIC_2,  MENU_OUTLINE_SMALL, 0u);
    m_aBannerText[0].SetColor3(COLOR_MENU_WHITE * 0.75f);
    m_aBannerText[1].SetColor3(COLOR_MENU_WHITE * 0.75f);
    m_aBannerText[2].SetColor3(COLOR_MENU_WHITE * 0.75f);
    m_aBannerText[3].SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryText); ++i)
        m_aStoryText[i].Construct(MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL, 0u);

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
            //sPlayerView& oView = m_aView[i];

            // render player
            //oView.aHealthBar[0]  .Render();
            //oView.aHealthBar[1]  .Render();
            //oView.aHealthValue[0].Render();
            //oView.aHealthValue[1].Render();
            //oView.oScore         .Render();
            //oView.oCombo         .Render();
            //oView.oChainValue    .Render();   // # swapped
            //oView.oChainBar      .Render();   // # swapped
        }

        if(m_fAlphaBoss)
        {
            // render boss
            m_aBossHealthBar[0]  .Render();
            m_aBossHealthBar[1]  .Render();
            m_aBossHealthValue[0].Render();
            m_aBossHealthValue[1].Render();
            //m_aBossTime[0]       .Render();
            //m_aBossTime[1]       .Render();
            //m_BossName           .Render();
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

    if(this->IsStoryActive(0.0f))
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryText); ++i)
            m_aStoryText[i].Render();
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
        const coreFloat fWidth   = oView.aHealthBar[0].GetSize().x - 0.01f;
        oView.aHealthBar[1].SetSize     (coreVector2(fPercent * fWidth, oView.aHealthBar[1].GetSize().y));
        oView.aHealthBar[1].SetTexSize  (coreVector2(fPercent, 1.0f));
        oView.aHealthBar[1].SetTexOffset(coreVector2(i ? (1.0f-fPercent) : 0.0f, 0.0f));

        // set health bar color
        const coreVector3 vColor = COLOR_HEALTH(fPercent);
        oView.aHealthBar[0].SetColor3(vColor * 0.2f);
        oView.aHealthBar[1].SetColor3(vColor * 0.9f);

        // set chain bar size and color
        oView.oChainBar.SetSize  (coreVector2(0.05f * MIN(pPlayer->GetChainCooldown()*1.1f, 1.0f), 0.005f));
        oView.oChainBar.SetColor3(COLOR_CHAIN(pPlayer->GetChainCooldown()));

        // display player values
        oView.aHealthValue[1].SetText(PRINT("%d / %d", pPlayer->GetCurHealth(), pPlayer->GetMaxHealth()));
        oView.oScore         .SetText(PRINT("%07d",    pPlayer->GetScoreMission()));
        oView.oCombo         .SetText(PRINT("x%.1f",   pPlayer->GetCurCombo()));
        oView.oChainValue    .SetText(pPlayer->GetCurChain() ? PRINT("+%d", pPlayer->GetCurChain()) : "");

        // set player transparency
        oView.aHealthBar[0]  .SetAlpha(m_fAlphaAll);
        oView.aHealthBar[1]  .SetAlpha(m_fAlphaAll);
        oView.aHealthValue[0].SetAlpha(m_fAlphaAll);
        oView.aHealthValue[1].SetAlpha(m_fAlphaAll);
        oView.oScore         .SetAlpha(m_fAlphaAll);
        oView.oCombo         .SetAlpha(m_fAlphaAll);
        oView.oChainBar      .SetAlpha(m_fAlphaAll);
        oView.oChainValue    .SetAlpha(m_fAlphaAll);

        // move player
        oView.aHealthBar[0]  .Move();
        oView.aHealthBar[1]  .Move();
        oView.aHealthValue[0].Move();
        oView.aHealthValue[1].Move();
        oView.oScore         .Move();
        oView.oCombo         .Move();
        oView.oChainBar      .Move();
        oView.oChainValue    .Move();
    }

    // check for active boss
    cBoss* pBoss = g_pGame->GetMission()->GetCurBoss();
    if(pBoss)
    {
        // set health bar size
        const coreFloat fPercent = pBoss->GetCurHealthPct();
        const coreFloat fWidth   = m_aBossHealthBar[0].GetSize().x - 0.01f;
        m_aBossHealthBar[1].SetPosition(coreVector2(fWidth * (fPercent-1.0f) * 0.5f, m_aBossHealthBar[1].GetPosition().y));
        m_aBossHealthBar[1].SetSize    (coreVector2(fWidth *  fPercent,              m_aBossHealthBar[1].GetSize    ().y));
        m_aBossHealthBar[1].SetTexSize (coreVector2(fPercent, 1.0f));

        // set health bar color
        const coreVector3 vColor = COLOR_HEALTH(fPercent * 0.5f);
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
    const coreFloat fBanner = g_pGame->GetTimeGame() - m_fBannerStart;
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
        const coreFloat fTextOffset = (fAnimation + 2.0f) * 0.012f;
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

    // 
    const coreFloat fStory = g_pGame->GetTimeGame() - m_fStoryStart;
    if((fStory <= INTERFACE_STORY_DURATION) && (fStory >= 0.0f))
    {
        // 
        const coreFloat fVisibility = MIN(fStory, INTERFACE_STORY_DURATION - fStory, 1.0f / INTERFACE_STORY_SPEED) * INTERFACE_STORY_SPEED;

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStoryText); ++i)
        {
            m_aStoryText[i].SetAlpha(fVisibility);
            m_aStoryText[i].Move();
        }
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
    m_fBannerStart = g_pGame->GetTimeGame();
    m_bBannerType  = INTERFACE_BANNER_TYPE_BOSS;
    {
        // and realign objects as boss banner
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL, 0u);

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
    m_fBannerStart = g_pGame->GetTimeGame();
    m_bBannerType  = INTERFACE_BANNER_TYPE_MISSION;
    {
        // and realign objects as mission banner
        m_aBannerText[3].Construct(MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL, 0u);

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
    // compare with game-time offset
    return ((g_pGame->GetTimeGame() - m_fBannerStart) <= INTERFACE_BANNER_DURATION) ? true : false;
}


// ****************************************************************
// 
void cInterface::ShowStory(const coreChar* pcRow1, const coreChar* pcRow2)
{
    const coreFloat fHeight = (pcRow2 ? 0.02f : 0.0f);

    // 
    m_aStoryText[0].SetText(pcRow1);
    m_aStoryText[0].SetPosition(coreVector2(0.0f, fHeight));

    if(pcRow2)
    {
        // 
        m_aStoryText[1].SetText(pcRow2);
        m_aStoryText[1].SetPosition(coreVector2(0.0f, -fHeight));
    }

    // 
    m_fStoryStart = g_pGame->GetTimeGame();
}


// ****************************************************************
// 
coreBool cInterface::IsStoryActive(const coreFloat fOffset)const
{
    // 
    ASSERT(fOffset < INTERFACE_STORY_DURATION)
    return ((g_pGame->GetTimeGame() - m_fStoryStart) <= (INTERFACE_STORY_DURATION - fOffset)) ? true : false;
}


// ****************************************************************
// 
void cInterface::UpdateLayout()
{
    /*
    for(coreUintW i = 0u; i < INTERFACE_VIEWS; ++i)
    {
        sPlayerView& oView = m_aView[i];

        const coreUint8 iType = 2u;

        const coreFloat A = ((iType == 1u) ? -1.0f : 1.0f); // type
        const coreFloat B = ((i     == 1u) ? -1.0f : 1.0f); // player

        const coreFloat fSide = A * B;
        const coreFloat fOffset = ((iType == 1u) ? 0.015f : 0.0f);

        oView.m_aHealthBar[0]  .SetPosition(coreVector2( (0.0f + fOffset) * fSide, 0.005f));
        oView.m_aHealthBar[1]  .SetPosition(oView.m_aHealthBar[0].GetPosition() + coreVector2(0.01f * fSide,0.01f) * 0.5f);
        oView.m_aHealthValue[0].SetPosition(oView.m_aHealthBar[0].GetPosition() + coreVector2(0.035f * fSide,0.01f));
        oView.m_aHealthValue[1].SetPosition(oView.m_aHealthBar[0].GetPosition() + coreVector2((oView.m_aHealthBar[0].GetSize().x - 0.035f) * fSide, 0.01f));

        oView.m_Score          .SetPosition(coreVector2(0.01f * fSide,0.005f) * coreVector2(1.0f,-1.0f));
        oView.m_Combo          .SetPosition(coreVector2(0.012f * fSide,0.035f) * coreVector2(1.0f,-1.0f));
        oView.m_ChainBar       .SetPosition(oView.m_Score.GetPosition() + coreVector2(0.001f * fSide,0.055f));
        oView.m_ChainValue     .SetPosition(oView.m_Score.GetPosition() + coreVector2(0.001f * fSide,0.03f));

        const coreVector2 vAlignment = coreVector2( A * B, 1.0f);
        oView.m_aHealthBar[0]  .SetAlignment(vAlignment);
        oView.m_aHealthBar[1]  .SetAlignment(vAlignment);
        oView.m_aHealthValue[0].SetAlignment(vAlignment);
        oView.m_aHealthValue[1].SetAlignment(vAlignment.InvertedX());
        oView.m_Score          .SetAlignment(vAlignment.InvertedY());
        oView.m_Combo          .SetAlignment(vAlignment.InvertedY());
        oView.m_ChainBar       .SetAlignment(vAlignment);
        oView.m_ChainValue     .SetAlignment(vAlignment);

        const coreVector2 vCenter = coreVector2(-0.5f * B,-0.5f) * g_vMenuCenter;
        oView.m_aHealthBar[0]  .SetCenter(vCenter);
        oView.m_aHealthBar[1]  .SetCenter(vCenter);
        oView.m_aHealthValue[0].SetCenter(vCenter);
        oView.m_aHealthValue[1].SetCenter(vCenter);
        oView.m_Score          .SetCenter(vCenter.InvertedY());
        oView.m_Combo          .SetCenter(vCenter.InvertedY());
        oView.m_ChainBar       .SetCenter(vCenter);
        oView.m_ChainValue     .SetCenter(vCenter);
    }
    */
}