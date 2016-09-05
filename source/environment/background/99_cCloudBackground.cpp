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
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowModel());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < CLOUD_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, CLOUD_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = Core::Manager::Memory->New<coreObject3D>(oBase);

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
    m_Cover.DefineTexture(0u, "environment_clouds_blue.png");
    m_Cover.DefineProgram("menu_grey_program");
    m_Cover.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Cover.SetColor3    (COLOR_SHIP_ICE * 0.5f);
    m_Cover.SetTexSize   (coreVector2(1.0f,1.0f) * SQRT2 * 1.2f);

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
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // 
        m_Cover.Render();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}


// ****************************************************************
// move the cloud background
void cCloudBackground::__MoveOwn()
{
    // 
    m_fOffset.Update(-0.08f * g_pEnvironment->GetSpeed());

    // 
    m_Cover.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Cover.SetTexOffset(coreVector2(0.0f, FRACT(m_fOffset)));
    m_Cover.Move();

    // 
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}