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
cCloudBackground::cCloudBackground()noexcept
: m_vRainMove (coreVector2(-0.5f,-1.2f))
, m_fOffset   (0.0f)
, m_Loaded    ()
{
    coreBatchList* pList1;

    // 
    this->__InitOwn();

    // allocate cloud list
    pList1 = new coreBatchList(CLOUD_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < CLOUD_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, CLOUD_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * MIN1(0.0f + 1.0f * fHeight/50.0f));
            pObject->SetAlpha    (0.85f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,4.0f, 0.0f,4.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, CLOUD_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == CLOUD_CLOUD_RESERVE)
    }

    // 
    m_Rain.DefineTexture(0u, "effect_rain.png");
    m_Rain.DefineProgram("effect_weather_rain_cloud_program");
    m_Rain.SetPosition  (coreVector2(0.0f,0.0f));
    m_Rain.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Rain.SetAlpha     (0.55f);

    // 
    m_Cover.DefineTexture(0u, "environment_clouds_grey.png");
    m_Cover.DefineProgram("menu_grey_program");
    m_Cover.SetPosition  (coreVector2(0.0f,0.0f));
    m_Cover.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Cover.SetColor3    (LERP(COLOR_MENU_PURPLE, coreVector3(1.0f,1.0f,1.0f), 0.45f) * 0.5f);
    m_Cover.SetTexSize   (coreVector2(1.0f,1.0f) * SQRT2 * 1.2f);
}


// ****************************************************************
// destructor
cCloudBackground::~cCloudBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cCloudBackground::__InitOwn()
{
    m_Loaded.Release();
    
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_cloud.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
        m_Loaded.Acquire();
    });
}


// ****************************************************************
// 
void cCloudBackground::__ExitOwn()
{
    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(m_Loaded && pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cCloudBackground::__RenderOwnBefore()
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
// 
void cCloudBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Rain.GetProgram().IsUsable()) return;
    if(!m_Rain.GetProgram()->Enable())  return;

    // 
    coreProgram* pLocal = m_Rain.GetProgram().GetResource();
    for(coreUintW i = 0u; i < CLOUD_RAIN_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Rain.GetTexOffset() + coreVector2(0.56f,0.36f) * I_TO_F(POW2(i));
        const coreFloat   fNewScale     = 1.0f - 0.15f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Rain.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the cloud background
void cCloudBackground::__MoveOwn()
{
    // 
    const coreVector2 vEnvMove   = coreVector2(0.3f,1.0f) * (-0.35f * MAX0(g_pEnvironment->GetSpeed()));
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 5.5f;
    const coreVector2 vTexOffset = m_Rain.GetTexOffset() + (coreVector2(0.0f, -m_vRainMove.Length()) + vEnvMove) * (1.0f * TIME);

    // 
    m_Rain.SetDirection(MapToAxisInv(-m_vRainMove.InvertedX().Normalized(), g_pEnvironment->GetDirection()));
    m_Rain.SetTexSize  (vTexSize);
    m_Rain.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Rain.Move();

    // 
    m_fOffset.Update(-0.08f * g_pEnvironment->GetSpeed());

    // 
    m_Cover.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Cover.SetTexOffset(coreVector2(0.005f * g_pEnvironment->GetSideOffset(), FRACT(m_fOffset)));
    m_Cover.Move();

    // adjust volume of the base sound-effect
    if(m_Loaded && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
    
    
    
    const coreFloat fCloudMove = 0.0018f * (1.0f + ABS(g_pEnvironment->GetSpeed())) * TIME;


    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pCloud = (*pList->List())[i];
        pCloud->SetTexOffset((pCloud->GetTexOffset() + MapToAxis(coreVector2(fCloudMove * ((pCloud->GetDirection().x < 0.0f) ? -1.0f : 1.0f), 0.0f), pCloud->GetDirection().xy())).Processed(FRACT));
    }
    pList->MoveNormal();
}