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
cSpaceBackground::cSpaceBackground()noexcept
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // TODO: shadow with pseudo-outdoor object, setting algorithm to not creating geometry (beware of GetHeight())          

    // allocate stone list
    pList1 = new coreBatchList(SPACE_METEOR_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SPACE_METEOR_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        //oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("test", CORE_RESOURCE_UPDATE_AUTO, "data/textures/environment_stone_diff2.png"));
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SPACE_METEOR_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SPACE_METEOR_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(-30.0f, -20.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            

            // set object properties
            pObject->SetPosition   (coreVector3(vPosition, fHeight+0.2f));
            pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f) * 2.0f);
            pObject->SetDirection  (coreVector3::Rand());
            pObject->SetOrientation(coreVector3::Rand());
            pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

            // add object to the list
            if(Core::Rand->Bool()) 
            {
                pObject->DefineTexture(0u, "environment_stone_diff.png");
                pList2->BindObject(pObject);
            }
            else
                pList1->BindObject(pObject);
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SPACE_METEOR_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SPACE_METEOR_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        //ASSERT(pList1->GetCurCapacity() == SPACE_METEOR_RESERVE)
    }
}


// ****************************************************************
// 
void cSpaceBackground::__MoveOwn()
{
    // 
    //for(coreUintW j = 0u; j < 2u; ++j)
    //{
    //    coreBatchList* pList = m_apGroundObjectList[j];
    //    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    //    {
    //        coreObject3D* pStone = (*pList->List())[i];
    //        if(!pStone->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;
    //
    //        // 
    //        pStone->SetPosition (coreVector3(0.0f, pStone->GetPosition().yz()));
    //        pStone->SetDirection(coreVector3(0.0f, 0.0f, 0.0f));
    //    }
    //    pList->MoveNormal();
    //}
}