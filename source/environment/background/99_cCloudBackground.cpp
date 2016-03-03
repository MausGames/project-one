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
cCloudBackground::cCloudBackground()noexcept
: m_fOffset (0.0f)
{
    coreBatchList* pList1;

    // allocate cloud list
    pList1 = new coreBatchList(CLOUD_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        for(coreUintW i = 0u, ie = CLOUD_CLOUD_NUM; i < ie; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(CLOUD_CLOUD_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // load object resources
            coreObject3D* pObject = new coreObject3D();
            pObject->DefineModel  (Core::Manager::Object->GetLowModel());
            pObject->DefineTexture(0u, "environment_clouds_mid.png");
            pObject->DefineProgram("environment_clouds_program");

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor4   (coreVector4(1.0f * (0.8f + 0.2f * fHeight/60.0f), 1.0f, 1.0f, 0.85f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite   (pList1);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == CLOUD_CLOUD_RESERVE)
    }

    // 
    m_Overlay.DefineTexture(0u, "environment_clouds_blue.png");
    m_Overlay.DefineProgram("menu_grey_program");
    m_Overlay.SetSize      (coreVector2(1.0f,1.0f) * SQRT(2.0f));
    m_Overlay.SetColor3    (COLOR_SHIP_ICE * 0.5f);
    m_Overlay.SetTexSize   (coreVector2(1.0f,1.0f) * SQRT(2.0f) * 1.2f);

    // load wind sound-effect
    m_pWindSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav");
    m_pWindSound.GetHandle()->OnLoadOnce([this, pResource = m_pWindSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cCloudBackground::~cCloudBackground()
{
    // stop wind sound-effect
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->Stop();
}


// ****************************************************************
// render the cloud background
void cCloudBackground::__RenderOwn()
{
    // 
    glDisable(GL_DEPTH_TEST);
    m_Overlay.Render();
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the cloud background
void cCloudBackground::__MoveOwn()
{
    // 
    m_fOffset.Update(-0.08f * g_pEnvironment->GetSpeed());

    // 
    m_Overlay.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Overlay.SetTexOffset(coreVector2(0.0f, FRACT(m_fOffset)));
    m_Overlay.Move();

    // 
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}