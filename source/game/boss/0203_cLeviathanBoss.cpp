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
}


// ****************************************************************
// 
void cLeviathanBoss::Render()
{
    // 
  //  if(!m_aiCounter[ATTACK_STATUS])
        this->cEnemy::Render();
}


// ****************************************************************
// 
void cLeviathanBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 1u; i < LEVIATHAN_PARTS; ++i)
        this->__GetPart(i)->Resurrect();
}


// ****************************************************************
// 
void cLeviathanBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 1u; i < LEVIATHAN_PARTS; ++i)
        this->__GetPart(i)->Kill(bAnimated);

    // 
    this->__SetRotaAttack(0, bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cLeviathanBoss::__RenderOwnAttack()
{
    DEPTH_PUSH

    if(m_aiCounter[RENDER_TYPE])
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
        //this->cEnemy::Render();
    }
}


// ****************************************************************
// 
void cLeviathanBoss::__MoveOwn()
{
    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                m_iPhase = 30u;//++m_iPhase;   
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            ++m_iPhase;

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

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                m_aRay    [i].SetAlpha(fAlpha);
                m_aRayWave[i].SetAlpha(fAlpha);
            }

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

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

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                m_aRay    [i].SetSize(TORUS_RAY_SIZE     * coreVector3(fWidth, fLength, fWidth));
                m_aRayWave[i].SetSize(TORUS_RAYWAVE_SIZE * coreVector3(fWidth, fLength, fWidth));
            }

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f/4.0f)
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
            cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
            if(pOutdoor)
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
                {
                    coreObject3D& oRay = m_aRay[i];
                    WARN_IF(!oRay.IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

                    const coreVector3& vRayDir   = oRay.GetDirection();
                    const coreVector3  vRayStart = oRay.GetPosition () - oRay.GetSize().y * vRayDir;
                    coreFloat          fRayLen   = TORUS_RAY_SIZE.y;

                    if(vRayDir.z < -0.15f)
                    {
                        const coreVector3 vIntersect = pOutdoor->RetrieveIntersect(vRayStart, vRayDir);
                        //this->__CreateOverdrive(i, vIntersect, fTime, true);

                        fRayLen = (vIntersect - vRayStart).Length() * 0.5f;
                    }
                    else if(vRayDir.z > 0.48f)
                    {
                        fRayLen = TORUS_RAY_SIZE.y * (1.0f - (vRayDir.z-0.48f));

                        const coreVector3 vIntersect = vRayStart + vRayDir * (fRayLen * 2.0f);
                        //this->__CreateOverdrive(i, vIntersect, fTime, false);
                    }
                    else m_avVector[OVERDRIVE_HIT + i] = coreVector3(0.0f,0.0f,0.0f);

                    oRay.SetSize   (coreVector3(TORUS_RAY_SIZE.x, fRayLen, TORUS_RAY_SIZE.z));
                    oRay.SetTexSize(coreVector2(TORUS_RAY_TEXSIZE.x, TORUS_RAY_TEXSIZE.y * (fRayLen / TORUS_RAY_SIZE.y)));
                }
            }

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                m_aiCounter[ROTATION_DIRECTION] = 1 - m_aiCounter[ROTATION_DIRECTION];
            }
        });
    }

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

    //for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    //{
    //    cEnemy* pPart = this->__GetPart(i);
    //
    //    // 
    //    const coreFloat fAngle = m_fAnimation - (I_TO_F(i)*0.17f + (i ? 0.02f : 0.0f)) * 0.4f * PI;
    //
    //    // 
    //    coreVector3 vPos, vDir;
    //    cLeviathanBoss::__CalcCurvePosDir(coreVector3(-1.0f, 0.5f * (m_aiCounter[JUMP_SIDE] ? -1.0f : 1.0f), 0.0f).Normalized(), fAngle, coreVector3(50.0f,50.0f,35.0f), &vPos, &vDir);
    //    vPos = vPos + coreVector3(0.0f, -20.0f - m_avVector[FALL_BEHIND].x, -20.0f);
    //
    //    // 
    //    pPart->SetPosition  (vPos);
    //    pPart->SetDirection (vDir);
    //    pPart->DefaultAxiate(m_fAnimation * ((i & 0x01u) ? 1.0f : -1.0f));
    //}
    //
    //for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    //{
    //    cEnemy* pPart = this->__GetPart(i);
    //
    //    // 
    //    const coreFloat fHeight = g_pEnvironment->GetBackground()->GetOutdoor()->RetrieveHeight(pPart->GetPosition().xy());
    //
    //    // 
    //    const coreBool iOldEnabled =  pPart->IsEnabled(CORE_OBJECT_ENABLE_ALL);
    //    const coreBool iNewEnabled = (pPart->GetPosition().z > fHeight);
    //
    //    // 
    //    pPart->SetEnabled(iNewEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    //
    //    // 
    //    if(iOldEnabled != iNewEnabled)
    //    {
    //        //g_pSpecialEffects->MacroExplosionPhysicalSmall(coreVector3(pPart->GetPosition().xy(), fHeight));
    //        //g_pSpecialEffects->CreateSplashSmoke(coreVector3(pPart->GetPosition().xy(), fHeight), 25.0f, 30u);
    //    }
    //}

    for(coreUintW i = 0u; i < LEVIATHAN_PARTS; ++i)
    {
        cEnemy* pPart = this->__GetPart(i);

        const coreFloat fAngle = 2.0f * coreFloat(Core::System->GetTotalTime()) - 3.5f * (I_TO_F(i)*0.17f + (i ? 0.02f : 0.0f)) * 0.4f * PI;

        coreVector3 vPos, vDir;
        cLeviathanBoss::__CalcCurvePosDir(coreVector3(0.0f,0.0f,1.0f), fAngle, coreVector3(15.0f,15.0f,1.0f), &vPos, &vDir);

        pPart->SetPosition  (vPos);
        pPart->SetDirection (vDir);
        pPart->DefaultAxiate(m_fAnimation * ((i & 0x01u) ? 1.0f : -1.0f));
    }



#undef TORUS_RAY_OFFSET
#define TORUS_RAY_OFFSET (i ? 3.0f : 4.0f)

    if(m_aiCounter[ATTACK_STATUS])
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
        {
            cEnemy* pPart = this->__GetPart(i);

            const coreVector3 &vPos  = pPart->GetPosition();
            const coreVector3 &vDir  = pPart->GetDirection();
            coreVector3       vColor = coreMath::IsNear(vDir.z, 0.0f, 0.15f) ? (COLOR_ENERGY_YELLOW * (0.8f)) :
                                                                               (COLOR_ENERGY_BLUE   * (0.8f - 0.4f * ABS(vDir.z)));

            // 
            vColor = LERP(m_aRay[i].GetColor3(), vColor, 0.3f);
            STATIC_ASSERT(FRAMERATE_VALUE == 60.0f)

            // 
            m_aRay[i].SetPosition   (vPos + vDir * (m_aRay[i].GetSize().y + TORUS_RAY_OFFSET));
            m_aRay[i].SetDirection  (vDir);
            m_aRay[i].SetOrientation(this->GetOrientation());
            m_aRay[i].SetColor3     (vColor);
            m_aRay[i].SetTexOffset  (coreVector2(0.4f,0.3f) * m_fAnimation);
            m_aRay[i].Move();

            // 
            m_aRayWave[i].SetPosition   (vPos + vDir * (m_aRayWave[i].GetSize().y + TORUS_RAY_OFFSET));
            m_aRayWave[i].SetDirection  (-vDir);
            m_aRayWave[i].SetOrientation(this->GetOrientation());
            m_aRayWave[i].SetColor3     (vColor);
            m_aRayWave[i].SetTexOffset  (coreVector2(-0.3f,-0.6f) * m_fAnimation);
            m_aRayWave[i].Move();
        }
    }

}


// ****************************************************************
// 
void cLeviathanBoss::__SetRotaAttack(const coreInt16 iType, const coreBool bAnimated)
{
    if(m_aiCounter[ATTACK_STATUS] == iType) return;
    m_aiCounter[ATTACK_STATUS] = iType;
    m_aiCounter[RENDER_TYPE]   = iType;

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
        if(bAnimated)
        {
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                // 
                const coreVector3 vDir = this->__GetPart(i)->GetDirection();
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
void cLeviathanBoss::__EnableRay(const coreUintW iIndex)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay = m_aRay[iIndex];

    // 
    if(oRay.GetType()) return;
    oRay.ChangeType(TYPE_OBJECT(1));
}


// ****************************************************************
// 
void cLeviathanBoss::__DisableRay(const coreUintW iIndex)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aRay))
    coreObject3D& oRay = m_aRay[iIndex];

    // 
    if(!oRay.GetType()) return;
    oRay.ChangeType(0);
}


// ****************************************************************
// 
FUNC_NOALIAS void cLeviathanBoss::__CalcCurvePosDir(const coreVector3& vAxis, const coreFloat fAngle, const coreVector3& vScale, coreVector3* OUTPUT vPosition, coreVector3* OUTPUT vDirection)
{
    ASSERT(vAxis.IsNormalized() && vPosition && vDirection)

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(fAngle, vAxis).m123();
    const coreVector3 vDir  = coreVector3(1.0f,0.0f,0.0f);//coreVector3(vAxis.xy().Normalized().Rotated90(), 0.0f);
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


// laser in sinus-form bewegen, entweder unabhängig oder all gleich
// (wärend background sich bewegt) intro durch springen aus boden, fliegt mit spieler mit, fliegt in boden wieder rein, kommt sofort wieder raus, wie delfin, background stoppt, boss beginnt
// wenn der boss stirbt, knallt er in den behällter der mit viel licht aufspringt (in 2 teile) und P1 kommt zum vorschein, der bildschirm wird hell und mission 3 startet