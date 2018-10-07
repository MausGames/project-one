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
#define ROTATION_DIRECTION (0u)
#define BALL_STATUS        (1u)
#define TURRETS_ENABLED    (2u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_Turret     (TORUS_TURRETS)
, m_TurretHull (TORUS_TURRETS)
, m_fAnimation (0.0f)
, m_fRotation  (0.0f)
, m_fTurretion (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.7f,1.7f,1.7f));

    // configure the boss
    this->Configure(2800, COLOR_SHIP_GREY);

    // 
    m_Emitter.DefineModel  ("object_boss_torus_emitter.md3");
    m_Emitter.DefineTexture(0u, "effect_energy.png");
    m_Emitter.DefineProgram("effect_energy_invert_program");
    m_Emitter.SetSize      (this->GetSize());
    m_Emitter.SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
    m_Emitter.SetTexSize   (coreVector2(1.25f,1.25f));

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].DefineModel  ("object_boss_torus_circle.md3");
        m_aCircle[i].DefineTexture(0u, "effect_energy.png");
        m_aCircle[i].DefineProgram("effect_energy_invert_program");
        m_aCircle[i].SetSize      (this->GetSize());
        m_aCircle[i].SetColor3    (COLOR_ENERGY_GREEN * 0.8f);
        m_aCircle[i].SetTexSize   (coreVector2(1.25f,1.25f));
    }

    // 
    m_Turret    .DefineProgram("effect_energy_invert_inst_program");
    m_TurretHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_TURRETS_RAW; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pTurret = &m_aTurretRaw[i];
            pTurret->DefineModel  ("object_cube.md3");
            pTurret->DefineTexture(0u, "effect_energy.png");
            pTurret->DefineProgram("effect_energy_invert_program");

            // set object properties
            pTurret->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 2.85f);
            pTurret->SetColor3 (COLOR_ENERGY_CYAN * 0.7f);
            pTurret->SetTexSize(coreVector2(0.8f,0.3f));
            pTurret->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_TurretHull.BindObject(pTurret);
                 else m_Turret    .BindObject(pTurret);
        }
    }
}


// ****************************************************************
// 
void cTorusBoss::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->BindObject(&m_aCircle[i]);

    // 
    g_pGlow->BindList(&m_Turret);
    g_pGlow->BindList(&m_TurretHull);
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    if(!m_aiCounter[BALL_STATUS])
    {
        // 
        pMission->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized());
    }
    else
    {
        // 
        for(coreUintW i = 1u, ie = m_aiCounter[BALL_STATUS]; i < ie; ++i)
            pMission->DisableBall(i, bAnimated);
    }

    // 
    g_pGlow->UnbindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);

    // 
    g_pGlow->UnbindList(&m_Turret);
    g_pGlow->UnbindList(&m_TurretHull);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnUnder()
{
    if(m_aiCounter[TURRETS_ENABLED])
    {
        // 
        m_TurretHull.Render();
    }
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnAttack()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Emitter.Render();

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].Render();
    }
    glEnable(GL_DEPTH_TEST);

    if(m_aiCounter[TURRETS_ENABLED])
    {
        DEPTH_PUSH

        // 
        m_Turret.Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Turret);
    }
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{
    // 
    coreVector2 vNewOri = coreVector2(0.0f, m_aiCounter[ROTATION_DIRECTION] ? -1.0f : 1.0f);

    // 
    m_fAnimation.UpdateMod(-1.0f, 10.0f);
    m_fRotation .UpdateMod(-1.2f, 2.0f*PI);

    // ################################################################
    // ################################################################

    // 
    const coreVector2 vNewDir = coreVector2::Direction(m_fRotation);
    this->SetDirection  (coreVector3(vNewDir.x, -vNewOri.y*vNewDir.y, vNewOri.x*vNewDir.y));
    this->SetOrientation(coreVector3(0.0f,       vNewOri.x,           vNewOri.y));

    // pre-calculate rotation quaternion
    this->coreObject3D::Move();

    if(m_aiCounter[TURRETS_ENABLED])
    {
        // 
        m_fTurretion.UpdateMod(-0.2f, 10.0f);

        // 
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(m_fTurretion * PI), 0.0f);
        const coreVector3 vOri  = coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y);
        const coreVector2 vTex  = coreVector2(0.2f,1.0f) * m_fTurretion;
        const coreFloat   fTime = FMOD(m_fTurretion * -5.0f, 2.0f);

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            coreObject3D* pTurret = (*m_Turret    .List())[i];
            coreObject3D* pHull   = (*m_TurretHull.List())[i];
            if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            pTurret->SetDirection  (vDir);
            pTurret->SetOrientation(vOri);
            pTurret->SetTexOffset  (vTex);

            // 
            pHull->SetSize       (pTurret->GetSize       () * (1.0f + 0.25f*fTime));
            pHull->SetDirection  (pTurret->GetDirection  ());
            pHull->SetOrientation(pTurret->GetOrientation());
            pHull->SetAlpha      (pTurret->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pTurret->GetTexOffset  ());
        }

        // 
        m_Turret    .MoveNormal();
        m_TurretHull.MoveNormal();

        // 
        PHASE_CONTROL_TICKER(2u, 0u, 10.0f)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            {
                coreObject3D* pTurret = (*m_Turret.List())[i];
                if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pTurret->GetPosition ().xy();
                const coreVector2 vDir = pTurret->GetDirection().xy();

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
            }
        });

        // 
        cPlayer::TestCollision(TYPE_TORUS_TURRET, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pTurret, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            pPlayer->TakeDamage(10, ELEMENT_CYAN, vIntersection.xy());

            // 
            g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_CYAN);
        });

        // 
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_TORUS_TURRET, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pTurret, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            pBullet->Deactivate(true, vIntersection.xy());
        });
    }

    // constantly shoot into all directions
    PHASE_CONTROL_TICKER(3u, 0u, 1.0f)
    {
        // 
        const coreMatrix2 vRota = coreMatrix3::Rotation(DEG_TO_RAD(120.0f)).m12();

        for(coreUintW i = 5u; i--; )
        {
            coreVector2 avDir[3];

            // 
            avDir[0] = this->__GetRotaDirection(DEG_TO_RAD(I_TO_F(i-2u) * 7.0f)).xy();
            avDir[1] = avDir[0] * vRota;
            avDir[2] = avDir[1] * vRota;

            for(coreUintW j = 0u; j < 3u; ++j)
            {
                const coreVector2& vDir = avDir[j];
                const coreVector2  vPos = this->GetPosition().xy() + vDir * 8.0f;

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.2f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_YELLOW);
            }
        }
    });

    // 
    m_Emitter.SetPosition   (this->GetPosition());
    m_Emitter.SetDirection  (this->GetDirection());
    m_Emitter.SetOrientation(this->GetOrientation());
    m_Emitter.SetAlpha      (m_aTimer[3].GetValue(CORE_TIMER_GET_NORMAL));
    m_Emitter.SetTexOffset  (coreVector2(-0.1f,-0.4f) * m_fAnimation);
    m_Emitter.Move();

    // 
    const coreFloat fFade  = 1.0f - FRACT(ABS(m_fAnimation));
    m_aCircle[1].SetSize(this->GetSize() * (0.5f + 0.5f*fFade));
    m_aCircle[1].SetAlpha(fFade);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].SetPosition   (this->GetPosition   ());
        m_aCircle[i].SetDirection  (this->GetDirection  ());
        m_aCircle[i].SetOrientation(this->GetOrientation());
        m_aCircle[i].SetTexOffset  (coreVector2(0.1f * m_fAnimation, 0.0f));
        m_aCircle[i].Move();
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableTurret(const coreUintW iIndex, const coreVector2& vPosition)
{
    // 
    ASSERT(iIndex < TORUS_TURRETS)
    coreObject3D* pTurret = (*m_Turret    .List())[iIndex];
    coreObject3D* pHull   = (*m_TurretHull.List())[iIndex];

    // 
    if(pTurret->GetType()) return;
    pTurret->ChangeType(TYPE_TORUS_TURRET);

    // 
    m_aiCounter[TURRETS_ENABLED] += 1;

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(vPosition,  0.0f));
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pTurret);
    nInitFunc(pHull);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cTorusBoss::__DisableTurret(const coreUintW iIndex, const coreBool bAnimated)
{
    // 
    ASSERT(iIndex < TORUS_TURRETS)
    coreObject3D* pTurret = (*m_Turret    .List())[iIndex];
    coreObject3D* pHull   = (*m_TurretHull.List())[iIndex];

    // 
    if(!pTurret->GetType()) return;
    pTurret->ChangeType(0);

    // 
    m_aiCounter[TURRETS_ENABLED] -= 1;

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pTurret);
    nExitFunc(pHull);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pTurret->GetPosition(), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
FUNC_LOCAL coreVector3 cTorusBoss::__GetRotaDirection(const coreFloat fBaseAngle)
{
    // 
    return m_vRotation.QuatApply(coreVector3(coreVector2::Direction(fBaseAngle), 0.0f)).Normalized();
}