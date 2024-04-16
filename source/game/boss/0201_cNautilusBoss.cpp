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
#define CONTAINER_ATTACHED (0u)
#define DASH_SIDE          (1u)
#define SPIN_SIDE          (2u)
#define STOMP_COUNT        (3u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cNautilusBoss::cNautilusBoss()noexcept
: m_fClawAngle (0.0f)
, m_fAnimation (0.0f)

, m_Ink (NAUTILUS_INK_RAWS)   
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
    m_Ink.DefineProgram("environment_clouds_inst_program");
    {
        for(coreUintW i = 0u; i < NAUTILUS_INK_RAWS; ++i)  
        {
            // load object resources
            coreObject3D* pInk = &m_aInkRaw[i];
            pInk->DefineModel  (Core::Manager::Object->GetLowQuad());
            pInk->DefineTexture(0u, "environment_clouds_high.png");
            pInk->DefineProgram("environment_clouds_program");

            // set object properties
            pInk->SetSize   (coreVector3(coreVector2(1.0f,1.0f) * 40.0f     *0.7f, 1.0f)); 
            //pInk->SetColor3 (coreVector3(1.0f,1.0f,1.0f) * 0.1f); 
            pInk->SetColor3 (coreVector3(1.0f,1.0f,1.0f) * 0.2f); 
            pInk->SetAlpha  (1.0f);
            pInk->SetEnabled((i < 1000) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);


            pInk->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            //pInk->SetDirection(coreVector3(coreVector2::Direction(I_TO_F(i) * 2.0f*PI/I_TO_F(NAUTILUS_INK_RAWS)), 0.0f));
            pInk->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));


              
            pInk->SetTexSize(coreVector2(1.0f,1.0f) * 0.6f);

            //pInk->SetPosition(coreVector3(coreVector2::Rand() * Core::Rand->Float(20.0f) + coreVector2(-0.5f,0.5f) * FOREGROUND_AREA, 0.0f));
            pInk->SetPosition(coreVector3(coreVector2::Rand() * Core::Rand->Float(10.0f) + coreVector2(-0.5f,0.5f) * FOREGROUND_AREA, 0.0f));
            if(i < (NAUTILUS_INK_RAWS / 2u)) pInk->SetPosition(-pInk->GetPosition());

            pInk->SetPosition(pInk->GetPosition() * coreVector3(1.0f,0.0f,1.0f));

            pInk->SetPosition(coreVector3((I_TO_F(i - NAUTILUS_INK_RAWS/2u) + 0.5f) /I_TO_F(NAUTILUS_INK_RAWS/2u) * FOREGROUND_AREA.x * 1.25f, 0.0f, 0.0f));


            // add object to the list
            m_Ink.BindObject(pInk);
        }
    }

    /*

    
    g_pSpecialEffects->AddScreenSplatter(coreVector2( 0.35f, 0.35f), coreVector2( 0.0f, 1.0f), 0.3f,  1u);
    g_pSpecialEffects->AddScreenSplatter(coreVector2( 0.15f, 0.29f), coreVector2( 0.0f, 1.0f), 0.25f, 4u);
    g_pSpecialEffects->AddScreenSplatter(coreVector2( 0.2f,  0.39f), coreVector2( 1.0f, 0.0f), 0.22f, 2u);

    //g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.35f,-0.32f), coreVector2( 1.0f, 0.0f), 0.2f,  4u);
   // g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.4f,-0.4f), coreVector2( 1.0f, 0.0f), 0.25f, 1u);
    g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.26f,-0.32f), coreVector2( 0.0f, -1.0f), 0.2f,  4u);
    g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.38f,-0.4f), coreVector2( 0.0f, -1.0f), 0.25f, 1u);

    //g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.32f,-0.34f), coreVector2( 0.0f,1.0f), 0.36f, 5u);
    //g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.35f,-0.38f), coreVector2( 0.0f,1.0f), 0.3f,  2u);
    
   // g_pSpecialEffects->AddScreenSplatter(coreVector2( 0.39f,-0.34f), coreVector2(-1.0f,0.0f), 0.33f, 0u);

    //for(coreUintW i = 0u; i < 11u; ++i)
    //    g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.25f,0.25f, -0.25f,0.25f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.5f), 6u);
    for(coreUintW i = 0u; i < 4u; ++i)
        g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.35f,-0.1f, -0.35f,-0.1f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.5f), 6u);
    for(coreUintW i = 0u; i < 4u; ++i)
        g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(0.1f,0.35f, -0.35f,-0.1f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.5f), 6u);
    for(coreUintW i = 0u; i < 4u; ++i)
        g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.35f,-0.1f, 0.1f,0.35f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.5f), 6u);
    for(coreUintW i = 0u; i < 4u; ++i)
        g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(0.1f,0.35f, 0.1f,0.35f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.5f), 6u);
    //for(coreUintW i = 0u; i < 4u; ++i)
    //    g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.3f,0.3f, -0.3f,0.3f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.4f), 6u);

   // for(coreUintW i = 0u; i < 1u; ++i)
   //     g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.45f,-0.35f, -0.25f,0.25f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.2f), 2u);
   // for(coreUintW i = 0u; i < 1u; ++i)
   //     g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(0.35f,0.45f, -0.25f,0.25f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.2f), 2u);
   // for(coreUintW i = 0u; i < 1u; ++i)
   //     g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.25f,0.25f, -0.45f,-0.35f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.2f), 2u);
   // for(coreUintW i = 0u; i < 1u; ++i)
   //     g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.25f,0.25f, 0.35f,0.45f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.2f), 2u);
    g_pSpecialEffects->AddScreenSplatter(coreVector2(0.200605780f,-0.432354182f), coreVector2(-0.938443184f,0.345431268f), 0.137678146f, 2u);

    g_pSpecialEffects->AddScreenSplatter(coreVector2(0.41f,-0.25f), coreVector2( 0.0f, 1.0f), 0.22f, 2u);

    
    g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.38f,0.4f) * 0.5f + coreVector2(-0.26f,0.32f) * 0.5f, coreVector2(1.0f,1.0f).Normalized(), 0.25f, 1u);

 //  for(coreUintW i = 0u; i < 6u; ++i)
 //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.3f,0.3f, -0.3f,0.3f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.4f), 1u);
 //  for(coreUintW i = 0u; i < 6u; ++i)
 //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.3f,0.3f, -0.3f,0.3f), coreVector2::Rand(), Core::Rand->Float(0.1f,0.4f), 4u);

    //for(coreUintW i = 0u; i < 3u; ++i)
    //    g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-0.3f,0.3f, -0.3f,0.3f), coreVector2::Rand(), Core::Rand->Float(0.2f,0.4f), 2u);

    //g_pSpecialEffects->AddScreenSplatter(coreVector2(-0.25f,-0.25f), coreVector2(1.0f,0.0f), 0.2f,  2u);
 

 */
  //  constexpr coreFloat A = 0.05f;
  //  constexpr coreFloat B = 0.4f;
  //  for(coreUintW i = 0u; i < 4u; ++i)
  //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-B,-A, -B,-A), coreVector2::Rand(), Core::Rand->Float(0.15f,0.25f), Core::Rand->Int(1));
  //  for(coreUintW i = 0u; i < 4u; ++i)
  //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(A,B, -B,-A), coreVector2::Rand(), Core::Rand->Float(0.15f,0.25f), Core::Rand->Int(1));
  //  for(coreUintW i = 0u; i < 4u; ++i)
  //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(-B,-A, A,B), coreVector2::Rand(), Core::Rand->Float(0.15f,0.25f), Core::Rand->Int(1));
  //  for(coreUintW i = 0u; i < 4u; ++i)
  //      g_pSpecialEffects->AddScreenSplatter(coreVector2::Rand(A,B, A,B), coreVector2::Rand(), Core::Rand->Float(0.15f,0.25f), Core::Rand->Int(1));
}


// ****************************************************************
// 
void cNautilusBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (coreVector2(1000.0f,1000.0f));
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(true);

    // 
    m_aiCounter[CONTAINER_ATTACHED] = 1;
}


// ****************************************************************
// 
void cNautilusBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cNautilusBoss::__RenderOwnOver()
{
    //const coreMatrix2 mRota = coreMatrix3::Rotation(coreFloat(Core::System->GetTotalTime()) * 0.15f).m12();

    for(coreUintW i = 0u; i < NAUTILUS_INK_RAWS; ++i)  
    {

        m_aInkRaw[i].SetPosition(coreVector3((I_TO_F(i - NAUTILUS_INK_RAWS/2u) + 0.5f) /I_TO_F(NAUTILUS_INK_RAWS/2u) * FOREGROUND_AREA.x * 1.1f, (SIN(I_TO_F(i))*0.3f + SIN(I_TO_F(i) / I_TO_F(NAUTILUS_INK_RAWS) * 2.0f * PI + coreFloat(Core::System->GetTotalTime()) * 0.3f)) * FOREGROUND_AREA.y * 0.1f + /*0.8f*/0.0f * FOREGROUND_AREA.y, 0.0f));

       // if(i < (NAUTILUS_INK_RAWS / 2u))  m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition().InvertedY());
       // if(i < (NAUTILUS_INK_RAWS / 4u))  m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition().InvertedY());
       // if(i < (NAUTILUS_INK_RAWS * 3u / 4u) && i >= (NAUTILUS_INK_RAWS / 2u))  m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition().InvertedY());

        //m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition() * mRota);
        //m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition().InvertedY());
        m_aInkRaw[i].SetPosition(m_aInkRaw[i].GetPosition().RotatedZ90());


            //m_aInkRaw[i].SetTexOffset(m_aInkRaw[i].GetTexOffset() + 0.1f * Core::System->GetTime());
        m_aInkRaw[i].SetTexOffset((m_aInkRaw[i].GetTexOffset() + coreVector2(0.0f,-1.0f) * (0.03f * Core::System->GetTime() * g_pEnvironment->GetSpeed())).Processed(FRACT));
    }


    m_Ink.MoveNormal();

   // glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
    // 
    //m_Ink.Render();
    //glBlendFunc(FOREGROUND_BLEND_DEFAULT);
}


// ****************************************************************
// 
void cNautilusBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

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

            if((iTick / 5u) & 0x01u) g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos + vSide, -vDir)->ChangeSize(1.2f);
                                else g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos - vSide, -vDir)->ChangeSize(1.2f);
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();

            const coreUint16 iType = 1u << (CLAMP(F_TO_UI((1.0f - this->GetCurHealthPct()) * (4.0f / fHealth)), 1u, 3u) - 1u);

            const coreUintW iInc = MIN(iTick, 24u);
            for(coreUintW i = 24u - iInc, ie = 24u + iInc; i < ie; ++i)
            {
                if(((i / iType) + (iTick & 0x01u)) & 0x01u) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 5.0f + 60.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.1f);
            }
        });

        if((this->GetCurHealthPct() <= (1.0f - fHealth)) && coreMath::IsNear(COS(m_fAnimation), 0.0f, 0.01f))
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
                pMission->SetContainerForce(coreVector2(fSpinSign * 550.0f, 0.0f));
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
            const cPlayer*  pPlayer = this->NearestPlayer();
            const coreFloat fAim    = (pPlayer->GetPosition().x - this->GetPosition().x) * Core::System->GetTime() * fTime * 3.0f;

            this->SetPosition         (coreVector3(this->GetPosition().x + fAim, this->GetPosition().y, 0.0f));
            this->DefaultOrientateLerp(0.0f*PI, 20.0f*PI, LERPB(0.0f, 1.0f, 1.0f - fTime));

            PHASE_CONTROL_TICKER(1u, 0u, 1.8f + 2.0f * fTime, LERP_LINEAR)
            {
                const coreVector2 vDir = this->GetDirection().xy();
                const coreVector2 vPos = this->GetPosition ().xy() + vDir * 5.0f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos,                                         vDir)->ChangeSize(1.4f);
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

        if(m_aiCounter[STOMP_COUNT] < 3)
        {
            PHASE_CONTROL_TIMER(0u, 1.3f, LERP_LINEAR)
            {
                const coreFloat vNewX = m_vLastPosition.x - 0.4f * SIGN(m_vLastPosition.x);

                this->SetPosition(coreVector3(coreVector2(LERP(m_vLastPosition.x, vNewX, fTime), m_vLastPosition.y + 0.4f * SIN(PI * fTime)) * FOREGROUND_AREA, 0.0f));

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
                    if(!m_aiCounter[DASH_SIDE]) this->DefaultMultiateLerp(m_aiCounter[DASH_SIDE] ? (2.0f*PI) : (1.0f*PI), m_aiCounter[DASH_SIDE] ? (3.0f*PI) : (2.0f*PI),                                 fDelayTime);
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
            }
        });
    }

    // ################################################################
    // ################################################################

    if((m_iPhase < 10u) && Core::System->GetTime())
    {
        // 
        m_fAnimation.UpdateMod(1.0f, 2.0f*PI);

        // 
        const coreFloat fPrevX = 0.6f * SIN(m_fAnimation - Core::System->GetTime());
        const coreFloat fCurX  = 0.6f * SIN(m_fAnimation);

        // 
        this->SetPosition (coreVector3(fCurX * FOREGROUND_AREA.x, this->GetPosition().yz()));
        this->SetDirection(coreVector3(fCurX - fPrevX, Core::System->GetTime(), 0.0f).Normalized());
    }

    if(m_aiCounter[CONTAINER_ATTACHED])
    {
        // 
        pContainer->SetPosition (this->GetPosition () + NAUTILUS_ATTACH_DIST * this->GetDirection());
        pContainer->SetDirection(this->GetDirection());
    }

    // 
    const coreVector2& vImpact = pMission->GetContainerImpact();
    if(!vImpact.IsNull())
    {
        for(coreUintW i = 20u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact,  vDir)->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact, -vDir)->ChangeSize(1.3f);
            // TODO: lower blue balls get overlapped   
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
}