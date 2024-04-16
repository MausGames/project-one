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
, m_fCoverScale  (1.0f)
, m_fMeteorSpeed (1.0f)
, m_iCopyLower   (0u)
, m_iCopyUpper   (0u)
, m_vNebulaMove  (coreVector2(0.0f,0.0f))
{
    coreBatchList* pList1;

    // 
    this->__InitOwn();

    // 
    m_pOutdoor = new cOutdoor();
    m_pOutdoor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // allocate meteor list
    pList1 = new coreBatchList(SPACE_METEOR_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        coreBool bBlack = false;

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
            const coreFloat   fHeight   = Core::Rand->Float(-30.0f, -5.0f);   // # shadow issues below -30.0f

            if(!cBackground::_CheckIntersectionQuick3(pList1, coreVector3(vPosition, fHeight), POW2(11.5f)))
            {
                // create object
                coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                // set object properties
                pObject->SetPosition   (coreVector3(vPosition, fHeight));
                pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f) * 2.1f);
                pObject->SetDirection  (coreVector3::Rand());
                pObject->SetOrientation(coreVector3::Rand());
                pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (bBlack ? 0.5f : 1.0f));

                // add object to the list
                pList1->BindObject(pObject);

                // 
                bBlack = !bBlack;
            }
        }

        // 
        m_iCopyUpper = pList1->List()->size();

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SPACE_METEOR_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_iCopyLower = pList1->List()->size() - m_iCopyUpper;

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    m_Cover.DefineTexture(0u, "environment_space_inside.png");
    m_Cover.DefineProgram("menu_grey_program");
    m_Cover.SetPosition  (coreVector2(0.0f,0.0f));
    m_Cover.SetColor3    (LERP(COLOR_MENU_MAGENTA, coreVector3(1.0f,1.0f,1.0f), 0.35f) * 1.3f);

    // 
    m_Cover2.DefineTexture(0u, "environment_space_outside.png");
    m_Cover2.DefineProgram("menu_single_program");
    m_Cover2.SetPosition  (coreVector2(0.0f,0.0f));
    m_Cover2.SetColor3    (LERP(COLOR_MENU_MAGENTA, coreVector3(1.0f,1.0f,1.0f), 0.15f) * 1.3f);
    m_Cover2.SetAlpha     (0.7f);
    m_Cover2.SetTexSize   (coreVector2(1.0f,1.0f) * 4.2f);

    // 
    m_Nebula.DefineTexture(0u, "environment_clouds_low.png");
    m_Nebula.DefineProgram("effect_weather_nebula_program");
    m_Nebula.SetPosition  (coreVector2(0.0f,0.0f));
    m_Nebula.SetAlpha     (0.2f);
}


// ****************************************************************
// destructor
cSpaceBackground::~cSpaceBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cSpaceBackground::__InitOwn()
{
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_space.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cSpaceBackground::__ExitOwn()
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
void cSpaceBackground::__RenderOwnBefore()
{
    glDepthMask(false);
    {
        glDisable(GL_BLEND);
        {
            // 
            m_Cover.Render();
        }
        glEnable(GL_BLEND);

        // 
        m_Cover2.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cSpaceBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Nebula.GetProgram().IsUsable()) return;
    if(!m_Nebula.GetProgram()->Enable())  return;

    coreRand oRand(1u);

    // 
    coreProgram* pLocal = m_Nebula.GetProgram().GetResource();
    for(coreUintW i = 0u; i < SPACE_NEBULA_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Nebula.GetTexOffset() + coreVector2::Rand(0.0f,1.0f, 0.0f,1.0f, &oRand);
        const coreFloat   fNewScale     = 1.0f - 0.15f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale * 0.3f));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Nebula.Render();
    }
    glEnable(GL_DEPTH_TEST);
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
    const coreVector2 vEnvMove    = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vSize       = coreVector2(1.0f,1.0f) * SQRT2 * m_fCoverScale;
    const coreVector2 vTexOffset  = m_Cover .GetTexOffset() + (coreVector2(0.0f,0.0f) + vEnvMove) * (0.05f * TIME);
    const coreVector2 vTexOffset2 = m_Cover2.GetTexOffset() + (coreVector2(0.0f,0.0f) + vEnvMove) * (0.28f * TIME);

    // 
    m_Cover.SetSize     (vSize);
    m_Cover.SetDirection(MapToAxis(g_pEnvironment->GetDirection().InvertedX(), m_vCoverDir));
    m_Cover.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Cover.Move();

    // 
    m_Cover2.SetSize     (vSize);
    m_Cover2.SetDirection(m_Cover.GetDirection());
    m_Cover2.SetTexOffset(vTexOffset2.Processed(FRACT));
    m_Cover2.Move();

    // 
    const coreVector2 vEnvMove2   = coreVector2(0.0f,1.0f) * (-0.15f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize    = coreVector2(1.0f,1.0f) * 5.4f;
    const coreVector2 vTexOffset3 = m_Nebula.GetTexOffset() + (m_vNebulaMove.InvertedX() + vEnvMove2) * (0.5f * TIME);

    // 
    m_Nebula.SetSize     (vSize);
    m_Nebula.SetDirection(MapToAxisInv(g_pEnvironment->GetDirection().InvertedX(), m_vCoverDir));
    m_Nebula.SetColor3   (m_Cover.GetColor3().LowRatio());
    m_Nebula.SetTexSize  (vTexSize);
    m_Nebula.SetTexOffset(vTexOffset3.Processed(FRACT));
    m_Nebula.Move();

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
}