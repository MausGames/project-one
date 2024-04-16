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
cGrassBackground::cGrassBackground()noexcept
: m_fLeafTime (Core::Rand->Float(10.0f))
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("grass", "dust", OUTDOOR_ALGORITHM_GRASS, 4.0f, true);

    // allocate stone list
    pList1 = new coreBatchList(GRASS_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < GRASS_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_STONE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -23.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(5.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f) * 1.3f);
                    pObject->SetDirection  (coreVector3::Rand());
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.2f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, GRASS_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);

        // 
        m_apWaterRefList.push_back(pList1);
    }

    // allocate reed lists
    pList1 = new coreBatchList(GRASS_PLANT_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(GRASS_PLANT_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_plant.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < GRASS_PLANT_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_PLANT_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -23.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+176.0f) % 80 < 60))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(5.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(5.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(5.0f)))
                    {
                        // determine object type
                        const coreBool bType = (Core::Rand->Bool(0.4f) || (fHeight < -21.0f));

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_plant_05.md3" : "environment_plant_01.md3");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.2f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(1.0f, 1.1f * Core::Rand->Float(0.57f, 0.63f), 0.5f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, GRASS_PLANT_1_RESERVE);
        m_apGroundObjectList.push_front(pList1);   // # front

        cBackground::_FillInfinite(pList2, GRASS_PLANT_2_RESERVE);
        m_apGroundObjectList.push_front(pList2);   // # front

        // bind list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(GRASS_SHIP_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("ship_player_atk_high.md3");
        oBase.DefineTexture(0u, "ship_player.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < GRASS_SHIP_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_SHIP_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight < -23.0f) && (F_TO_SI(vPosition.y+150.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(20.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(15.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(15.0f)) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(15.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                    pObject->SetOrientation(coreVector3(0.0f,0.0f,-1.0f));
                    pObject->SetColor3     (COLOR_SHIP_GREY * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.2f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, GRASS_SHIP_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);

        // 
        m_apWaterRefList.push_back(pList1);
    }

    // allocate flower list
    pList1 = new coreBatchList(GRASS_FLOWER_RESERVE);
    pList1->DefineProgram("effect_decal_light_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_flowers.png");
        oBase.DefineProgram("effect_decal_light_program");

        for(coreUintW i = 0u; i < GRASS_FLOWER_NUM; ++i)
        {
            for(coreUintW j = 20u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_FLOWER_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if(fHeight > -15.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(6.35f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3(coreVector2(1.0f,1.0f) * Core::Rand->Float(9.0f, 10.0f), 1.0f) * 1.3f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));
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

        // 
        this->_StoreHeight(pList1, -0.8f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, GRASS_FLOWER_RESERVE);
        m_apDecalObjectList.push_back(pList1);
    }

    // allocate leaf list
    pList1 = new coreBatchList(GRASS_LEAF_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_leaf.png");
        oBase.DefineProgram("effect_decal_program");

        for(coreUintW i = 0u; i < GRASS_LEAF_NUM; ++i)
        {
            for(coreUintW j = 10u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_LEAF_NUM);
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 40.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(25.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(2.4f,0.0f,1.0f) * 1.3f);
                    pObject->SetColor3  (coreVector3(1.0f,1.0f,1.0f) * (0.75f + 0.2f * fHeight/40.0f));
                    pObject->SetTexSize (coreVector2(0.5f,0.85f));

                    // add object to the list
                    pList1->BindObject(pObject);
                    break;
                }
            }
        }

        // 
        coreSet<coreObject3D*>* papContent = pList1->List();
        while(papContent->size() % 4u)
        {
            POOLED_DELETE(s_MemoryPool, papContent->back())
            papContent->pop_back();
        }

        // 
        m_iLeafNum = papContent->size();
        ASSERT(!(m_iLeafNum % 4u))

        // post-process list and add to the air
        cBackground::_FillInfinite(pList1, GRASS_LEAF_RESERVE);
        m_apAirObjectList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(GRASS_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < GRASS_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, GRASS_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f));
            pObject->SetAlpha    (0.85f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, GRASS_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == GRASS_CLOUD_RESERVE)
    }

    // 
    this->SetGroundDensity(3u, 0.0f);
}


// ****************************************************************
// destructor
cGrassBackground::~cGrassBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cGrassBackground::__InitOwn()
{
    // create water-surface object
    m_pWater = new cWater("environment_clouds_blue.png");

    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_grass.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cGrassBackground::__ExitOwn()
{
    // 
    SAFE_DELETE(m_pWater)

    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// move the grass background
void cGrassBackground::__MoveOwn()
{
    // 
    m_fLeafTime.Update(0.5f * MAX(ABS(g_pEnvironment->GetSpeed()), 2.0f));

    // 
    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pLeaf = (*pList->List())[i];
        if(!pLeaf->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(POW2(i) % m_iLeafNum);
        const coreFloat   fTime   = m_fLeafTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat   fPos    = SIN(fTime * 0.05f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);
        const coreVector2 vOri    = coreVector2::Direction(fTime);
        const coreVector2 vDir    = MapStepRotated90(vOri, i % 4u);

        // 
        pLeaf->SetPosition   (coreVector3(fPos, pLeaf->GetPosition().yz()));
        pLeaf->SetDirection  (coreVector3(vDir, 0.0f));
        pLeaf->SetOrientation(OriRoundDir(vOri, vDir));

        // get currently visible polygon side
        const coreBool bSide = (coreVector3::Dot(g_pEnvironment->GetCameraPos() - pLeaf->GetPosition(), pLeaf->GetOrientation()) >= 0.0f);

        // simulate two-sided polygon (flip vertex-order and change texture)
        pLeaf->SetSize     (coreVector3(pLeaf->GetSize().x, pLeaf->GetSize().x * (bSide ? 0.7f : -0.7f), pLeaf->GetSize().z));
        pLeaf->SetTexOffset(coreVector2(bSide ? 0.5f : 0.0f, 0.15f));
    }
    pList->MoveNormal();

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
}