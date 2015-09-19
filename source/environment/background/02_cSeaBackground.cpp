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
        pList1->ShrinkToFit();
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate weed lists
    pList1 = new coreBatchList(SEA_WEED_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        for(coreUintW i = 0u; i < SEA_WEED_NUM; ++i)
        {
            for(coreUintW j = 0u; j < 4u; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_WEED_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+166.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  ("environment_reed_01.md3"); 
                        pObject->DefineTexture(0u, "environment_reed.png"); 
                        pObject->DefineTexture(1u, "environment_grass_norm.png"); 
                        pObject->DefineProgram("object_ground_program"); 

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize       (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 2.0f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
                        pObject->SetColor3     (coreVector3(1.0f, 1.0f * Core::Rand->Float(0.55f, 0.65f), 0.5f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
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
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(SEA_ANIMAL_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const coreFloat   fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

            // test for valid values
            if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                   !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f) &&
                   !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, 25.0f))
                {


                    const coreFloat A = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(0.0f, OUTDOOR_DETAIL));
                    const coreFloat B = m_pOutdoor->RetrieveHeight(vPosition + coreVector2(OUTDOOR_DETAIL, 0.0f));
                    const coreFloat C = m_pOutdoor->RetrieveHeight(vPosition - coreVector2(0.0f, OUTDOOR_DETAIL));
                    const coreFloat D = m_pOutdoor->RetrieveHeight(vPosition - coreVector2(OUTDOOR_DETAIL, 0.0f));

                    const coreVector3 vOri = -coreVector3::Cross(coreVector3(0.0f, OUTDOOR_DETAIL * -2.0f, C - A).Normalize(),
                                                                 coreVector3(OUTDOOR_DETAIL * -2.0f, 0.0f, D - B).Normalize()).Normalize();


                    // determine object type
                    const coreBool bType = Core::Rand->Int(3) ? true : false;

                    // load object resources
                    coreObject3D* pObject = new coreObject3D();
                    pObject->DefineModel  (bType ? "environment_seashell.md3" : "environment_starfish.md3");
                    pObject->DefineTexture(0u, "environment_sea.png");
                    pObject->DefineTexture(1u, Core::Manager::Resource->Load<coreTexture>("test", CORE_RESOURCE_UPDATE_AUTO, "data/textures/environment_sea_norm.png"));
                    pObject->DefineProgram("object_ground_program");
                       
                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, fHeight+0.1f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.2f, 2.6f));
                    pObject->SetDirection  (vOri);
                    pObject->SetOrientation(coreVector3(coreVector2::Rand(), 0.0f));
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.7f, 0.85f));

                    // add object to the list
                    if(bType) pList1->BindObject(pObject);
                         else pList2->BindObject(pObject);
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apDecalObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2);
        pList2->ShrinkToFit();
        m_apDecalObjectList.push_back(pList2);



        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }
}