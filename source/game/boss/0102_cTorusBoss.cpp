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
#define ATTACK_STATUS      (1u)
#define BALL_STATUS        (2u)


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");

    // 
    this->SetSize             (coreVector3(1.7f,1.7f,1.7f));
    this->SetCollisionModifier(coreVector3(0.8f,0.8f,1.0f));

    // configure the boss
    this->Configure(2800, COLOR_SHIP_GREY);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        m_aRay[i].DefineModel  ("object_tube.md3");
        m_aRay[i].DefineTexture(0u, "effect_energy.png");
        m_aRay[i].DefineProgram("effect_energy_invert_program");
        m_aRay[i].SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
        m_aRay[i].SetTexSize   (TORUS_RAY_TEXSIZE);
        m_aRay[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
    {
        m_aRayWave[i].DefineModel  ("object_tube.md3");
        m_aRayWave[i].DefineTexture(0u, "effect_energy.png");
        m_aRayWave[i].DefineProgram("effect_energy_direct_program");
        m_aRayWave[i].SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
        m_aRayWave[i].SetTexSize   (coreVector2(0.5f,0.5f));
        m_aRayWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Emitter.DefineModel  ("object_boss_torus_emitter.md3");
    m_Emitter.DefineTexture(0u, "effect_energy.png");
    m_Emitter.DefineProgram("effect_energy_invert_program");
    m_Emitter.SetSize      (this->GetSize());
    m_Emitter.SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
    m_Emitter.SetTexSize   (coreVector2(1.25f,1.25f));
    m_Emitter.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

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
    g_pEnvironment->GetBackground()->AddList(0u, 128u, "effect_decal_inst_program");

    STATIC_ASSERT(ARRAY_SIZE(m_aRay) == ARRAY_SIZE(m_aRayWave))
}


// ****************************************************************
// 
void cTorusBoss::Render()
{
    // 
    if(!m_aiCounter[ATTACK_STATUS])
        cEnemy::Render();
}


// ****************************************************************
// 
void cTorusBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->BindObject(&m_aCircle[i]);
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn(const coreBool bAnimated)
{
    if(!m_aiCounter[BALL_STATUS])
    {
        // 
        s_cast<cViridoMission*>(g_pGame->GetMission())->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalize());
    }
    else
    {
        // 
        for(coreUintW i = 1u, ie = m_aiCounter[BALL_STATUS]; i < ie; ++i)
            s_cast<cViridoMission*>(g_pGame->GetMission())->DisableBall(i, true);
    }

    // 
    g_pGame->GetBulletManagerEnemy()->ClearBullets(bAnimated);   

    // 
    this->__SetRotaAttack(0, bAnimated);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnAttack()
{
    DEPTH_PUSH

    if(m_aiCounter[ATTACK_STATUS])
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            m_aRay[i].Render();
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[i]);

        glDisable(GL_DEPTH_TEST);
        {
            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
                m_aRayWave[i].Render();
        }
        glEnable(GL_DEPTH_TEST);

        // 
        cEnemy::Render();

        // 
        m_Emitter.Render();

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].Render();
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        {
            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
                m_aCircle[i].Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{
    // 
    coreVector2 vNewOri = coreVector2(0.0f, m_aiCounter[ROTATION_DIRECTION] ? -1.0f : 1.0f);

    // 
    m_fAnimation.Update(-1.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            // 
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);

            // 
            if(PHASE_SUB(0.85f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            // 
            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 10u && m_iPhase <= 14u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_SMOOTH)
        {
            // 
            coreVector2 vPosFrom, vPosTo;
            coreFloat   fAngFrom, fAngTo;
            switch(m_iPhase)
            {
            default: ASSERT(false)
            case 10u: vPosFrom = coreVector2( 0.0f,  0.75f); vPosTo = coreVector2( 0.65f, 0.65f); fAngFrom =  0.0f*PI; fAngTo = -0.3f*PI; break;
            case 11u: vPosFrom = coreVector2( 0.65f, 0.65f); vPosTo = coreVector2( 0.0f, -0.65f); fAngFrom = -0.3f*PI; fAngTo = -1.0f*PI; break;
            case 12u: vPosFrom = coreVector2( 0.0f, -0.65f); vPosTo = coreVector2(-0.65f, 0.65f); fAngFrom = +1.0f*PI; fAngTo =  0.3f*PI; break;
            case 13u: vPosFrom = coreVector2(-0.65f, 0.65f); vPosTo = coreVector2( 0.0f,  0.0f);  fAngFrom =  0.3f*PI; fAngTo =  0.0f*PI; break;
            case 14u: vPosFrom = coreVector2( 0.0f,  0.0f);  vPosTo = coreVector2( 0.65f, 0.65f); fAngFrom =  0.0f*PI; fAngTo = -0.3f*PI; break;
            }

            if(m_aiCounter[ROTATION_DIRECTION])
            {
                // 
                this->DefaultMoveLerp(vPosFrom.InvertedX(), vPosTo.InvertedX(), fTime);
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(-LERP(fAngFrom, fAngTo, fTime)));
            }
            else
            {
                // 
                this->DefaultMoveLerp(vPosFrom, vPosTo, fTime);
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(fAngFrom, fAngTo, fTime)));
            }

            // 
            if(PHASE_FINISHED)
                m_iPhase += 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 20u && m_iPhase <= 24u)
    {
        PHASE_CONTROL_TICKER(0u, 36u, 12.0f)
        {
            const coreFloat fAngle = I_TO_F(iTick) * (m_aiCounter[ROTATION_DIRECTION] ? 12.0f : -12.0f);

            // 
            for(coreUintW i = 4u; i--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 90.0f + fAngle));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, this, this->GetPosition().xy(), vDir)->MakeBlue();
            }

            // 
            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 1u, COLOR_ENERGY_BLUE);

            // 
            if(PHASE_FINISHED)
            {
                switch(m_iPhase)
                {
                case 23u: m_iPhase  = 30u; break;   // 
                case 24u: m_iPhase  = 11u; break;   // first move
                default:  m_iPhase -=  9u; break;   // next move
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f)
        {
            ++m_iPhase;

            // 
            this->__SetRotaAttack(1, true);
            this->SetPosition(coreVector3(0.0f,0.0f,0.0f));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_LINEAR)
        {
            const coreFloat fAlpha = 0.6f * (1.0f-fTime);

            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                m_aRay    [i].SetAlpha(fAlpha);
                m_aRayWave[i].SetAlpha(fAlpha);
            }

            // 
            m_Emitter.SetAlpha(fTime);

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                // 
                this->__SetRotaAttack(2, true);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_TIMER(0u, 4.0f, LERP_LINEAR)
        {
            const coreFloat& fLength = fTime;
            const coreFloat  fWidth  = 1.0f + (1.0f-fTime);

            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                m_aRay    [i].SetSize(TORUS_RAY_SIZE     * coreVector3(fWidth, fLength, fWidth));
                m_aRayWave[i].SetSize(TORUS_RAYWAVE_SIZE * coreVector3(fWidth, fLength, fWidth));
            }

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f/4.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/12.0f, LERP_SMOOTH)
        {
            // 
            vNewOri = coreVector2::Direction(PI * fTime + (m_aiCounter[ROTATION_DIRECTION] ? PI : 0.0f));

            // 
            cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
            if(pOutdoor)
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
                {
                    coreObject3D& oRay = m_aRay[i];
                    WARN_IF(!oRay.IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

                    // 
                    const coreVector3& vRayDir   = oRay.GetDirection();
                    const coreVector3  vRayStart = oRay.GetPosition () - oRay.GetSize().y * vRayDir;
                    coreFloat          fRayLen   = TORUS_RAY_SIZE.y;

                    // 
                    if(vRayDir.z < -0.15f)
                    {
                        // 
                        const coreVector3 vIntersect = pOutdoor->RetrieveIntersect(vRayStart, vRayDir);
                        this->__CreateOverdrive(i, vIntersect, fTime, true);

                        // 
                        fRayLen = (vIntersect - vRayStart).Length() * 0.5f;
                    }
                    else if(vRayDir.z > 0.48f)
                    {
                        // 
                        fRayLen = TORUS_RAY_SIZE.y * (1.0f - (vRayDir.z-0.48f));

                        // 
                        const coreVector3 vIntersect = vRayStart + vRayDir * (fRayLen * 2.0f);
                        this->__CreateOverdrive(i, vIntersect, fTime, false);
                    }
                    else m_avVector[i] = coreVector3(0.0f,0.0f,0.0f);

                    // 
                    oRay.SetSize   (coreVector3(TORUS_RAY_SIZE.x, fRayLen, TORUS_RAY_SIZE.z));
                    oRay.SetTexSize(coreVector2(TORUS_RAY_TEXSIZE.x, TORUS_RAY_TEXSIZE.y * (fRayLen / TORUS_RAY_SIZE.y)));
                }
            }

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                // 
                m_aiCounter[ROTATION_DIRECTION] = 1 - m_aiCounter[ROTATION_DIRECTION];
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f/4.0f)
        {
            m_iPhase = (m_aiCounter[BALL_STATUS] < coreInt16(VIRIDO_BALLS)) ? 40u : 14u;

            // 
            this->__SetRotaAttack(0, true);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f)
        {
            ++m_iPhase;

            // 
            const coreVector2 vDir = (m_aiCounter[BALL_STATUS] ? coreVector2(1.0f,0.5f) : coreVector2(-0.5f,1.0f)).Normalize();
            s_cast<cViridoMission*>(g_pGame->GetMission())->EnableBall(m_aiCounter[BALL_STATUS], this->GetPosition().xy(), vDir);

            // 
            ++m_aiCounter[BALL_STATUS];
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f)
        {
            m_iPhase = 14u;
        });
    }

    // ################################################################

    PHASE_CONTROL_TICKER(2u, 0u, 0.5f)
    {
        // 
        for(coreUintW i = 5u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 36.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, this->GetPosition().xy(),  vDir)->MakeGreen();
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, this->GetPosition().xy(),  vDir)->MakeGreen();
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, this->GetPosition().xy(), -vDir)->MakeGreen();
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, this->GetPosition().xy(), -vDir)->MakeGreen();
        }

        // 
        g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
    });

    // 
    const coreVector2 vNewDir = coreVector2::Direction(m_fAnimation * 1.2f);
    this->SetDirection  (coreVector3(vNewDir.x, -vNewOri.y*vNewDir.y, vNewOri.x*vNewDir.y));
    this->SetOrientation(coreVector3(0.0f,       vNewOri.x,           vNewOri.y));

    // pre-calculate rotation quaternion
    coreObject3D::Move();

    // 
    if(m_aiCounter[ATTACK_STATUS])
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
        {
            // 
            const coreVector3 vDir = this->__GetRotaDirection(DEG_TO_RAD(I_TO_F(i) * 120.0f));

            // 
            m_aRay[i].SetPosition   (this->GetPosition() + vDir * (m_aRay[i].GetSize().y + TORUS_RAY_OFFSET));
            m_aRay[i].SetDirection  (vDir);
            m_aRay[i].SetOrientation(this->GetOrientation());
            m_aRay[i].SetColor3     (COLOR_ENERGY_YELLOW * (0.8f - 0.2f * ABS(vDir.z)));
            m_aRay[i].SetTexOffset  (coreVector2(0.4f,0.3f) * m_fAnimation);
            m_aRay[i].Move();

            // 
            m_aRayWave[i].SetPosition   (this->GetPosition() + vDir * (m_aRayWave[i].GetSize().y + TORUS_RAY_OFFSET));
            m_aRayWave[i].SetDirection  (-vDir);
            m_aRayWave[i].SetOrientation(this->GetOrientation());
            m_aRayWave[i].SetTexOffset  (coreVector2(-0.3f,-0.6f) * m_fAnimation);
            m_aRayWave[i].Move();
        }

        // 
        m_Emitter.SetPosition   (this->GetPosition   ());
        m_Emitter.SetDirection  (this->GetDirection  ());
        m_Emitter.SetOrientation(this->GetOrientation());
        m_Emitter.SetTexOffset  (coreVector2(-0.1f,-0.4f) * m_fAnimation);
        m_Emitter.Move();
    }

    // 
    const coreFloat fFade  = 1.0f - FRACT(ABS(m_fAnimation));
    m_aCircle[1].SetSize(this->GetSize() * (0.5f + 0.5f*fFade));
    m_aCircle[1].SetAlpha(fFade);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        // 
        m_aCircle[i].SetPosition   (this->GetPosition   ());
        m_aCircle[i].SetDirection  (this->GetDirection  ());
        m_aCircle[i].SetOrientation(this->GetOrientation());
        m_aCircle[i].SetTexOffset  (coreVector2(0.1f * m_fAnimation, 0.0f));
        m_aCircle[i].Move();
    }

    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_OBJECT(1), [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pRay, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

    });
}


// ****************************************************************
// 
coreVector3 cTorusBoss::__GetRotaDirection(const coreFloat fBaseAngle)
{
    // 
    return m_vRotation.QuatApply(coreVector3(coreVector2::Direction(fBaseAngle), 0.0f)).Normalize();
}


// ****************************************************************
// 
void cTorusBoss::__SetRotaAttack(const coreInt16 iType, const coreBool bAnimated)
{
    if(m_aiCounter[ATTACK_STATUS] == iType) return;
    m_aiCounter[ATTACK_STATUS] = iType;

    // 
    switch(iType)
    {
    // 
    case 1:
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
        {
            // 
            g_pGlow->BindObject(&m_aRay[i]);   // # batch
            m_aRay[i].SetSize   (TORUS_RAY_SIZE);
            m_aRay[i].SetAlpha  (0.0f);
            m_aRay[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
        {
            // 
            g_pGlow->BindObject(&m_aRayWave[i]);   // # batch
            m_aRayWave[i].SetSize   (TORUS_RAYWAVE_SIZE);
            m_aRayWave[i].SetAlpha  (0.0f);
            m_aRayWave[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        // 
        g_pGlow->BindObject(&m_Emitter);
        m_Emitter.SetAlpha  (0.0f);
        m_Emitter.SetEnabled(CORE_OBJECT_ENABLE_ALL);

        // 
        if(bAnimated)
        {
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                // 
                const coreVector3 vDir = this->__GetRotaDirection(DEG_TO_RAD(I_TO_F(i) * 120.0f));
                g_pSpecialEffects->CreateSplashColor(vDir * TORUS_RAY_OFFSET, SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);
            }
        }
        break;
    }

    // 
    case 2:
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
        {
            // 
            m_aRay    [i].SetSize (coreVector3(0.0f,0.0f,0.0f));
            m_aRay    [i].SetAlpha(1.0f);
            m_aRayWave[i].SetSize (coreVector3(0.0f,0.0f,0.0f));
            m_aRayWave[i].SetAlpha(0.85f);
        }

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            this->__EnableRay(i);

        // 
        if(bAnimated)
        {
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                // 
                const coreVector3& vDir = m_aRay[i].GetDirection();
                g_pSpecialEffects->MacroEruptionColorBig(vDir * TORUS_RAY_OFFSET, vDir.xy(), COLOR_ENERGY_YELLOW);
            }
        }
        break;
    }

    // 
    case 0:
    {
        // 
        auto nDisableFunc = [](coreObject3D* OUTPUT pObject)
        {
            // 
            if(pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->UnbindObject(pObject);
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay);     ++i) nDisableFunc(&m_aRay    [i]);
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i) nDisableFunc(&m_aRayWave[i]);
        nDisableFunc(&m_Emitter);

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            this->__DisableRay(i);

        // 
        if(bAnimated)
        {
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                // 
                const coreVector3& vDir = m_aRay[i].GetDirection();
                for(coreUintW j = 25u; j--; ) g_pSpecialEffects->CreateSplashColor(vDir * (TORUS_RAY_OFFSET + 2.0f*I_TO_F(j)), 10.0f, 1u, COLOR_ENERGY_YELLOW);
            }
        }
        break;
    }

    default:
        ASSERT(false)
        break;
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableRay(const coreUintW iIndex)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay = m_aRay[iIndex];

    // 
    if(oRay.GetType()) return;
    oRay.ChangeType(TYPE_OBJECT(1));
}


// ****************************************************************
// 
void cTorusBoss::__DisableRay(const coreUintW iIndex)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay = m_aRay[iIndex];

    // 
    if(!oRay.GetType()) return;
    oRay.ChangeType(0);
}


// ****************************************************************
// 
void cTorusBoss::__CreateOverdrive(const coreUintW iIndex, const coreVector3& vIntersect, const coreFloat fTime, const coreBool bGround)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))

    // 
    constexpr coreFloat fMin = 2.5f;
    constexpr coreFloat fMax = 5.0f;
    coreVector3& vOldHit = m_avVector[iIndex];

    // 
    if(vOldHit.IsNull()) vOldHit = vIntersect;
    else
    {
    gtScrewYou:

        // 
        const coreVector3 vDiff = vIntersect - vOldHit;
        const coreFloat   fLen  = vDiff.Length();

        // 
        if(fLen > fMin)
        {
            // 
            const coreVector3 vNewHit   = (fLen > fMax) ? LERP(vOldHit, vIntersect, fMax*RCP(fLen)) : vIntersect;
            const coreVector3 vDecalPos = (vOldHit + vNewHit) * 0.5f;

            // 
            if(ABS(vDecalPos.x) < BACKGROUND_OBJECT_RANGE &&
               ABS(vDecalPos.y) < BACKGROUND_OBJECT_RANGE)
            {
                if(bGround)
                {
                    const coreBool    bRotated   = Core::Rand->Bool();
                    const coreVector2 vDecalSize = coreVector2(Core::Rand->Float(5.0f, 6.5f), MIN(fLen, fMax)*1.8f);
                    const coreVector2 vDecalDir  = vDiff.xy().Normalize();

                    // load object resources
                    coreObject3D* pObject = new coreObject3D();
                    pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                    pObject->DefineTexture(0u, "effect_soot.png");
                    pObject->DefineProgram("effect_decal_program");

                    // set object properties
                    pObject->SetSize     (coreVector3(bRotated ? vDecalSize.yx()       : vDecalSize, 1.0f));
                    pObject->SetDirection(coreVector3(bRotated ? vDecalDir.Rotated90() : vDecalDir,  0.0f));

                    // add object to the background
                    g_pEnvironment->GetBackground()->AddObject(pObject, vDecalPos, 0u);
                }

                // 
                g_pSpecialEffects->CreateSplashFire (vNewHit,  5.0f, 2u);
                g_pSpecialEffects->CreateSplashColor(vNewHit, 25.0f, 2u, COLOR_FIRE_ORANGE);
            }

            // 
            g_pSpecialEffects->ShakeScreen(0.1f + 0.55f * SIN(PI * fTime));

            // 
            vOldHit = vNewHit;
            goto gtScrewYou;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * Core::System->GetTime() * OUTDOOR_DETAIL;
}