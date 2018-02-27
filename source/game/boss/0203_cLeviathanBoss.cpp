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
// counter identifier
#define JUMP_SIDE (0u)


// ****************************************************************
// vector identifier
#define FALL_BEHIND (0u)


// ****************************************************************
// constructor
cLeviathanBoss::cLeviathanBoss()noexcept
: m_fAnimation (-1.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_leviathan_head_high.md3");
    this->DefineModelLow ("ship_boss_leviathan_head_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.5f,1.5f,1.5f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_BLUE);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBody); ++i)
    {
        m_aBody[i].DefineModelHigh("ship_boss_leviathan_body_high.md3");
        m_aBody[i].DefineModelLow ("ship_boss_leviathan_body_low.md3");
        m_aBody[i].SetSize        (coreVector3(1.7f,1.7f,1.7f));
        m_aBody[i].Configure      (500, COLOR_SHIP_BLUE);
    }

    // 
    m_Tail.DefineModelHigh("ship_boss_leviathan_tail_high.md3");
    m_Tail.DefineModelLow ("ship_boss_leviathan_tail_low.md3");
    m_Tail.SetSize        (coreVector3(1.7f,1.7f,1.7f));
    m_Tail.Configure      (500, COLOR_SHIP_BLUE);
}


// ****************************************************************
// 
void cLeviathanBoss::__MoveOwn()
{
    // ################################################################
    // ################################################################

    // 
    m_fAnimation.Update(0.5f*PI);

    // 
    const coreFloat fHeadHeight = g_pEnvironment->GetBackground()->GetOutdoor()->RetrieveHeight(this->GetPosition().xy());
    if(this->GetPosition().z < fHeadHeight)
    {
        // below 
        m_avVector[FALL_BEHIND].x += Core::System->GetTime() * g_pEnvironment->GetSpeed() * MIN(OUTDOOR_DETAIL, (fHeadHeight - this->GetPosition().z) * 0.5f);
    }
    else
    {
        // above 
        m_avVector[FALL_BEHIND].x = 0.0f;
    }

    if(m_fAnimation > 0.0f)
    {
        // 
        const coreFloat fTailHeight = g_pEnvironment->GetBackground()->GetOutdoor()->RetrieveHeight(m_Tail.GetPosition().xy());
        if(m_Tail.GetPosition().z < fTailHeight)
        {
            // 
            m_fAnimation = -0.8f * PI;
            m_aiCounter[JUMP_SIDE] = 1 - m_aiCounter[JUMP_SIDE];
        }
    }

    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        const coreFloat fAngle = m_fAnimation - (I_TO_F(i)*0.17f + (i ? 0.02f : 0.0f)) * 0.4f * PI;

        // 
        coreVector3 vPos, vDir;
        cLeviathanBoss::__CalcCurvePosDir(coreVector3(-1.0f, 0.5f * (m_aiCounter[JUMP_SIDE] ? -1.0f : 1.0f), 0.0f).Normalized(), fAngle, coreVector3(50.0f,50.0f,35.0f), &vPos, &vDir);
        vPos = vPos + coreVector3(0.0f, -20.0f - m_avVector[FALL_BEHIND].x, -20.0f);

        // 
        pPart->SetPosition  (vPos);
        pPart->SetDirection (vDir);
        pPart->DefaultAxiate(m_fAnimation * ((i & 0x01u) ? 1.0f : -1.0f));
    }

    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        const coreFloat fHeight = g_pEnvironment->GetBackground()->GetOutdoor()->RetrieveHeight(pPart->GetPosition().xy());

        // 
        const coreBool iOldEnabled =  pPart->IsEnabled(CORE_OBJECT_ENABLE_ALL);
        const coreBool iNewEnabled = (pPart->GetPosition().z > fHeight);

        // 
        pPart->SetEnabled(iNewEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        // 
        if(iOldEnabled != iNewEnabled)
        {
            //g_pSpecialEffects->MacroExplosionPhysicalSmall(coreVector3(pPart->GetPosition().xy(), fHeight));
            //g_pSpecialEffects->CreateSplashSmoke(coreVector3(pPart->GetPosition().xy(), fHeight), 25.0f, 30u);
        }
    }
}


// ****************************************************************
// 
FUNC_NOALIAS void cLeviathanBoss::__CalcCurvePosDir(const coreVector3& vAxis, const coreFloat fAngle, const coreVector3& vScale, coreVector3* OUTPUT vPosition, coreVector3* OUTPUT vDirection)
{
    ASSERT(vAxis.IsNormalized() && vPosition && vDirection)

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(fAngle, vAxis).m123();
    const coreVector3 vDir  = coreVector3(vAxis.xy().Normalized().Rotated90(), 0.0f);
    const coreVector3 vPos  = (vDir * mRota).Normalized();

    // 
    (*vPosition)  = vPos * vScale;
    (*vDirection) = coreVector3::Cross(vAxis, (vPos / vScale).Normalized());
}


// ****************************************************************
// 
cEnemy* cLeviathanBoss::__GetPart(const coreUintW iIndex)
{
    ASSERT(iIndex < LEVIATHAN_PARTS)

    // 
    if(iIndex == 0u) return this;
    if(iIndex == 4u) return &m_Tail;
                     return &m_aBody[iIndex - 1u];
}


