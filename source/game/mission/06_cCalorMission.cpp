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
cCalorMission::cCalorMission()noexcept
: m_Star        (CALOR_STARS)
, m_StarChain   (CALOR_STARS * CALOR_CHAINS)
, m_apStarOwner {}
, m_iStarState  (0u)
, m_fSwing      (0.0f)
{
    // 
    m_apBoss[0] = &m_Fenrir;
    m_apBoss[1] = &m_Shelob;
    m_apBoss[2] = &m_Zeroth;

    // 
    m_Star     .DefineProgram("object_ship_glow_inst_program");       
    m_StarChain.DefineProgram("object_ship_glow_inst_program");       
    {
        for(coreUintW i = 0u; i < CALOR_STARS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (CALOR_CHAINS + 1u);

            // load object resources
            coreObject3D* pStar = &m_aStarRaw[i];
            pStar->DefineModel  (iType ? "object_sphere.md3" : "object_star.md3");
            pStar->DefineTexture(0u, "ship_enemy.png");      
            pStar->DefineProgram("object_ship_glow_program");     

            // set object properties
            pStar->SetSize   (iType ? coreVector3(0.7f,0.7f,0.7f) : coreVector3(1.2f,1.2f,1.2f));
            pStar->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_StarChain.BindObject(pStar);
                 else m_Star     .BindObject(pStar);
        }
    }

    // 
    cShadow::GetGlobalContainer()->BindList(&m_Star);
    cShadow::GetGlobalContainer()->BindList(&m_StarChain);
}


// ****************************************************************
// destructor
cCalorMission::~cCalorMission()
{
    // 
    m_Snow.Disable(0.0f);

    // 
    cShadow::GetGlobalContainer()->UnbindList(&m_Star);
    cShadow::GetGlobalContainer()->UnbindList(&m_StarChain);

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i) this->DisableStar(i, false);
}


// ****************************************************************
// 
void cCalorMission::EnableStar(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    WARN_IF(pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apStarOwner[iIndex] = pOwner;

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pStar);
    for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nInitFunc(pChain + i);
}


// ****************************************************************
// 
void cCalorMission::DisableStar(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apStarOwner[iIndex] = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        nExitFunc(pStar);
        for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nExitFunc(pChain + i);
    }
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnBottom()
{
    // 
    m_Snow.Render();
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_StarChain.Render();
        m_Star     .Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnAfter()
{
    // 
    m_fSwing.UpdateMod(2.0f, 2.0f*PI);

    // 
    m_Snow.Move();

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        coreObject3D* pStar = (*m_Star.List())[i];
        if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apStarOwner[i];
        if(pOwner)
        {
            const coreVector2 vDir = coreVector2::Direction(m_fSwing);

            // 
            pStar->SetPosition (coreVector3(pOwner->GetPosition().xy() + vDir * 10.0f, 0.0f));
            pStar->SetDirection(coreVector3(vDir, 0.0f));
        }

        // 
        for(coreUintW j = 0u; j < CALOR_CHAINS; ++j)
        {
            coreObject3D* pChain = (*m_StarChain.List())[i*CALOR_CHAINS + j];

            
            const coreFloat fOffset = I_TO_F(j) / I_TO_F(CALOR_CHAINS);
            
            const coreVector2 vPos = LERP(pOwner->GetPosition().xy(), pStar->GetPosition().xy(), fOffset);

            pChain->SetPosition(coreVector3(vPos, 0.0f));
            
        }
    }

    // 
    m_Star     .MoveNormal();
    m_StarChain.MoveNormal();
}