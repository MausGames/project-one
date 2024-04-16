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
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);

            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pMission->SetContainerClamp(true);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        constexpr coreFloat fHealth = 0.1f;

        PHASE_CONTROL_TICKER(0u, 0u, 20.0f, LERP_LINEAR)
        {
          //  if((iTick / 5u) & 0x01u) return;

            const coreVector2 vDir  = this->GetDirection().xy();
            const coreVector2 vPos  = this->GetPosition ().xy() - vDir * 5.0f;
            const coreVector2 vSide = vDir.Rotated90() * 5.0f;

            //if((iTick / 5u) & 0x01u) g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos + vSide, -vDir)->ChangeSize(1.2f);
            //                    else g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos - vSide, -vDir)->ChangeSize(1.2f);

            const coreVector2 vPlayerPos = this->NearestPlayerSide()->GetPosition().xy();
            if((iTick / 5u) & 0x01u) g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos + vSide, (vPlayerPos - (vPos + vSide)).Normalized())->ChangeSize(1.2f);
                                else g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos - vSide, (vPlayerPos - (vPos - vSide)).Normalized())->ChangeSize(1.2f);

            //g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, (this->NearestPlayer()->GetPosition().xy() - this->GetPosition().xy()).Normalized())->ChangeSize(1.2f);
        });

        PHASE_CONTROL_TICKER(1u, 0u, 0.6f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();

            const coreUint16 iType = 1u << (CLAMP(F_TO_UI((1.0f - this->GetCurHealthPct()) * (4.0f / fHealth)), 1u, 3u) - 1u);

            const coreUintW iInc = MIN(iTick, 24u);
            for(coreUintW i = 24u - iInc, ie = 24u + iInc; i < ie; ++i)
            {
                if(((i / iType) + (iTick & 0x01u)) & 0x01u) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 5.0f + 60.0f));

               // g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.2f, this, vPos, vDir)->ChangeSize(1.15f);
            }
        });

        if((this->GetCurHealthPct() <= (1.0f - fHealth)) && coreMath::IsNear(COS(m_fMovement), 0.0f, 0.01f))
        {
            PHASE_CHANGE_TO(10u)

            if(this->GetPosition().x < 0.0f) m_aiCounter[SPIN_SIDE] = 1;
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            const coreFloat fSpinSign = m_aiCounter[SPIN_SIDE] ? -1.0f : 1.0f;

            if(PHASE_TIME_BEFORE(0.5f)) this->DefaultMoveLerp(m_vLastPosition,        coreVector2(0.0f,0.0f),  LERPS(0.0f, 1.0f, (fTime)      * 2.0f));
                                   else this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.75f), LERPS(0.0f, 1.0f, (fTime-0.5f) * 2.0f));

            this->DefaultRotateLerp(0.0f*PI, fSpinSign * 31.0f*PI, LERPS(0.0f, 1.0f, fTime));

            if(PHASE_TIME_POINT(0.5f))
            {
                m_aiCounter[CONTAINER_ATTACHED] = 0;
                m_aiCounter[IMPACT_COUNT]       = 0;
                pMission->SetContainerForce(coreVector2(fSpinSign * 550.0f, 0.0f));
                pMission->SetContainerClamp(true);
            }

            PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(LERPS(0.0f*PI, fSpinSign * 15.0f*PI, fTime));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos,  vDir)->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos, -vDir)->ChangeSize(1.1f);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(20u)
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const cPlayer*  pPlayer = this->NearestPlayerDual(m_aiCounter[DASH_SIDE]);
            const coreFloat fAim    = (pPlayer->GetPosition().x - this->GetPosition().x) * TIME * fTime * 3.0f;

            this->SetPosition         (coreVector3(this->GetPosition().x + fAim, this->GetPosition().y, 0.0f));
            this->DefaultOrientateLerp(0.0f*PI, 20.0f*PI, LERPB(0.0f, 1.0f, 1.0f - fTime));

            PHASE_CONTROL_TICKER(1u, 0u, 1.8f + 2.0f * fTime, LERP_LINEAR)
            {
                const coreVector2 vDir = this->GetDirection().xy();
                const coreVector2 vPos = this->GetPosition ().xy() + vDir * 5.0f;

                //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos,                                         vDir)->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2( 3.0f,0.0f), vDir)->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2(-3.0f,0.0f), vDir)->ChangeSize(1.4f);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 3.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition + coreVector2(0.0f, m_aiCounter[DASH_SIDE] ? -0.1f : 0.1f), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        PHASE_CONTROL_TIMER(0u, 2.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(m_vLastPosition.x, m_aiCounter[DASH_SIDE] ? 1.0f : -1.0f), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                SET_BIT(m_aiCounter[STOMP_COUNT], 8u, (m_vLastPosition.x < 0.0f))

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TICKER(1u, 55u, 40.0f, LERP_LINEAR)
        {
            const coreVector2 vPos    = m_vLastPosition * FOREGROUND_AREA;
            const coreVector2 vDir    = coreVector2(0.0f, m_aiCounter[DASH_SIDE] ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(iTick + 3u) * 0.06f * FOREGROUND_AREA.x, 0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos + vOffset, vDir)->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos - vOffset, vDir)->ChangeSize(1.4f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos - vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
        });

        if((m_aiCounter[STOMP_COUNT] & 0xFF) < 2)
        {
            PHASE_CONTROL_TIMER(0u, 1.3f, LERP_BREAK_REV)
            {
                const coreFloat vNewX = m_vLastPosition.x + (HAS_BIT(m_aiCounter[STOMP_COUNT], 8u) ? 0.4f : -0.4f);

                this->SetPosition(coreVector3(coreVector2(LERP(m_vLastPosition.x, vNewX, fTime), m_vLastPosition.y + -0.4f * this->GetDirection().y * SIN(PI * fTime)) * FOREGROUND_AREA, 0.0f));

                if(PHASE_FINISHED)
                {
                    PHASE_RESET(0u)
                    PHASE_RESET(1u)

                    m_aiCounter[STOMP_COUNT] += 1;

                    this->StorePosition();

                    g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                }
            });
        }
        else
        {
            PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
            {
                if(PHASE_TIME_AFTER(0.2f))
                {
                    const coreFloat fDelayTime = LERPS(0.0f, 1.0f, (fTime - 0.2f) / 0.8f);

                    this->DefaultMoveLerp    (m_vLastPosition,                         coreVector2(m_vLastPosition.x, m_aiCounter[DASH_SIDE] ? 0.75f : -0.75f), fDelayTime);
                    if(!m_aiCounter[DASH_SIDE]) this->DefaultMultiateLerp(m_aiCounter[DASH_SIDE] ? (2.0f*PI) : (1.0f*PI), m_aiCounter[DASH_SIDE] ? (3.0f*PI) : (2.0f*PI),                fDelayTime);  
                }

                if(PHASE_FINISHED && !m_aTimer[1].GetStatus())
                {
                    PHASE_CHANGE_TO(m_aiCounter[DASH_SIDE] ? 30u : 20u)
                    PHASE_RESET(1u)

                    m_aiCounter[DASH_SIDE]   = 1 - m_aiCounter[DASH_SIDE];
                    m_aiCounter[STOMP_COUNT] = 0;
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_LINEAR)
        {
            this->DefaultMoveLerp    (m_vLastPosition, (pContainer->GetPosition().xy() - NAUTILUS_ATTACH_DIST * this->GetDirection().xy()) / FOREGROUND_AREA, LERPB(0.0f, 1.0f, fTime));
            //this->DefaultRotateLerp(1.0f*PI,               0.0f*PI, LERPS(0.0f, 1.0f, fTime));

            m_fClawAngle = SIN(PI * fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(10u)

                m_aiCounter[CONTAINER_ATTACHED] = 1;
                m_aiCounter[SPIN_SIDE]          = 1 - m_aiCounter[SPIN_SIDE];

                pMission->SetContainerClamp(false);
            }
        });
    }

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
        this->SetDirection(coreVector3(fCurX - fPrevX, TIME, 0.0f).Normalized()); // TODO 1: not time-portable   
    }

    if(m_aiCounter[CONTAINER_ATTACHED])
    {
        // 
        pContainer->SetPosition (this->GetPosition () + NAUTILUS_ATTACH_DIST * this->GetDirection());
        pContainer->SetDirection(this->GetDirection());
    }

    // 
    const coreVector2 vImpact = pMission->GetContainerImpact();
    if(!vImpact.IsNull())
    {
        if(++m_aiCounter[IMPACT_COUNT] >= 2) pMission->SetContainerForce(coreVector2(0.0f,0.0f));

        for(coreUintW i = 20u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact,  vDir)->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact, -vDir)->ChangeSize(1.3f);
            // TODO 1: lower blue balls get overlapped   
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

    // TODO 1: line movement 
    //const coreVector2 vLinePos = coreVector2(0.0f, 0.1f * SIN(2.0f*PI * 0.08f * coreFloat(Core::System->GetTotalTime()))); 
}

// ****************************************************************
// 
void cNautilusBoss::__CreateInk(const coreUintW iIndex, const coreVector2 vPosition)
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
void cNautilusBoss::__EnableBullet(const coreVector2 vStart, const coreVector2 vEnd)
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