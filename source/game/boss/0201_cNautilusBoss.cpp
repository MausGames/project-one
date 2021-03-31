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
// counter identifier
#define CONTAINER_ATTACHED (0u)
#define DASH_SIDE          (1u)
#define SPIN_SIDE          (2u)
#define STOMP_COUNT        (3u)
#define IMPACT_COUNT       (4u)
#define INK_INDEX          (5u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cNautilusBoss::cNautilusBoss()noexcept
: m_fClawAngle (0.0f)
, m_vInkTarget (coreVector2(FLT_MAX,FLT_MAX))
, m_afInkAlpha {}
, m_fAnimation (0.0f)
, m_fMovement  (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_nautilus_body_high.md3");
    this->DefineModelLow ("ship_boss_nautilus_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.0f,2.0f,2.0f));

    // configure the boss
    this->Configure(8000, COLOR_SHIP_PURPLE);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].DefineModelHigh("ship_boss_nautilus_claw_high.md3");
        m_aClaw[i].DefineModelLow ("ship_boss_nautilus_claw_low.md3");
        m_aClaw[i].SetSize        (coreVector3(2.0f,2.0f,2.0f));
        m_aClaw[i].Configure      (1, COLOR_SHIP_PURPLE);
        m_aClaw[i].SetParent      (this);
    }

    // 
    m_InkBullet.DefineModel  ("bullet_spear.md3");
    m_InkBullet.DefineTexture(0u, "effect_energy.png");
    m_InkBullet.DefineProgram("effect_energy_flat_direct_program");
    m_InkBullet.SetSize      (coreVector3(1.45f,1.55f,1.45f) * 4.0f);
    m_InkBullet.SetColor3    (COLOR_ENERGY_CYAN * 0.8f);
    m_InkBullet.SetTexSize   (coreVector2(0.5f,0.2f));
    m_InkBullet.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cNautilusBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (HIDDEN_POS);
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(true);

    // 
    g_pWindscreen->GetInk()->Enable(false);

    // 
    m_aiCounter[CONTAINER_ATTACHED] = 1;
}


// ****************************************************************
// 
void cNautilusBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pWindscreen->GetInk()->Disable(bAnimated ? NAUTILUS_INK_SPEED : 0.0f);

    // 
    this->__DisableBullet(bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cNautilusBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_InkBullet.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyObject(&m_InkBullet);
}


// ****************************************************************
// 
void cNautilusBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    // ################################################################
    // ################################################################

    if((m_iPhase < 10u) && TIME)
    {
        // 
        m_fMovement.UpdateMod(1.0f, 2.0f*PI);

        // 
        const coreFloat fPrevX = 0.6f * SIN(m_fMovement - TIME);
        const coreFloat fCurX  = 0.6f * SIN(m_fMovement);

        // 
        this->SetPosition (coreVector3(fCurX * FOREGROUND_AREA.x, this->GetPosition().yz()));
        this->SetDirection(coreVector3(fCurX - fPrevX, Core::System->GetTime(), 0.0f).Normalized());
    }

    if(m_aiCounter[CONTAINER_ATTACHED])
    {
        // 
        pContainer->SetPosition (this->GetPosition () + NAUTILUS_ATTACH_DIST * this->GetDirection());
        pContainer->SetDirection(this->GetDirection());
    }

    // 
    const coreVector2& vImpact = pMission->GetContainerImpact();
    if(!vImpact.IsNull())
    {
        for(coreUintW i = 20u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact,  vDir)->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact, -vDir)->ChangeSize(1.3f);
        }

        // 
        g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
    }

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.11f * m_fClawAngle, this->GetOrientation()).m123();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].SetPosition   (this->GetPosition   ());
        m_aClaw[i].SetDirection  (this->GetDirection  () * (i ? mRota : mRota.Transposed()));
        m_aClaw[i].SetOrientation(this->GetOrientation() * (i ? -1.0f : 1.0f));
    }

    // 
    if(m_iPhase != 10u)
    {
        PHASE_CONTROL_TICKER(3u, 0u, (2.0f/NAUTILUS_INK_TIME) - 0.03f, LERP_LINEAR)
        {
            const coreVector3 vPos = this->GetPosition();

            // 
            this->__EnableBullet(vPos.xy(), this->NearestPlayerDual(m_aiCounter[INK_INDEX])->GetPosition().xy());

            // 
            g_pSpecialEffects->CreateSplashSmoke(vPos, SPECIAL_SPLASH_TINY, coreVector3(0.1f,0.1f,0.1f));
            g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
        });
    }

    // 
    if(m_InkBullet.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        const coreVector3 vPos    = m_InkBullet.GetPosition();
        const coreVector3 vDir    = m_InkBullet.GetDirection();
        const coreVector3 vNewPos = coreVector3(vPos.xy() + vDir.xy() * (40.0f * TIME), 0.0f);

        // 
        m_InkBullet.SetPosition (vNewPos);
        m_InkBullet.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.2f));
        m_InkBullet.Move();

        // 
        if(coreVector2::Dot(vNewPos.xy() - m_vInkTarget, vDir.xy()) > 0.0f)
        {
            m_aiCounter[INK_INDEX] = 1 - m_aiCounter[INK_INDEX];

            // 
            this->__DisableBullet(true);
            this->__CreateInk(m_aiCounter[INK_INDEX], vNewPos.xy());

            // 
            g_pSpecialEffects->CreateSplashSmoke(vNewPos, SPECIAL_SPLASH_BIG, coreVector3(0.1f,0.1f,0.1f));
            g_pSpecialEffects->CreateSplashColor(vNewPos, SPECIAL_SPLASH_BIG, COLOR_ENERGY_CYAN);
        }
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_afInkAlpha); ++i)
    {
        m_afInkAlpha[i].Update(-1.0f);
        g_pWindscreen->GetInk()->SetBlotAlpha(i, CLAMP(MIN(m_afInkAlpha[i], NAUTILUS_INK_TIME - m_afInkAlpha[i]) * NAUTILUS_INK_SPEED, 0.0f, 1.0f));
    }

    // TODO: line movement 
    //const coreVector2 vLinePos = coreVector2(0.0f, 0.1f * SIN(2.0f*PI * 0.08f * coreFloat(Core::System->GetTotalTime()))); 
}

// ****************************************************************
// 
void cNautilusBoss::__CreateInk(const coreUintW iIndex, const coreVector2& vPosition)
{
    ASSERT(iIndex < ARRAY_SIZE(m_afInkAlpha))

    // 
    ASSERT(m_afInkAlpha[iIndex] <= 0.0f)
    m_afInkAlpha[iIndex] = NAUTILUS_INK_TIME;

    // 
    g_pWindscreen->GetInk()->SetBlotPosition(iIndex, vPosition);
}


// ****************************************************************
// 
void cNautilusBoss::__EnableBullet(const coreVector2& vStart, const coreVector2& vEnd)
{
    WARN_IF(m_InkBullet.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    const coreVector2 vDir = (vEnd - vStart).Normalized();

    // 
    m_InkBullet.SetPosition (coreVector3(vStart, 0.0f));
    m_InkBullet.SetDirection(coreVector3(vDir,   0.0f));

    // 
    m_InkBullet.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_InkBullet);

    // 
    m_vInkTarget = vEnd;
}


// ****************************************************************
// 
void cNautilusBoss::__DisableBullet(const coreBool bAnimated)
{
    if(!m_InkBullet.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_InkBullet.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_InkBullet);
}