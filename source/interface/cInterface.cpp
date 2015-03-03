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
// constructor
cInterface::sPlayerView::sPlayerView()noexcept
{
    // create view objects
    m_aHealthBar[0].DefineProgram("default_2d_program");
    m_aHealthBar[0].DefineTexture(0, "menu_detail_01.png");
    m_aHealthBar[0].SetPosition  (coreVector2( 0.0f, 0.005f));
    m_aHealthBar[0].SetSize      (coreVector2( 4.0f, 0.4f) * 0.07f);
    m_aHealthBar[0].SetCenter    (coreVector2(-0.5f,-0.5f) * g_vMenuCenter);
    m_aHealthBar[0].SetAlignment (coreVector2( 1.0f, 1.0f));

    m_aHealthBar[1].DefineProgram("default_2d_program");
    m_aHealthBar[1].DefineTexture(0, "menu_detail_01.png");
    m_aHealthBar[1].SetPosition  (m_aHealthBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f);
    m_aHealthBar[1].SetSize      (m_aHealthBar[0].GetSize()     - coreVector2(0.01f,0.01f));
    m_aHealthBar[1].SetCenter    (m_aHealthBar[0].GetCenter());
    m_aHealthBar[1].SetAlignment (m_aHealthBar[0].GetAlignment());

    m_aHealthValue[0].Construct   (MENU_FONT_SMALL, 0);
    m_aHealthValue[0].SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.05f,0.01f));
    m_aHealthValue[0].SetCenter   (m_aHealthBar[0].GetCenter());
    m_aHealthValue[0].SetAlignment(m_aHealthBar[0].GetAlignment());

    m_aHealthValue[1].Construct   (MENU_FONT_SMALL, 10);
    m_aHealthValue[1].SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(m_aHealthBar[0].GetSize().x-0.05f,0.01f));
    m_aHealthValue[1].SetCenter   (m_aHealthBar[0].GetCenter());
    m_aHealthValue[1].SetAlignment(coreVector2(-1.0f,1.0f));

    m_ScoreMission.Construct   (MENU_FONT_MEDIUM, 7);
    m_ScoreMission.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.017f,0.035f));
    m_ScoreMission.SetCenter   (m_aHealthBar[0].GetCenter());
    m_ScoreMission.SetAlignment(m_aHealthBar[0].GetAlignment());

    m_Combo.Construct   (MENU_FONT_SMALL, 6);
    m_Combo.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(m_aHealthBar[0].GetSize().x-0.017f,0.038f));
    m_Combo.SetCenter   (m_aHealthBar[0].GetCenter());
    m_Combo.SetAlignment(coreVector2(-1.0f,1.0f));

    m_Chain.Construct   (MENU_FONT_SMALL, 8);
    m_Chain.SetPosition (m_ScoreMission.GetPosition() + coreVector2(0.001f,0.03f));
    m_Chain.SetCenter   (m_ScoreMission.GetCenter());
    m_Chain.SetAlignment(m_ScoreMission.GetAlignment());
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreByte& iNumSides)noexcept
: m_fAlphaAll  (0.0f)
, m_fAlphaBoss (0.0f)
, m_bVisible   (true)
{
    // create view objects
    std::memset(m_apView, 0, sizeof(m_apView));
    for(int i = 0; i < iNumSides; ++i) m_apView[i] = new sPlayerView();

    // create interface objects
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].DefineTexture(0, "menu_detail_02.png");
    m_aBossHealthBar[0].SetPosition  (coreVector2( 0.0f,-0.005f));
    m_aBossHealthBar[0].SetSize      (coreVector2( 8.0f, 0.4f) * 0.07f);
    m_aBossHealthBar[0].SetCenter    (coreVector2(-0.5f, 0.5f) * g_vMenuCenter);
    m_aBossHealthBar[0].SetAlignment (coreVector2( 1.0f,-1.0f));

    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].DefineTexture(0, "menu_detail_02.png");
    m_aBossHealthBar[1].SetPosition  (m_aBossHealthBar[0].GetPosition() + coreVector2(0.01f,-0.01f) * 0.5f);
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize()     - coreVector2(0.01f, 0.01f));
    m_aBossHealthBar[1].SetCenter    (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthBar[1].SetAlignment (m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthValue[0].Construct   (MENU_FONT_SMALL, 7);
    m_aBossHealthValue[0].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(0.05f,-0.01f));
    m_aBossHealthValue[0].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[0].SetAlignment(m_aBossHealthBar[0].GetAlignment());

    m_aBossHealthValue[1].Construct   (MENU_FONT_SMALL, 14);
    m_aBossHealthValue[1].SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(m_aBossHealthBar[0].GetSize().x-0.05f,-0.01f));
    m_aBossHealthValue[1].SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthValue[1].SetAlignment(coreVector2(-1.0f,-1.0f));

    m_BossName.Construct   (MENU_FONT_MEDIUM, 0);
    m_BossName.SetPosition (coreVector2(-0.009f,-0.004f));
    m_BossName.SetCenter   (coreVector2( 0.5f,   0.5f) * g_vMenuCenter);
    m_BossName.SetAlignment(coreVector2(-1.0f,  -1.0f));

    m_BannerBar.DefineProgram("menu_animate_program");
    m_BannerBar.DefineTexture(0, "menu_detail_03.png");
    m_BannerBar.DefineTexture(1, "menu_background_black.png");
    m_BannerBar.SetCenter    (coreVector2(0.0f,0.1f) * g_vMenuCenter);

    m_BannerMain.Construct  (MENU_FONT_BIG, 0);
    m_BannerMain.SetPosition(coreVector2(0.0f,-0.012f));
    m_BannerMain.SetCenter  (m_BannerBar.GetCenter());
    m_BannerMain.SetColor3  (coreVector3(1.0f,1.0f,1.0f) * MENU_CONTRAST_WHITE);

    m_aBannerSub[0].Construct  (MENU_FONT_MEDIUM, 0);
    m_aBannerSub[0].SetPosition(coreVector2(0.0f,0.027f));
    m_aBannerSub[0].SetCenter  (m_BannerBar.GetCenter());
    m_aBannerSub[0].SetColor3  (coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);

    m_aBannerSub[1].Construct(MENU_FONT_HUGE, 0);
    m_aBannerSub[1].SetCenter(m_BannerBar.GetCenter());
    m_aBannerSub[1].SetColor3(coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);

    m_aBannerSub[2].Construct(MENU_FONT_HUGE, 0);
    m_aBannerSub[2].SetCenter(m_BannerBar.GetCenter());
    m_aBannerSub[2].SetColor3(coreVector3(0.75f,0.75f,0.75f) * MENU_CONTRAST_WHITE);

    m_aTimeBoss[0].Construct   (MENU_FONT_MEDIUM, 5);
    m_aTimeBoss[0].SetPosition (m_BossName.GetPosition() + coreVector2(-0.023f,-0.025f));
    m_aTimeBoss[0].SetCenter   (m_BossName.GetCenter());
    m_aTimeBoss[0].SetAlignment(m_BossName.GetAlignment());

    m_aTimeBoss[1].Construct   (MENU_FONT_MEDIUM, 2);
    m_aTimeBoss[1].SetPosition (m_BossName.GetPosition() + coreVector2(0.0f,-0.025f));
    m_aTimeBoss[1].SetCenter   (m_BossName.GetCenter());
    m_aTimeBoss[1].SetAlignment(m_BossName.GetAlignment());
}


// ****************************************************************
// destructor
cInterface::~cInterface()
{
    // 
    for(int i = 0; i < INTERFACE_VIEWS; ++i)
        SAFE_DELETE(m_apView[i])
}



// ****************************************************************
// render the interface
void cInterface::Render()
{
    if(m_fAlphaAll)
    {
        // 
        for(int i = 0; i < INTERFACE_VIEWS; ++i)
        {
            sPlayerView* pView = m_apView[i];
            if(!pView) continue;

            // 
            pView->m_aHealthBar[0]  .Render();
            pView->m_aHealthBar[1]  .Render();
            pView->m_aHealthValue[0].Render();
            pView->m_aHealthValue[1].Render();
            pView->m_ScoreMission   .Render();
            pView->m_Combo          .Render();
            pView->m_Chain          .Render();
        }

        if(m_fAlphaBoss)
        {
            // 
            m_aBossHealthBar[0]  .Render();
            m_aBossHealthBar[1]  .Render();
            m_aBossHealthValue[0].Render();
            m_aBossHealthValue[1].Render();
            m_BossName           .Render();
            m_aTimeBoss[0]       .Render();
            m_aTimeBoss[1]       .Render();
        }

        if(g_pGame->GetTimeMission() <= INTERFACE_MISSION_DURATION + INTERFACE_MISSION_START)
        {
            // 
            m_BannerBar    .Render();
            m_aBannerSub[1].Render();
            m_aBannerSub[2].Render();
            m_aBannerSub[0].Render();
            m_BannerMain   .Render();
        }
    }
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    // 
    if(m_bVisible) {if(m_fAlphaAll < 1.0f) m_fAlphaAll = MIN(m_fAlphaAll + Core::System->GetTime(), 1.0f);}
              else {if(m_fAlphaAll > 0.0f) m_fAlphaAll = MAX(m_fAlphaAll - Core::System->GetTime(), 0.0f);}

    // 
    for(int i = 0; i < INTERFACE_VIEWS; ++i)
    {
        sPlayerView* pView = m_apView[i];
        if(!pView) continue;

        cPlayer* pPlayer = g_pGame->GetPlayer(i);

        // 
        const float fPercent = I_TO_F(pPlayer->GetCurHealth()) * RCP(I_TO_F(pPlayer->GetMaxHealth()));
        pView->m_aHealthBar[1].SetSize   ((pView->m_aHealthBar[0].GetSize() - coreVector2(0.01f,0.01f)) * coreVector2(fPercent, 1.0f));
        pView->m_aHealthBar[1].SetTexSize(coreVector2(fPercent, 1.0f));

        // 
        const coreVector3 vColor = cMenu::HealthColor(fPercent);
        pView->m_aHealthBar[0].SetColor3(vColor * 0.2f);
        pView->m_aHealthBar[1].SetColor3(vColor * 0.9f);

        // 
        pView->m_aHealthValue[1].SetText(PRINT("%d / %d", pPlayer->GetCurHealth(), pPlayer->GetMaxHealth()));
        pView->m_ScoreMission   .SetText(PRINT("%06d",    pPlayer->GetScoreMission()));
        pView->m_Combo          .SetText(PRINT("x%.1f",   pPlayer->GetCurCombo()));
        pView->m_Chain          .SetText(pPlayer->GetCurChain() ? PRINT("+%d", pPlayer->GetCurChain()) : "");

        // 
        pView->m_aHealthBar[0]  .SetAlpha(m_fAlphaAll);
        pView->m_aHealthBar[1]  .SetAlpha(m_fAlphaAll);
        pView->m_aHealthValue[0].SetAlpha(m_fAlphaAll);
        pView->m_aHealthValue[1].SetAlpha(m_fAlphaAll);
        pView->m_ScoreMission   .SetAlpha(m_fAlphaAll);
        pView->m_Combo          .SetAlpha(m_fAlphaAll);
        pView->m_Chain          .SetAlpha(m_fAlphaAll);

        // 
        pView->m_aHealthBar[0]  .Move();
        pView->m_aHealthBar[1]  .Move();
        pView->m_aHealthValue[0].Move();
        pView->m_aHealthValue[1].Move();
        pView->m_ScoreMission   .Move();
        pView->m_Combo          .Move();
        pView->m_Chain          .Move();
    }

    // 
    cBoss* pBoss = g_pGame->GetMission()->GetCurBoss();
    if(pBoss)
    {
        // 
        const float fPercent = I_TO_F(pBoss->GetCurHealth()) * RCP(I_TO_F(pBoss->GetMaxHealth()));
        m_aBossHealthBar[1].SetSize   ((m_aBossHealthBar[0].GetSize() - coreVector2(0.01f,0.01f)) * coreVector2(fPercent, 1.0f));
        m_aBossHealthBar[1].SetTexSize(coreVector2(fPercent, 1.0f));

        // 
        const coreVector3 vColor = cMenu::HealthColor(fPercent * 0.5f);
        m_aBossHealthBar[0].SetColor3(vColor * 0.2f);
        m_aBossHealthBar[1].SetColor3(vColor * 0.9f);

        // 
        m_aBossHealthValue[0].SetText(PRINT("[%.0f%%]", fPercent * 100.0f));
        m_aBossHealthValue[1].SetText(PRINT("%d / %d",  pBoss->GetCurHealth(), pBoss->GetMaxHealth()));

        // 
        const float fTimeBoss = MAX(g_pGame->GetTimeBoss(g_pGame->GetMission()->GetCurBossIndex()), 0.0f);
        m_aTimeBoss[0].SetText(PRINT("%.0f.", FLOOR(      fTimeBoss)));
        m_aTimeBoss[1].SetText(PRINT("%.0f",  FLOOR(FRACT(fTimeBoss)*10.0f)));
    }

    // TODO ### 
    // if(pBoss) {if(m_fAlphaBoss < 1.0f) m_fAlphaBoss = MIN(m_fAlphaBoss + Core::System->GetTime(), 1.0f);}
    //      else {if(m_fAlphaBoss > 0.0f) m_fAlphaBoss = MAX(m_fAlphaBoss - Core::System->GetTime(), 0.0f);}

    // 
    const float fAlphaBossFull = m_fAlphaAll * m_fAlphaBoss;
    m_aBossHealthBar[0]  .SetAlpha(fAlphaBossFull);
    m_aBossHealthBar[1]  .SetAlpha(fAlphaBossFull);
    m_aBossHealthValue[0].SetAlpha(fAlphaBossFull);
    m_aBossHealthValue[1].SetAlpha(fAlphaBossFull);
    m_BossName           .SetAlpha(fAlphaBossFull);
    m_aTimeBoss[0]       .SetAlpha(fAlphaBossFull);
    m_aTimeBoss[1]       .SetAlpha(fAlphaBossFull);

    // 
    m_aBossHealthBar[0]  .Move();
    m_aBossHealthBar[1]  .Move();
    m_aBossHealthValue[0].Move();
    m_aBossHealthValue[1].Move();
    m_BossName           .Move();
    m_aTimeBoss[0]       .Move();
    m_aTimeBoss[1]       .Move();

    // 
    const float fTimeMission = g_pGame->GetTimeMission() - INTERFACE_MISSION_START;
    if((fTimeMission <= INTERFACE_MISSION_DURATION) && (fTimeMission >= 0.0f))
    {
        // 
        const float fPercent   = MIN(fTimeMission, INTERFACE_MISSION_DURATION - fTimeMission, 1.0f / INTERFACE_MISSION_SPEED) * INTERFACE_MISSION_SPEED;
        const float fAnimation = LERPB(0.0f, INTERFACE_MISSION_ANIMATION, fTimeMission / INTERFACE_MISSION_ANIMATION);

        // slash banner from left to right (# direction can be swapped, also alpha value is used as texture coordinate correction)
        const bool bLeftRight = (fTimeMission < INTERFACE_MISSION_SWAP) ? false : true;
        m_BannerBar.SetPosition (coreVector2((bLeftRight ? 0.5f : -0.5f) * (1.0f-fPercent), 0.0f));
        m_BannerBar.SetAlpha    (bLeftRight ? fPercent : 1.0f);

        // stretch and animate banner
        m_BannerBar.SetSize     (coreVector2(fPercent, 1.0f) * (coreVector2(4.0f,0.8f) * 0.25f));
        m_BannerBar.SetTexSize  (coreVector2(fPercent, 1.0f));
        m_BannerBar.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

        // 
        const float fSubOffset = (fAnimation + 3.0f) * 0.01f;
        m_aBannerSub[1].SetPosition(coreVector2( fSubOffset,  0.019f));
        m_aBannerSub[2].SetPosition(coreVector2(-fSubOffset, -0.01f));

        // 
        m_BannerMain   .SetAlpha(fPercent);
        m_aBannerSub[0].SetAlpha(fPercent);
        m_aBannerSub[1].SetAlpha(fPercent * 0.2f);
        m_aBannerSub[2].SetAlpha(fPercent * 0.2f);

        // 
        m_BannerBar    .Move();
        m_BannerMain   .Move();
        m_aBannerSub[0].Move();
        m_aBannerSub[1].Move();
        m_aBannerSub[2].Move();
    }
}


// ****************************************************************
// 
void cInterface::FillMission(const char* pcMain, const char* pcSub)
{
    // 
    m_BannerMain   .SetText(pcMain);
    m_aBannerSub[0].SetText(pcSub);
    m_aBannerSub[1].SetText(pcMain);
    m_aBannerSub[2].SetText(pcMain);

    // 
    m_BannerMain   .SetAlpha(0.0f);
    m_aBannerSub[0].SetAlpha(0.0f);
    m_aBannerSub[1].SetAlpha(0.0f);
    m_aBannerSub[2].SetAlpha(0.0f);
}


// ****************************************************************
// 
void cInterface::FillBoss(const char* pcMain, const char* pcSub)
{
    // 
    m_BossName.SetText(pcMain);
}