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
cSeaBackground::cSeaBackground()noexcept
: m_fWaveTime (0.0f)
{
    // TODO: bubbles ?    

    coreBatchList* pList1;
    coreBatchList* pList2;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "earth", 6u, 4.0f);

    // create underwater-surface object
    m_pWater = new cUnderWater();

    // allocate stone list
    pList1 = new coreBatchList(SEA_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SEA_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_STONE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -23.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 25.0f))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f));
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

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SEA_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate weed lists
    pList1 = new coreBatchList(SEA_WEED_RESERVE);
    pList1->DefineProgram("object_wave_inst_program");   // TODO: add to shadow-class!    
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_weed.md3");
        oBase.DefineTexture(0u, "environment_reed.png");
        oBase.DefineTexture(1u, "environment_grass_norm.png");
        oBase.DefineProgram("object_wave_program");

        for(coreUintW i = 0u; i < SEA_WEED_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_WEED_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -23.0f) && (fHeight < -18.0f))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 1.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 9.0f))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3::Rand(0.7f,1.6f, 0.7f,1.6f, 0.7f,1.6f) * 4.0f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand().Processed(ABS) * coreVector2(-1.0f,1.0f), 0.0f));
                        pObject->SetOrientation(coreVector3(coreVector2::Rand(-0.1f,0.1f, -0.1f,0.1f), 1.0f).Normalized());
                        pObject->SetColor3     (coreVector3(0.5f, Core::Rand->Float(0.55f, 0.65f) * (Core::Rand->Bool() ? 0.75f : 1.0f), 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -0.8f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SEA_WEED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);

        // bind weed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate animal lists
    pList1 = new coreBatchList(SEA_ANIMAL_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SEA_ANIMAL_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_sea_diff.png");
        oBase.DefineTexture(1u, "environment_sea_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SEA_ANIMAL_NUM; ++i)
        {
            for(coreUintW j = 8u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition   = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_ANIMAL_NUM);
                const coreFloat   fHeight     = m_pOutdoor->RetrieveBackHeight(vPosition);
                const coreFloat   fHeightUp   = m_pOutdoor->RetrieveBackHeight(coreVector2(vPosition.x,     vPosition.y + OUTDOOR_DETAIL));
                const coreFloat   fHeightDown = m_pOutdoor->RetrieveBackHeight(coreVector2(vPosition.x, MAX(vPosition.y - OUTDOOR_DETAIL, I_TO_F(OUTDOOR_VIEW / 2u))));

                // test for valid values
                if((fHeight > fHeightDown) && (fHeight < fHeightUp) &&
                   (fHeight > -22.0f)      && (fHeight < -16.0f)    && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 4.0f))
                    {
                        // 
                        const coreVector3 vDirection   = m_pOutdoor->RetrieveBackNormal(vPosition);
                        const coreVector3 vTangent     = coreVector3::Cross(vDirection, -CAMERA_DIRECTION).Normalized();
                        const coreVector3 vOrientation = coreVector3::Cross(vDirection, vTangent);

                        // determine object type
                        const coreBool bType = Core::Rand->Bool(0.75f) ? true : false;

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_seashell.md3" : "environment_starfish.md3");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f));
                        pObject->SetDirection  (vDirection);
                        pObject->SetOrientation(vOrientation);
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 0.85f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.9f);
        this->_StoreHeight(pList2, 0.9f);
        this->_StoreNormalList(pList1);
        this->_StoreNormalList(pList2);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SEA_ANIMAL_1_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SEA_ANIMAL_2_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    /*
    // allocate algae list
    pList1 = new coreBatchList(SEA_ALGAE_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_algae.png");
        oBase.DefineProgram("effect_decal_program");

        for(coreUintW i = 0u; i < SEA_ALGAE_NUM; ++i)
        {
            for(coreUintW j = 20u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SEA_ALGAE_NUM);
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 40.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 700.0f))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(2.0f,2.0f,1.0f));
                    pObject->SetColor3  (coreVector3(0.5f,1.0f,1.0f) * Core::Rand->Float(0.8f, 1.0f));
                    pObject->SetTexSize (coreVector2(0.5f,1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                    break;
                }
            }
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1, SEA_ALGAE_RESERVE);
        m_apDecalObjectList.push_back(pList1);
    }
    */

    // 
    m_pUnderSound = Core::Manager::Resource->Get<coreSound>("environment_under.wav");
    m_pUnderSound.OnUsableOnce([this, pResource = m_pUnderSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cSeaBackground::~cSeaBackground()
{
    // 
    if(m_pUnderSound->EnableRef(this))
        m_pUnderSound->Stop();
}


// ****************************************************************
// 
void cSeaBackground::__UpdateOwn()
{
    const coreBatchList*  pGround  = m_apGroundObjectList[0];
    const coreProgramPtr& pProgram = pGround->IsInstanced() ? pGround->GetProgram() : pGround->List()->front()->GetProgram();

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // 
    pProgram->Enable();
    pProgram->SendUniform("u_v1Time", m_fWaveTime);
}


// ****************************************************************
// move the sea background
void cSeaBackground::__MoveOwn()
{
    // 
    m_fWaveTime.Update(1.4f);

    // 
    if(m_pUnderSound->EnableRef(this))
        m_pUnderSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}