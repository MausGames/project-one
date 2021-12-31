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
cSpaceBackground::cSpaceBackground()noexcept
: m_vCoverDir    (coreVector2(0.0f,1.0f))
, m_fMeteorSpeed (1.0f)
, m_iCopyLower   (0u)
, m_iCopyUpper   (0u)
{
    coreBatchList* pList1;

    // 
    m_pOutdoor = new cOutdoor();
    m_pOutdoor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // allocate stone list
    pList1 = new coreBatchList(SPACE_METEOR_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SPACE_METEOR_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SPACE_METEOR_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(-30.0f, -20.0f);

            if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(10.0f)))
            {
                // create object
                coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                // set object properties
                pObject->SetPosition   (coreVector3(vPosition, fHeight));
                pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f) * 2.0f);
                pObject->SetDirection  (coreVector3::Rand());
                pObject->SetOrientation(coreVector3::Rand());
                pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (Core::Rand->Bool() ? 0.5f : 1.0f));

                // add object to the list
                pList1->BindObject(pObject);
            }
        }

        // 
        m_iCopyUpper = pList1->List()->size();

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SPACE_METEOR_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_iCopyLower = pList1->List()->size() - m_iCopyUpper;

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    m_Cover.DefineTexture(0u, "environment_space.png");
    m_Cover.DefineProgram("menu_grey_program");
    m_Cover.SetPosition  (coreVector2(0.0f,0.0f));
    m_Cover.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Cover.SetColor3    (LERP(COLOR_MENU_MAGENTA, coreVector3(1.0f,1.0f,1.0f), 0.5f));
}


// ****************************************************************
// 
void cSpaceBackground::__RenderOwnBefore()
{
    glDepthMask(false);
    glDisable(GL_BLEND);
    {
        // 
        m_Cover.Render();
    }
    glDepthMask(true);
    glEnable(GL_BLEND);
}


// ****************************************************************
// 
void cSpaceBackground::__MoveOwn()
{
    // 
    const coreMatrix3 mRota = coreMatrix4::RotationY(TIME * -0.7f * m_fMeteorSpeed).m123();

    // 
    coreBatchList* pList = m_apGroundObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pMeteor = (*pList->List())[i];
        if((i >= m_iCopyLower) && (i < m_iCopyUpper) && !pMeteor->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        pMeteor->SetDirection  ((pMeteor->GetDirection  () * mRota).Normalized());
        pMeteor->SetOrientation((pMeteor->GetOrientation() * mRota).Normalized());
    }
    pList->MoveNormal();

    // 
    const coreVector2 vMove      = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexOffset = m_Cover.GetTexOffset() + (coreVector2(0.0f,0.0f) + vMove) * (0.05f * TIME);

    // 
    m_Cover.SetDirection(MapToAxis(g_pEnvironment->GetDirection().InvertedX(), m_vCoverDir));
    m_Cover.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Cover.Move();
}