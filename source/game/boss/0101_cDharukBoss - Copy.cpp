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
#define DUPLICATE_STATUS (0u)
#define CURRENT_SIDE     (1u)
#define CURRENT_ROW      (2u)
#define BOOMERANG_TARGET (3u)
#define RESURRECT_STATUS (4u)


// ****************************************************************
// vector identifier
#define TRANSITION_POSITION    (0u)
#define TRANSITION_DIRECTION   (1u)
#define TRANSITION_ORIENTATION (2u)
#define ATTACK_TARGET          (3u)


// ****************************************************************
// constructor
cDharukBoss::cDharukBoss()noexcept
: m_DuplicateTrail (DHARUK_DUPLICATE_RAWS)
, m_Boomerang      (DHARUK_BOOMERANGS)
, m_BoomerangTrail (DHARUK_BOOMERANGS * DHARUK_TRAILS)
, m_iPackedDir     (0u)
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_dharuk_high.md3");
    this->DefineModelLow ("ship_boss_dharuk_low.md3");

    // set object properties
    this->SetSize(coreVector3(3.3f,3.3f,3.3f));

    // configure the boss
    this->Configure(2800, COLOR_SHIP_RED);

    // create duplicate object
    m_Duplicate.DefineModelHigh("ship_boss_dharuk_high.md3");
    m_Duplicate.DefineModelLow ("ship_boss_dharuk_low.md3");
    m_Duplicate.DefineTexture  (0u, "effect_energy.png");
    m_Duplicate.DefineProgram  ("effect_energy_invert_program");
    m_Duplicate.SetSize        (this->GetSize());
    m_Duplicate.Configure      (1, COLOR_ENERGY_RED * 0.8f);

    // create duplicate trail list
    m_DuplicateTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_DUPLICATE_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = (i % DHARUK_TRAILS) + 1u;

            // load object resources
            coreObject3D* pDuplicate = &m_aDuplicateRaw[i];
            pDuplicate->DefineModel  ("ship_boss_dharuk_low.md3");
            pDuplicate->DefineTexture(0u, "effect_energy.png");
            pDuplicate->DefineProgram("effect_energy_invert_program");

            // set object properties
            pDuplicate->SetSize  (this->GetSize());
            pDuplicate->SetColor3(COLOR_ENERGY_RED * (0.15 + 0.15f * I_TO_F(iType)));
            pDuplicate->SetAlpha (0.15f + 0.2f * I_TO_F(iType));

            // add object to the list
            m_DuplicateTrail.BindObject(pDuplicate);
        }
    }

    // create boomerang lists
    m_Boomerang     .DefineProgram("effect_energy_invert_inst_program");
    m_BoomerangTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_BOOMERANGS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (DHARUK_TRAILS + 1u);

            // load object resources
            coreObject3D* pBoomerang = &m_aBoomerangRaw[i];
            pBoomerang->DefineModel  ("object_boss_dharuk_boomerang.md3");
            pBoomerang->DefineTexture(0u, "effect_energy.png");
            pBoomerang->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBoomerang->SetSize   (coreVector3(1.4f,1.4f,1.4f) * 1.1f);
            pBoomerang->SetColor3 (COLOR_ENERGY_RED * (iType ? (0.15 + 0.15f * I_TO_F(iType)) : 0.8f));
            pBoomerang->SetTexSize(coreVector2(1.5f,1.5f));
            pBoomerang->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BoomerangTrail.BindObject(pBoomerang);
                 else m_Boomerang     .BindObject(pBoomerang);
        }
    }
}


// ****************************************************************
// 
void cDharukBoss::__ResurrectOwn()
{
    if(m_aiCounter[RESURRECT_STATUS] != 0)
        return;

    // 
    g_pGlow->BindObject(&m_Duplicate);
    g_pGlow->BindList  (&m_DuplicateTrail);
    g_pGlow->BindList  (&m_Boomerang);
    g_pGlow->BindList  (&m_BoomerangTrail);
}


// ****************************************************************
// 
void cDharukBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
        this->__DisableBoomerang(i, bAnimated);

    // 
    if((m_aiCounter[RESURRECT_STATUS] == 0) && bAnimated)
    {
        m_aiCounter[RESURRECT_STATUS]= 1;

        // swap boss object with duplicate object
        /*
        this->DefineProgram(m_Duplicate.GetProgram());
        this->DefineTexture(0u, m_Duplicate.GetTexture(0u));
        this->SetColor3    (m_Duplicate.GetColor3());
        this->ChangeType   (TYPE_OBJECT(0));
        if(m_aiCounter[DUPLICATE_STATUS] != 0)
        {
            this->SetPosition   (m_Duplicate.GetPosition   ());
            this->SetDirection  (m_Duplicate.GetDirection  ());
            this->SetOrientation(m_Duplicate.GetOrientation());
        }
        */

        // 
        m_avVector[TRANSITION_POSITION] .xy (this->GetPosition   ().xy() / FOREGROUND_AREA);
        m_avVector[TRANSITION_DIRECTION].x = this->GetDirection  ().xy().Angle();
        m_avVector[TRANSITION_DIRECTION].y = [](){coreFloat i = g_pEnvironment->GetDirection().Angle(); if(i > PI) i -= 2.0f*PI; return i;}();
        m_avVector[TRANSITION_ORIENTATION] = this->GetOrientation();

        // 
        m_Duplicate.Kill(false);
        m_aiCounter[DUPLICATE_STATUS] = 0;
        m_aiCounter[BOOMERANG_TARGET] = 0;

        // 
        this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        coreObject3D::Move();

        // 
        this->Configure(700, this->GetBaseColor());
        this->Resurrect(this->GetPosition().xy(), this->GetDirection().xy());

        // 
        m_iPhase = 30u;
        PHASE_RESET(0u)
        PHASE_RESET(1u)
        PHASE_RESET(2u)

        [](void){{}},[]()->void{}(); return;
    }

    // 
    g_pGlow->UnbindObject(&m_Duplicate);
    g_pGlow->UnbindList  (&m_DuplicateTrail);
    g_pGlow->UnbindList  (&m_Boomerang);
    g_pGlow->UnbindList  (&m_BoomerangTrail);

    // 
    g_pGame->GetMission()->SetCurBoss(MISSION_NO_BOSS);
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnUnder()
{
    // 
    m_DuplicateTrail.Render();

    // 
    m_BoomerangTrail.Render();
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnAttack()
{
    DEPTH_PUSH

    // 
    m_Boomerang.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Boomerang);
}


// ****************************************************************
// 
void cDharukBoss::__MoveOwn()
{
    // 
    cPlayer* pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());

    // 
    const coreVector2 vOldBasePos   = this->GetPosition   ().xy();
    const coreFloat   fOldBaseAngle = this->GetDirection  ().xy().Angle();
    const coreFloat   fOldBaseOri   = this->GetOrientation().xz().Angle();   // # not y

    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            this->DefaultMoveLerp  (coreVector2(DHARUK_WIDTH, 1.5f), coreVector2(DHARUK_WIDTH, -1.5f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                         5.0f*PI,                          fTime);

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_PAUSE(0u, FLT_MAX)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp  (coreVector2(0.0f,1.5f), coreVector2(0.0f, DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(0.0f*PI,                5.0f*PI,                          fTime);

            if(PHASE_TIME_POINT(0.85f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        //this->SetOrientation(coreVector3(1.0f,0.0f,1.0f).Normalized());
        //PHASE_CONTROL_PAUSE(0u, 2.0f/1000.0f)
        //{
        //    m_iPhase = 10u;
        //});
        PHASE_CONTROL_PAUSE(0u, 2.0f)
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

            //PHASE_CONTROL_TICKER(0u, 0u, 2.0f + 4.0f * SIN(fTime*PI))   // increase shoot-speed with movement-speed
            //{
            //    const coreVector2 vDir = coreVector2(0.0f, -SIGN(this->GetPosition().y));
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, this->GetPosition().xy(), vDir)->MakeGreen()->MakeSmaller(1.5f);
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, this->GetPosition().xy(), vDir)->MakeGreen()->MakeSmaller(1.5f);
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, this->GetPosition().xy(), vDir)->MakeGreen()->MakeSmaller(1.5f);
            //
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, this->GetPosition().xy(), -vDir)->MakeGreen()->MakeSmaller(1.5f);
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, this->GetPosition().xy(), -vDir)->MakeGreen()->MakeSmaller(1.5f);
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, this->GetPosition().xy(), -vDir)->MakeGreen()->MakeSmaller(1.5f);
            //
            //    if(m_aiCounter[DUPLICATE_STATUS])
            //    {
            //        const coreVector2 vDir2 = coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y));
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, m_Duplicate.GetPosition().xy(), vDir2)->MakeGreen();
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, m_Duplicate.GetPosition().xy(), vDir2)->MakeGreen();
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, m_Duplicate.GetPosition().xy(), vDir2)->MakeGreen();
            //    
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, m_Duplicate.GetPosition().xy(), -vDir2)->MakeGreen();
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, m_Duplicate.GetPosition().xy(), -vDir2)->MakeGreen();
            //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, m_Duplicate.GetPosition().xy(), -vDir2)->MakeGreen();
            //    }
            //});

            UNUSED const coreFloat fHeight = 1.6f / 3.0f / 2.0f;//I_TO_F(m_aiCounter[CURRENT_ROW]) * 0.4f;

            this->DefaultMoveLerp(coreVector2(bSecond ? (fSideSign *  3.5f) : 0.0f, DHARUK_HEIGHT),
                                  coreVector2(           fSideSign * -3.5f,         DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(1.0f*PI, bSecond ? (15.0f*PI) : (11.0f*PI), fSideTime);

            if(m_aiCounter[DUPLICATE_STATUS])
            {
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(fSideSign * -2.0f*PI * fTime));

                if(bSecond && (ABS(this->GetPosition().x) < 1.5f*FOREGROUND_AREA.x))
                    this->SetPosition(this->GetPosition().InvertedY());
            }


            const coreVector3 vOldPos = this->GetPosition();

            coreBool bChange = false;
            this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), 1.5f, &bChange), 0.0f));

            if(ABS(this->GetOldPos().x - this->GetPosition().x) < FOREGROUND_AREA.x && //!bChange &&
               (//PHASE_POSITION_POINT(this, -0.95f * FOREGROUND_AREA.x, x) ||   
                //PHASE_POSITION_POINT(this, -0.75f * FOREGROUND_AREA.x, x) ||
                //PHASE_POSITION_POINT(this, -0.55f * FOREGROUND_AREA.x, x) ||
                PHASE_POSITION_POINT(this, -0.7f * FOREGROUND_AREA.x, x) ||
                PHASE_POSITION_POINT(this, -0.3f * FOREGROUND_AREA.x, x) ||
                PHASE_POSITION_POINT(this,  0.1f * FOREGROUND_AREA.x, x) ||
                PHASE_POSITION_POINT(this,  0.5f * FOREGROUND_AREA.x, x) ||
                PHASE_POSITION_POINT(this,  0.9f * FOREGROUND_AREA.x, x)// ||
                //PHASE_POSITION_POINT(this,  0.65f * FOREGROUND_AREA.x, x) ||
                //PHASE_POSITION_POINT(this,  0.85f * FOREGROUND_AREA.x, x)
                ))
            {
                const coreVector2 vDir = coreVector2(0.0f, -SIGN(this->GetPosition().y));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, m_vPositionPoint, vDir)->MakeGreen()->MakeSmaller(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, m_vPositionPoint, vDir)->MakeGreen()->MakeSmaller(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, m_vPositionPoint, vDir)->MakeGreen()->MakeSmaller(1.4f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, m_vPositionPoint, -vDir)->MakeGreen()->MakeSmaller(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, m_vPositionPoint, -vDir)->MakeGreen()->MakeSmaller(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, m_vPositionPoint, -vDir)->MakeGreen()->MakeSmaller(1.4f);

                if(m_aiCounter[DUPLICATE_STATUS])
                {
                    const coreVector2 vDir2 = coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y));  
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, -m_vPositionPoint, vDir2)->MakeGreen()->MakeSmaller(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, -m_vPositionPoint, vDir2)->MakeGreen()->MakeSmaller(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, -m_vPositionPoint, vDir2)->MakeGreen()->MakeSmaller(1.4f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, this, -m_vPositionPoint, -vDir2)->MakeGreen()->MakeSmaller(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, -m_vPositionPoint, -vDir2)->MakeGreen()->MakeSmaller(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, this, -m_vPositionPoint, -vDir2)->MakeGreen()->MakeSmaller(1.4f);
                }
            }

            this->SetPosition(vOldPos);



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
        {
                ++m_iPhase;
                PHASE_RESET(0u)
                PHASE_RESET(1u)
                PHASE_RESET(2u)

                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];
            }
        //const coreBool bSecond = (m_iPhase == 14u) ? true : false;
        //
        //const coreVector2 vDiff  = pPlayer->GetPosition().xy() - this->GetPosition().xy();
        //const coreFloat   fSpeed = ((ABS(vDiff.x) > ABS(vDiff.y)) ? 1.0f/*6.0f*/ : 1.0f) * ((bSecond ? 1.0f : 0.0f) + (m_aiCounter[DUPLICATE_STATUS] ? 1.0f : 0.0f));
        //
        //PHASE_CONTROL_TICKER(0u, 0u, fSpeed)
        //{
        //    const coreObject3D* pOwner = (!bSecond || iTick % 2u) ? pBase : this;
        //
        //    const coreVector2 vPos =  pOwner ->GetPosition().xy();
        //    const coreVector2 vDir = (pPlayer->GetPosition().xy() - vPos).Normalize();
        //
        //    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, this, vPos, vDir)->MakeOrange()->MakeSmaller(1.2f);
        //    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 3u, COLOR_ENERGY_ORANGE);
        //});
        //
        //PHASE_CONTROL_TICKER(1u, 0u, 2.0f)
        //{
        //    const coreVector2 vTan = this->GetDirection().xy().Rotate90() * ((iTick % 2u) ? -1.0f : 1.0f);
        //
        //    for(coreUintW i = 0u/*6u - MIN(iTick + 1u, 6u)*/; i < 6u; ++i)
        //    {
        //        const coreFloat fOffset = I_TO_F(i - 1u);
        //
        //        const coreVector2 vPos =  this->GetPosition ().xy() + vTan * (2.0f*fOffset + 2.0f) - this->GetDirection().xy() * (2.0f*fOffset);
        //        const coreVector2 vDir = (this->GetDirection().xy() + vTan * (0.1f*fOffset)).Normalize();
        //
        //        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vPos,             vDir)                      ->MakeBlue();
        //        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vPos + vDir*2.5f, vDir)                      ->MakeBlue();
        //        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vPos,             -this->GetDirection().xy())->MakeBlue();
        //    }
        //});
        //
        //PHASE_CONTROL_TIMER(2u, 1.0f/2.0f, LERP_SMOOTH)
        //{
        //    const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fTime : (1.0f - fTime);
        //
        //    this->DefaultOrientateLerp(0.0f*PI, 4.0f*PI, fSideTime);
        //
        //    if(PHASE_FINISHED)
        //    {
        //        ++m_iPhase;
        //        PHASE_RESET(0u)
        //        PHASE_RESET(1u)
        //        PHASE_RESET(2u)
        //
        //        m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];
        //    }
        //});
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u || m_iPhase == 15u)
    {++m_iPhase; 
        //PHASE_CONTROL_TICKER(0u, 0u, (m_iPhase == 12u) ? 2.0f : 0.75f)
        //{
        //    ++m_iPhase;
        //});
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

            PHASE_CONTROL_TICKER(1u, 0u, 3.5f)
            {
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();

                if(m_aiCounter[DUPLICATE_STATUS])
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, m_Duplicate.GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, m_Duplicate.GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, m_Duplicate.GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, m_Duplicate.GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();
                }
            });

            this->DefaultMoveLerp  (coreVector2(fSideSign * 0.5f, DHARUK_HEIGHT), coreVector2(fSideSign * 0.5f, -1.5f), fOwnTime);
            this->DefaultRotateLerp(1.0f*PI,                                      5.0f*PI,                              fSideTime);

            if(PHASE_FINISHED)
            {
                ++m_iPhase;
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 17u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            const coreFloat fSideTime = m_aiCounter[CURRENT_SIDE] ? fTime : (1.0f - fTime);

            this->DefaultMoveLerp  (coreVector2(0.0f,1.5f), coreVector2(0.0f, DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(1.0f*PI,                5.0f*PI,                          fSideTime);

            if(PHASE_FINISHED)
                m_iPhase = 20u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TICKER(0u, 5u, m_Boomerang.GetCurEnabled() ? (2.2f/3.0f) : FRAMERATE_VALUE)
        {
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f : 1.0f;

            if(iTick < DHARUK_BOOMERANGS)
                this->__EnableBoomerang(iTick, this->GetPosition().xy(), coreVector2((iTick % 2u) ? fSideSign : -fSideSign, 0.0f));

            if(PHASE_FINISHED)
                ++m_iPhase;   // further down
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/10.0f, LERP_SMOOTH)
        {
            const coreFloat fSideSign = m_aiCounter[CURRENT_SIDE] ? -1.0f : 1.0f;

            this->SetPosition      (coreVector3(coreVector2::Direction(LERP(0.0f*PI, fSideSign * -4.0f*PI, fTime)) * FOREGROUND_AREA * DHARUK_HEIGHT, 0.0f));
            this->DefaultRotateLerp(1.0f*PI, fSideSign * 21.0f*PI, fTime);

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                m_aiCounter[BOOMERANG_TARGET] = 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 24u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            coreBool bDisabled = true;

            for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
            {
                coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
                if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                bDisabled = false;

                if((this->GetPosition().xy().InvertedY() - pBoomerang->GetPosition().xy()).LengthSq() < 4.0f)
                    this->__DisableBoomerang(i, true);
            }

            if(PHASE_FINISHED && bDisabled)
            {
                m_iPhase = m_aiCounter[DUPLICATE_STATUS] ? 25u : 26u;

                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];

                m_aiCounter[BOOMERANG_TARGET] = 0;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 25u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.2f/2.0f)
        {
            m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 26u)
    {
        PHASE_CONTROL_PAUSE(0u, FLT_MAX)
        {
            ++m_iPhase;

            ASSERT(m_aiCounter[DUPLICATE_STATUS] == 0)
            m_aiCounter[DUPLICATE_STATUS] = 1;

            m_Duplicate.Resurrect();
            m_Duplicate.SetPosition   (-this->GetPosition   ());
            m_Duplicate.SetDirection  (-this->GetDirection  ());
            m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
            m_Duplicate.SetAlpha      (0.0f);
            m_Duplicate.ChangeType    (TYPE_OBJECT(0));

            g_pSpecialEffects->MacroExplosionColorBig(m_Duplicate.GetPosition(), COLOR_ENERGY_RED);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 27u)
    {
        PHASE_CONTROL_TIMER(0u, 2.2f/2.0f, LERP_SMOOTH)
        {
            m_Duplicate.SetAlpha(MIN(fTime*10.0f, 1.0f));

            if(PHASE_FINISHED)
            {
                m_iPhase = 10u;

                m_aiCounter[DUPLICATE_STATUS] = 2;
            }
        });
    }

    //// ################################################################
    //// 
    //else if(m_iPhase == 28u)
    //{
    //    PHASE_CONTROL_TIMER(0u, 1.0f/6.0f, LERP_SMOOTH)
    //    {
    //        this->SetPosition (coreVector3(coreVector2::Direction(LERP(0.0f*PI, -2.0f*PI, fTime)) * FOREGROUND_AREA * DHARUK_HEIGHT, 0.0f));
    //        this->SetDirection(coreVector3(coreVector2::Direction(LERP(1.0f*PI,  7.0f*PI, fTime)), 0.0f));
    //
    //        m_Duplicate.SetPosition (coreVector3(coreVector2::Direction(LERP(0.0f*PI, -3.0f*PI, fTime)) * FOREGROUND_AREA * DHARUK_HEIGHT, 0.0f));
    //        m_Duplicate.SetDirection(coreVector3(coreVector2::Direction(LERP(1.0f*PI, 10.0f*PI, fTime)), 0.0f));
    //
    //        if(PHASE_FINISHED)
    //        {
    //            m_iPhase = 10u;
    //
    //            m_aiCounter[DUPLICATE_STATUS] = 2;
    //        }
    //    });
    //}

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fOwnTime = LERPB(1.0f, 0.0f, 1.0f - fTime);

            this->DefaultMoveLerp  (     m_avVector[TRANSITION_POSITION] .xy(), 2.0f*m_avVector[TRANSITION_POSITION].xy().Normalized(),              fOwnTime);
            this->DefaultRotateLerp(     m_avVector[TRANSITION_DIRECTION].x,    4.0f*PI,                                                             fOwnTime);
            this->SetOrientation   (LERP(m_avVector[TRANSITION_ORIENTATION],    coreVector3(0.0f, 0.0f, SIGN(m_avVector[TRANSITION_ORIENTATION].z)), fOwnTime).Normalized());

            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(m_avVector[TRANSITION_DIRECTION].y, 0.0f*PI, fTime*0.5f)));

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fOwnTime = LERPB(0.0f, 1.0f, fTime);

            this->DefaultMoveLerp  (coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fOwnTime);
            this->DefaultRotateLerp(0.0f*PI,                5.0f*PI,                 fOwnTime);
            this->SetOrientation   (coreVector3(0.0f,0.0f,1.0f));

            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(m_avVector[TRANSITION_DIRECTION].y, 0.0f*PI, fTime*0.5f + 0.5f)));

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TICKER(0u, 6u, m_Boomerang.GetCurEnabled() ? (2.2f/1.0f) : FRAMERATE_VALUE)
        {
            // TODO: charge   

            if(iTick < DHARUK_BOOMERANGS)
                this->__EnableBoomerang(iTick, this->GetPosition().xy(), coreVector2(0.0f,-1.0f));

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u || m_iPhase == 34u)
    {
        const coreBool bSecond = (m_iPhase == 34u) ? true : false;
        constexpr coreUint16 iBack = 22u;
        constexpr coreUint16 iLoop = 40u;

        PHASE_CONTROL_TICKER(0u, bSecond ? (2u * iLoop) : iLoop, bSecond ? 20.0f : 10.0f)
        {
            if((iTick % iBack) < 10u)
            {
                const coreVector2 vDir = coreVector2::Direction(PI * (1.0f + 0.07f * (I_TO_F(iTick % iBack) - 4.5f)) * (((iTick % iLoop) >= iBack) ? -1.0f : 1.0f));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, this->GetPosition().xy(),             vDir)->MakeBlue();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, this->GetPosition().xy() + vDir*2.5f, vDir)->MakeBlue();

                //if(m_aiCounter[DUPLICATE_STATUS])
                //{
                //    const coreVector2 vDir2 = -vDir;
                //    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, m_Duplicate.GetPosition().xy(),              vDir2)->MakeBlue();
                //    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, m_Duplicate.GetPosition().xy() + vDir2*2.5f, vDir2)->MakeBlue();
                //}
            }

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.15f, LERP_LINEAR)
        {
            const coreFloat fOwnTime = LERPB(1.0f, 0.0f, 1.0f - fTime);

            this->DefaultMoveLerp  (coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.5f), fOwnTime);
            this->DefaultRotateLerp(1.0f*PI,                 5.0f*PI,                 fOwnTime);

            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(0.0f*PI, 1.0f*PI, fTime)));

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                m_aiCounter[CURRENT_SIDE] = 0;
                m_avVector[ATTACK_TARGET].xy(coreVector2(pPlayer->GetPosition().x / FOREGROUND_AREA.x, 2.0f));
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 36u)
    {
        PHASE_CONTROL_TIMER(1u, 0.8936f/2.0f, LERP_LINEAR)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 3.5f)
            {
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2( 1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.25f, this, this->GetPosition().xy() + coreVector2(0.0f, 1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.35f, this, this->GetPosition().xy() + coreVector2(0.0f,-1.5f), coreVector2(-1.0f,0.0f))->MakeOrange();
            });

            this->DefaultMoveLerp  (m_avVector[ATTACK_TARGET].xy(), coreVector2(m_avVector[ATTACK_TARGET].x, -1.5f), fTime);
            this->DefaultRotateLerp(1.0f*PI,                        7.0f*PI,                                         fTime);

            if(PHASE_FINISHED)
            {
                PHASE_RESET(1u)

                m_aTimer[0].SetValue(m_aiCounter[CURRENT_SIDE] ? 0.0f : 0.5f);
                m_aiCounter[CURRENT_SIDE] = 1 - m_aiCounter[CURRENT_SIDE];

                m_avVector[ATTACK_TARGET].xy(coreVector2(pPlayer->GetPosition().x / FOREGROUND_AREA.x, 2.0f));
            }
        });
    }

    // ################################################################

    // 
    coreBool bChange = false;
    this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), 1.5f, &bChange), 0.0f));
    if(bChange)
    {
        // 
        if(++m_aiCounter[CURRENT_ROW] >= 2) m_aiCounter[CURRENT_ROW] = -2;
    }

    // 
    if(m_aiCounter[RESURRECT_STATUS]) this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // 
    if(m_aiCounter[DUPLICATE_STATUS] == 2)
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
        const coreVector2 vCurBasePos   = this->GetPosition   ().xy();
        const coreFloat   fCurBaseAngle = this->GetDirection  ().xy().Angle();
        const coreFloat   fCurBaseOri   = this->GetOrientation().xz().Angle();   // # not y
        const coreVector2 vOffsetPos    =          (vCurBasePos -  vOldBasePos)   * fRcpTime * 0.05f;
        const coreFloat   fOffsetAngle  = AngleDiff(fCurBaseAngle, fOldBaseAngle) * fRcpTime * 0.055f;
        const coreFloat   fOffsetOri    = AngleDiff(fCurBaseOri,   fOldBaseOri)   * fRcpTime * 0.06f;

        for(coreUintW i = 0u, ie = DHARUK_TRAILS * (m_aiCounter[DUPLICATE_STATUS] ? 2u : 1u); i < ie; ++i)
        {
            // 
            const coreFloat fSide   = (i < DHARUK_TRAILS) ? 1.0f : -1.0f;
            const coreFloat fFactor = I_TO_F(DHARUK_TRAILS - (i % DHARUK_TRAILS));

            // 
            const coreVector2 vNewPos =                        vCurBasePos   - vOffsetPos   * fFactor;
            const coreVector2 vNewDir = coreVector2::Direction(fCurBaseAngle - fOffsetAngle * fFactor);
            const coreVector2 vNewOri = coreVector2::Direction(fCurBaseOri   - fOffsetOri   * fFactor);

            // 
            m_aDuplicateRaw[i].SetPosition   (coreVector3(fSide * vNewPos,   0.0f));
            m_aDuplicateRaw[i].SetDirection  (coreVector3(fSide * vNewDir,   0.0f));
            m_aDuplicateRaw[i].SetOrientation(coreVector3(fSide * vNewOri.x, 0.0f, vNewOri.y));
            m_aDuplicateRaw[i].SetTexOffset  (coreVector2(0.0f, m_fAnimation));
            m_aDuplicateRaw[i].Move();
        }
    }

    // 
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
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
            const coreVector2 vTarget = (m_aiCounter[BOOMERANG_TARGET]) ? this->GetPosition().xy().InvertedY() : pPlayer->GetPosition().xy();
            const coreVector2 vFlyAbs = vFly.Processed(ABS);
            vNewPos = vNewPos * vFlyAbs + vTarget * vFlyAbs.yx();

            // 
            g_pSpecialEffects->CreateBlowColor(coreVector3(vNewPos, 0.0f), coreVector3(vFly, 0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_RED);
        }

        // 
        pBoomerang->SetPosition (coreVector3(vNewPos, 0.0f));
        pBoomerang->SetDirection(coreVector3(vNewDir, 0.0f));
        pBoomerang->SetAlpha    (MIN(pBoomerang->GetAlpha() + 10.0f * Core::System->GetTime(), 1.0f));
        pBoomerang->SetTexOffset(coreVector2(0.0f, m_fAnimation));

        for(coreUintW j = 0u; j < DHARUK_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BoomerangTrail.List())[i*DHARUK_TRAILS + j];

            // 
            const coreVector2 vNewPos2 = pBoomerang->GetPosition().xy() - vFly * FOREGROUND_AREA * (0.06f * I_TO_F(DHARUK_TRAILS - j));
            const coreVector2 vNewDir2 = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos2.x + vNewPos2.y) * (vFly.x + vFly.y));

            // 
            pTrail->SetPosition (coreVector3(vNewPos2, 0.0f));
            pTrail->SetDirection(coreVector3(vNewDir2, 0.0f));
            pTrail->SetAlpha    (pBoomerang->GetAlpha() * (0.15f + 0.2f * I_TO_F(j + 1u)));
            pTrail->SetTexOffset(coreVector2(0.0f, m_fAnimation));
        }
    }

    // 
    m_Boomerang     .MoveNormal();
    m_BoomerangTrail.MoveNormal();

    // 
    m_Duplicate.ActivateModelLow();

    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_OBJECT(0), [this](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBoomerang, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage((pBoomerang == &m_Duplicate) ? 10 : 5, ELEMENT_RED);

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_RED);
    });

    // 
    m_Duplicate.ActivateModelHigh();
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__RepeatPosition(const coreVector2& vPosition, const coreFloat fThreshold, coreBool* OUTPUT pbChange)
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
    (*pbChange) = (vRepeatPos.x != vPosition.x) || (vRepeatPos.y != vPosition.y);

    return vRepeatPos;
}


// ****************************************************************
// 
void cDharukBoss::__EncodeDirection(const coreUintW iIndex, const coreVector2& vDirection)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    if(vDirection.x + vDirection.y > 0.0f)
         ADD_BIT   (m_iPackedDir, 0u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 0u + 2u*iIndex);

    // 
    if(coreMath::InRange(vDirection.y, 0.0f, CORE_MATH_PRECISION))
         ADD_BIT   (m_iPackedDir, 1u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 1u + 2u*iIndex);

    STATIC_ASSERT(DHARUK_BOOMERANGS*2u <= sizeof(m_iPackedDir)*8u)
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__DecodeDirection(const coreUintW iIndex)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    const coreFloat P = CONTAINS_BIT(m_iPackedDir, 0u + 2u*iIndex) ? 1.0f : -1.0f;
    const coreBool  X = CONTAINS_BIT(m_iPackedDir, 1u + 2u*iIndex) ? true : false;

    // 
    return coreVector2((X) ? P : 0.0f, (!X) ? P : 0.0f);
}


// ****************************************************************
// 
void cDharukBoss::__EnableBoomerang(const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // 
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    if(pBoomerang->GetType()) return;
    pBoomerang->ChangeType(TYPE_OBJECT(0));

    // 
    this->__EncodeDirection(iIndex, vDirection);

    // 
    auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(vPosition, 0.0f));
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nInitFunc(pTrail + i);

    // 
    g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPosition, 0.0f), vDirection, COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__DisableBoomerang(const coreUintW iIndex, const coreBool bAnimated)
{
    if(m_Boomerang.List()->empty()) return;

    // 
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    if(!pBoomerang->GetType()) return;
    pBoomerang->ChangeType(0);

    // 
    auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nExitFunc(pTrail + i);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pBoomerang->GetPosition(), COLOR_ENERGY_RED);
}