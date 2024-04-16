///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// in finaler phase, erster durchgängiger strahl sollte in die spawn-zone der residue geschosse reingehn, damit man den angriff mit weniger druck sieht
// geschosse sollten nicht starten (oder weitergehen)( während boss noch zu tief ist, weil sie sonst in der luft starten
// angriffe oder ähnliches sollten verhindern, dass der spieler in die mitte geht bei den kreis-förmigen phasen
// parts die sich in der pfeil-phase bewegen haben sehr wobbly gewirkt, statische position wirkt interessanter (vom problem-lösungs-aspekt her)
// in finaler phase, die ganze zeit rundum-laser war voll nervig, aber is trotzdem interessant den bildschirm leicht zu verdecken während man durch geschosse schwimmt
// gelbe und blaue farbe des lasers ist inspiriert vom organischen gegner in Gradius, auch der rundum-laser in finaler phase
// (zwischen ende von grow-bullet abschnitt und 5-laser angriff braucht es ne pause um die beiden angriffe mental zu trennen)
// ACHIEVEMENT: destroy the same player-ship two times by the same laser
// TODO 1: hard-mode: alle laser-einschläge erzeugen zusätzliche geschosse (betrifft alle phasen außer grow-bullet und arrow)
// TODO 1: quad-bullet teppich verdeckt laser-einschlag

// NOTE: never ever use exactly 4 rays with equal distribution, the resulting laser+fire pattern does NOT look good


// ****************************************************************
// counter identifier
#define JUMP_SIDE       (0u)
#define ROTATION_STATUS (1u)
#define FLIP_SIDE       (2u)
#define CYCLE_COUNT     (3u)
#define OLD_DAMAGE      (4u)
#define EMERGE_TARGET   (5u)
#define EMERGE_PART     (6u)
#define ARROW_ACTIVE    (7u)
#define SPIN_PART       (8u)
#define SMOOTH_STATE    (9u)
#define WAVE_SHOTS      (10u)


// ****************************************************************
// vector identifier
#define FALL_BEHIND    (0u)
#define ROTATION_ANGLE (1u)
#define OVERDRIVE_HIT  (2u)   // # uses 2u - 6u
#define SCATTER_FORCE  (2u)   // # uses 2u - 6u
#define SMOOTH_MOVE    (7u)
#define CURTAIN_VALUE  (8u)
#define ENV_ROTATION   (9u)
#define RAY_HIT        (10u)


// ****************************************************************
// constructor
cLeviathanBoss::cLeviathanBoss()noexcept
: m_Ray         (LEVIATHAN_RAYS)
, m_RayWave     (LEVIATHAN_RAYS)
, m_afRayTime   {}
, m_afRayLen    {}
, m_iRayState   (0u)
, m_Mark        (LEVIATHAN_MARKS)
, m_iDecalState (0u)
, m_avSwimDir   {}
, m_aiSwimCount {}
, m_fAnimation  (0.0f)
, m_fMovement   (-3.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize             (coreVector3(0.0f,0.0f,0.0f));
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);

    // configure the boss
    this->Configure(9700, COLOR_SHIP_CYAN);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    PHASE_HEALTH_GOAL({9700, 8000, 5000, 3200, /*2400,*/ 1700, 0})

    // 
    m_Head.DefineModelHigh     ("ship_boss_leviathan_head_high.md3");
    m_Head.DefineModelLow      ("ship_boss_leviathan_head_low.md3");
    m_Head.DefineVolume        ("ship_boss_leviathan_head_volume.md3");
    m_Head.SetSize             (coreVector3(1.5f,1.5f,1.5f) * 1.3f);
    m_Head.SetCollisionModifier(this->GetCollisionModifier());
    m_Head.Configure           (500, COLOR_SHIP_CYAN);
    m_Head.AddStatus           (ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_SECRET | ENEMY_STATUS_CHAIN | ENEMY_STATUS_DEACTIVATE);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS_BODIES; ++i)
    {
        m_aBody[i].DefineModelHigh     ("ship_boss_leviathan_body_high.md3");
        m_aBody[i].DefineModelLow      ("ship_boss_leviathan_body_low.md3");
        m_aBody[i].DefineVolume        ("ship_boss_leviathan_body_volume.md3");
        m_aBody[i].SetSize             (coreVector3(1.7f,1.7f,1.7f) * 1.3f);
        m_aBody[i].SetCollisionModifier(this->GetCollisionModifier());
        m_aBody[i].Configure           (300, COLOR_SHIP_CYAN);
        m_aBody[i].AddStatus           (ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_SECRET | ENEMY_STATUS_CHAIN | ENEMY_STATUS_DEACTIVATE);
    }

    // 
    m_Tail.DefineModelHigh     ("ship_boss_leviathan_tail_high.md3");
    m_Tail.DefineModelLow      ("ship_boss_leviathan_tail_low.md3");
    m_Tail.DefineVolume        ("ship_boss_leviathan_tail_volume.md3");
    m_Tail.SetSize             (coreVector3(1.7f,1.7f,1.7f) * 1.3f);
    m_Tail.SetCollisionModifier(this->GetCollisionModifier());
    m_Tail.Configure           (300, COLOR_SHIP_CYAN);
    m_Tail.AddStatus           (ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_SECRET | ENEMY_STATUS_CHAIN | ENEMY_STATUS_DEACTIVATE);

    // 
    m_Ray    .DefineProgram("effect_energy_inst_program");
    m_RayWave.DefineProgram("effect_energy_direct_inst_program");
    {
        for(coreUintW i = 0u; i < LEVIATHAN_RAYS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pRay = &m_aRayRaw[i];
            pRay->DefineModel  ("object_tube.md3");
            pRay->DefineTexture(0u, "effect_energy.png");
            pRay->DefineProgram(iType ? "effect_energy_direct_program" : "effect_energy_program");

            // set object properties
            pRay->SetTexSize(iType ? LEVIATHAN_RAYWAVE_TEXSIZE : LEVIATHAN_RAY_TEXSIZE);
            pRay->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_RayWave.BindObject(pRay);
                 else m_Ray    .BindObject(pRay);
        }
    }

    // 
    m_Mark.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < LEVIATHAN_MARKS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pMark = &m_aMarkRaw[i];
            pMark->DefineModel  ("object_cube_top.md3");
            pMark->DefineTexture(0u, "effect_energy.png");
            pMark->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pMark->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 2.7f);
            pMark->SetColor3 (COLOR_ENERGY_WHITE * 0.6f);
            pMark->SetTexSize(coreVector2(1.0f,1.0f) * 0.4f);
            pMark->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Mark.BindObject(pMark);
        }
    }

    // 
    m_pLaserSound  = Core::Manager::Resource->Get<coreSound>("effect_laser.wav");
    m_pRumbleSound = Core::Manager::Resource->Get<coreSound>("effect_rumble.wav");
}


// ****************************************************************
// destructor
cLeviathanBoss::~cLeviathanBoss()
{
    // 
    this->Kill(false);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_RAYS;  ++i) this->__DisableRay (i, false);
    for(coreUintW i = 0u; i < LEVIATHAN_MARKS; ++i) this->__DisableMark(i, false);
}


// ****************************************************************
// 
void cLeviathanBoss::ResurrectDemo()
{
    // 
    m_bForeshadow = true;

    // 
    m_iPhase = 200u;
    this->Resurrect();
}


// ****************************************************************
// 
void cLeviathanBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        pPart->AddStatus(ENEMY_STATUS_HIDDEN);
        pPart->Resurrect();
    }

    // 
    g_pGlow->BindList(&m_Ray);
    g_pGlow->BindList(&m_RayWave);
    g_pGlow->BindList(&m_Mark);

    if(m_iPhase < 200u)
    {
        // 
        this->_ResurrectBoss();
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__KillOwn(const coreBool bAnimated)
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u; i < NEVO_TILES;  ++i) pMission->DisableTile (i, bAnimated);
    for(coreUintW i = 0u; i < NEVO_ARROWS; ++i) pMission->DisableArrow(i, bAnimated);

    // 
    pMission->ResetCollEnemyBullet();
    pMission->ResetCollBulletBullet();

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_RAYS;  ++i) this->__DisableRay (i, bAnimated);
    for(coreUintW i = 0u; i < LEVIATHAN_MARKS; ++i) this->__DisableMark(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        this->__GetPart(i)->Kill(bAnimated);

    // 
    g_pGlow->UnbindList(&m_Ray);
    g_pGlow->UnbindList(&m_RayWave);
    g_pGlow->UnbindList(&m_Mark);

    // 
    if(m_pLaserSound ->EnableRef(this)) m_pLaserSound ->Stop();
    if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();

    // 
    m_iPhase = 0u;
}


// ****************************************************************
// 
void cLeviathanBoss::__RenderOwnBottom()
{
    if(m_Ray.GetNumEnabled())
    {
        DEPTH_PUSH

        FOR_EACH(it, *m_Ray.List()) if((*it)->GetDirection().z < -0.1f) (*it)->Render();
        FOR_EACH(it, *m_Ray.List()) if((*it)->GetDirection().z < -0.1f) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(*it);
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__RenderOwnUnder()
{
    if(m_Ray.GetNumEnabled())
    {
        DEPTH_PUSH

        if(std::all_of(m_Ray.List()->begin(), m_Ray.List()->end(), [](const coreObject3D* pObject) {return InBetween(pObject->GetDirection().z, -0.1f, 0.1f);}))
        {
            m_Ray.Render();
            g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ray);
        }
        else
        {
            FOR_EACH(it, *m_Ray.List()) if(InBetween((*it)->GetDirection().z, -0.1f, 0.1f)) (*it)->Render();
            FOR_EACH(it, *m_Ray.List()) if(InBetween((*it)->GetDirection().z, -0.1f, 0.1f)) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(*it);
        }
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__RenderOwnOver()
{
    if(m_Ray.GetNumEnabled())
    {
        DEPTH_PUSH_SHIP

        FOR_EACH(it, *m_Ray.List()) if((*it)->GetDirection().z >= 0.1f) (*it)->Render();
        FOR_EACH(it, *m_Ray.List()) if((*it)->GetDirection().z >= 0.1f) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(*it);

        // 
        m_RayWave.Render();
    }

    DEPTH_PUSH

    // 
    m_Mark.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Mark);
}


// ****************************************************************
// 
void cLeviathanBoss::__MoveOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(-1.0f,   10.0f);
    m_fMovement .UpdateMod(0.5f*PI, 4.0f*PI);

    // 
    coreVector2 vNewOri    = coreVector2(0.0f,-1.0f);
    
    coreFloat fCenterOffset = 0.0f;
    coreFloat afCenterSpin[LEVIATHAN_PARTS] = {};

    coreFloat fRayTime = 0.0f;

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        const coreFloat fHeadHeight = g_pEnvironment->RetrieveSafeHeight(m_Head.GetPosition().xy());
        if(false && m_Head.GetPosition().z < fHeadHeight)
        {
            // below
            m_avVector[FALL_BEHIND].x -= TIME * g_pEnvironment->GetSpeed() * MIN(OUTDOOR_DETAIL, (fHeadHeight - m_Head.GetPosition().z) * 0.5f);
        }
        else
        {
            // above
            m_avVector[FALL_BEHIND].x = 0.0f;
        }

        for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        {
            cEnemy* pPart = this->__GetPart(i);

            const coreFloat fAngle = m_fMovement - cLeviathanBoss::__GetPartDistance(i) * (0.4f*PI);

            coreVector3 vPos, vDir;
            cLeviathanBoss::__CalcCurvePosDir(coreVector3(-1.0f, (m_aiCounter[JUMP_SIDE] & 0x01u) ? -0.5f : 0.5f, 0.0f).Normalized(), fAngle, coreVector3(50.0f,50.0f,35.0f), &vPos, &vDir);

            vPos += coreVector3(0.0f, m_avVector[FALL_BEHIND].x - 20.0f, -20.0f);

            pPart->SetPosition (vPos);
            pPart->SetDirection(vDir);
        }

        if(m_fMovement > 0.0f)
        {
            const coreFloat fTailHeight = g_pEnvironment->RetrieveSafeHeight(m_Tail.GetPosition().xy());
            if(m_Tail.GetPosition().z < fTailHeight)
            {
                m_fMovement = -1.0f*PI;

                if(++m_aiCounter[JUMP_SIDE] == 2)
                    PHASE_CHANGE_TO(2u)   // # 1 is skipped
            }
        }
    }

    // ################################################################
    // 
    else if((m_iPhase == 1u) || (m_iPhase == 2u) || (m_iPhase == 3u))
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                const coreFloat fAngle = (2.0f*PI) * fTime - cLeviathanBoss::__GetPartDistance(i) * (0.5f*PI) + (0.6f*PI);

                coreVector3 vPos, vDir;
                cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,1.0f,0.0f), fAngle, coreVector3(FOREGROUND_AREA * 0.8f, 20.0f), &vPos, &vDir);

                vPos += coreVector3(0.0f,0.0f,-20.0f);

                if(m_iPhase == 2u)
                {
                    vPos = vPos.InvertedX();
                    vDir = vDir.InvertedX();

                    vPos += coreVector3(0.0f, FOREGROUND_AREA.y * 0.8f, 0.0f);
                }
                else if(m_iPhase == 3u)
                {
                    vPos += coreVector3(0.0f, FOREGROUND_AREA.y * -0.8f, 0.0f);
                }

                pPart->SetPosition (vPos);
                pPart->SetDirection(vDir);
            }

            if(m_iPhase == 2u)
            {
                if(PHASE_TIME_POINT(0.1f))
                    this->_StartBoss();
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_AGAIN(0u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        const coreFloat fSpeedClose = (m_aiCounter[CYCLE_COUNT] ? 1.1f : 1.0f);
        const coreFloat fSpeedTime  = 0.05f / fSpeedClose;
        const coreFloat fSpeedAngle = 25.0f * fSpeedClose * (g_pGame->IsEasy() ? 0.65f : 1.0f);
        const coreFloat fTurn       = 0.7f;

        PHASE_CONTROL_TIMER(0u, fSpeedTime, LERP_LINEAR)
        {
            const coreFloat fCloseTime = MIN1(fTime * fSpeedClose);
            const coreFloat fTurnTime  = STEPS(fTurn, 1.0f, fTime);
            const coreFloat fBaseAngle = fTime * fSpeedAngle;
            const coreFloat fRadius    = LERPS(LEVIATHAN_RADIUS_OUTER, m_aiCounter[CYCLE_COUNT] ? LEVIATHAN_RADIUS_INNER_2 : LEVIATHAN_RADIUS_INNER_1, fCloseTime);

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                const coreFloat fOffset = cLeviathanBoss::__CalcAngle(cLeviathanBoss::__GetPartDistance(i) * 7.65f, fRadius);
                const coreFloat fAngle  = fBaseAngle * LERPS(0.5f, 1.0f, fCloseTime) - fOffset * (0.4f*PI);

                coreVector3 vPos, vDir;
                if(fAngle < 0.5f*PI)
                {
                    cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,-1.0f,0.0f), fAngle + (1.0f*PI), coreVector3(LEVIATHAN_RADIUS_OUTER, fRadius, 20.0f), &vPos, &vDir);

                    vPos += coreVector3(0.0f, fRadius, -20.0f);
                }
                else
                {
                    cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,0.0f,1.0f), fAngle, coreVector3(fRadius, fRadius, 1.0f), &vPos, &vDir);
                }

                if((fTime >= fTurn) && m_aiCounter[CYCLE_COUNT])
                {
                    const coreVector3 vStarDir = coreVector3(coreVector2::Direction(fBaseAngle - I_TO_F(i) * (0.4f*PI)).Rotated90(), 0.0f);
                    const coreVector3 vNewPos  = LERPS(vPos, vStarDir * fRadius, fTurnTime);
                    const coreVector3 vNewDir  = LERPS(vDir, vStarDir,           fTurnTime).Normalized();

                    pPart->SetPosition (vNewPos);
                    pPart->SetDirection(vNewDir);
                }
                else
                {
                    pPart->SetPosition (vPos);
                    pPart->SetDirection(vDir);
                }

                if(m_aiCounter[FLIP_SIDE])
                {
                    pPart->SetPosition (pPart->GetPosition ().InvertedX());
                    pPart->SetDirection(pPart->GetDirection().InvertedX());
                }
            }

            if(m_aiCounter[CYCLE_COUNT] && (m_Head.GetPosition().z >= -10.0f))
            {
                if(PHASE_TIME_POINT(0.2f))
                {
                    this->__EnableRay(0u, true);
                }

                if(PHASE_TIME_POINT(0.4f))
                {
                    for(coreUintW i = 1u; i < LEVIATHAN_RAYS; ++i)
                        this->__EnableRay(i, true);
                }

                if(PHASE_TIME_POINT(0.7f))
                {
                    if(this->_ResurrectHelper(ELEMENT_BLUE, true))
                    {
                        g_pGame->GetHelper(ELEMENT_BLUE)->SetPosition(coreVector3(0.0f,0.0f,0.0f));
                    }
                }
            }
            else
            {
                if(PHASE_TIME_POINT(0.2f))
                {
                    this->__EnableRay(0u, true);
                }

                if(PHASE_TIME_POINT(0.25f))
                {
                    this->_ResurrectHelper(ELEMENT_ORANGE, true);
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(m_aiCounter[CYCLE_COUNT] ? 23u : 22u)

                m_aiCounter[ROTATION_STATUS]  = 1;
                m_avVector [ROTATION_ANGLE].x = fBaseAngle;
                m_avVector [ROTATION_ANGLE].y = fSpeedTime * fSpeedAngle;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        // not used
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        // not used
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        coreBool bComplete1 = true;

        static coreBool   bComplete2 = false;
        static coreUint32 iTileState = 0u;
        static coreUint32 iTileScore = 0u;
        static coreUint32 aiRemember[GAME_PLAYERS] = {};

        STATIC_ASSERT(LEVIATHAN_TILES <= sizeof(iTileState)   *8u)
        STATIC_ASSERT(LEVIATHAN_TILES <= sizeof(aiRemember[0])*8u)
        STATIC_ASSERT(LEVIATHAN_TILES <= NEVO_TILES)

        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_TILES; ++i)
            {
                if((i == 0u) || (i == 3u) || (i == 12u) || (i == 15u)) continue;
                pMission->EnableTile(i, 4u);
            }

            bComplete2 = false;
            iTileState = 0u;
            iTileScore = 0u;
            std::memset(aiRemember, 0, sizeof(aiRemember));

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                if(pPart->GetCurHealth() > 0)
                {
                    pPart->RemoveStatus(ENEMY_STATUS_SECRET | ENEMY_STATUS_DEACTIVATE);
                    pPart->AddStatus(ENEMY_STATUS_INVINCIBLE);
                }
            }
        }

        const coreUint32 iTileStateOld = iTileState;

        for(coreUintW i = 0u; i < LEVIATHAN_TILES; ++i)
        {
            coreObject3D* pTile = pMission->GetTile(i);
            if(!pMission->IsTileEnabled(i)) continue;

            const coreVector2 vDir = coreVector2(1.0f,1.0f).Normalized();
            const coreVector2 vPos = MapToAxis(coreVector2(I_TO_F(i % 4u) - 1.5f, I_TO_F(i / 4u) - 1.5f) * 0.25f * FOREGROUND_AREA.x * 2.2f, vDir);

            pTile->SetPosition (coreVector3(vPos + vPos.Processed(SIGN) * 5.0f, 0.0f));
            pTile->SetDirection(coreVector3(vDir, 0.0f));

            if(!bComplete2)
            {
                g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW j)
                {
                    if(pPlayer->IsRolling()) return;

                    const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - pTile->GetPosition().xy(), pTile->GetDirection().xy());

                    if((ABS(vDiff.x) < pTile->GetCollisionRange().x) &&
                       (ABS(vDiff.y) < pTile->GetCollisionRange().y))
                    {
                        if(!HAS_BIT(aiRemember[j], i)) TOGGLE_BIT(iTileState, i)
                        ADD_BIT(aiRemember[j], i)

                        if(HAS_BIT(iTileState, i) && !HAS_BIT(iTileScore, i))
                        {
                            ADD_BIT(iTileScore, i)
                            pPlayer->GetScoreTable()->RefreshCooldown();
                            pPlayer->GetScoreTable()->AddChain(50);
                        }
                    }
                    else
                    {
                        REMOVE_BIT(aiRemember[j], i)
                    }
                });

                if(HAS_BIT(iTileState, i))
                {
                    pMission->SetTileStyle(i, 1u);

                    if(!HAS_BIT(iTileStateOld, i)) g_pSpecialEffects->PlaySound(pTile->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_CLICK);
                }
                else
                {
                    pMission->SetTileStyle(i, 0u);
                    bComplete1 = false;

                    if(HAS_BIT(iTileStateOld, i)) g_pSpecialEffects->PlaySound(pTile->GetPosition(), 1.0f, 0.9f, SOUND_EFFECT_CLICK);
                }
            }
        }

        if(!bComplete2)
        {
            bComplete2 = bComplete1;
            if(bComplete2)
            {
                for(coreUintW i = 0u; i < LEVIATHAN_TILES; ++i)
                {
                    pMission->SetTileStyle(i, 2u);
                    pMission->DisableTile(i, true);
                }

                this->__DisableRay(0, true);

                for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                {
                    cEnemy* pPart = this->__GetPart(i);

                    if(pPart->GetCurHealth() > 0)
                    {
                        pPart->AddStatus(ENEMY_STATUS_SECRET | ENEMY_STATUS_DEACTIVATE);
                        pPart->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                        g_pSpecialEffects->CreateSplashColor(pPart->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_BLUE);
                        g_pSpecialEffects->PlaySound(pPart->GetPosition(), 0.7f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
                    }
                }

                g_pSpecialEffects->MacroExplosionColorSmall(m_Head.GetPosition(), COLOR_ENERGY_ORANGE);
                g_pSpecialEffects->PlaySound(m_Head.GetPosition(), 0.7f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }
        }
        else
        {
            PHASE_CONTROL_TICKER(0u, 0u, 0.6f, LERP_LINEAR)
            {
                if((iTick % 2u) == 1u) return;

                this->__DisableRay(0, true);
                this->__EnableRay(0, true);
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/12.0f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING)
            {
                m_pRumbleSound->PlayRelative(this, 0.0f, 1.0f, true, SOUND_EFFECT);
            }

            vNewOri = coreVector2::Direction(PI*fTime + PI);

            fRayTime = fTime;

            if(m_pRumbleSound->EnableRef(this))
            {
                m_pRumbleSound->SetVolume(MAX0(SIN(fTime * (1.0f*PI))) * 3.0f);
            }

            if(PHASE_TIME_POINT(0.3f))
            {
                this->_ResurrectHelper(ELEMENT_RED, false);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 24u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_SMOOTH)
        {
            vNewOri = coreVector2(0.0f,1.0f);
            fCenterOffset = fTime * 10.0f;

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                {
                    this->__GetPart(i)->AddStatus(ENEMY_STATUS_GHOST_PLAYER);
                }

                this->_ResurrectHelper(ELEMENT_PURPLE, false);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 25u)
    {
        PHASE_CONTROL_TIMER(0u, m_avVector[ROTATION_ANGLE].y / (2.0f*PI) * 0.8f, LERP_HERMITE3)
        {
            afCenterSpin[m_aiCounter[SPIN_PART]] = fTime * (-4.0f*PI);

            vNewOri = coreVector2(0.0f,1.0f);
            fCenterOffset = 10.0f;

            if(PHASE_FINISHED)
            {
                PHASE_AGAIN(0u)

                m_aiCounter[SPIN_PART] = (m_aiCounter[SPIN_PART] + 2) % LEVIATHAN_PARTS;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                const coreVector2 vForce = m_avVector[SCATTER_FORCE + i].xy() * TIME;
                const coreVector3 vPos   = pPart->GetPosition();
                const coreVector2 vDir   = (m_aiCounter[CYCLE_COUNT] < 3) ? vPos.xy().Normalized() : MapStepRotated45(m_avSwimDir[i], i);

                pPart->SetPosition(coreVector3(vPos.xy() + vDir * vForce.x, vPos.z + vForce.y));

                m_avVector[SCATTER_FORCE + i].x *= FrictionFactor(0.5f);
                m_avVector[SCATTER_FORCE + i].y -= TIME * 150.0f;
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            coreUint8 iPhase;
            switch(m_aiCounter[CYCLE_COUNT])
            {
            default: UNREACHABLE
            case 1: iPhase = 10u; break;
            case 2: iPhase = 50u; break;
            case 3: iPhase = 80u; break;
            }

            PHASE_CHANGE_TO(iPhase)

            m_aiCounter[FLIP_SIDE] = 1 - m_aiCounter[FLIP_SIDE];

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                this->__GetPart(i)->RemoveStatus(ENEMY_STATUS_GHOST);
            }

            this->__RefreshHealth((m_aiCounter[CYCLE_COUNT] == 2) ? 1500 : 600, 600, 600);

            if(m_aiCounter[CYCLE_COUNT] <= 2)
            {
                g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT((m_aiCounter[CYCLE_COUNT] == 2) ? 1u : 0u));
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                this->__GetPart(i)->AddStatus(ENEMY_STATUS_GHOST_PLAYER);
            }
        }

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                this->__GetPart(i)->SetPosition(coreVector3(HIDDEN_POS, -40.0f));
            }

            coreVector2 vTarget;
            switch(m_aiCounter[EMERGE_TARGET] % 8)
            {
            default: UNREACHABLE
            case 0: vTarget = coreVector2( 0.0f, 1.0f); break;
            case 1: vTarget = coreVector2( 1.0f,-1.0f); break;
            case 2: vTarget = coreVector2(-1.0f, 0.0f); break;
            case 3: vTarget = coreVector2( 1.0f, 1.0f); break;
            case 4: vTarget = coreVector2( 0.0f,-1.0f); break;
            case 5: vTarget = coreVector2(-1.0f, 1.0f); break;
            case 6: vTarget = coreVector2( 1.0f, 0.0f); break;
            case 7: vTarget = coreVector2(-1.0f,-1.0f); break;
            }
            vTarget = vTarget.Normalized();

            coreUint8 iArrow;
            switch((m_aiCounter[ARROW_ACTIVE] & 0xFFu) % 6u)
            {
            default: UNREACHABLE
            case 0: iArrow = 0u; break;
            case 1: iArrow = 1u; break;
            case 2: iArrow = 2u; break;
            case 3: iArrow = 3u; break;
            case 4: iArrow = 0u; break;
            case 5: iArrow = 1u; break;
            }

            cEnemy* pCurPart = &m_aBody[m_aiCounter[EMERGE_PART]];

            if(pCurPart->HasStatus(ENEMY_STATUS_HIDDEN))
            {
                pMission->DisableArrow(0u, false);
                REMOVE_BIT(m_aiCounter[ARROW_ACTIVE], 8u)
            }
            else
            {
                if(!HAS_BIT(m_aiCounter[ARROW_ACTIVE], 8u) && (pCurPart->GetCurHealth() > 0))
                {
                    pMission->EnableArrow(0u, pCurPart, StepRotated90(iArrow));
                    ADD_BIT(m_aiCounter[ARROW_ACTIVE], 8u)
                }
            }

            pMission->SetCollEnemyBullet([COLL_THIS, COLL_VAL(pMission)](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                if(HAS_BIT(m_aiCounter[ARROW_ACTIVE], 8u) && pMission->GetArrowEnabled())
                {
                    const coreVector2 vSide = AlongCrossNormal(pBullet->GetFlyDir());
                    const coreUint8   iPack = PackDirection(vSide);

                    if(iPack == pMission->GetArrowDir(0u))
                    {
                        cPlayer*        pPlayer = d_cast<cPlayer*>(pBullet->GetOwner());
                        const coreInt32 iDamage = pEnemy->GetMaxHealth() / 2;

                        if(pEnemy->TakeDamage(iDamage, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer, true))
                        {
                            pPlayer->GetScoreTable()->AddChain(iDamage * (g_pGame->IsMulti() ? GAME_PLAYERS : 1));

                            m_aiCounter[ARROW_ACTIVE] += 1;

                            pMission->DisableArrow(0u, false);
                            REMOVE_BIT(m_aiCounter[ARROW_ACTIVE], 8u)

                            g_pSpecialEffects->MacroExplosionColorSmall(pEnemy->GetPosition(), COLOR_ENERGY_GREEN);
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
                        }
                    }
                }

                pBullet->Deactivate(true, vIntersection.xy());
                pBullet->Ignore();
            });

            const coreVector2 vPos    = vTarget * FOREGROUND_AREA * 0.6f;
            const coreFloat   fHeight = LERPB(-30.0f, 0.0f, CLAMP01(MIN(fTime, 1.0f - fTime) * 3.0f));

            pCurPart->SetPosition (coreVector3(vPos, fHeight));
            pCurPart->SetDirection(coreVector3(0.0f,0.0f,1.0f));

            pCurPart->SetCollisionModifier(this->GetCollisionModifier() * coreVector3(1.0f, pCurPart->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 10.0f, 1.0f));

            vNewOri = coreVector2(1.0f,0.0f);

            if(fHeight >= -15.0f)
            {
                if(m_aiCounter[EMERGE_PART] == 0)
                {
                    PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
                    {
                        if(g_pGame->IsEasy() && (iTick % 2u)) return;

                        const coreVector2 vDir = coreVector2::Direction(fTime * (4.0f*PI));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.9f, this, vPos, vDir)->ChangeSize(1.4f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.4f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 1u, COLOR_ENERGY_RED);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    });
                }
                else if(m_aiCounter[EMERGE_PART] == 1)
                {
                    PHASE_CONTROL_TICKER(1u, 0u, 5.0f, LERP_LINEAR)
                    {
                        const coreFloat fBase = pCurPart->AimAtPlayerDual(((iTick % 4u) < 2u) ? 0u : 1u).Angle();
                        const coreUintW iNum  = g_pGame->IsEasy() ? 3u : 7u;

                        for(coreUintW j = iNum; j--; )
                        {
                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iNum - 1u) * 0.5f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.6f);
                        }

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_YELLOW);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    });
                }
                else if(m_aiCounter[EMERGE_PART] == 2)
                {
                    PHASE_CONTROL_TICKER(1u, 0u, 2.0f, LERP_LINEAR)
                    {
                        for(coreUintW j = 40u; j--; )
                        {
                            if((j % 10u) < (g_pGame->IsEasy() ? 8u : 6u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 3.5f) * 4.5f + ((iTick % 2u) ? 22.5f : 0.0f)));

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.0f, this, vPos,  vDir)->ChangeSize(1.6f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.0f, this, vPos, -vDir)->ChangeSize(1.6f);
                        }

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_PURPLE);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    });
                }
                else UNREACHABLE
            }

            if(m_aiCounter[EMERGE_PART] == 2u)
            {
                this->_ResurrectHelper(ELEMENT_GREEN, true);
            }

            if(PHASE_FINISHED)
            {
                coreBool bFinished = true;
                for(coreUintW i = 0u; i < LEVIATHAN_PARTS_BODIES; ++i)
                {
                    if(++m_aiCounter[EMERGE_PART] >= coreInt32(LEVIATHAN_PARTS_BODIES)) m_aiCounter[EMERGE_PART] = 0;
                    if(m_aBody[m_aiCounter[EMERGE_PART]].GetCurHealth() > 0)
                    {
                        bFinished = false;
                        break;
                    }
                }

                if(bFinished)
                {
                    PHASE_CHANGE_TO(60u)

                    pMission->ResetCollEnemyBullet();

                    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                    {
                        cEnemy* pPart = this->__GetPart(i);

                        pPart->SetCollisionModifier(this->GetCollisionModifier());
                        pPart->RemoveStatus(ENEMY_STATUS_GHOST_PLAYER);
                    }

                    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(2u));
                }
                else
                {
                    PHASE_RESET(0u)   // not again, too complex

                    m_aiCounter[EMERGE_TARGET] += 1;

                    pMission->DisableArrow(0u, false);
                    REMOVE_BIT(m_aiCounter[ARROW_ACTIVE], 8u)
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 3u; ++i)
                this->__EnableRay(i + 1u, false);

            m_Head.AddStatus(ENEMY_STATUS_GHOST);
        }

        const coreFloat fSpeed = LERP(1.0f, 0.5f, STEP(0.5f, 1.0f, m_Head.GetCurHealthPct())) * (g_pGame->IsEasy() ? 0.65f : 1.0f);

        for(coreUintW i = 0u; i < LEVIATHAN_PARTS_BODIES; ++i)
        {
            cEnemy* pPart = &m_aBody[i];

            if(PHASE_BEGINNING2)
            {
                const coreFloat fSide = (i % 2u) ? -1.0f : 1.0f;

                pPart->SetPosition (coreVector3(fSide *  1.0f, 1.2f + I_TO_F(i) * 0.8f, 0.0f) * FOREGROUND_AREA3);
                pPart->SetDirection(coreVector3(fSide * -1.0f, 0.0f,                    0.0f));
            }

            coreVector2 vCurPos = pPart->GetPosition ().xy() + coreVector2(0.0f,-1.0f) * FOREGROUND_AREA * (0.5f * fSpeed * TIME);
            coreVector2 vCurDir = pPart->GetDirection().xy();

            if(vCurPos.y < -FOREGROUND_AREA.y * 1.2f)
            {
                vCurPos.y += FOREGROUND_AREA.y * 2.4f;

                vCurPos.x  = -vCurPos.x;
                vCurDir.x  = -vCurDir.x;
            }

            vCurPos.x = SIGN(vCurPos.x) * LERPS(1.0f, 1.4f, m_avVector[SMOOTH_MOVE].y) * FOREGROUND_AREA.x;

            pPart->SetPosition (coreVector3(vCurPos, 0.0f));
            pPart->SetDirection(coreVector3(vCurDir, 0.0f));
        }

        const coreFloat fHeight = LERPS(1.4f, 0.8f, CLAMP01((m_fPhaseTime - 4.0f) * 0.5f) - m_avVector[SMOOTH_MOVE].y) * FOREGROUND_AREA.y;

        m_Head.SetPosition (coreVector3(0.0f, fHeight, 0.0f));
        m_Head.SetDirection(coreVector3(m_Head.AimAtPlayerDual(m_aiCounter[WAVE_SHOTS] % 2).Normalized(), 0.0f));

        m_Tail.SetPosition(coreVector3(HIDDEN_POS, 0.0f));

        if(PHASE_MAINTIME_POINT(4.0f))
        {
            m_Head.RemoveStatus(ENEMY_STATUS_GHOST);
        }

        if(PHASE_MAINTIME_POINT(7.0f))
        {
            this->_ResurrectHelper(ELEMENT_YELLOW, false);
        }

        if(m_aiCounter[SMOOTH_STATE] == 0)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 1.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(((iTick % 4u) * 3u) % 5u) - 2.0f) * 0.3f + ((iTick % 2u) ? 1.0f : -1.0f) * 0.03f, -1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(2, 0.5f, this, vPos, vDir)->ChangeSize(1.0f)->ChangeCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * (1.0f / 0.95f));   // start with 1.0f
            });
        }
        else if(m_aiCounter[SMOOTH_STATE] == 1)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 15.0f, LERP_LINEAR)
            {
                if((iTick % 10u) < 2u) return;
                if(g_pGame->IsEasy() && ((iTick % 20u) < 10u)) return;

                const coreVector2 vPos = m_Head.GetPosition ().xy();
                const coreVector2 vDir = m_Head.GetDirection().xy();

                const coreFloat fSpeed = 1.1f * (1.0f + 0.08f * I_TO_F(iTick % 10u));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.9f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);

                if((iTick % 20u) == 19u) m_aiCounter[WAVE_SHOTS] += 1;
            });

            PHASE_CONTROL_TICKER(1u, 0u, 2.0f/2.5f, LERP_LINEAR)
            {
                if((iTick % 2u) == 1u) return;

                for(coreUintW i = 0u; i < 3u; ++i)
                {
                    this->__DisableRay(i + 1u, true);
                    this->__EnableRay(i + 1u, true);
                }
            });
        }

        if(m_Head.ReachedHealth(700))
        {
            m_aiCounter[SMOOTH_STATE] = 1;

            for(coreUintW i = 0u; i < 3u; ++i)
                this->__DisableRay(i + 1u, true);

            g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cGrowBullet>(true);

            g_pSpecialEffects->MacroExplosionColorSmall(m_Head.GetPosition(), COLOR_ENERGY_GREEN);
            g_pSpecialEffects->PlaySound(m_Head.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
        }
        else if(m_Head.ReachedDeath())
        {
            m_aiCounter[SMOOTH_STATE] = 2;

            for(coreUintW i = 0u; i < 3u; ++i)
                this->__DisableRay(i + 1u, true);
        }

        if(m_aiCounter[SMOOTH_STATE] == 2)
        {
            m_avVector[SMOOTH_MOVE].y += 0.5f * TIME;
            if(m_avVector[SMOOTH_MOVE].y >= 1.0f)
            {
                PHASE_CHANGE_TO(70u)

                this->__RefreshHealth(500, 300, 300);

                g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(3u));
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                m_avVector[CURTAIN_VALUE].x = -100.0f;
            }

            m_Head.DefaultMoveLerp(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f), fTime);
            m_Head.SetDirection   (coreVector3(0.0f,1.0f,0.0f));

            for(coreUintW i = 1u; i < LEVIATHAN_PARTS; ++i)
            {
                this->__GetPart(i)->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->StorePosition(m_Head.GetPosition().xy());
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_HERMITE3)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableRay(0u, true);
            }

            const coreFloat fOffset = -SIN(fTime * (2.0f*PI));

            m_Head.SetPosition (coreVector3(fOffset * 0.85f, m_vLastPosition.y, 0.0f) * FOREGROUND_AREA3);
            m_Head.SetDirection(coreVector3(fOffset * 0.1f, 1.0f, 0.0f).Normalized());

            m_avVector[CURTAIN_VALUE].x = LERP(-1.2f, 1.2f, STEP(0.25f, 0.75f, fTime)) * FOREGROUND_AREA.x;

            if(PHASE_TIME_POINT(0.75f))
            {
                this->__DisableRay(0u, true);
                g_pSpecialEffects->PlaySound(m_Head.GetPosition(), 1.0f, 1.5f, SOUND_EFFECT_DUST);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_Head.SetDirection(coreVector3(0.0f,1.0f,0.0f));

                for(coreUintW i = 1u; i < LEVIATHAN_PARTS; ++i)
                {
                    cEnemy* pPart = this->__GetPart(i);

                    pPart->SetPosition (coreVector3(m_Head.GetPosition().xy() - m_Head.GetDirection().xy() * cLeviathanBoss::__GetPartDistance(i) * FOREGROUND_AREA * 1.2f, 0.0f));
                    pPart->SetDirection(coreVector3(1.0f,0.0f,0.0f));
                }

                for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                    m_avSwimDir[i] = m_Head.GetDirection().xy();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 72u)
    {
        for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        {
            cEnemy* pPart = this->__GetPart(i);

            const coreFloat fSpeed = LERP(35.0f, 20.0f, STEP(0.0f, 1700.0f, I_TO_F(this->GetCurHealth()))) * MIN1(m_fPhaseTime * 0.5f) * (g_pGame->IsEasy() ? 0.8f : 1.0f);

            coreVector2 vCurPos = pPart->GetPosition().xy() + m_avSwimDir[i] * (fSpeed * TIME);

            if(((vCurPos.x < -FOREGROUND_AREA.x * 1.4f) && (m_avSwimDir[i].x < 0.0f)) ||
               ((vCurPos.x >  FOREGROUND_AREA.x * 1.4f) && (m_avSwimDir[i].x > 0.0f)) ||
               ((vCurPos.y < -FOREGROUND_AREA.y * 1.4f) && (m_avSwimDir[i].y < 0.0f)) ||
               ((vCurPos.y >  FOREGROUND_AREA.y * 1.4f) && (m_avSwimDir[i].y > 0.0f)))
            {
                coreVector2 vNewDir;
                switch(m_aiSwimCount[i] % 8u)
                {
                default: UNREACHABLE
                case 0u: vNewDir = coreVector2( 1.0f, 0.5f); break;
                case 1u: vNewDir = coreVector2( 0.5f,-1.0f); break;
                case 2u: vNewDir = coreVector2(-1.0f, 0.5f); break;
                case 3u: vNewDir = coreVector2( 0.5f, 1.0f); break;
                case 4u: vNewDir = coreVector2( 1.0f,-0.5f); break;
                case 5u: vNewDir = coreVector2(-0.5f,-1.0f); break;
                case 6u: vNewDir = coreVector2(-1.0f,-0.5f); break;
                case 7u: vNewDir = coreVector2(-0.5f, 1.0f); break;
                }

                m_aiSwimCount[i] += 1u;

                vCurPos = (coreVector2::Dot(vCurPos, m_avSwimDir[i]) - FOREGROUND_AREA.x * 3.0f) * vNewDir;   // make sure laser does not pop in
                m_avSwimDir[i] = vNewDir.Normalized();

                if((m_aiSwimCount[i] == 1u) && (i != 0u && i != 4u))
                {
                    this->__EnableRay(i, false);
                }

                if((m_aiSwimCount[i] == 3u) && (pPart == &m_aBody[2]))
                {
                    this->_ResurrectHelper(ELEMENT_CYAN, false);
                }
            }

            pPart->SetPosition(coreVector3(vCurPos, 0.0f));

            if(i != 0u && i != 4u)
            {
                if(m_aiSwimCount[i])
                {
                    pPart->SetDirection(coreVector3(m_avSwimDir[i].Rotated90() * (((m_aiSwimCount[i] / 3u) % 2u) ? 1.0f : -1.0f), 0.0f));
                }
                else
                {
                    //pPart->SetDirection(OriRoundDir(coreVector2::Direction(m_fPhaseTime * 0.15f * (4.0f*PI) + I_TO_F(i)), m_avSwimDir[i]));
                    pPart->SetDirection(coreVector3(m_avSwimDir[i], 0.0f));
                }
            }
            else
            {
                pPart->SetDirection(coreVector3(m_avSwimDir[i], 0.0f));
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                pPart->SetColor3(COLOR_SHIP_BLACK);
                pPart->AddStatus(ENEMY_STATUS_GHOST);
            }
        }

        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                const coreFloat fAngle = (2.0f*PI) * fTime - cLeviathanBoss::__GetPartDistance(i) * (0.5f*PI) + (0.6f*PI);

                coreVector3 vPos, vDir;
                cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,1.0f,0.0f), fAngle, coreVector3(FOREGROUND_AREA * 0.8f, 20.0f), &vPos, &vDir);

                vPos += coreVector3(0.0f,0.0f,-20.0f);

                if(m_iPhase == 80u)
                {
                    vPos = vPos.InvertedX();
                    vDir = vDir.InvertedX();
                }

                pPart->SetPosition (vPos);
                pPart->SetDirection(vDir);
            }

            if(PHASE_TIME_POINT(0.52f))
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        PHASE_CONTROL_TICKER(0u, 4u, 2.5f, LERP_LINEAR)
        {
            cEnemy* pPart = this->__GetPart(LEVIATHAN_PARTS - iTick - 1u);

            pPart->Kill(false);

            g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(pPart->GetPosition());
            g_pSpecialEffects->PlaySound(pPart->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_05);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 84u)
    {
        PHASE_CONTROL_TICKER(0u, 20u, 10.0f, LERP_LINEAR)
        {
            if(PHASE_FINISHED)
            {
                this->Kill(false);

                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(2u, m_Head.GetPosition().xy());
                }

                g_pSpecialEffects->CreateExplosion (m_Head.GetPosition());
                g_pSpecialEffects->CreateSplashDark(m_Head.GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (m_Head.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (m_Head.GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);
            }
            else
            {
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);
                const coreFloat   fLen = (iTick % 2u) ? 5.0f : 3.0f;
                const coreVector2 vPos = m_Head.GetPosition().xy() + vDir * fLen;

                g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(coreVector3(vPos, 0.0f));
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_05);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
        if(PHASE_BEGINNING2)
        {
            m_Head.SetPosition(coreVector3(0.0f, 1.3f,0.0f) * FOREGROUND_AREA3);

            this->__EnableRay(0u, false);
        }

        cPlayer* pPlayer = g_pGame->GetPlayer(0u);

        PHASE_CONTROL_TICKER(0u, 0u, 60.0f, LERP_LINEAR)
        {
            //g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 100.0f, 2u, COLOR_FIRE_ORANGE);
        });

        PHASE_CONTROL_TIMER(1u, 1.0f/1.3f, LERP_HERMITE3)
        {
            m_Head.SetDirection(coreVector3(coreVector2::Direction(LERP(0.5f*PI, 1.5f*PI, fTime)), 0.0f));
            
            if(PHASE_TIME_AFTER(0.5f))
            {
                PHASE_CONTROL_TICKER(0u, 0u, 60.0f, LERP_LINEAR)
                {
                    g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 100.0f, 2u, COLOR_FIRE_ORANGE);
                });
            }
        });

        if(PHASE_MAINTIME_POINT(1.3f))
        {
            //g_pSpecialEffects->MacroExplosionDarkBig(pPlayer->GetPosition());
                g_pSpecialEffects->CreateExplosion (pPlayer->GetPosition());
                g_pSpecialEffects->CreateSplashDark(pPlayer->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (pPlayer->GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);
            
            pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
            
            this->__DisableRay(0u, true);

            m_Head.SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 201u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 202u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            m_Head.SetPosition(coreVector3(0.0f, LERPBR(1.3f, -2.3f, fTime), 0.0f) * FOREGROUND_AREA3);

            for(coreUintW i = 1u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                pPart->SetPosition (coreVector3(m_Head.GetPosition().xy() - m_Head.GetDirection().xy() * cLeviathanBoss::__GetPartDistance(i) * FOREGROUND_AREA * 1.2f, 0.0f));
                pPart->SetDirection(m_Head.GetDirection());
            }

            if(PHASE_FINISHED)
            {
                this->Kill(false);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(m_aiCounter[ROTATION_STATUS])
    {
        if(m_aiCounter[FLIP_SIDE])
        {
            // 
            const coreVector2 vBaseDir = coreVector2::Direction(m_avVector[ROTATION_ANGLE].x).Rotated90();
            const coreVector3 vRotaDir = coreVector3(vBaseDir.x, -vNewOri.y*vBaseDir.y, vNewOri.x*vBaseDir.y);
            const coreVector4 vQuat    = coreMatrix4::Orientation(vRotaDir, coreVector3(0.0f, vNewOri)).m123().ToQuat();

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                // 
                const coreVector3 vNewDir = vQuat.QuatApply(coreVector3(coreVector2::Direction(I_TO_F(i) * (0.4f*PI)), 0.0f));
                const coreVector3 vNewPos = vNewDir * (LEVIATHAN_RADIUS_INNER_2 + fCenterOffset);

                // 
                pPart->SetPosition (m_aiCounter[FLIP_SIDE] ? vNewPos.InvertedX() : vNewPos);
                pPart->SetDirection(m_aiCounter[FLIP_SIDE] ? vNewDir.InvertedX() : vNewDir);

                // 
                if(afCenterSpin[i]) pPart->SetDirection(MapToAxis(pPart->GetDirection(), coreVector2::Direction(afCenterSpin[i])));
            }

            // 
            m_avVector[ROTATION_ANGLE].x += m_avVector[ROTATION_ANGLE].y * TIME;
        }
        else
        {
            const coreFloat fBaseAngle = m_avVector[ROTATION_ANGLE].x;
            const coreFloat fRadius    = LEVIATHAN_RADIUS_INNER_1;

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
            {
                cEnemy* pPart = this->__GetPart(i);

                const coreFloat fOffset = cLeviathanBoss::__CalcAngle(cLeviathanBoss::__GetPartDistance(i) * 7.65f, fRadius);
                const coreFloat fAngle  = fBaseAngle - fOffset * (0.4f*PI);

                coreVector3 vPos, vDir;
                cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,0.0f,1.0f), fAngle, coreVector3(fRadius, fRadius, 1.0f), &vPos, &vDir);

                pPart->SetPosition (vPos);
                pPart->SetDirection(vDir);
            }

            // 
            m_avVector[ROTATION_ANGLE].x += m_avVector[ROTATION_ANGLE].y * TIME;
        }
    }

    if((m_iPhase >= 10u) && (m_iPhase < 30u))
    {
        if(!m_aiCounter[CYCLE_COUNT] || (m_iPhase < 20u))
        {
            PHASE_CONTROL_TICKER(2u, 0u, g_pGame->IsEasy() ? 0.8f : 1.0f, LERP_LINEAR)
            {
                if(!iTick || (iTick % 2u)) return;

                for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                {
                    const cEnemy* pPart = this->__GetPart(i);

                    const coreVector2 vPos = pPart->GetPosition().xy();
                    const coreVector2 vDir = pPart->GetPosition().xy().Normalized() * -1.0f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.7f, this, vPos, vDir)->ChangeSize(1.8f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            });
        }
    }


    if(m_bActive)
    {
        if((m_iPhase >= 10u) && !((m_iPhase >= 30u) && (m_iPhase < 40u)))
        {
            if(m_aiCounter[CYCLE_COUNT] == 0)
            {
                PHASE_CONTROL_TIMER(4u, 0.05f, LERP_SMOOTH)
                {
                    g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(0.0f*PI, 3.0f*PI, fTime)));
                });
            }
            else if(m_aiCounter[CYCLE_COUNT] == 1)
            {
                PHASE_CONTROL_TIMER(4u, 0.05f, LERP_SMOOTH)
                {
                    g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(3.0f*PI, 0.0f*PI, fTime)));
                });
            }
            else if(m_aiCounter[CYCLE_COUNT] == 2)
            {
                PHASE_CONTROL_TIMER(4u, 0.05f, LERP_SMOOTH)
                {
                    g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(0.0f*PI, 3.0f*PI, fTime)));
                });
            }
        }
    }
    else
    {
        PHASE_CONTROL_TIMER(4u, 0.25f, LERP_BREAK)
        {
            //g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[ENV_ROTATION].y, 0.0f*PI, fTime)));   end rotation
        });
    }

    constexpr coreFloat fQuadSpeed = 2.0f;

    if((m_iPhase >= 70u) && (m_iPhase < 80u))
    {
        PHASE_CONTROL_TICKER(1u, 0u, 4.0f * fQuadSpeed, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 21u; ++i)
            {
                if((i % 2u) == (iTick % 2u)) continue;

                const coreFloat A = SIN(I_TO_F(iTick) * (0.075f*PI) + I_TO_F(i - 10u) * (0.15f*PI));
                if(A < (g_pGame->IsEasy() ? 0.5f : 0.0f)) continue;

                const coreVector2 vPos = coreVector2((I_TO_F(i) - 10.0f) / 9.0f, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                if(vPos.x < m_avVector[CURTAIN_VALUE].x)
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.0f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            }
        });

        pMission->SetCollBulletBullet([](const cBullet* pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(pBulletEnemy->GetID() != cQuadBullet::ID) return;

            pBulletEnemy->Deactivate(true, vIntersection.xy(), pBulletPlayer->GetFlyDir());

            g_pGame->PlayVanishSound(pBulletEnemy->GetPosition());
        });

        const coreVector2 vMove1 = coreVector2(0.0f, -10.0f * fQuadSpeed) * TIME;

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
        {
            const coreVector2 vNewPos = pBullet->GetPosition().xy() + vMove1;

            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));

            if(vNewPos.y < -FOREGROUND_AREA.y * 1.25f) pBullet->Deactivate(false);
        });
    }

    for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
    {
        coreObject3D* pRay = (*m_Ray.List())[i];
        if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const cEnemy*     pPart = this->__GetPart(i);
        const coreVector3 vPos  = pPart->GetPosition();
        const coreVector3 vDir  = pPart->GetDirection();

        pRay->SetPosition (vPos + vDir * (pRay->GetSize().y + LEVIATHAN_RAY_OFFSET(i)));
        pRay->SetDirection(vDir);

        const coreVector3 vRayDir   = pRay->GetDirection();
        const coreVector3 vRayStart = pRay->GetPosition () - pRay->GetSize().y * vRayDir;
        coreFloat         fRayLen   = LEVIATHAN_RAY_SIZE.y;

        if(vRayDir.z < -0.15f)
        {
            const coreVector3 vIntersect = g_pEnvironment->RetrieveSafeIntersect(vRayStart, vRayDir);
            this->__CreateOverdrive(i, vIntersect, fRayTime, true);

            fRayLen = (vIntersect - vRayStart).Length() * 0.5f;
        }
        else if(vRayDir.z > 0.4f)
        {
            fRayLen = LEVIATHAN_RAY_SIZE.y * (1.0f - (vRayDir.z-0.4f));

            const coreVector3 vIntersect = vRayStart + vRayDir * (fRayLen * 2.0f);
            this->__CreateOverdrive(i, vIntersect, fRayTime, false);
        }
        else m_avVector[OVERDRIVE_HIT + i].xyz(coreVector3(0.0f,0.0f,0.0f));

        pRay->SetSize(coreVector3(LEVIATHAN_RAY_SIZE.x, fRayLen, LEVIATHAN_RAY_SIZE.z));
    }

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
    {
        coreObject3D* pRay  = (*m_Ray    .List())[i];
        coreObject3D* pWave = (*m_RayWave.List())[i];
        if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat fOldTime = m_afRayTime[i];
        m_afRayTime[i].Update(g_pGame->IsEasy() ? 0.7f : 0.8f);
        const coreFloat fNewTime = m_afRayTime[i];

        // 
        const cEnemy*     pPart = this->__GetPart(i);
        const coreVector3 vPos  = pPart->GetPosition();
        const coreVector3 vDir  = pPart->GetDirection();

        // 
        const coreVector3 vColor  = coreMath::IsNear(vDir.z, 0.0f, LEVIATHAN_RAY_HEIGHT) ? COLOR_ENERGY_ORANGE : (COLOR_ENERGY_BLUE * 0.9f);
        const coreFloat   fAlpha  = (fNewTime < 1.0f) ? (0.6f * (1.0f - fNewTime)) : 1.0f;
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/5.0f);

        if(m_avVector[OVERDRIVE_HIT + i].xyz().IsNull())
        {
            const coreVector3 vRayPos = vPos + vDir * LEVIATHAN_RAY_OFFSET(i);
            const coreVector3 vRayDir = vDir;

            coreFloat fHit = 1.0f;
            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([&](const cGrowBullet* pBullet)
            {
                coreFloat fHitDistance = 0.0f;
                coreUint8 iHitCount    = 1u;
                if(Core::Manager::Object->TestCollision(pBullet, vRayPos, vRayDir, &fHitDistance, &iHitCount))
                {
                    fHit = MIN(fHit, fHitDistance * 0.5f / LEVIATHAN_RAY_SIZE.y);
                }
            });
            
            if(!pRay->GetType()) fHit = 1.0f;

            // 
            const coreFloat   fLength = (fNewTime < 1.0f) ? 1.0f : MIN1((fNewTime - 1.0f) * 8.0f);
            const coreFloat   fWidth  = 2.0f - fLength;
            //const coreVector3 vSize   = coreVector3(fWidth, /*fLength*/1.0f, fWidth);
        
            const coreFloat fDiff = fHit - m_afRayLen[i];
            if(fDiff > 0.0f) m_afRayLen[i] += SIGN(fDiff) * 3.0f * TIME * SmoothTowards(ABS(fDiff), 0.2f);
                        else m_afRayLen[i]  = fHit;

            // 
            pRay ->SetSize(LEVIATHAN_RAY_SIZE     * coreVector3(fWidth, m_afRayLen[i], fWidth));
            pWave->SetSize(LEVIATHAN_RAYWAVE_SIZE * coreVector3(fWidth, 1.0f,          fWidth * 0.8f));
        }

        // 
        pRay->SetPosition (vPos + vDir * (pRay->GetSize().y + LEVIATHAN_RAY_OFFSET(i)));
        pRay->SetDirection(vDir);
        pRay->SetColor3   (vColor);
        pRay->SetAlpha    (fAlpha);
        pRay->SetTexSize  (coreVector2(LEVIATHAN_RAY_TEXSIZE.x, LEVIATHAN_RAY_TEXSIZE.y * (pRay->GetSize().y * (1.0f/LEVIATHAN_RAY_SIZE.y))));
        pRay->SetTexOffset(coreVector2(0.4f,0.3f) * (m_fAnimation + fOffset));

        // 
        pWave->SetPosition (vPos + vDir * (pWave->GetSize().y  + LEVIATHAN_RAY_OFFSET(i)));
        pWave->SetDirection(-vDir);
        pWave->SetColor3   (vColor);
        pWave->SetAlpha    (fAlpha * 0.85f);
        pWave->SetTexOffset((coreVector2(-0.3f,-0.6f) * (m_fAnimation + fOffset)).Processed(FRACT));

        if((fOldTime < 1.0f) && (fNewTime >= 1.0f))
        {
            // 
            this->__BeginRay(i);

            // 
            g_pSpecialEffects->MacroEruptionColorBig(vPos + vDir * LEVIATHAN_RAY_OFFSET(i), vDir.xy(), COLOR_ENERGY_ORANGE);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(pPart->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
            g_pSpecialEffects->PlaySound(pPart->GetPosition(), 0.4f, 1.0f, SOUND_EFFECT_FIRE);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
        }
    }

    // 
    m_Ray    .MoveNormal();
    m_RayWave.MoveNormal();

    if(m_Ray.GetNumEnabled())
    {
        coreVector2 vCenter = coreVector2(0.0f,0.0f);

        // create fire-effect at the screen border             
        PHASE_CONTROL_TICKER(3u, 0u, 30.0f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
            {
                const coreObject3D* pRay = (*m_Ray.List())[i];
                if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
                if(!pRay->GetType())                          continue;

                // 
                const coreVector3 vPos = pRay->GetPosition();
                const coreVector3 vDir = pRay->GetDirection();
                const coreFloat   fLen = pRay->GetSize().y;
                
                if(fLen <= 0.0f) continue;

                // 
                const coreVector2 vTestFrom = g_pForeground->Project2D(vPos - vDir * fLen);
                const coreVector2 vTestTo   = g_pForeground->Project2D(vPos + vDir * fLen);

                // 
                if((ABS(vTestFrom.x) <= 0.55f) && (ABS(vTestFrom.y) <= 0.55f) && ((ABS(vTestTo.x) > 0.55f) || (ABS(vTestTo.y) > 0.55f)))
                {
                    const coreVector2 vNorm      = (vTestTo - vTestFrom).Normalized();
                    const coreVector2 vEffectPos = vPos.xy() + vNorm * g_pForeground->RayIntersection(vPos.xy(), vNorm);

                    // 
                    g_pSpecialEffects->CreateSplashFire (coreVector3(vEffectPos, 0.0f),  5.0f, 3u, COLOR_FIRE_ORANGE);
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), 25.0f, 2u, COLOR_FIRE_ORANGE);
                    
                    if(!fRayTime) vCenter += vEffectPos;
                }
                
                else if(!vDir.z && (fLen < LEVIATHAN_RAY_SIZE.y))
                {
                    const coreVector2 vEffectPos = vPos.xy() + vDir.xy() * fLen;

                    // 
                    g_pSpecialEffects->CreateSplashFire (coreVector3(vEffectPos, 0.0f),  5.0f, 3u, COLOR_FIRE_ORANGE);
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), 25.0f, 2u, COLOR_FIRE_ORANGE);
                    
                    if(!fRayTime) vCenter += vEffectPos;
                }
            }

            if(m_pLaserSound->EnableRef(this))
            {
                g_pSpecialEffects->ExternSetSource(m_pLaserSound, coreVector3(vCenter / I_TO_F(coreMath::PopCount(m_iRayState)), 0.0f));
                m_pLaserSound->SetVolume(1.0f);
            }
        });

        if(m_iPhase < 200u)
        {
            for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
            {
                const coreObject3D* pRay = (*m_Ray.List())[i];
                if(!pRay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
                if(!pRay->GetType())                          continue;

                if(!coreMath::IsNear(pRay->GetDirection().z, 0.0f, LEVIATHAN_RAY_HEIGHT)) continue;

                const coreVector2 vRayDir = pRay->GetDirection().xy().Normalized();
                const coreVector2 vRayPos = pRay->GetPosition ().xy() - vRayDir * pRay->GetSize().y;

                cPlayer::TestCollision(PLAYER_TEST_NORMAL, vRayPos, vRayDir, pRay, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;
                    if(pfHitDistance[0] <= 0.0f) return;
                    if(pfHitDistance[0] > pRay->GetSize().y * 2.0f) return;

                    const coreVector2 vIntersection = vRayPos + vRayDir * pfHitDistance[0];

                    // 
                    pPlayer->TakeDamage(5, ELEMENT_YELLOW, vIntersection);

                    // 
                    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vIntersection, 0.0f), COLOR_ENERGY_ORANGE);

                    // 
                    if(pPlayer->ReachedDeath())
                    {
                        const coreUintW iIndex = g_pGame->GetPlayerIndex(pPlayer);

                        if(GET_BITVALUE(m_aiCounter[RAY_HIT], 8u, iIndex * 8u) == i + 1u)
                        {
                            pMission->GiveBadge(3u, BADGE_ACHIEVEMENT, pPlayer->GetPosition());
                        }

                        SET_BITVALUE(m_aiCounter[RAY_HIT], 8u, iIndex * 8u, i + 1u)
                        STATIC_ASSERT(GAME_PLAYERS == 2u)
                    }
                });
            }
        }
    }

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        const coreVector3 vPos    = pPart->GetPosition();
        const coreFloat   fHeight = g_pEnvironment->RetrieveSafeHeight(pPart->GetPosition().xy());

        // 
        const coreBool bOldHidden =  pPart->HasStatus(ENEMY_STATUS_HIDDEN);
        const coreBool bNewHidden = (pPart->GetPosition().z <= fHeight);

        // 
        if(bNewHidden) pPart->AddStatus   (ENEMY_STATUS_HIDDEN);
                  else pPart->RemoveStatus(ENEMY_STATUS_HIDDEN);

        // 
        if(bOldHidden != bNewHidden)
        {
            const coreVector2 vProjectedPos = g_pForeground->Project3D(vPos);

            // 
            if(g_pForeground->IsVisiblePoint(vProjectedPos))
            {
                // 
                g_pSpecialEffects->CreateSplashSmoke(coreVector3(vPos.xy(), fHeight), 30.0f, 30u, coreVector3(1.0f,1.0f,1.0f));
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos.xy(), fHeight), 50.0f, 15u, COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                g_pSpecialEffects->PlaySound(coreVector3(vPos.xy(), fHeight), 0.4f, bNewHidden ? 2.0f : 1.2f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                // 
                const coreBool    bRotated   = i % 2u;
                const coreBool    bFlipped   = (i / 2u) % 2u;
                const coreVector3 vDecalPos  = coreVector3(vPos.xy(), fHeight);
                const coreVector2 vDecalSize = coreVector2(1.0f,1.0f) * 15.0f;
                const coreVector2 vDecalDir  = coreVector2(0.0f,1.0f);

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
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 4u, "effect_decal_single_inst_program", LIST_KEY);
            }
        }

        // 
        if(!bNewHidden) pPart->DefaultAxiate(m_fMovement * ((i & 0x01u) ? 1.0f : -1.0f), coreVector3(0.0f, vNewOri));
    }

    // 
    for(coreUintW i = 0u; i < LEVIATHAN_MARKS; ++i)
    {
        coreObject3D& oMark = m_aMarkRaw[i];
        if(!oMark.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cEnemy* pPart = this->__GetPart(i);

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction(m_fAnimation * (1.0f*PI));

        // 
        oMark.SetPosition (pPart->GetPosition());
        oMark.SetDirection(coreVector3(vDir, 0.0f));
        oMark.SetAlpha    (pPart->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 1.0f);
        oMark.SetTexOffset(coreVector2(0.0f, FRACT(0.2f * m_fAnimation + fOffset)));
    }

    // 
    m_Mark.MoveNormal();

    // 
    this->__UpdateHealth();

    if(m_aiCounter[CYCLE_COUNT] < 3)
    {
        coreBool bFinished = true;
        for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
        {
            if(this->__GetPart(i)->GetCurHealth())
            {
                bFinished = false;
                break;
            }
        }

        // 
        if(bFinished && ((m_iPhase < 30u) || (m_iPhase >= 40u)))
        {
            // 
            for(coreUintW i = 0u; i < LEVIATHAN_TILES; ++i)
                pMission->DisableTile(i, true);

            // 
            pMission->ResetCollBulletBullet();

            // 
            for(coreUintW i = 0u; i < LEVIATHAN_RAYS; ++i)
                this->__DisableRay(i, true);

            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                this->__GetPart(i)->AddStatus(ENEMY_STATUS_GHOST);

            // 
            m_aiCounter[ROTATION_STATUS] = 0;
            m_aiCounter[CYCLE_COUNT]    += 1;

            // 
            if(m_aiCounter[CYCLE_COUNT] == 3)
                this->_EndBoss();

            // 
            for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
                m_avVector[SCATTER_FORCE + i].xy(coreVector2(1.0f,1.0f) * 30.0f);

            // 
            m_avVector[ENV_ROTATION].y = g_pEnvironment->GetDirection().Angle();

            // 
            PHASE_CHANGE_TO(30u)
            PHASE_RESET(2u)

            // 
            this->_ResurrectHelper(ELEMENT_MAGENTA, false);
        }
    }
    
    
    
    
    if((m_iPhase < 200u) && !this->HasStatus(ENEMY_STATUS_DEAD))
    {
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pYellowHelper->GetLifeTime() * 0.3f;

        pYellowHelper->SetPosition(coreVector3(-0.9f, LERPS(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pOrangeHelper->GetLifeTime() * 0.5f;

        const coreVector2 vDir = m_Head.GetDirection().xy();
        const coreVector2 vPos = m_Head.GetPosition ().xy() + vDir * (50.0f * fTime);

        pOrangeHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_ORANGE, false);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pRedHelper->GetLifeTime() * 0.3f;

        pRedHelper->SetPosition(coreVector3(0.0f, LerpSmoothRev(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_RED, false);
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = MAX0(pMagentaHelper->GetLifeTime() * 0.5f - 0.6f);

        pMagentaHelper->SetPosition(LERP(coreVector3(-0.2f,-1.3f,0.0f), coreVector3(1.3f,0.2f,0.0f), fTime) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_MAGENTA, false);
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat   fTime = STEP(0.0f*PI, 4.0f*PI, ABS(afCenterSpin[0]));
        const coreFloat   fLen  = LERP(1.3f * SQRT2, 0.9f, SIN(fTime * PI));
        const coreVector2 vDir  = -m_Head.GetPosition().xy().Normalized();

        pPurpleHelper->SetPosition(coreVector3(vDir * fLen, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_PURPLE, false);
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pBlueHelper->GetLifeTime() * 0.2f;

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_BLUE, true);
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = m_aBody[2].GetPosition ().xy();
        const coreVector2 vDir = m_aBody[2].GetDirection().xy();

        pCyanHelper->SetPosition(coreVector3(vPos + vDir * 7.0f, 0.0f));

        if((pCyanHelper->GetLifeTime() > 5.0f) && !g_pForeground->IsVisiblePoint(pCyanHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_CYAN, false);
        }
        else if(!m_bActive)
        {
            this->_KillHelper(ELEMENT_CYAN, true);
        }
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pGreenHelper->GetLifeTime() * 0.5f;

        pGreenHelper->SetPosition(coreVector3(1.3f - 0.5f * SIN(fTime * (1.0f*PI)), 0.0f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_GREEN, false);
    }
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__EnableRay(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)
    coreObject3D* pRay  = (*m_Ray    .List())[iIndex];
    coreObject3D* pWave = (*m_RayWave.List())[iIndex];

    // 
    WARN_IF(pRay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableRay(iIndex, false);

    // 
    m_afRayTime[iIndex] = bAnimated ? 0.0f : 1.0f;
    m_afRayLen [iIndex] = 1.0f;

    // 
    if(bAnimated) g_pSpecialEffects->PlaySound(m_Head.GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);

    // 
    if(!bAnimated) this->__BeginRay(iIndex);

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetColor3 (COLOR_ENERGY_ORANGE);
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pRay);
    nInitFunc(pWave);

    if(bAnimated)
    {
        // 
        const cEnemy*     pPart = this->__GetPart(iIndex);
        const coreVector3 vPos  = pPart->GetPosition();
        const coreVector3 vDir  = pPart->GetDirection();

        // 
        g_pSpecialEffects->CreateSplashColor(vPos + vDir * LEVIATHAN_RAY_OFFSET(iIndex), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);
    }
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
    REMOVE_BIT(m_iRayState, iIndex)

    // 
    if(!m_iRayState)
    {
        if(m_pLaserSound->EnableRef(this)) m_pLaserSound->Stop();
    }

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
        const coreUintW   iNum  = F_TO_UI(pRay->GetSize().y * 1.5f);

        // 
        for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (LEVIATHAN_RAY_OFFSET(iIndex) + 2.0f*I_TO_F(j) / 1.5f), 10.0f, 1u, COLOR_ENERGY_ORANGE);
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__BeginRay(const coreUintW iIndex)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)
    coreObject3D* pRay  = (*m_Ray.List())[iIndex];

    // 
    m_afRayLen[iIndex] = 0.0f;
    pRay->SetSize(pRay->GetSize() * coreVector3(1.0f,0.0f,1.0f));

    // 
    pRay->ChangeType(TYPE_LEVIATHAN_RAY);

    // 
    if(!m_iRayState)
    {
        g_pSpecialEffects->ExternPlayPosition(m_pLaserSound, this, 0.0f, 0.5f, true, SOUND_EFFECT, coreVector3(0.0f,0.0f,0.0f));
    }

    // 
    ADD_BIT(m_iRayState, iIndex)
    STATIC_ASSERT(LEVIATHAN_RAYS <= sizeof(m_iRayState)*8u)
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
        while(true)
        {
            // 
            const coreVector3 vDiff = vIntersect - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

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
                       else g_pWindscreen                  ->AddObject(pObject, vDecalPos, 2.0f, 128u, "effect_decal_single_inst_program", LIST_KEY);

                // 
                g_pSpecialEffects->CreateSplashFire (vNewHit,  5.0f, bGround ? 3u : 6u, COLOR_FIRE_ORANGE);
                g_pSpecialEffects->CreateSplashColor(vNewHit, 25.0f, bGround ? 2u : 4u, COLOR_FIRE_ORANGE);
            }

            // 
            const coreFloat fStrength = 0.1f + 0.3f * SIN(PI * fTime);
            g_pSpecialEffects->ShakeScreen(fStrength);
            g_pSpecialEffects->RumblePlayer(NULL, fStrength, 250u);

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    if(bGround) vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[OVERDRIVE_HIT + iIndex].xyz(vOldHit);
}


// ****************************************************************
// 
void cLeviathanBoss::__EnableMark(const coreUintW iIndex)
{
    ASSERT(iIndex < LEVIATHAN_MARKS)
    coreObject3D& oMark = m_aMarkRaw[iIndex];

    // 
    WARN_IF(oMark.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableMark(iIndex, false);

    // 
    oMark.SetAlpha  (0.0f);
    oMark.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cLeviathanBoss::__DisableMark(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < LEVIATHAN_MARKS)
    coreObject3D& oMark = m_aMarkRaw[iIndex];

    // 
    if(!oMark.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oMark.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(oMark.GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_WHITE * 0.8f);
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
        iNewDamage += pPart->GetLostHealth();

        if(pPart->ReachedDeath())
        {
            // 
            pPart->SetColor3(COLOR_SHIP_BLACK);
            pPart->AddStatus(ENEMY_STATUS_INVINCIBLE);

            // 
            this->__EnableMark(i);

            // 
            g_pSpecialEffects->MacroExplosionDarkSmall(pPart->GetPosition());
            g_pSpecialEffects->PlaySound(pPart->GetPosition(), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
        }
    }

    // 
    this->SetCurHealth(this->GetMaxHealth() - m_aiCounter[OLD_DAMAGE] - iNewDamage);
}


// ****************************************************************
// 
void cLeviathanBoss::__RefreshHealth(const coreInt32 iHead, const coreInt32 iBody, const coreInt32 iTail)
{
    // 
    const coreInt32 aiHealth[] = {iHead, iBody, iBody, iBody, iTail};
    STATIC_ASSERT(ARRAY_SIZE(aiHealth) == LEVIATHAN_PARTS)

    // 
    m_aiCounter[OLD_DAMAGE] = this->GetLostHealth();

    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        // 
        pPart->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

        // 
        this->__DisableMark(i, false);

        // 
        pPart->SetMaxHealth(aiHealth[i]);
        pPart->SetCurHealth(aiHealth[i]);
        pPart->RefreshColor(1.0f);

        // 
        if(pPart->IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            g_pSpecialEffects->CreateSplashSmoke(pPart->GetPosition(), 30.0f, 30u, coreVector3(1.0f,1.0f,1.0f));
            g_pSpecialEffects->CreateSplashColor(pPart->GetPosition(), 50.0f, 15u, COLOR_ENERGY_WHITE * 0.8f);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
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
    const coreVector3 vPos  = vDir * mRota;

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
    ASSERT(iIndex < LEVIATHAN_PARTS)

    // 
    return (I_TO_F(iIndex) * 0.17f + (iIndex ? 0.02f : 0.0f)) * 1.2f;
}