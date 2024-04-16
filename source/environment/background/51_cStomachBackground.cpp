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
cStomachBackground::cStomachBackground()noexcept
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("mars", "rock", 9u, 4.0f);

    // create water-surface object
    m_pWater = new cWater("environment_mars_diff.png");

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
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, STOMACH_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor4   (coreVector4(LERP(COLOR_MENU_PURPLE, coreVector3(1.0f,1.0f,1.0f), 0.5f) * (0.8f + 0.2f * fHeight/60.0f), 0.65f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite   (pList1, STOMACH_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == STOMACH_CLOUD_RESERVE)
    }
}


// ****************************************************************
// 
void cStomachBackground::__RenderOwnAfter()
{
    // 
    glDisable(GL_DEPTH_TEST);
    m_Headlight.Render();
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cStomachBackground::__MoveOwn()
{
   // ASSERT(!g_pEnvironment->GetSpeed())   // # no movement

    // 
    const coreFloat fMove = Core::System->GetTime() * 6.0f;

    // 
    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pCloud = (*pList->List())[i];

        // 
        coreFloat fNewPos = pCloud->GetPosition().y - fMove;
        if(fNewPos <= -I_TO_F(OUTDOOR_VIEW / 2u) * OUTDOOR_DETAIL) fNewPos += I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL;

        // 
        pCloud->SetPosition(coreVector3(pCloud->GetPosition().x, fNewPos, pCloud->GetPosition().z));
    }
    pList->MoveNormal();
}


// ****************************************************************
// 
void cStomachBackground::__UpdateOwn()
{
    // 
    m_Headlight.UpdateDefault();
}