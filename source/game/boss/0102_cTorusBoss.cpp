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
#define RAY_STATUS         (2u)


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModel   ("ship_boss_torus_high.md3");
    this->DefineModelLow("ship_boss_torus_low.md3");

    // 
    this->SetSize             (coreVector3(1.7f,1.7f,1.7f));
    this->SetCollisionModifier(coreVector3(0.8f,0.8f,1.0f));

    // configure the boss
    this->Configure(10000, coreVector3(0.0f/360.0f, 0.0f/100.0f, 60.0f/100.0f).HSVtoRGB());

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        m_aRay[i].DefineModel  ("object_tube.md3");
        m_aRay[i].DefineTexture(0u, "effect_energy.png");
        m_aRay[i].DefineProgram("effect_energy_invert_program");
        m_aRay[i].SetColor3    (coreVector3(0.9f,0.8f,0.4f) * 0.8f);
        m_aRay[i].SetTexSize   (TORUS_RAY_TEXSIZE);
        m_aRay[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
    {
        m_aRayWave[i].DefineModel  ("object_tube.md3");
        m_aRayWave[i].DefineTexture(0u, "effect_energy.png");
        m_aRayWave[i].DefineProgram("effect_energy_direct_program");
        m_aRayWave[i].SetColor3    (coreVector3(0.9f,0.8f,0.4f) * 0.8f);
        m_aRayWave[i].SetTexSize   (coreVector2(0.5f,0.5f));
        m_aRayWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Emitter.DefineModel  ("object_circle_03.md3");
    m_Emitter.DefineTexture(0u, "effect_energy.png");
    m_Emitter.DefineProgram("effect_energy_invert_program");
    m_Emitter.SetSize      (this->GetSize());
    m_Emitter.SetColor3    (coreVector3(0.9f,0.8f,0.4f) * 0.8f);
    m_Emitter.SetTexSize   (coreVector2(1.25f,1.25f));
    m_Emitter.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].DefineModel  ("object_circle_02.md3");
        m_aCircle[i].DefineTexture(0u, "effect_energy.png");
        m_aCircle[i].DefineProgram("effect_energy_invert_program");
        m_aCircle[i].SetSize      (this->GetSize());
        m_aCircle[i].SetColor3    (coreVector3(0.3f,0.7f,0.3f) * 0.8f);
        m_aCircle[i].SetTexSize   (coreVector2(1.25f,1.25f));
    }

    // 
    g_pEnvironment->GetBackground()->AddList(0u, 128u, "effect_decal_inst_program");

    STATIC_ASSERT(ARRAY_SIZE(m_aRay) == ARRAY_SIZE(m_aRayWave))
}


// ****************************************************************
// destructor
cTorusBoss::~cTorusBoss()
{
    // 
    this->__SetRotaAttack(0, false);
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
void cTorusBoss::__KillOwn()
{
    // 
    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

    // 
    this->__SetRotaAttack(0, true);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnWeak()
{
    glDepthRange(0.0f, 0.998f);   // slight depth offset to prevent z-fighting
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].Render();

        // 
        if(!m_aiCounter[RAY_STATUS])
        {
            glDisable(GL_CULL_FACE);
            glDepthMask(false);
            {
                // 
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
                    m_aRay[i].Render();

                // 
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
                    m_aRayWave[i].Render();

                // 
                m_Emitter.Render();
            }
            glEnable(GL_CULL_FACE);
            glDepthMask(true);
        }
    }
    glDepthRange(GAME_DEPTH_WEAK);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnStrong()
{
    // 
    if(m_aiCounter[RAY_STATUS])
    {
        glDepthFunc(GL_ALWAYS);
        {
            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
                m_aRay[i].Render();
        }
        glDepthFunc(GL_LEQUAL);
    }
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnAfter()
{
    // 
    if(m_aiCounter[RAY_STATUS])
    {
        glDepthFunc(GL_ALWAYS);
        {
            // 
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
                m_aRayWave[i].Render();

            // 
            m_Emitter.Render();
        }
        glDepthFunc(GL_LEQUAL);
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

            // 
            this->DefaultMoveLerp(vPosFrom, vPosTo, fTime);

            // 
            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(fAngFrom, fAngTo, fTime)));

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
            for(coreUintW i = 3u; i--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 120.0f + fAngle));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, this, this->GetPosition().xy(), vDir);
            }

            // 
            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 1u, coreVector3(0.09f,0.387f,0.9f));

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
        PHASE_CONTROL_TIMER(0u, 1.0f/6.0f, LERP_LINEAR)
        {
            // 
            if(PHASE_FINISHED)
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
            cBackground* pBackground = g_pEnvironment->GetBackground();
            if(pBackground->GetOutdoor())
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
                {
                    coreObject3D& oRay = m_aRay[i];

                    // 
                    const coreVector3& vDir = oRay.GetDirection();
                    const coreVector3  vPos = oRay.GetPosition () - oRay.GetSize().y * vDir;

                    // 
                    if(vDir.z < -0.1f)
                    {
                        const coreVector3 vIntersect = pBackground->GetOutdoor()->RetrieveIntersect(vPos, vDir);

                        if(m_avVector[i].z)
                        {
                            const coreVector3 vDiff = vIntersect - m_avVector[i];
                            const coreFloat   fLen  = vDiff.Length();

                            if(fLen > 2.5f)
                            {
                                const coreVector3 vRealPoint = (fLen > 5.0f) ? (m_avVector[i] + vDiff.Normalized()*5.0f) : vIntersect;
                                const coreVector3 vNewPos = (m_avVector[i] + vRealPoint) * 0.5f;

                                coreObject3D* pObject = new coreObject3D();
                                pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                                pObject->DefineTexture(0u, "effect_soot.png");
                                pObject->DefineProgram("effect_decal_program");

                                const coreBool bRota = Core::Rand->Bool();
                                const coreVector2 vDir  = vDiff.xy().Normalize();
                                const coreVector2 vSize = coreVector2(Core::Rand->Float(5.0f, 6.5f), MIN(fLen, 5.0f)*1.8f);

                                // set object properties
                                pObject->SetSize     (coreVector3(bRota ? vSize.yx()       : vSize, 1.0f));
                                pObject->SetDirection(coreVector3(bRota ? vDir.Rotated90() : vDir,  0.0f));

                                pBackground->AddObject(pObject, vNewPos, 0u);

                                g_pSpecialEffects->CreateSplashFire (vNewPos,  5.0f, 2u);
                                g_pSpecialEffects->CreateSplashColor(vNewPos, 25.0f, 2u, COLOR_FIRE_F);
                                g_pSpecialEffects->ShakeScreen      (0.05f + 0.45f * SIN(PI * fTime));

                                m_avVector[i] = vRealPoint;
                            }
                        }
                        else m_avVector[i] = vIntersect;


                        m_avVector[i].y -= g_pEnvironment->GetSpeed() * Core::System->GetTime() * OUTDOOR_DETAIL;

                        const coreFloat fLen = (vPos - vIntersect).Length() * 0.5f;

                        oRay.SetSize   (coreVector3(TORUS_RAY_SIZE.x, fLen, TORUS_RAY_SIZE.z));
                        oRay.SetTexSize(coreVector2(TORUS_RAY_TEXSIZE.x, TORUS_RAY_TEXSIZE.y * (fLen / TORUS_RAY_SIZE.y)));
                    }
                    else
                    {
                        oRay.SetSize   (TORUS_RAY_SIZE);
                        oRay.SetTexSize(TORUS_RAY_TEXSIZE);

                        m_avVector[i] = coreVector3(0.0f,0.0f,0.0f);
                    }
                }
            }





            // 
            if(PHASE_SUB(0.1f)) this->__SetRotaAttack(3, true);
            if(PHASE_SUB(0.9f)) this->__SetRotaAttack(4, true);

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
        PHASE_CONTROL_TIMER(0u, 1.0f/6.0f, LERP_LINEAR)
        {
            if(PHASE_FINISHED)
            {
                m_iPhase = 14u;

                // 
                this->__SetRotaAttack(0, true);
            }
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
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, this->GetPosition().xy(),  vDir);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, this->GetPosition().xy(),  vDir);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, this->GetPosition().xy(), -vDir);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, this->GetPosition().xy(), -vDir);
        }

        // 
        g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, coreVector3(0.3f,0.7f,0.3f));
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
            m_aRay[i].SetTexOffset  (coreVector2(0.4f,0.1f) * m_fAnimation);
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
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_OBJECT(0), [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pRay, const coreBool& bFirst)
    {
        if(!bFirst) return;

    });
}


// ****************************************************************
// 
coreVector3 cTorusBoss::__GetRotaDirection(const coreFloat& fBaseAngle)
{
    // TODO # change it into a central function if needed elsewhere 
    // 
    return m_vRotation.QuatApply(coreVector3(coreVector2::Direction(fBaseAngle), 0.0f)).Normalize();
}


// ****************************************************************
// 
void cTorusBoss::__SetRotaAttack(const coreInt16& iType, const coreBool& bAnimated)
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
            g_pGlow->BindObject(&m_aRay[i]);
            m_aRay[i].SetSize   (TORUS_RAY_SIZE);
            m_aRay[i].SetAlpha  (0.0f);
            m_aRay[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRayWave); ++i)
        {
            // 
            g_pGlow->BindObject(&m_aRayWave[i]);
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
                g_pSpecialEffects->CreateSplashColor(vDir * TORUS_RAY_OFFSET, SPECIAL_SPLASH_TINY, m_aRay[i].GetColor3());
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
            m_aRay    [i].SetSize(coreVector3(0.0f,0.0f,0.0f));
            m_aRayWave[i].SetSize(coreVector3(0.0f,0.0f,0.0f));
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
                const coreVector3  vCol = m_aRay[i].GetColor3();
                g_pDistortion->CreateBurst        (vDir * TORUS_RAY_OFFSET, vDir.xy(), DISTORTION_WAVE_SMALL);
                g_pSpecialEffects->CreateBlowColor(vDir * TORUS_RAY_OFFSET, vDir,      SPECIAL_BLOW_SMALL, vCol);

            }
        }

        break;
    }

    // 
    case 3:
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            this->__DisableRay(i);

        break;
    }

    // 
    case 4:
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            this->__EnableRay(i);

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
                const coreVector3  vCol = m_aRay[i].GetColor3();
                for(coreUintW j = 25u; j--; ) g_pSpecialEffects->CreateSplashColor(vDir * (TORUS_RAY_OFFSET + 2.0f*I_TO_F(j)), 10.0f, 1u, vCol);
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
void cTorusBoss::__EnableRay(const coreUintW& iIndex)
{
    if(CONTAINS_BIT(m_aiCounter[RAY_STATUS], iIndex)) return;
    ADD_BIT(m_aiCounter[RAY_STATUS], iIndex)

    // 
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay     = m_aRay    [iIndex];
    coreObject3D& oRayWave = m_aRayWave[iIndex];

    // 
    g_aaOutline[PRIO_STRONG][STYLE_FULL].BindObject(&oRay);
    oRay    .ChangeType(TYPE_OBJECT(0));
    oRay    .SetAlpha  (1.0f);
    oRayWave.SetAlpha  (0.85f);
}


// ****************************************************************
// 
void cTorusBoss::__DisableRay(const coreUintW& iIndex)
{
    if(!CONTAINS_BIT(m_aiCounter[RAY_STATUS], iIndex)) return;
    REMOVE_BIT(m_aiCounter[RAY_STATUS], iIndex)

    // 
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay     = m_aRay    [iIndex];
    coreObject3D& oRayWave = m_aRayWave[iIndex];

    // 
    g_aaOutline[PRIO_STRONG][STYLE_FULL].UnbindObject(&oRay);
    oRay    .ChangeType(0);
    oRay    .SetAlpha  (0.25f);
    oRayWave.SetAlpha  (0.25f);
}