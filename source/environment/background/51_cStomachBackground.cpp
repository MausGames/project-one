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
cStomachBackground::cStomachBackground()noexcept
: cBackground (false)
{
    coreBatchList* pList1;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("rock", "rock", OUTDOOR_ALGORITHM_STOMACH, 4.0f, false);

    // allocate cloud list
    pList1 = new coreBatchList(STOMACH_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_high.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < STOMACH_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.05f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, STOMACH_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f));
            pObject->SetAlpha    (0.65f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,4.0f, 0.0f,4.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, STOMACH_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCapacity() == STOMACH_CLOUD_RESERVE)
    }
}


// ****************************************************************
// destructor
cStomachBackground::~cStomachBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cStomachBackground::__InitOwn()
{
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_stomach.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cStomachBackground::__ExitOwn()
{
    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cStomachBackground::__RenderOwnAfter()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Headlight.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cStomachBackground::__MoveOwn()
{
    // adjust volume of the base sound-effect
    if(m_pBaseSound.IsUsable() && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
    
    
    
    const coreFloat fCloudMove = 0.0018f * (1.0f + ABS(g_pEnvironment->GetSpeed())) * TIME;

    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = LOOP_NONZERO(pList->GetSize()); i < ie; ++i)
    {
        coreObject3D* pCloud = (*pList->List())[i];
        pCloud->SetTexOffset((pCloud->GetTexOffset() + MapToAxis(coreVector2(fCloudMove * ((pCloud->GetDirection().x < 0.0f) ? -1.0f : 1.0f), 0.0f), pCloud->GetDirection().xy())).Processed(FRACT));
    }
    pList->MoveNormal();

    // 
    m_Headlight.Move();
}


// ****************************************************************
// 
void cStomachBackground::__UpdateOwn()
{
    // 
    m_Headlight.UpdateDefault(0u);
}