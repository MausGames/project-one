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
// counter identifier
#define JUMP_SIDE       (0u)
#define ROTATION_STATUS (1u)
#define FLIP_SIDE       (2u)
#define CYCLE_COUNT     (3u)
#define OLD_DAMAGE      (4u)


// ****************************************************************
// vector identifier
#define FALL_BEHIND    (0u)
#define CONTAINER_DATA (1u)
#define ROTATION_ANGLE (2u)
#define OVERDRIVE_HIT  (3u)   // # uses 3u - 7u
#define SCATTER_FORCE  (3u)   // # uses 3u - 7u


// ****************************************************************
// constructor
cLeviathanBoss::cLeviathanBoss()noexcept
: m_Ray         (LEVIATHAN_RAYS)
, m_RayWave     (LEVIATHAN_RAYS)
, m_afRayTime   {}
, m_iDecalState (0u)
, m_fAnimation  (0.0f)
, m_fMovement   (-3.5f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize(coreVector3(0.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_BLUE);

    // 
    m_Head.DefineModelHigh("ship_boss_leviathan_head_high.md3");
    m_Head.DefineModelLow ("ship_boss_leviathan_head_low.md3");
    m_Head.SetSize        (coreVector3(1.5f,1.5f,1.5f));
    m_Head.Configure      (500, COLOR_SHIP_BLUE);
    m_Head.AddStatus      (ENEMY_STATUS_IMMORTAL);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS_BODIES; ++i)
    {
        m_aBody[i].DefineModelHigh("ship_boss_leviathan_body_high.md3");
        m_aBody[i].DefineModelLow ("ship_boss_leviathan_body_low.md3");
        m_aBody[i].SetSize        (coreVector3(1.7f,1.7f,1.7f));
        m_aBody[i].Configure      (500, COLOR_SHIP_BLUE);
        m_aBody[i].AddStatus      (ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_Tail.DefineModelHigh("ship_boss_leviathan_tail_high.md3");
    m_Tail.DefineModelLow ("ship_boss_leviathan_tail_low.md3");
    m_Tail.SetSize        (coreVector3(1.7f,1.7f,1.7f));
    m_Tail.Configure      (500, COLOR_SHIP_BLUE);
    m_Tail.AddStatus      (ENEMY_STATUS_IMMORTAL);

    // 
    m_Ray    .DefineProgram("effect_energy_invert_inst_program");
    m_RayWave.DefineProgram("effect_energy_direct_inst_program");
    {
        for(coreUintW i = 0u; i < LEVIATHAN_RAYS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pRay = &m_aRayRaw[i];
            pRay->DefineModel  ("object_tube_open.md3");
            pRay->DefineTexture(0u, "effect_energy.png");
            pRay->DefineProgram(iType ? "effect_energy_direct_program" : "effect_energy_invert_program");

            // set object properties
            pRay->SetTexSize(iType ? LEVIATHAN_RAYWAVE_TEXSIZE : LEVIATHAN_RAY_TEXSIZE);
            pRay->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_RayWave.BindObject(pRay);
                 else m_Ray    .BindObject(pRay);
        }
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (coreVector2(0.0f,0.0f));
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(false);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        pPart->Resurrect();
        pPart->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);   // # after resurrection
    }

    // 
    g_pGlow->BindList(&m_Ray);
    g_pGlow->BindList(&m_RayWave);
}


// ****************************************************************
// 
void cLeviathanBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
        this->__DisableRay(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        this->__GetPart(i)->Kill(bAnimated);

    // 
    g_pGlow->UnbindList(&m_Ray);
    g_pGlow->UnbindList(&m_RayWave);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cLeviathanBoss::__RenderOwnOver()
{
    if(m_Ray.GetCurEnabled())
    {
        DEPTH_PUSH

        // 
        m_Ray.Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ray);

        glDisable(GL_DEPTH_TEST);
        {
            // 
            m_RayWave.Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // 
    m_fAnimation.UpdateMod(-1.0f,   10.0f);
    m_fMovement .UpdateMod(0.5f*PI, 4.0f*PI);

    // 
    coreVector2 vNewOri    = coreVector2(0.0f,-1.0f);
    coreBool    bOverdrive = false;

    // ################################################################
    // ################################################################

    // 
    if(m_aiCounter[ROTATION_STATUS])
    {
        // 
        const coreVector2 vBaseDir = coreVector2::Direction(m_avVector[ROTATION_ANGLE].x).Rotated90();
        const coreVector3 vRotaDir = coreVector3(vBaseDir.x, -vNewOri.y*vBaseDir.y, vNewOri.x*vBaseDir.y);
        const coreVector4 vQuat    = coreMatrix4::Orientation(vRotaDir, coreVector3(0.0f, vNewOri)).m123().ToQuat();

        for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        {
            cEnemy* pPart = this->__GetPart(i);

            // 
            const coreVector3 vNewDir = vQuat.QuatApply(coreVector3(coreVector2::Direction(I_TO_F(i) * 0.4f*PI), 0.0f));
            const coreVector3 vNewPos = vNewDir * LEVIATHAN_RADIUS_INNER;

            // 
            pPart->SetPosition (m_aiCounter[FLIP_SIDE] ? vNewPos.InvertedX() : vNewPos);
            pPart->SetDirection(m_aiCounter[FLIP_SIDE] ? vNewDir.InvertedX() : vNewDir);
        }

        // 
        m_avVector[ROTATION_ANGLE].x += m_avVector[ROTATION_ANGLE].y * TIME;
    }

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
    {
        coreObject3D* pRay  = (*m_Ray    .List())[i];
        coreObject3D* pWave = (*m_RayWave.List())[i];
        if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat fOldTime = m_afRayTime[i];
        m_afRayTime[i].Update(0.8f);
        const coreFloat fNewTime = m_afRayTime[i];

        // 
        const cEnemy*     pPart = this->__GetPart(i);
        const coreVector3 vPos  = pPart->GetPosition();
        const coreVector3 vDir  = pPart->GetDirection();

        // 
        const coreVector3 vColor = LERP(coreMath::IsNear(vDir.z, 0.0f, LEVIATHAN_RAY_HEIGHT) ? (COLOR_ENERGY_YELLOW * 0.8f) : (COLOR_ENERGY_BLUE * (0.8f - 0.4f * ABS(vDir.z))), pRay->GetColor3(), FrictionFactor(18.0f));
        const coreFloat   fAlpha = (fNewTime < 1.0f) ? (0.6f * (1.0f - fNewTime)) : 1.0f;

        if(!bOverdrive)
        {
            // 
            const coreFloat   fLength = (fNewTime < 1.0f) ? 1.0f : (MIN((fNewTime - 1.0f) * 5.0f, 1.0f));
            const coreFloat   fWidth  = 2.0f - fLength;
            const coreVector3 vSize   = coreVector3(fWidth, fLength, fWidth);

            // 
            pRay ->SetSize(LEVIATHAN_RAY_SIZE     * vSize);
            pWave->SetSize(LEVIATHAN_RAYWAVE_SIZE * vSize);
        }

        // 
        pRay->SetPosition (vPos + vDir * (pRay->GetSize().y + LEVIATHAN_RAY_OFFSET(i)));
        pRay->SetDirection(vDir);
        pRay->SetColor3   (vColor);
        pRay->SetAlpha    (fAlpha);
        pRay->SetTexSize  (coreVector2(LEVIATHAN_RAY_TEXSIZE.x, LEVIATHAN_RAY_TEXSIZE.y * (pRay->GetSize().y * (1.0f/LEVIATHAN_RAY_SIZE.y))));
        pRay->SetTexOffset(coreVector2(0.4f,0.3f) * m_fAnimation);

        // 
        pWave->SetPosition (vPos + vDir * (pWave->GetSize().y  + LEVIATHAN_RAY_OFFSET(i)));
        pWave->SetDirection(-vDir);
        pWave->SetColor3   (vColor);
        pWave->SetAlpha    (fAlpha * 0.85f);
        pWave->SetTexOffset(coreVector2(-0.3f,-0.6f) * m_fAnimation);

        if((fOldTime < 1.0f) && (fNewTime >= 1.0f))
        {
            // 
            pRay->ChangeType(TYPE_LEVIATHAN_RAY);

            // 
            g_pSpecialEffects->MacroEruptionColorBig(vPos + vDir * LEVIATHAN_RAY_OFFSET(i), vDir.xy(), COLOR_ENERGY_YELLOW);
        }
    }

    // 
    m_Ray    .MoveNormal();
    m_RayWave.MoveNormal();

    if(m_Ray.GetCurEnabled())
    {
        PHASE_CONTROL_TICKER(3u, 0u, 30.0f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
            {
                const coreObject3D* pRay = (*m_Ray.List())[i];
                if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;
                if(!pRay->GetType())                         continue;

                // 
                const coreVector3 vPos = pRay->GetPosition();
                const coreVector3 vDir = pRay->GetDirection();
                const coreFloat   fLen = pRay->GetSize().y;

                // 
                const coreVector2 vTestPos = g_pForeground->Project2D(vPos + vDir * fLen);

                // 
                if((ABS(vTestPos.x) > 0.5f) ||
                   (ABS(vTestPos.y) > 0.5f))
                {
                    const coreVector2 vNorm      = vDir.xy().Normalized();
                    const coreVector2 vEffectPos = vNorm * RCP(vNorm.Processed(ABS).Max()) * (FOREGROUND_AREA * 1.1f);

                    // 
                    g_pSpecialEffects->CreateSplashFire (coreVector3(vEffectPos, 0.0f),  5.0f, 3u, COLOR_FIRE_ORANGE);
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), 25.0f, 2u, COLOR_FIRE_ORANGE);
                }
            }
        });

        // 
        cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_LEVIATHAN_RAY, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pRay, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            if(!coreMath::IsNear(pRay->GetDirection().z, 0.0f, LEVIATHAN_RAY_HEIGHT)) return;

            // 
            pPlayer->TakeDamage(5, ELEMENT_YELLOW, vIntersection.xy());

            // 
            g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_YELLOW);
        });
    }

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        const coreVector3 vPos    = pPart->GetPosition();
        const coreFloat   fHeight = g_pEnvironment->RetrieveSafeHeight(pPart->GetPosition().xy());

        // 
        const coreBool bOldEnabled =  pPart->IsEnabled(CORE_OBJECT_ENABLE_ALL);
        const coreBool bNewEnabled = (pPart->GetPosition().z > fHeight);

        // 
        pPart->SetEnabled(bNewEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        // 
        if(bOldEnabled != bNewEnabled)
        {
            const coreVector2 vProjectedPos = g_pForeground->Project3D(vPos);

            // 
            if(g_pForeground->IsVisiblePoint(vProjectedPos))
            {
                // 
                if(pPart == &m_Head)
                {
                    for(coreUintW j = 5u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f));

                        // 
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vProjectedPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vProjectedPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vProjectedPos, -vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vProjectedPos, -vDir)->ChangeSize(1.3f);
                    }

                    // 
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vProjectedPos, 0.0f), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_GREEN);
                }

                // 
                g_pSpecialEffects->CreateSplashSmoke(coreVector3(vPos.xy(), fHeight), 30.0f, 30u, coreVector3(1.0f,1.0f,1.0f));
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos.xy(), fHeight), 50.0f, 15u, COLOR_ENERGY_WHITE);
            }
        }

        // 
        if(bNewEnabled) pPart->DefaultAxiate(m_fMovement * ((i & 0x01u) ? 1.0f : -1.0f));
    }

    if(m_aiCounter[ROTATION_STATUS])
    {
        // 
        const coreVector2 vDiff     = pContainer->GetPosition().xy() - m_Head.GetPosition().xy();
        const coreVector2 vHeadMove = m_Head.GetMove();

        // 
        if(!vDiff.IsNull() && !vHeadMove.IsNull()) m_avVector[CONTAINER_DATA].y += 100.0f * coreVector2::Dot(vDiff.Normalized().Rotated90(), vHeadMove.Normalized()) * RCP(vDiff.LengthSq()) * TIME;
    }

    // 
    m_avVector[CONTAINER_DATA].z += m_avVector[CONTAINER_DATA].y * TIME;
    m_avVector[CONTAINER_DATA].y *= FrictionFactor(0.25f);
    pContainer->SetDirection(coreVector3(coreVector2::Direction(m_avVector[CONTAINER_DATA].z), 0.0f));

    // 
    this->__UpdateHealth();

    // 
    if(this->ReachedHealthPct(0.75f) || this->ReachedHealthPct(0.5f))
    {
        // 
        for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
            this->__DisableRay(i, true);

        // 
        m_aiCounter[ROTATION_STATUS] = 0;
        m_aiCounter[CYCLE_COUNT]    += 1;

        if(m_aiCounter[CYCLE_COUNT] == 1)
        {
            // 
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                m_avVector[SCATTER_FORCE + i].xy(coreVector2(1.0f,1.0f) * 30.0f);

            // 
            PHASE_CHANGE_TO(30u)
        }
        else if(m_aiCounter[CYCLE_COUNT] == 2)
        {
            // 
            m_avVector[SCATTER_FORCE].w      = ROUND(m_Head.GetPosition().xy().Normalized().Angle() / (0.4f*PI)) * (0.4f*PI);
            m_avVector[SCATTER_FORCE + 1u].w = SIGN(-vNewOri.y);

            // 
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                const cEnemy* pPart = this->__GetPart(i);

                m_avVector[SCATTER_FORCE + i].xy (pPart->GetPosition ().xy() / FOREGROUND_AREA);
                m_avVector[SCATTER_FORCE + i].z = pPart->GetDirection().xy().Angle();
            }

            // 
            PHASE_CHANGE_TO(40u)
        }
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__EnableRay(const coreUintW iIndex)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)
    coreObject3D* pRay  = (*m_Ray    .List())[iIndex];
    coreObject3D* pWave = (*m_RayWave.List())[iIndex];

    // 
    WARN_IF(pRay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afRayTime[iIndex] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetColor3 (COLOR_ENERGY_YELLOW * 0.8f);
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pRay);
    nInitFunc(pWave);

    // 
    const cEnemy*     pPart = this->__GetPart(iIndex);
    const coreVector3 vPos  = pPart->GetPosition();
    const coreVector3 vDir  = pPart->GetDirection();

    // 
    g_pSpecialEffects->CreateSplashColor(vPos + vDir * LEVIATHAN_RAY_OFFSET(iIndex), SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);
}


// ****************************************************************
// 
void cLeviathanBoss::__DisableRay(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)
    coreObject3D* pRay  = (*m_Ray    .List())[iIndex];
    coreObject3D* pWave = (*m_RayWave.List())[iIndex];

    // 
    if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pRay->ChangeType(0);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pRay);
    nExitFunc(pWave);

    if(bAnimated)
    {
        // 
        const cEnemy*     pPart = this->__GetPart(iIndex);
        const coreVector3 vPos  = pPart->GetPosition();
        const coreVector3 vDir  = pPart->GetDirection();

        // 
        for(coreUintW j = 25u; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (LEVIATHAN_RAY_OFFSET(iIndex) + 2.0f*I_TO_F(j)), 10.0f, 1u, COLOR_ENERGY_YELLOW);
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__CreateOverdrive(const coreUintW iIndex, const coreVector3 vIntersect, const coreFloat fTime, const coreBool bGround)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)

    // 
    constexpr coreFloat fMin = 2.5f;
    constexpr coreFloat fMax = 5.0f;
    coreVector3 vOldHit = m_avVector[OVERDRIVE_HIT + iIndex].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vIntersect;
    else
    {
    gtAgain:

        // 
        const coreVector3 vDiff = vIntersect - vOldHit;
        const coreFloat   fLen  = vDiff.Length();

        // 
        if(fLen > fMin)
        {
            // 
            const coreVector3 vNewHit      = (fLen > fMax) ? LERP(vOldHit, vIntersect, fMax*RCP(fLen)) : vIntersect;
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                STATIC_ASSERT(sizeof(m_iDecalState)*8u >= LEVIATHAN_RAYS*2u)

                // 
                if(HAS_BIT(m_iDecalState, iIndex * 2u)) TOGGLE_BIT(m_iDecalState, iIndex * 2u + 1u)
                TOGGLE_BIT(m_iDecalState, iIndex * 2u)

                // 
                const coreBool    bRotated   = HAS_BIT(m_iDecalState, iIndex * 2u);
                const coreBool    bFlipped   = HAS_BIT(m_iDecalState, iIndex * 2u + 1u);
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(Core::Rand->Float(5.0f, 6.5f), MIN(fLen, fMax)*1.8f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "effect_soot.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3((bRotated ? vDecalSize.yx()       : vDecalSize),                            1.0f));
                pObject->SetDirection(coreVector3((bRotated ? vDecalDir.Rotated90() : vDecalDir) * (bFlipped ? -1.0f : 1.0f), 0.0f));
                pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));

                // add object to background or windscreen
                if(bGround) g_pEnvironment->GetBackground()->AddDecal (pObject, vDecalPos,       128u, "effect_decal_single_inst_program", LIST_KEY);
                       else g_pWindscreen                  ->AddObject(pObject, vDecalPos, 3.0f, 128u, "effect_decal_single_inst_program", LIST_KEY);

                // 
                g_pSpecialEffects->CreateSplashFire (vNewHit,  5.0f, bGround ? 3u : 6u, COLOR_FIRE_ORANGE);
                g_pSpecialEffects->CreateSplashColor(vNewHit, 25.0f, bGround ? 2u : 4u, COLOR_FIRE_ORANGE);
            }

            // 
            g_pSpecialEffects->ShakeScreen(0.1f + 0.55f * SIN(PI * fTime));

            // 
            vOldHit = vNewHit;
            goto gtAgain;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[OVERDRIVE_HIT + iIndex].xyz(vOldHit);
}


// ****************************************************************
// 
void cLeviathanBoss::__UpdateHealth()
{
    coreInt32 iNewDamage = 0;

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        iNewDamage += pPart->GetMaxHealth() - pPart->GetCurHealth();

        if(pPart->ReachedDeath())
        {
            // 
            pPart->DefineTexture(0u, "default_black.png");
            pPart->AddStatus(ENEMY_STATUS_INVINCIBLE);

            // 
            g_pSpecialEffects->MacroExplosionDarkSmall(pPart->GetPosition());
        }
    }

    // 
    this->SetCurHealth(this->GetMaxHealth() - m_aiCounter[OLD_DAMAGE] - iNewDamage);
}


// ****************************************************************
// 
void cLeviathanBoss::__RefreshHealth()
{
    // 
    m_aiCounter[OLD_DAMAGE] = this->GetMaxHealth() - this->GetCurHealth();

    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        pPart->DefineTexture(0u, "ship_enemy.png");
        pPart->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

        // 
        ASSERT(pPart->GetCurHealthPct() < 1.0f)
        pPart->SetCurHealth(pPart->GetMaxHealth());
        pPart->RefreshColor(1.0f);

        // 
        if(pPart->IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            g_pSpecialEffects->CreateSplashSmoke(pPart->GetPosition(), 30.0f, 30u, coreVector3(1.0f,1.0f,1.0f));
            g_pSpecialEffects->CreateSplashColor(pPart->GetPosition(), 50.0f, 15u, COLOR_ENERGY_WHITE);
        }
    }
}


// ****************************************************************
// 
FUNC_NOALIAS void cLeviathanBoss::__CalcCurvePosDir(const coreVector3 vAxis, const coreFloat fAngle, const coreVector3 vScale, coreVector3* OUTPUT vPosition, coreVector3* OUTPUT vDirection)
{
    ASSERT(vAxis.IsNormalized() && vPosition && vDirection)

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(fAngle, vAxis).m123();
    const coreVector3 vDir  = vAxis.xy().IsNull() ? coreVector3(1.0f,0.0f,0.0f) : coreVector3(vAxis.xy().Normalized().Rotated90(), 0.0f);
    const coreVector3 vPos  = (vDir * mRota).Normalized();

    // 
    (*vPosition)  = vPos * vScale;
    (*vDirection) = coreVector3::Cross(vAxis, (vPos / vScale)).Normalized();
}


// ****************************************************************
// 
FUNC_CONST coreFloat cLeviathanBoss::__CalcAngle(const coreFloat fDistance, const coreFloat fRadius)
{
    return (fDistance * RCP(fRadius) * (2.0f*PI));
}


// ****************************************************************
// 
cEnemy* cLeviathanBoss::__GetPart(const coreUintW iIndex)
{
    ASSERT(iIndex < LEVIATHAN_PARTS)

    // 
    if(iIndex == 0u) return &m_Head;
    if(iIndex == 4u) return &m_Tail;
                     return &m_aBody[iIndex - 1u];
}


// ****************************************************************
// 
FUNC_CONST coreFloat cLeviathanBoss::__GetPartDistance(const coreUintW iIndex)
{
    return (I_TO_F(iIndex) * 0.17f + (iIndex ? 0.02f : 0.0f));
}