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
    m_aHealthBar[0].SetPosition  (coreVector2( 0.01f, 0.01f));
    m_aHealthBar[0].SetSize      (coreVector2( 4.0f,  0.4f) * 0.07f);
    m_aHealthBar[0].SetCenter    (coreVector2(-0.5f, -0.5f) * g_vMenuCenter);
    m_aHealthBar[0].SetAlignment (coreVector2( 1.0f,  1.0f));
    m_aHealthBar[0].SetColor3    (COLOR_GREEN_F * 0.2f);

    m_aHealthBar[1].DefineProgram("default_2d_program");
    m_aHealthBar[1].DefineTexture(0, "menu_detail_01.png");
    m_aHealthBar[1].SetPosition  (m_aHealthBar[0].GetPosition() + coreVector2(0.01f,0.01f) * 0.5f);
    m_aHealthBar[1].SetSize      (m_aHealthBar[0].GetSize()     - coreVector2(0.01f,0.01f));
    m_aHealthBar[1].SetCenter    (m_aHealthBar[0].GetCenter());
    m_aHealthBar[1].SetAlignment (m_aHealthBar[0].GetAlignment());
    m_aHealthBar[1].SetColor3    (COLOR_GREEN_F * 0.9f);

    m_HealthValue.Construct   (MENU_FONT_SMALL, 10);
    m_HealthValue.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.03f,0.02f));
    m_HealthValue.SetCenter   (m_aHealthBar[0].GetCenter());
    m_HealthValue.SetAlignment(m_aHealthBar[0].GetAlignment());

    m_ScoreMission.Construct   (MENU_FONT_MEDIUM, 10);
    m_ScoreMission.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.03f,0.05f));
    m_ScoreMission.SetCenter   (m_aHealthBar[0].GetCenter());
    m_ScoreMission.SetAlignment(m_aHealthBar[0].GetAlignment());

    m_Combo.Construct   (MENU_FONT_SMALL, 10);
    m_Combo.SetPosition (m_aHealthBar[0].GetPosition() + coreVector2(0.22f,0.05f));
    m_Combo.SetCenter   (m_aHealthBar[0].GetCenter());
    m_Combo.SetAlignment(m_aHealthBar[0].GetAlignment());


    m_ScoreBoss;       // 
    m_Chain;           // 
}


// ****************************************************************
// constructor
cInterface::cInterface(const coreByte& iNumSides)noexcept
{

    std::memset(m_apView, 0, sizeof(m_apView));
    for(int i = 0; i < iNumSides; ++i) m_apView[i] = new sPlayerView();

    // create view objects
    m_aBossHealthBar[0].DefineProgram("default_2d_program");
    m_aBossHealthBar[0].DefineTexture(0, "menu_detail_02.png");
    m_aBossHealthBar[0].SetPosition  (coreVector2( 0.0f,-0.01f));
    m_aBossHealthBar[0].SetSize      (coreVector2( 8.0f, 0.4f) * 0.07f);
    m_aBossHealthBar[0].SetCenter    (coreVector2(-0.0f, 0.5f) * g_vMenuCenter);
    m_aBossHealthBar[0].SetAlignment (coreVector2( 0.0f,-1.0f));
    m_aBossHealthBar[0].SetColor3    (COLOR_GREEN_F * 0.2f);

    m_aBossHealthBar[1].DefineProgram("default_2d_program");
    m_aBossHealthBar[1].DefineTexture(0, "menu_detail_02.png");
    m_aBossHealthBar[1].SetPosition  (m_aBossHealthBar[0].GetPosition() + coreVector2(0.00f,-0.01f) * 0.5f);
    m_aBossHealthBar[1].SetSize      (m_aBossHealthBar[0].GetSize()     - coreVector2(0.01f, 0.01f));
    m_aBossHealthBar[1].SetCenter    (m_aBossHealthBar[0].GetCenter());
    m_aBossHealthBar[1].SetAlignment (m_aBossHealthBar[0].GetAlignment());
    m_aBossHealthBar[1].SetColor3    (COLOR_GREEN_F * 0.9f);

    m_BossHealthValue.Construct   (MENU_FONT_MEDIUM, 20);
    m_BossHealthValue.SetPosition (m_aBossHealthBar[0].GetPosition() + coreVector2(0.0f,-0.01f));
    m_BossHealthValue.SetCenter   (m_aBossHealthBar[0].GetCenter());
    m_BossHealthValue.SetAlignment(m_aBossHealthBar[0].GetAlignment());

    m_BossName.Construct   (MENU_FONT_MEDIUM, 0);
    m_BossName.SetPosition (coreVector2( 0.01f,-0.01f));
    m_BossName.SetCenter   (coreVector2(-0.5f,  0.5f) * g_vMenuCenter);
    m_BossName.SetAlignment(coreVector2( 1.0f, -1.0f));

    m_aTimeMission[0].Construct   (MENU_FONT_MEDIUM, 5);
    m_aTimeMission[0].SetPosition (coreVector2(-0.033f,-0.01f));
    m_aTimeMission[0].SetCenter   (coreVector2( 0.5f,   0.5f) * g_vMenuCenter);
    m_aTimeMission[0].SetAlignment(coreVector2(-1.0f,  -1.0f));

    m_aTimeMission[1].Construct   (MENU_FONT_MEDIUM, 2);
    m_aTimeMission[1].SetPosition (coreVector2(-0.01f,-0.01f));
    m_aTimeMission[1].SetCenter   (m_aTimeMission[0].GetCenter());
    m_aTimeMission[1].SetAlignment(m_aTimeMission[0].GetAlignment());

    m_aTimeBoss[0].Construct   (MENU_FONT_SMALL, 5);
    m_aTimeBoss[0].SetPosition (coreVector2(-0.026f,-0.04f));
    m_aTimeBoss[0].SetCenter   (m_aTimeMission[0].GetCenter());
    m_aTimeBoss[0].SetAlignment(m_aTimeMission[0].GetAlignment());

    m_aTimeBoss[1].Construct   (MENU_FONT_SMALL, 2);
    m_aTimeBoss[1].SetPosition (coreVector2(-0.01f,-0.04f));
    m_aTimeBoss[1].SetCenter   (m_aTimeMission[0].GetCenter());
    m_aTimeBoss[1].SetAlignment(m_aTimeMission[0].GetAlignment());



    m_BossHealthValue.SetText("10000");
    m_BossName.SetText("Crossfield");


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
    for(int i = 0; i < INTERFACE_VIEWS; ++i)
    {
        sPlayerView* pView = m_apView[i];
        if(!pView) continue;

        pView->m_aHealthBar[0].Render();
        pView->m_aHealthBar[1].Render();
        pView->m_HealthValue  .Render();
        pView->m_ScoreMission .Render();
        pView->m_Combo        .Render();
    }

    m_aBossHealthBar[0].Render();
    m_aBossHealthBar[1].Render();
    m_BossHealthValue  .Render();
    m_BossName         .Render();
    m_aTimeMission[0]  .Render();
    m_aTimeMission[1]  .Render();
    m_aTimeBoss[0]     .Render();
    m_aTimeBoss[1]     .Render();
}


// ****************************************************************
// move the interface
void cInterface::Move()
{
    for(int i = 0; i < INTERFACE_VIEWS; ++i)
    {
        sPlayerView* pView = m_apView[i];
        if(!pView) continue;

        cPlayer* pPlayer = g_pGame->GetPlayer(i);

        pView->m_HealthValue .SetText(PRINT("%d",    pPlayer->GetCurHealth()));
        pView->m_ScoreMission.SetText(PRINT("%06d",  pPlayer->GetScoreMission()));
        pView->m_Combo       .SetText(PRINT("x%.1f", pPlayer->GetCurCombo()));

        pView->m_aHealthBar[0].Move();
        pView->m_aHealthBar[1].Move();
        pView->m_HealthValue  .Move();
        pView->m_ScoreMission .Move();
        pView->m_Combo        .Move();
    }

    const float fTimeMission = MAX(g_pGame->GetTimeMission(), 0.0f);
    m_aTimeMission[0].SetText(PRINT("%.0f.", FLOOR(      fTimeMission)));
    m_aTimeMission[1].SetText(PRINT("%.0f",  FLOOR(FRACT(fTimeMission)*10.0f)));

    if(g_pGame->GetMission()->GetCurBoss())
    {
        const float fTimeBoss = MAX(g_pGame->GetTimeBoss(g_pGame->GetMission()->GetCurBossNum()), 0.0f);
        m_aTimeBoss[0].SetText(PRINT("%.0f.", FLOOR(      fTimeBoss)));
        m_aTimeBoss[1].SetText(PRINT("%.0f",  FLOOR(FRACT(fTimeBoss)*10.0f)));
    }
    else
    {
        m_aTimeBoss[0].SetText("");
        m_aTimeBoss[1].SetText("");
    }

    m_aBossHealthBar[0].Move();
    m_aBossHealthBar[1].Move();
    m_BossHealthValue  .Move();
    m_BossName         .Move();
    m_aTimeMission[0]  .Move();
    m_aTimeMission[1]  .Move();
    m_aTimeBoss[0]     .Move();
    m_aTimeBoss[1]     .Move();

}