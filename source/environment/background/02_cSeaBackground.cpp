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
        for(coreUintW i = 0u; i < SEA_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_STONE_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
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

    // allocate weed lists
    pList1 = new coreBatchList(SEA_WEED_RESERVE);
    pList1->DefineProgram("object_wave_inst_program");
    {
        for(coreUintW i = 0u; i < SEA_WEED_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 4u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_WEED_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -23.0f && fHeight < -18.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 1.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 9.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  ("environment_weed.md3"); 
                        pObject->DefineTexture(0u, "environment_reed.png"); 
                        pObject->DefineTexture(1u, "environment_grass_norm.png"); 
                        pObject->DefineProgram("object_wave_program"); 

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize       (coreVector3::Rand(0.7f,1.6f, 0.7f,1.6f, 0.7f,1.6f) * 4.0f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand().Abs() * coreVector2(-1.0f,1.0f), 0.0f));  
                        pObject->SetOrientation(coreVector3(coreVector2::Rand(-0.1f,0.1f, -0.1f,0.1f), 1.0f).Normalize());  
                        
                        if(Core::Rand->Bool()) pObject->SetColor3     (coreVector3(0.5f, 1.0f * Core::Rand->Float(0.55f, 0.65f), 1.0f));
                                          else pObject->SetColor3     (coreVector3(0.5f, 1.0f * Core::Rand->Float(0.55f, 0.65f)*0.75, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        m_apGroundObjectList.push_back(pList1);

        // bind weed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate animal lists
    pList1 = new coreBatchList(SEA_ANIMAL_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SEA_ANIMAL_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW i = 0u; i < SEA_ANIMAL_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 8u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_ANIMAL_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                const coreFloat   fHeight2   = m_pOutdoor->RetrieveHeight(coreVector2(vPosition.x,     vPosition.y+OUTDOOR_DETAIL));
                const coreFloat   fHeight3   = m_pOutdoor->RetrieveHeight(coreVector2(vPosition.x, MAX(vPosition.y-OUTDOOR_DETAIL, I_TO_F(OUTDOOR_VIEW/2))));

                // test for valid values
                if((fHeight < fHeight2) && (fHeight3 < fHeight) && vPosition.x >= -30.0f && fHeight > -22.0f && fHeight < -16.0f && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, 4.0f))
                    {


                        const coreFloat A = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(0.0f, OUTDOOR_DETAIL*0.35f));
                        const coreFloat B = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(OUTDOOR_DETAIL*0.35f, 0.0f));
                        const coreFloat C = m_pOutdoor->RetrieveHeight(vPosition - coreVector2(0.0f, OUTDOOR_DETAIL*0.35f));
                        const coreFloat D = m_pOutdoor->RetrieveHeight(vPosition - coreVector2(OUTDOOR_DETAIL*0.35f, 0.0f));
                        
                        const coreVector3 vOri = -coreVector3::Cross(coreVector3(0.0f, OUTDOOR_DETAIL * -0.7f, C - A).Normalize(),
                                                                     coreVector3(OUTDOOR_DETAIL * -0.7f, 0.0f, D - B).Normalize()).Normalize();

                        //const coreFloat A = m_pOutdoor->RetrieveHeight(vPosition + coreVector2( OUTDOOR_DETAIL*0.35f,  OUTDOOR_DETAIL*0.35f));
                        //const coreFloat B = m_pOutdoor->RetrieveHeight(vPosition + coreVector2( OUTDOOR_DETAIL*0.35f, -OUTDOOR_DETAIL*0.35f));
                        //const coreFloat C = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(-OUTDOOR_DETAIL*0.35f, -OUTDOOR_DETAIL*0.35f));
                        //const coreFloat D = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(-OUTDOOR_DETAIL*0.35f,  OUTDOOR_DETAIL*0.35f));
                        //
                        //const coreVector3 vOri = -coreVector3::Cross(coreVector3(OUTDOOR_DETAIL * -0.7f, OUTDOOR_DETAIL * -0.7f, C - A).Normalize(),
                        //                                             coreVector3(OUTDOOR_DETAIL * -0.7f, OUTDOOR_DETAIL *  0.7f, D - B).Normalize()).Normalize();


                        const coreVector3 vNew  = coreVector3::Cross(vOri, -CAMERA_DIRECTION).Normalize();
                        const coreVector3 vNew2 = coreVector3::Cross(vOri, vNew).Normalize();

                        // determine object type
                        const coreBool bType = Core::Rand->Int(3) ? true : false;

                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (bType ? "environment_seashell.md3" : "environment_starfish.md3");
                        pObject->DefineTexture(0u, "environment_sea_diff.png");
                        pObject->DefineTexture(1u, "environment_sea_norm.png");
                        pObject->DefineProgram("object_ground_program");
                       
                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight+0.9f));
                        pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f));
                        pObject->SetDirection  (vOri);
                        //pObject->SetOrientation(coreVector3(coreVector2::Rand(0.2f,0.7f, -1.0f,-0.5f).Normalize(), 0.0f));
                        //pObject->SetOrientation(coreVector3(-vPosition.x, -30.0f, 0.0f).Normalize());
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 0.85f));
                        pObject->SetOrientation(vNew2);
                        //if(vPosition.x > 0.0f) pObject->SetOrientation(pObject->GetOrientation().InvertedX());
                        
                        
                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);

        //m_apDecalObjectList 
            m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2);

        //m_apDecalObjectList 
            m_apGroundObjectList.push_back(pList2);



        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate algae list
    pList1 = new coreBatchList(SEA_ALGAE_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        for(coreUintW i = 0u; i < SEA_ALGAE_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 20u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_ALGAE_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 30.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 700.0f))
                {
                    // load object resources
                    coreObject3D* pObject = new coreObject3D();
                    pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                    pObject->DefineTexture(0u, "environment_algae.png");
                    pObject->DefineProgram("effect_decal_program");

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
        cBackground::_FillInfinite(pList1);
        m_apDecalObjectList.push_back(pList1);
    }
}


// ****************************************************************
// render the sea background
void cSeaBackground::__RenderOwn()
{
    const coreBatchList*  pGround  = m_apGroundObjectList[1];
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
}