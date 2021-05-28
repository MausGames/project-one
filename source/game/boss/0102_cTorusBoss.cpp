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
#define ROTATION_DIRECTION (0u)
#define EMIT_STATUS        (1u)
#define BALL_STATUS        (2u)
#define SKIP_SUMMON        (3u)
#define SUB_PHASE          (4u)


// ****************************************************************
// vector identifier
#define TUMBLE_DIRECTION (0u)


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_TurretHull      (TORUS_TURRETS)
, m_GunnerHull      (TORUS_GUNNERS)
, m_fAnimation      (0.0f)
, m_fRotationBoss   (0.0f)
, m_fRotationObject (0.0f)
, m_iTurretActive   (0u)
, m_iGunnerActive   (0u)
, m_iGunnerMove     (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.7f,1.7f,1.7f));

    // configure the boss
    this->Configure(4000, COLOR_SHIP_GREY);

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
        m_aCircle[i].SetColor3    (COLOR_ENERGY_CYAN * 0.8f);
        m_aCircle[i].SetTexSize   (coreVector2(1.25f,1.25f));
    }

    // 
    m_Summon.DefineModel  ("object_sphere.md3");
    m_Summon.DefineTexture(0u, "effect_energy.png");
    m_Summon.DefineProgram("effect_energy_invert_program");
    m_Summon.SetTexSize   (coreVector2(4.5f,4.5f));
    m_Summon.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
    {
        m_aTurret[i].DefineModelHigh("object_cube_rota.md3");
        m_aTurret[i].DefineModelLow ("object_cube_rota.md3");
        m_aTurret[i].DefineTexture  (0u, "effect_energy.png");
        m_aTurret[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aTurret[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.85f);
        m_aTurret[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aTurret[i].Configure      (100, COLOR_ENERGY_CYAN * 0.7f);
        m_aTurret[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_TurretHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            // load object resources
            coreObject3D* pTurret = &m_aTurretHullRaw[i];
            pTurret->DefineModel  ("object_cube_rota.md3");
            pTurret->DefineTexture(0u, "effect_energy.png");
            pTurret->DefineProgram("effect_energy_invert_program");

            // set object properties
            pTurret->SetSize   (m_aTurret[i].GetSize   ());
            pTurret->SetColor3 (m_aTurret[i].GetColor3 ());
            pTurret->SetTexSize(m_aTurret[i].GetTexSize());
            pTurret->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_TurretHull.BindObject(pTurret);
        }
    }

    // 
    for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
    {
        m_aGunner[i].DefineModelHigh("object_tetra_rota.md3");
        m_aGunner[i].DefineModelLow ("object_tetra_rota.md3");
        m_aGunner[i].DefineTexture  (0u, "effect_energy.png");
        m_aGunner[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aGunner[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
        m_aGunner[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aGunner[i].Configure      (100, COLOR_ENERGY_RED * 0.8f);
        m_aGunner[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_GunnerHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            // load object resources
            coreObject3D* pGunner = &m_aGunnerHullRaw[i];
            pGunner->DefineModel  ("object_tetra_rota.md3");
            pGunner->DefineTexture(0u, "effect_energy.png");
            pGunner->DefineProgram("effect_energy_invert_program");

            // set object properties
            pGunner->SetSize   (m_aGunner[i].GetSize   ());
            pGunner->SetColor3 (m_aGunner[i].GetColor3 ());
            pGunner->SetTexSize(m_aGunner[i].GetTexSize());
            pGunner->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_GunnerHull.BindObject(pGunner);
        }
    }

    STATIC_ASSERT(sizeof(m_iTurretActive)*8u >= TORUS_TURRETS)
    STATIC_ASSERT(sizeof(m_iGunnerActive)*8u >= TORUS_GUNNERS)
    STATIC_ASSERT(sizeof(m_iGunnerMove)  *8u >= TORUS_GUNNERS)
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
    g_pGlow->BindList(&m_TurretHull);
    g_pGlow->BindList(&m_GunnerHull);
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
    this->__DisableSummon();

    // 
    for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        this->__DisableTurret(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        this->__DisableGunner(i, bAnimated);

    // 
    g_pGlow->UnbindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);

    // 
    g_pGlow->UnbindList(&m_TurretHull);
    g_pGlow->UnbindList(&m_GunnerHull);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnUnder()
{
    if(m_iTurretActive || m_iGunnerActive)
    {
        DEPTH_PUSH

        glDepthMask(false);
        {
            // 
            m_TurretHull.Render();
            m_GunnerHull.Render();
        }
        glDepthMask(true);
    }
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnOver()
{
    DEPTH_PUSH_SHIP

    // 
    m_Emitter.Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        m_aCircle[i].Render();

    // 
    m_Summon.Render();
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    coreVector2 vNewOri = coreVector2(0.0f, m_aiCounter[ROTATION_DIRECTION] ? -1.0f : 1.0f);

    // 
    m_fAnimation   .UpdateMod(-1.0f,               10.0f);
    m_fRotationBoss.UpdateMod(TORUS_BOSS_ROTATION, 2.0f*PI);

    // ################################################################
    // ################################################################

    // 
    const coreVector2 vNewDir = coreVector2::Direction(m_fRotationBoss);
    this->SetDirection  (coreVector3(vNewDir.x, -vNewOri.y*vNewDir.y, vNewOri.x*vNewDir.y));
    this->SetOrientation(coreVector3(0.0f,       vNewOri.x,           vNewOri.y));

    if(m_iTurretActive || m_iGunnerActive)
    {
        // 
        m_fRotationObject.UpdateMod(TORUS_TURRET_SPEED, 10.0f);

        // 
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(m_fRotationObject * PI), 0.0f);
        const coreVector3 vOri  = coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y);
        const coreVector2 vTex  = coreVector2(0.2f,1.0f) * m_fRotationObject;
        const coreFloat   fTime = FMOD(m_fRotationObject * -5.0f, 2.0f);

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            cCustomEnemy* pTurret = &m_aTurret       [i];
            coreObject3D* pHull   = &m_aTurretHullRaw[i];
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

            // 
            if(pTurret->ReachedDeath()) this->__DisableTurret(i, true);
        }

        // 
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            cCustomEnemy* pGunner = &m_aGunner       [i];
            coreObject3D* pHull   = &m_aGunnerHullRaw[i];
            if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreVector3 vSize = pGunner->GetSize();

            // 
            const coreFloat   fMoveSide = HAS_BIT(m_iGunnerMove, i) ? -1.0f : 1.0f;
            const coreVector2 vMoveDir  = coreVector2(fMoveSide * FOREGROUND_AREA.x * TORUS_GUNNER_SPEED * TIME, 0.0f);
            const coreVector2 vNewPos   = pGunner->GetPosition().xy() + ((i & 0x02u) ? vMoveDir.yx() : vMoveDir);

            // 
            const auto nBounceEffect = [](const coreVector2& vEffectPos)
            {
                g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound        (coreVector3(vEffectPos, 0.0f), 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
            };

            // 
                 if((vNewPos.x < -FOREGROUND_AREA.x) && (fMoveSide < 0.0f)) {nBounceEffect(vNewPos + coreVector2(-vSize.x, 0.0f)); REMOVE_BIT(m_iGunnerMove, i)}
            else if((vNewPos.x >  FOREGROUND_AREA.x) && (fMoveSide > 0.0f)) {nBounceEffect(vNewPos + coreVector2( vSize.x, 0.0f)); ADD_BIT   (m_iGunnerMove, i)}
                 if((vNewPos.y < -FOREGROUND_AREA.y) && (fMoveSide < 0.0f)) {nBounceEffect(vNewPos + coreVector2(0.0f, -vSize.y)); REMOVE_BIT(m_iGunnerMove, i)}
            else if((vNewPos.y >  FOREGROUND_AREA.y) && (fMoveSide > 0.0f)) {nBounceEffect(vNewPos + coreVector2(0.0f,  vSize.y)); ADD_BIT   (m_iGunnerMove, i)}

            // 
            pGunner->SetPosition   (coreVector3(vNewPos, 0.0f));
            pGunner->SetDirection  (vDir);
            pGunner->SetOrientation(vOri);
            pGunner->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pGunner->GetPosition   ());
            pHull->SetSize       (pGunner->GetSize       () * 1.2f);
            pHull->SetDirection  (pGunner->GetDirection  ().InvertedX());
            pHull->SetOrientation(pGunner->GetOrientation());
            pHull->SetAlpha      (pGunner->GetAlpha      () * 0.5f);
            pHull->SetTexOffset  (pGunner->GetTexOffset  ());

            // 
            if(pGunner->ReachedDeath()) this->__DisableGunner(i, true);
        }

        // 
        m_TurretHull.MoveNormal();
        m_GunnerHull.MoveNormal();

        // 
        PHASE_CONTROL_TICKER(2u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            {
                cCustomEnemy* pTurret = &m_aTurret[i];
                if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pTurret->GetPosition ().xy();
                const coreVector2 vDir = pTurret->GetDirection().xy();

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
            }
        });

        // 
        PHASE_CONTROL_TICKER(3u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            {
                cCustomEnemy* pGunner = &m_aGunner[i];
                if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pGunner->GetPosition().xy();
                const coreVector2 vDir = (i & 0x02u) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.15f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_RED);
            }
        });
    }

    // constantly shoot into three directions
    PHASE_CONTROL_TICKER(4u, 0u, TORUS_BOSS_ROTATION * (2.0f / PI), LERP_LINEAR)
    {
        // 
        if(!m_aiCounter[EMIT_STATUS])
        {
            m_aiCounter[EMIT_STATUS] = 1;
            return;
        }

        // 
        const coreMatrix2 vMain = coreMatrix3::Rotation(DEG_TO_RAD(270.0f + I_TO_F(iTick & 0x03u) * (m_aiCounter[ROTATION_DIRECTION] ? -90.0f : 90.0f))).m12();
        const coreMatrix2 vSub  = coreMatrix3::Rotation(DEG_TO_RAD(120.0f)).m12();

        for(coreUintW i = 5u; i--; )
        {
            coreVector2 avDir[3];

            // 
            avDir[0] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i-2u) * 7.0f)) * vMain;
            avDir[1] = avDir[0] * vSub;
            avDir[2] = avDir[1] * vSub;

            for(coreUintW j = 0u; j < 3u; ++j)
            {
                const coreVector2 vDir = avDir[j];
                const coreVector2 vPos = this->GetPosition().xy() + vDir * 8.0f;

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.2f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_YELLOW);
            }
        }
    });

    // 
    if(!coreMath::IsNear(ABS(vNewOri.y), 1.0f))
        m_aiCounter[EMIT_STATUS] = 0;

    // 
    m_Emitter.SetPosition   (this->GetPosition   ());
    m_Emitter.SetDirection  (this->GetDirection  ());
    m_Emitter.SetOrientation(this->GetOrientation());
    m_Emitter.SetAlpha      (MAX(m_aTimer[4].GetValue(CORE_TIMER_GET_NORMAL) * 2.0f - 1.0f, 0.0f));
    m_Emitter.SetTexOffset  (coreVector2(-0.1f,-0.4f) * m_fAnimation);
    m_Emitter.SetEnabled    (m_aiCounter[EMIT_STATUS] ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
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

    // 
    m_Summon.SetTexOffset(coreVector2(0.0f, 0.3f * m_fAnimation));
    m_Summon.Move();

    // 
    if(this->ReachedHealthPct(0.7f) || this->ReachedHealthPct(0.4f))
    {
        // 
        m_aiCounter[SUB_PHASE] += 1;

        // 
        this->__DisableSummon();

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            this->__DisableTurret(i, true);

        // 
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            this->__DisableGunner(i, true);

        // 
        const coreVector2 vMove = this->GetMove();
        m_avVector[TUMBLE_DIRECTION].xy(vMove.IsNull() ? vMove : vMove.Normalized());
        m_avVector[TUMBLE_DIRECTION].z = g_pEnvironment->GetDirection().Angle();

        // 
        PHASE_CHANGE_TO(70u)
        PHASE_RESET(0u)
        PHASE_RESET(1u)
        PHASE_RESET(2u)
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableSummon(const coreVector2& vPosition, const coreVector3& vColor)
{
    WARN_IF(m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Summon.SetPosition(coreVector3(vPosition,0.0f));
    m_Summon.SetSize    (coreVector3(0.0f,0.0f,0.0f));
    m_Summon.SetColor3  (vColor);

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Summon);
}


// ****************************************************************
// 
void cTorusBoss::__DisableSummon()
{
    if(!m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Summon);
}


// ****************************************************************
// 
void cTorusBoss::__EnableTurret(const coreUintW iIndex, const coreVector2& vPosition)
{
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(!pTurret->HasStatus(ENEMY_STATUS_DEAD)) return;
    pTurret->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iTurretActive, iIndex))
    ADD_BIT(m_iTurretActive, iIndex)

    // 
    pTurret->SetPosition (coreVector3(vPosition, 0.0f));
    pTurret->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cTorusBoss::__DisableTurret(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(pTurret->HasStatus(ENEMY_STATUS_DEAD)) return;
    pTurret->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iTurretActive, iIndex))
    REMOVE_BIT(m_iTurretActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pTurret->GetPosition(), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cTorusBoss::__EnableGunner(const coreUintW iIndex, const coreVector2& vPosition)
{
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(!pGunner->HasStatus(ENEMY_STATUS_DEAD)) return;
    pGunner->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iGunnerActive, iIndex))
    ADD_BIT(m_iGunnerActive, iIndex)

    // 
    SET_BIT(m_iGunnerMove, iIndex, (iIndex < 2u))

    // 
    pGunner->SetPosition (coreVector3(vPosition, 0.0f));
    pGunner->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cTorusBoss::__DisableGunner(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(pGunner->HasStatus(ENEMY_STATUS_DEAD)) return;
    pGunner->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iGunnerActive, iIndex))
    REMOVE_BIT(m_iGunnerActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pGunner->GetPosition(), COLOR_ENERGY_RED);
}