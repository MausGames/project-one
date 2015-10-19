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
cGrassBackground::cGrassBackground(const coreUint8& iCloudDensity)noexcept
: m_fLeafTime (Core::Rand->Float(10.0f))
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("grass", "dust", 2u, 4.0f);

    // create water-surface object
    m_pWater = new cWater();

    // allocate stone list
    pList1 = new coreBatchList(GRASS_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW i = 0u; i < GRASS_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_STONE_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

            // test for valid values
            if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 25.0f))
                {
                    // load object resources
                    coreObject3D* pObject = new coreObject3D();
                    pObject->DefineModel  ("environment_stone_01.md3");
                    pObject->DefineTexture(0u, "environment_stone_diff.png");
                    pObject->DefineTexture(1u, "environment_stone_norm.png");
                    pObject->DefineProgram("object_ground_program");

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, fHeight+0.2f));
                    pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f));
                    pObject->SetDirection  (coreVector3::Rand());
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate reed lists
    pList1 = new coreBatchList(GRASS_REED_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(GRASS_REED_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW i = 0u; i < GRASS_REED_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 4u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_REED_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+176.0f) % 80 < 60))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f))
                    {
                        // determine object type
                        const coreBool bType = (Core::Rand->Int(3) || fHeight >= -20.0f) ? true : false;

                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (bType ? "environment_reed_01.md3" : "environment_reed_02.md3");
                        pObject->DefineTexture(0u, "environment_reed.png");
                        pObject->DefineTexture(1u, "environment_grass_norm.png");
                        pObject->DefineProgram("object_ground_program");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize       (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 2.1f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
                        pObject->SetColor3     (coreVector3(1.0f, 1.0f * Core::Rand->Float(0.57f, 0.63f), 0.5f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);

        cBackground::_FillInfinite(pList2);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate flower list
    pList1 = new coreBatchList(GRASS_FLOWER_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        for(coreUintW i = GRASS_FLOWER_NUM; i--; )
        {
            for(coreUintW j = 0u; j < 20u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_FLOWER_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -15.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 40.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                        pObject->DefineTexture(0u, "environment_flowers.png");
                        pObject->DefineProgram("effect_decal_program");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize     (coreVector3(coreVector2(1.0f,1.0f) * Core::Rand->Float(9.0f, 10.0f), 1.0f));
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetAlpha    (0.98f);
                        pObject->SetTexSize  (coreVector2(0.5f,0.5f));
                        pObject->SetTexOffset(coreVector2(Core::Rand->Bool() ? 0.5f : 0.0f, Core::Rand->Bool() ? 0.5f : 0.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        m_apDecalObjectList.push_back(pList1);
    }

    // allocate leaf list
    pList1 = new coreBatchList(GRASS_LEAF_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        for(coreUintW i = 0u; i < GRASS_LEAF_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 20u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_LEAF_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 30.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 700.0f))
                {
                    // load object resources
                    coreObject3D* pObject = new coreObject3D();
                    pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                    pObject->DefineTexture(0u, "environment_leaf.png");
                    pObject->DefineProgram("effect_decal_program");

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(2.0f,0.0f,1.0f));
                    pObject->SetColor3  (coreVector3(1.0f * (0.8f + 0.2f * fHeight/30.0f), 1.0f, 1.0f));
                    pObject->SetTexSize (coreVector2(0.5f,0.85f));

                    // add object to the list
                    pList1->BindObject(pObject);
                    break;
                }
            }
        }

        // 
        coreSet<coreObject3D*>* papContent = pList1->List();
        if(papContent->size() % 2u)
        {
            SAFE_DELETE(papContent->back())
            papContent->pop_back();
        }

        // 
        m_iLeafNum = papContent->size();
        ASSERT(!(m_iLeafNum % 2u))

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1);
        m_apAirObjectList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(GRASS_CLOUD_RESERVE * iCloudDensity);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        for(coreUintW i = 0u, ie = GRASS_CLOUD_NUM * iCloudDensity; i < ie; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(0.1f, 0.3f) * (Core::Rand->Bool() ? 1.0f : -1.0f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_CLOUD_NUM * iCloudDensity)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // load object resources
            coreObject3D* pObject = new coreObject3D();
            pObject->DefineModel  (Core::Manager::Object->GetLowModel());
            pObject->DefineTexture(0u, "environment_clouds_mid.png");
            pObject->DefineProgram("environment_clouds_program");

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.1f,2.1f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f * (0.8f + 0.2f * fHeight/60.0f), 1.0f, 1.0f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1);
        ASSERT(pList1->GetCurCapacity() == GRASS_CLOUD_RESERVE * iCloudDensity)
        m_apAirObjectList.push_back(pList1);
    }

    // load nature sound-effect
    m_pNatureSound = Core::Manager::Resource->Get<coreSound>("environment_nature.wav");
    m_pNatureSound.GetHandle()->OnLoadOnce([=]()
    {
        m_pNatureSound->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cGrassBackground::~cGrassBackground()
{
    // stop nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->Stop();
}


// ****************************************************************
// move the grass background
void cGrassBackground::__MoveOwn()
{
    // 
    m_fLeafTime.Update(1.0f);

    // 
    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pLeaf = (*pList->List())[i];
        if(!pLeaf->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i % m_iLeafNum);
        const coreFloat   fTime   = m_fLeafTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat   fPos    = SIN(fTime * 0.1f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);
        const coreVector2 vDir    = coreVector2::Direction(fTime);

        // 
        pLeaf->SetPosition   (coreVector3(fPos, pLeaf->GetPosition().yz()));
        pLeaf->SetDirection  (coreVector3(vDir, 0.0f));
        pLeaf->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));

        // get currently visible polygon side
        const coreBool bSide = (coreVector3::Dot(g_pEnvironment->GetCameraPos() - pLeaf->GetPosition(), pLeaf->GetOrientation()) >= 0.0f) ? true : false;

        // simulate two-sided polygon (flip vertex-order and change texture)
        pLeaf->SetSize     (coreVector3(pLeaf->GetSize().x, bSide ? 1.4f : -1.4f, pLeaf->GetSize().z));
        pLeaf->SetTexOffset(coreVector2(bSide ? 0.5f : 0.0f, 0.15f));
    }
    pList->MoveNormal();

    // adjust volume of the nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->SetVolume(MAX(g_pEnvironment->GetTransition().GetValue((g_pEnvironment->GetBackground() == this) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED), 0.0f));

    // TODO: sound-volume per config value 
}


// ****************************************************************
// constructor
cCloudBackground::cCloudBackground()noexcept
: cGrassBackground (GRASS_CLOUD_DENSITY)
, m_fWindVolume    (1.0f)
{
    // 
    m_Overlay.DefineTexture(0u, "environment_clouds_blue.png");
    m_Overlay.DefineProgram("menu_grey_program");
    m_Overlay.SetSize      (coreVector2(1.0f,1.0f));
    m_Overlay.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);
    m_Overlay.SetTexSize   (coreVector2(1.0f,1.0f) * 1.2f);
    m_Overlay.Move();

    // load wind sound-effect
    m_pWindSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav");
    m_pWindSound.GetHandle()->OnLoadOnce([=]()
    {
        m_pWindSound->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });


    m_pOutdoor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);  
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
// 
void cCloudBackground::SetCloudAlpha(const coreFloat& fAlpha)
{
    // 
    auto apCloud = this->GetAirObjectList()->front()->List();

    // 
    for(coreUintW i = 0u, ie = apCloud->size(); i < ie; ++i)
    {
        if(i % GRASS_CLOUD_DENSITY)
            (*apCloud)[i]->SetAlpha((i % 2u) ? MAX(fAlpha*2.0f - 1.0f, 0.0f) : MIN(fAlpha*2.0f, 1.0f));
    }

    // 
    m_fWindVolume = fAlpha;
}


// ****************************************************************
// 
void cCloudBackground::SetOverlayAlpha(const coreFloat& fAlpha)
{
    // 
    m_Overlay.SetAlpha  (fAlpha);
    m_Overlay.SetEnabled(fAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cCloudBackground::ReduceClouds()
{
    // 
    auto pAir    = this->GetAirObjectList()->front();
    auto apCloud = pAir->List();

    ASSERT(GRASS_CLOUD_NUM * GRASS_CLOUD_DENSITY <= apCloud->size())

    // 
    for(coreUintW i = 0u, ie = apCloud->size(); i < ie; ++i)
    {
        if(i % GRASS_CLOUD_DENSITY)
            SAFE_DELETE((*apCloud)[i]);
    }

    // 
    FOR_EACH_DYN(it, *apCloud)
    {
        if(*it) DYN_KEEP  (it)
           else DYN_REMOVE(it, *apCloud)
    }

    // 
    pAir->MoveNormal();
}


// ****************************************************************
// render the cloudy grass background
void cCloudBackground::__RenderOwn()
{
    if(m_Overlay.IsEnabled(CORE_OBJECT_ENABLE_ALL))
    {
        // 
        glDisable(GL_DEPTH_TEST);
        m_Overlay.Render();
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// move the cloudy grass background
void cCloudBackground::__MoveOwn()
{
    // 
    m_Overlay.SetTexOffset(coreVector2(0.0f, m_Overlay.GetTexOffset().y - 0.65f/8.0f * Core::System->GetTime() * g_pEnvironment->GetSpeed()));

    // 
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->SetVolume(m_fWindVolume * MAX(g_pEnvironment->GetTransition().GetValue((g_pEnvironment->GetBackground() == this) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED), 0.0f));

    // 
    cGrassBackground::__MoveOwn();

    //if(g_pGame)
    //g_pGame->ForEachPlayer([](const cPlayer* pPlayer)
    //{
    //    const coreVector3& vPos = pPlayer->GetPosition ();
    //    const coreVector3& vDir = pPlayer->GetDirection();
    //    const coreVector3  vTan = coreVector3(vDir.xy().Rotate90(), 0.0f);
    //
    //    g_pSpecialEffects->CreateTrailSmoke(vPos - 5.0f*vDir + 2.0f*vTan, -vDir, g_pEnvironment->GetSpeed()/0.7f, 1u);
    //    g_pSpecialEffects->CreateTrailSmoke(vPos - 5.0f*vDir - 2.0f*vTan, -vDir, g_pEnvironment->GetSpeed()/0.7f, 1u);
    //});


    auto nControlObjectsFunc = [](std::vector<coreBatchList*>* OUTPUT papList)
    {
        FOR_EACH(it, *papList)
        {
            FOR_EACH(et, *(*it)->List())
            {
                coreObject3D* pObject = (*et);
                pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            }

            (*it)->MoveNormal();
        }
    };
    nControlObjectsFunc(&m_apGroundObjectList);
    nControlObjectsFunc(&m_apDecalObjectList);
    //nControlObjectsFunc(&m_apAirObjectList);

}