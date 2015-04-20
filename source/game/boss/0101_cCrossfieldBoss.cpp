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
// 
#define DUPLICATE_STATUS (0u)
#define CURRENT_SIDE     (1u)
#define BOOMERANG_TARGET (2u)
#define RESURRECT_STATUS (3u)


// ****************************************************************
// constructor
cCrossfieldBoss::cCrossfieldBoss()noexcept
: m_Boomerang      (CROSSFIELD_BOOMERANGS)
, m_BoomerangTrail (CROSSFIELD_BOOMERANGS * CROSSFIELD_TRAILS)
, m_iPackedDir     (0u)
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModel   ("ship_boss_crossfield_high.md3");
    this->DefineModelLow("ship_boss_crossfield_low.md3");

    // 
    this->SetSize(coreVector3(3.0f,3.0f,3.0f));

    // configure the boss
    this->Configure(500, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());

    // create duplicate object
    m_Duplicate.DefineModel  ("ship_boss_crossfield_high.md3");
    m_Duplicate.DefineTexture(0u, "effect_energy.png");
    m_Duplicate.DefineProgram("effect_energy_invert_program");
    m_Duplicate.SetSize      (this->GetSize());
    m_Duplicate.SetColor3    (COLOR_RED_F * 0.8f);
    m_Duplicate.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // create duplicate trail objects
    for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
    {
        m_aDuplicateTrail[i].DefineModel  ("ship_boss_crossfield_low.md3");
        m_aDuplicateTrail[i].DefineTexture(0u, "effect_energy.png");
        m_aDuplicateTrail[i].DefineProgram("effect_energy_invert_program");
        m_aDuplicateTrail[i].SetSize      (this->GetSize());
        m_aDuplicateTrail[i].SetColor3    (COLOR_RED_F * (0.2f * I_TO_F(i + 1u)));
        m_aDuplicateTrail[i].SetAlpha     (0.25f * I_TO_F(i + 1u));
    }

    // create boomerang lists
    m_Boomerang     .DefineProgram("effect_energy_invert_inst_program");
    m_BoomerangTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CROSSFIELD_BOOMERANGS * (CROSSFIELD_TRAILS + 1u); ++i)
        {
            // determine object type
            const coreUintW iType = i % (CROSSFIELD_TRAILS + 1u);

            // load object resources
            coreObject3D* pBoomerang = new coreObject3D();
            pBoomerang->DefineModel  ("object_boomerang.md3");
            pBoomerang->DefineTexture(0u, "effect_energy.png");
            pBoomerang->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBoomerang->SetSize   (coreVector3(1.4f,1.4f,1.4f));
            pBoomerang->SetColor3 (COLOR_RED_F * (iType ? (0.2f * I_TO_F(iType)) : 0.8f));
            pBoomerang->SetTexSize(coreVector2(1.5f,1.5f));
            pBoomerang->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BoomerangTrail.BindObject(pBoomerang);
                 else m_Boomerang     .BindObject(pBoomerang);
        }
    }
}


// ****************************************************************
// destructor
cCrossfieldBoss::~cCrossfieldBoss()
{
    // 
    for(coreUintW i = 0u; i < CROSSFIELD_BOOMERANGS ; ++i)
        this->__DisableBoomerang(i, false);

    // 
    FOR_EACH(it, *m_Boomerang     .List()) SAFE_DELETE(*it)
    FOR_EACH(it, *m_BoomerangTrail.List()) SAFE_DELETE(*it)

    // 
    m_aiCounter[RESURRECT_STATUS] = 1;
}


// ****************************************************************
// 
void cCrossfieldBoss::__ResurrectOwn()
{
    if(m_aiCounter[RESURRECT_STATUS] != 0)
        return;

    // 
    g_pOutlineFull->BindObject(&m_Duplicate);
    g_pGlow->BindObject(&m_Duplicate);

    // 
    for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
        g_pGlow->BindObject(&m_aDuplicateTrail[i]);

    // 
    g_pOutlineFull->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_BoomerangTrail);
}


// ****************************************************************
// 
void cCrossfieldBoss::__KillOwn()
{
    // 
    for(coreUintW i = 0u; i < CROSSFIELD_BOOMERANGS; ++i)
        this->__DisableBoomerang(i, true);

    // 
    g_pGame->GetBulletManager()->ClearBullets(TYPE_BULLET_ENEMY);

    // 
    if(m_aiCounter[RESURRECT_STATUS] == 0)
    {
        m_aiCounter[RESURRECT_STATUS] = 1;

        // 
        this->DefineProgram(m_Duplicate.GetProgram());
        this->DefineTexture(0u, m_Duplicate.GetTexture(0u));
        this->SetColor3    (m_Duplicate.GetColor3());
        if(m_aiCounter[DUPLICATE_STATUS] != 0)
        {
            this->SetPosition   (m_Duplicate.GetPosition   ());
            this->SetDirection  (m_Duplicate.GetDirection  ());
            this->SetOrientation(m_Duplicate.GetOrientation());
        }

        // 
        m_avVector[0].xy (this->GetPosition   ().xy() / FOREGROUND_AREA);
        m_avVector[1].x = this->GetDirection  ().xy()   .Angle();
        m_avVector[1].y = [](){coreFloat i = g_pEnvironment->GetDirection().Angle(); if(i > PI) i -= 2.0f*PI; return i;}();
        m_avVector[2]   = this->GetOrientation();

        [](void){{}},[]()->void{}();

        // 
        m_Duplicate.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aiCounter[DUPLICATE_STATUS] = 0;
        m_aiCounter[BOOMERANG_TARGET] = 0;

        // 
        this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        coreObject3D::Move();

        // 
        this->Configure(1000, this->GetBaseColor());
        this->Resurrect(this->GetPosition().xy(), this->GetDirection().xy());

        // 
        m_iPhase = 30u;
        PHASE_RESET(0u)
        PHASE_RESET(1u)
        PHASE_RESET(2u)

        return;
    }

    // 
    g_pOutlineFull->UnbindObject(&m_Duplicate);
    g_pGlow->UnbindObject(&m_Duplicate);

    // 
    for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
        g_pGlow->UnbindObject(&m_aDuplicateTrail[i]);

    // 
    g_pOutlineFull->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_BoomerangTrail);
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwnBefore()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
            m_aDuplicateTrail[i].Render();

        // 
        m_BoomerangTrail.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwnAfter()
{
    // 
    m_Duplicate.Render();

    // 
    m_Boomerang.Render();
}


// ****************************************************************
// 
void cCrossfieldBoss::__MoveOwn()
{
    // 
    cPlayer* pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());

    // 
    const coreObject3D* pBase         = m_aiCounter[DUPLICATE_STATUS] ? &m_Duplicate : this;
    const coreVector2   vOldBasePos   = pBase->GetPosition ().xy();
    const coreFloat     fOldBaseAngle = pBase->GetDirection().xy().Angle();

    // 
    m_fAnimation.Update(0.2f);

    // ################################################################
    // intro side rush
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            // 
            this->DefaultMoveLerp  (coreVector2(-0.667f,2.0f), coreVector2(-0.667f,-1.5f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                   6.0f*PI,                    fTime);

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // intro top smooth break
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            // 
            this->DefaultMoveLerp  (coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                5.0f*PI,                 fTime);

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
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u || m_iPhase == 13u)
    {
        PHASE_CONTROL_TIMER(1u, 0.2f, LERP_SMOOTH)
        {
            const coreBool  bSecond   = (m_iPhase == 13u) ? true : false;
            const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fTime : (1.0f - fTime);
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f :  1.0f;

            PHASE_CONTROL_TICKER(0u, 0u, 2.0f + 4.0f * SIN(fTime*PI))   // increase shoot-speed with movement-speed
            {
                // 
                const coreVector2 vDir = coreVector2(0.0f, -SIGN(this->GetPosition().y));
                g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.3f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), vDir);
                g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.4f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), vDir);
                g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.5f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), vDir);

                if(m_aiCounter[DUPLICATE_STATUS])
                {
                    // 
                    const coreVector2 vDir = coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y));
                    g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.3f, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), vDir);
                    g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.4f, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), vDir);
                    g_pGame->GetBulletManager()->AddBullet<cWaveBullet>(5, 1.5f, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), vDir);
                }
            });

            // 
            this->DefaultMoveLerp(coreVector2(bSecond ? (fSideSign *  4.5f) : 0.0f, 0.75f),
                                  coreVector2(           fSideSign * -4.5f,         0.75f), fTime);
            this->DefaultRotateLerp(1.0f*PI, 11.0f*PI, fSideTime);

            // 
            if(m_aiCounter[DUPLICATE_STATUS] == 1)
            {
                // 
                if(PHASE_SUB(0.5f)) m_aiCounter[DUPLICATE_STATUS] = 2;
            }
            if(m_aiCounter[DUPLICATE_STATUS])
            {
                // 
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(fSideSign * (bSecond ? -2.5f*PI : -2.0f*PI) * fTime));

                // 
                if(bSecond && (ABS(this->GetPosition().x) < 2.0f*FOREGROUND_AREA.x))
                    this->SetPosition(this->GetPosition().InvertedY());
            }

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;
                PHASE_RESET(0u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u || m_iPhase == 14u)
    {
        // 
        const coreVector2 vToPlayer = (pPlayer->GetPosition().xy() - pBase->GetPosition().xy()).Normalize();
        const coreBool    bProvoke  = m_aiCounter[DUPLICATE_STATUS] ? (ABS(vToPlayer.x) < ABS(vToPlayer.y*1.1f)) :
                                                                      (ABS(vToPlayer.x) > ABS(vToPlayer.y));

        PHASE_CONTROL_TICKER(0u, 0u, bProvoke ? 6.0f : (m_aiCounter[DUPLICATE_STATUS] ? 1.0f : 2.0f))
        {
            // 
            g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, 1.3f, this, TYPE_BULLET_ENEMY, pBase->GetPosition().xy(), vToPlayer);
        });

        PHASE_CONTROL_TICKER(1u, 0u, 2.0f)
        {
            // 
            const coreVector2 vTan = this->GetDirection().xy().Rotate90() * ((iTick % 2u) ? -1.0f : 1.0f);

            // 
            for(coreUintW i = 6u - MIN(iTick + 1u, 6u); i < 6u; ++i)
            {
                const coreFloat fOffset = I_TO_F(i - 1u);

                // 
                const coreVector2 vPos =  this->GetPosition ().xy() + vTan * (2.0f*fOffset + 2.0f) - this->GetDirection().xy() * (2.0f*fOffset);
                const coreVector2 vDir = (this->GetDirection().xy() + vTan * (0.1f*fOffset)).Normalize();

                // 
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, 1.0f, this, TYPE_BULLET_ENEMY, vPos,             vDir);
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, 1.0f, this, TYPE_BULLET_ENEMY, vPos + vDir*2.5f, vDir);
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, 1.0f, this, TYPE_BULLET_ENEMY, vPos,             -this->GetDirection().xy());
            }
        });

        PHASE_CONTROL_TIMER(2u, (1.0f/10.0f), LERP_SMOOTH)
        {
            const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fTime : (1.0f - fTime);

            // 
            this->DefaultOrientateLerp(0.0f*PI, 20.0f*PI, fSideTime);

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;
                PHASE_RESET(0u)
                PHASE_RESET(1u)
                PHASE_RESET(2u)

                // 
                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u || m_iPhase == 15u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 16u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fOwnTime  = LERPB(1.0f, 0.0f, 1.0f - fTime);
            const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fOwnTime : (1.0f - fOwnTime);
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f    :  1.0f;

            // 
            this->DefaultMoveLerp  (coreVector2(fSideSign * 0.5f, 0.75f), coreVector2(fSideSign * 0.5f, -1.5f), fOwnTime);
            this->DefaultRotateLerp(1.0f*PI,                              5.0f*PI,                              fSideTime);

            // 
            if(m_aiCounter[DUPLICATE_STATUS])
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(fSideSign * 0.5f*PI, 0.0f, fTime)));

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 17u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fTime : (1.0f - fTime);

            // 
            this->DefaultMoveLerp  (coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);
            this->DefaultRotateLerp(1.0f*PI,                5.0f*PI,                 fSideTime);

            // 
            if(PHASE_FINISHED)
                m_iPhase = 20u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 1.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TICKER(0u, 6u, m_Boomerang.GetCurEnabled() ? (2.2f/3.0f) : (1.0f/FRAMERATE_TIME))
        {
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f : 1.0f;

            // 
            if(iTick < CROSSFIELD_BOOMERANGS)
                this->__EnableBoomerang(iTick, this->GetPosition().xy(), coreVector2((iTick % 2u) ? fSideSign : -fSideSign, 0.0f));

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        PHASE_CONTROL_TIMER(0u, (1.0f/15.0f), LERP_SMOOTH)
        {
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f : 1.0f;

            // 
            this->SetPosition      (coreVector3(coreVector2::Direction(LERP(0.0f*PI, fSideSign * -6.0f*PI, fTime)) * FOREGROUND_AREA * 0.75f, 0.0f));
            this->DefaultRotateLerp(1.0f*PI, fSideSign * 25.0f*PI, fTime);

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                // 
                m_aiCounter[BOOMERANG_TARGET] = 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            coreBool bDisabled = true;

            // 
            for(coreUintW i = 0u, ie = m_Boomerang.List()->size(); i < ie; ++i)
            {
                coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
                if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                // 
                bDisabled = false;

                // 
                if((this->GetPosition().xy() - pBoomerang->GetPosition().xy()).LengthSq() < 4.0f)
                    this->__DisableBoomerang(i, true);
            }

            // 
            if(PHASE_FINISHED && bDisabled)
            {
                m_iPhase = m_aiCounter[DUPLICATE_STATUS] ? 24u : 25u;

                // 
                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];

                // 
                m_aiCounter[BOOMERANG_TARGET] = 0;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 24u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, (2.2f/2.0f))
        {
            m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 25u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.2f)
        {
            ++m_iPhase;

            // 
            ASSERT(m_aiCounter[DUPLICATE_STATUS] == 0)
            m_aiCounter[DUPLICATE_STATUS] = 1;

            // 
            m_Duplicate.SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_Duplicate.SetAlpha(0.0f);

            // 
            g_pDistortion    ->CreateWave       (this->GetPosition(), DISTORTION_WAVE_BIG);
            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_RED_F);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 26u)
    {
        PHASE_CONTROL_TIMER(0u, (2.2f/2.0f), LERP_SMOOTH)
        {
            // 
            m_Duplicate.SetAlpha(MIN(fTime*20.0f, 1.0f));

            if(PHASE_FINISHED)
                m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            // 
            this->DefaultMoveLerp  (     m_avVector[0].xy(), 2.0f*m_avVector[0].xy().Normalize(), fTime);
            this->DefaultRotateLerp(     m_avVector[1].x,    3.0f*PI,                             fTime);
            this->SetOrientation   (LERP(m_avVector[2],      coreVector3(0.0f,0.0f,1.0f),         fTime).Normalize());

            // 
            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(m_avVector[1].y, 0.0f*PI, fTime)));

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            // 
            this->DefaultMoveLerp  (coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                5.0f*PI,                 fTime);

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TICKER(0u, 8u, m_Boomerang.GetCurEnabled() ? (2.2f/1.0f) : (1.0f/FRAMERATE_TIME))
        {
            // 
            if(iTick < CROSSFIELD_BOOMERANGS)
                this->__EnableBoomerang(iTick, this->GetPosition().xy(), coreVector2(0.0f,-1.0f));

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_LINEAR)
        {
            const coreFloat fOwnTime = LERPB(1.0f, 0.0f, 1.0f - fTime);

            // 
            this->DefaultMoveLerp  (coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.5f), fOwnTime);
            this->DefaultRotateLerp(1.0f*PI,                 5.0f*PI,                 fOwnTime);

            // 
            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(0.0f*PI, 1.0f*PI, fTime)));

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                // 
                m_aiCounter[CURRENT_SIDE] = 0;
                m_avVector[0].xy(coreVector2(pPlayer->GetPosition().x / FOREGROUND_AREA.x, 2.0f));
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_TIMER(1u, 0.9f/2.0f, LERP_LINEAR)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 3.5f)
            {
                // 
                g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, 1.25f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2( 1.0f,0.0f));
                g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, 1.35f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2( 1.0f,0.0f));
                g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, 1.25f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2(-1.0f,0.0f));
                g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, 1.35f, this, TYPE_BULLET_ENEMY, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2(-1.0f,0.0f));
            });

            // 
            this->DefaultMoveLerp  (m_avVector[0].xy(), coreVector2(m_avVector[0].x, -1.5f), fTime);
            this->DefaultRotateLerp(1.0f*PI,            7.0f*PI,                             fTime);

            // 
            if(PHASE_FINISHED)
            {
                PHASE_RESET(1u)

                // 
                m_aTimer[0].SetValue(m_aiCounter[CURRENT_SIDE] ? 0.0f : 0.5f);
                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];

                // 
                m_avVector[0].xy(coreVector2(pPlayer->GetPosition().x / FOREGROUND_AREA.x, 2.0f));
            }
        });
    }

    // ################################################################

    // 
    coreBool bChange = false;
    this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), 2.0f, &bChange), 0.0f));

    // 
    if(m_aiCounter[RESURRECT_STATUS]) this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // 
    if(m_aiCounter[DUPLICATE_STATUS] == 1)
    {
        // 
        m_Duplicate.SetPosition   ( this->GetPosition   ().InvertedX());
        m_Duplicate.SetDirection  ( this->GetDirection  ().InvertedX());
        m_Duplicate.SetOrientation(-this->GetOrientation());
    }
    else if(m_aiCounter[DUPLICATE_STATUS] == 2)
    {
        // 
        m_Duplicate.SetPosition   (-this->GetPosition   ());
        m_Duplicate.SetDirection  (-this->GetDirection  ());
        m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
    }
    m_Duplicate.SetTexOffset(coreVector2(0.0f, m_fAnimation));
    m_Duplicate.Move();

    // 
    if(Core::System->GetTime())
    {
        const coreFloat fRcpTime = RCP(Core::System->GetTime());

        // 
        const coreVector2 vCurBasePos   = pBase->GetPosition ().xy();
        const coreFloat   fCurBaseAngle = pBase->GetDirection().xy().Angle();
        const coreVector2 vOffsetPos    =          (vCurBasePos -  vOldBasePos)   * fRcpTime * 0.05f;
        const coreFloat   fOffsetAngle  = AngleDiff(fCurBaseAngle, fOldBaseAngle) * fRcpTime * 0.05f;

        for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
        {
            // 
            const coreVector2 vNewPos =                        vCurBasePos   - vOffsetPos   * I_TO_F(CROSSFIELD_TRAILS-i);
            const coreVector2 vNewDir = coreVector2::Direction(fCurBaseAngle - fOffsetAngle * I_TO_F(CROSSFIELD_TRAILS-i));

            // 
            m_aDuplicateTrail[i].SetPosition   (coreVector3(vNewPos, 0.0f));
            m_aDuplicateTrail[i].SetDirection  (coreVector3(vNewDir, 0.0f));
            m_aDuplicateTrail[i].SetOrientation(pBase->GetOrientation());
            m_aDuplicateTrail[i].SetTexOffset  (coreVector2(0.0f, m_fAnimation));
            m_aDuplicateTrail[i].Move();
        }
    }

    // 
    for(coreUintW i = 0u, ie = m_Boomerang.List()->size(); i < ie; ++i)
    {
        coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
        if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vFly = this->__DecodeDirection(i);

        // 
        coreVector2       vNewPos = pBoomerang->GetPosition().xy() + vFly * FOREGROUND_AREA * (1.5f * Core::System->GetTime());
        const coreVector2 vNewDir = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos.x + vNewPos.y) * (vFly.x + vFly.y));

        // 
        vNewPos = this->__RepeatPosition(vNewPos, 1.35f, &bChange);
        if(bChange)
        {
            // 
            const coreVector2 vTarget = (m_aiCounter[BOOMERANG_TARGET]) ? this->GetPosition().xy() : pPlayer->GetPosition().xy();
            vNewPos = vNewPos * vFly.Abs() + vTarget * vFly.Abs().yx();

            // 
            g_pDistortion    ->CreateWave       (coreVector3(vNewPos, 0.0f), DISTORTION_WAVE_SMALL);
            g_pSpecialEffects->CreateDirectionColor(coreVector3(vNewPos, 0.0f), coreVector3(vFly, 0.0f), SPECIAL_SPLASH_SMALL, COLOR_RED_F);
        }

        // 
        pBoomerang->SetPosition (coreVector3(vNewPos, 0.0f));
        pBoomerang->SetDirection(coreVector3(vNewDir, 0.0f));
        pBoomerang->SetAlpha    (MIN(pBoomerang->GetAlpha() + 10.0f * Core::System->GetTime(), 1.0f));
        pBoomerang->SetTexOffset(coreVector2(0.0f, m_fAnimation));

        for(coreUintW j = 0u; j < CROSSFIELD_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BoomerangTrail.List())[i*CROSSFIELD_TRAILS + j];

            // 
            const coreVector2 vNewPos = pBoomerang->GetPosition().xy() - vFly * FOREGROUND_AREA * (0.06f * I_TO_F(CROSSFIELD_TRAILS - j));
            const coreVector2 vNewDir = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos.x + vNewPos.y) * (vFly.x + vFly.y));

            // 
            pTrail->SetPosition (coreVector3(vNewPos, 0.0f));
            pTrail->SetDirection(coreVector3(vNewDir, 0.0f));
            pTrail->SetAlpha    (pBoomerang->GetAlpha() * (0.25f * I_TO_F(j + 1u)));
            pTrail->SetTexOffset(coreVector2(0.0f, m_fAnimation));
        }
    }

    // 
    m_Boomerang     .MoveNormal();
    m_BoomerangTrail.MoveNormal();

    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_OBJECT(0), [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBoomerang, const coreBool& bFirst)
    {
        if(!bFirst) return;

        // 
        pPlayer->TakeDamage(5);

        const coreVector3 vPos = (pPlayer->GetPosition() + pBoomerang->GetPosition()) * 0.5f;

        g_pDistortion    ->CreateWave       (vPos, DISTORTION_WAVE_SMALL);
        g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_SMALL, COLOR_RED_F);
        g_pSpecialEffects->PlaySound(vPos, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    });
}


// ****************************************************************
// 
coreVector2 cCrossfieldBoss::__RepeatPosition(const coreVector2& vPosition, const coreFloat& fThreshold, coreBool* OUTPUT pbChange)
{
    const coreVector2 vThreshVec = fThreshold * FOREGROUND_AREA;

    // 
    coreVector2 vRepeatPos = vPosition;
    while(vRepeatPos.x < -vThreshVec.x) vRepeatPos.x += 2.0f * vThreshVec.x;
    while(vRepeatPos.x >  vThreshVec.x) vRepeatPos.x -= 2.0f * vThreshVec.x;
    while(vRepeatPos.y < -vThreshVec.y) vRepeatPos.y += 2.0f * vThreshVec.y;
    while(vRepeatPos.y >  vThreshVec.y) vRepeatPos.y -= 2.0f * vThreshVec.y;

    // 
    ASSERT(pbChange)
    *pbChange = (vRepeatPos.x != vPosition.x) || (vRepeatPos.y != vPosition.y);

    return vRepeatPos;
}


// ****************************************************************
// 
void cCrossfieldBoss::__EncodeDirection(const coreUintW& iIndex, const coreVector2& vDirection)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)

    // 
    if(vDirection.x + vDirection.y > 0.0f)
         ADD_BIT   (m_iPackedDir, 0u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 0u + 2u*iIndex);

    // 
    if(coreMath::InRange(vDirection.y, 0.0f, CORE_MATH_PRECISION))
         ADD_BIT   (m_iPackedDir, 1u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 1u + 2u*iIndex);

    STATIC_ASSERT(CROSSFIELD_BOOMERANGS*2u < sizeof(m_iPackedDir)*8u)
}


// ****************************************************************
// 
coreVector2 cCrossfieldBoss::__DecodeDirection(const coreUintW& iIndex)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)

    // 
    const coreFloat P = CONTAINS_BIT(m_iPackedDir, 0u + 2u*iIndex) ? 1.0f : -1.0f;
    const coreBool  X = CONTAINS_BIT(m_iPackedDir, 1u + 2u*iIndex) ? true : false;

    // 
    return coreVector2((X) ? P : 0.0f, (!X) ? P : 0.0f);
}


// ****************************************************************
// 
void cCrossfieldBoss::__EnableBoomerang(const coreUintW& iIndex, const coreVector2& vPosition, const coreVector2& vDirection)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)

    // 
    if((*m_Boomerang.List())[iIndex]->GetType()) return;
    (*m_Boomerang.List())[iIndex]->ChangeType(TYPE_OBJECT(0));

    // 
    this->__EncodeDirection(iIndex, vDirection);

    // 
    auto nInitBoomerangFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(vPosition, 0.0f));
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitBoomerangFunc((*m_Boomerang     .List())[iIndex]);
    nInitBoomerangFunc((*m_Boomerang     .List())[iIndex]);
    nInitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 0u]);
    nInitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 1u]);
    nInitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 2u]);

    // 
    g_pDistortion    ->CreateWave          (coreVector3(vPosition, 0.0f), DISTORTION_WAVE_SMALL);
    g_pSpecialEffects->CreateDirectionColor(coreVector3(vPosition, 0.0f), coreVector3(vDirection, 0.0f), SPECIAL_SPLASH_SMALL, COLOR_RED_F);
}


// ****************************************************************
// 
void cCrossfieldBoss::__DisableBoomerang(const coreUintW& iIndex, const coreBool& bAnimated)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)
    if(m_Boomerang.List()->empty()) return;

    // 
    if(!(*m_Boomerang.List())[iIndex]->GetType()) return;
    (*m_Boomerang.List())[iIndex]->ChangeType(0);

    // 
    auto nExitBoomerangFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitBoomerangFunc((*m_Boomerang     .List())[iIndex]);
    nExitBoomerangFunc((*m_Boomerang     .List())[iIndex]);
    nExitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 0u]);
    nExitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 1u]);
    nExitBoomerangFunc((*m_BoomerangTrail.List())[iIndex*CROSSFIELD_TRAILS + 2u]);

    // 
    g_pDistortion    ->CreateWave       ((*m_Boomerang.List())[iIndex]->GetPosition(), DISTORTION_WAVE_SMALL);
    g_pSpecialEffects->CreateSplashColor((*m_Boomerang.List())[iIndex]->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_RED_F);
}


//fTime > 1.0f ermöglichen (eigene variable), hold weg, reset-zwang
            //    ticks auch > endtick, __end aber immer true bei time>=1.0 und tick>=endtick

/*

const coreFloat fAngle = vToPlayer.Angle();
            for(coreUintW i = 0u; i < 5u; ++i)
            {
                const coreVector2 vDir = coreVector2::Direction(fAngle + 0.2f * I_TO_F(i - 2u));
                g_pGame->GetBulletManager()->AddBullet<cConeBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), vDir);
            }

*/
