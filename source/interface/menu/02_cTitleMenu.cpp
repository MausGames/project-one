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
cTitleMenu::cTitleMenu()noexcept
: coreMenu (1u, SURFACE_TITLE_DEFAULT)
, m_Effect (g_pSpecialEffects->GetParticleDark())
{
    // create menu objects
    m_GameLogo.DefineTexture(0u, "game_logo.png");
    m_GameLogo.DefineProgram("default_2d_program");
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.10f));
    m_GameLogo.SetSize      (coreVector2(1.0f,1.0f) * 0.52f);
    m_GameLogo.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.05f);

    m_StartMessage.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_StartMessage.SetPosition(coreVector2(0.0f,-0.24f));
    m_StartMessage.SetText    ("Press Start");

    m_aVersionInfo[0].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionInfo[0].SetPosition (coreVector2(0.0f, 0.03f));
    m_aVersionInfo[0].SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_aVersionInfo[0].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionInfo[0].SetText     ("(c) 2010-2018 Martin Mauersics (@MausGames)");

    m_aVersionInfo[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionInfo[1].SetPosition (m_aVersionInfo[0].GetPosition() + coreVector2(0.0f,-0.02f));
    m_aVersionInfo[1].SetCenter   (m_aVersionInfo[0].GetCenter());
    m_aVersionInfo[1].SetAlignment(m_aVersionInfo[0].GetAlignment());
    m_aVersionInfo[1].SetText     (PRINT("Project One - v0.1.0a third edition - %s %.5s", __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_GameLogo);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_StartMessage);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_aVersionInfo[0]);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_aVersionInfo[1]);


    //m_Effect.CreateParticle(10u, [](coreParticle* OUTPUT pParticle)
    //{
    //    pParticle->SetScaleStc(3.5f);
    //    pParticle->SetSpeed   (0.0f);
    //});
}


// ****************************************************************
// destructor
cTitleMenu::~cTitleMenu()
{

    m_Effect.Clear(); // TODO: race condition


}


// ****************************************************************
// move the title menu
void cTitleMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_TITLE_DEFAULT:
        {
            // 
            m_StartMessage.SetColor3(coreVector3(1.0f,1.0f,1.0f) * LERP(MENU_LIGHT_IDLE, MENU_LIGHT_ACTIVE, 0.5f + 0.5f * SIN(10.0f * coreFloat(Core::System->GetTotalTime()))));

            //m_GameLogo.SetColor3(coreVector3(1.0f,1.0f,1.0f) * LERP(0.8f, 1.0f, 0.5f + 0.5f * SIN(3.0f * coreFloat(Core::System->GetTotalTime()))));

            if(this->GetAlpha() >= 1.0f)
            {
                if(Core::Input->GetAnyButton(CORE_INPUT_PRESS))
                {
                    // 
                    m_iStatus = 1;
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }


    m_Effect.ForEachParticle([&](coreParticle* OUTPUT pParticle, const coreUintW i)
    {
        const coreFloat fTime = FMOD(coreFloat(Core::System->GetTotalTime()) * 0.1f + 0.1f * I_TO_F(i), 1.0f);

        pParticle->SetPositionStc(coreVector3(coreVector2((0.08f + 0.0f * fTime) * SIN((fTime + (I_TO_F(i) / 10.0f)) * 4.0f*PI), 0.27f - 0.54f * fTime) * (FOREGROUND_AREA * 4.5f), 5.0f));
        pParticle->SetAngleStc   (4.0f*PI * fTime * ((i & 0x01u) ? -1.0f : 1.0f));
        pParticle->SetColor4Stc  (coreVector4(0.0f, 173.0f/255.0f * 0.6f, 223.0f/255.0f * 0.6f, 0.9f * this->GetAlpha()));
        // TODO: as ink shader   
    });
}