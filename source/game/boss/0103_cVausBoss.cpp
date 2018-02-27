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
#define SCOUT_RESURRECTIONS (0u)
#define IGNORE_BALL         (1u)
#define SUB_PHASE           (2u)
#define LASER_SHOT          (3u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cVausBoss::cVausBoss()noexcept
: m_iScoutOrder (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_vaus_high.md3");
    this->DefineModelLow ("ship_boss_vaus_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.5f,2.5f,2.5f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_YELLOW);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        m_aRay[i].DefineModel  ("object_tube.md3");
        m_aRay[i].DefineTexture(0u, "effect_energy.png");
        m_aRay[i].DefineProgram("effect_energy_invert_program");
        m_aRay[i].SetColor3    (i ? (COLOR_ENERGY_RED * 0.9f) : (COLOR_ENERGY_BLUE * 0.8f));
        m_aRay[i].SetTexSize   (TORUS_RAY_TEXSIZE);          
        m_aRay[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    {
        m_aCompanion[i].DefineModelHigh("object_boss_vaus_companion_high.md3");
        m_aCompanion[i].DefineModelLow ("object_boss_vaus_companion_low.md3");
        m_aCompanion[i].SetSize        (coreVector3(2.2f,2.2f,2.2f));
        m_aCompanion[i].Configure      (1, COLOR_SHIP_YELLOW);
    }

    // TODO: companions must rotate sometimes (after bounce ?)
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized());
    pMission->EnablePaddle(0u, this);
}


// ****************************************************************
// 
void cVausBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->DisableBall(0u, bAnimated);

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        pMission->DisablePaddle(i, bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnAttack()
{
    if(m_aRay[1].IsEnabled(CORE_OBJECT_ENABLE_RENDER) ||
       m_aRay[2].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_aRay[1].Render();
        m_aRay[2].Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[1]);
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[2]);
    }

    if(m_aRay[0].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_aRay[0].Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[0]);
    }
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);
    cEnemySquad*    pSquad   = pMission->GetEnemySquad(0u);

    constexpr coreFloat fLimit1 = 0.8f;
    constexpr coreFloat fLimit2 = 0.1f;

    // ################################################################
    // ################################################################


    // boss geht weiter runter mit lebenspunkte bis zur grenze bei der finale phase beginnt


    //PHASE_CONTROL_TICKER(1u, 0u, 2.0f)         
    //{
    //    //if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
    //    //i = 1u - i;
    //    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    //    {
    //        const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
    //        const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();
    //
    //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeRed();
    //    }
    //
    //    //const coreVector2 vPos = pBall->GetPosition ().xy();
    //    //const coreVector2 vDir = -pBall->GetDirection().xy();//(g_pGame->FindPlayer(vPos)->GetPosition().xy() - vPos).Normalize();
    //    //
    //    //g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeGreen();
    //});





    if(m_iPhase < 10u)
    {
        // 
        PHASE_CONTROL_TICKER(2u, 0u, 1.2f)
        {
            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->MakeOrange();
            });
        });

        // 
        pSquad->ForEachEnemyAll([this](cEnemy* OUTPUT pEnemy, const coreUintW i) // TODO don't calculate if all dead (m_iPhase < 10u) 
        {
            const coreUintW x = i % VAUS_SCOUTS_X;
            const coreUintW y = i / VAUS_SCOUTS_X;

            const coreBool    bFront   = (y + (CONTAINS_BIT(m_iScoutOrder, x) ? 1u : 0u)) & 0x01u;
            const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + (bFront ? 0.2f : 0.0f));

            if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
            {
                if(!bFront) TOGGLE_BIT(m_iScoutOrder, x)

                if(m_aiCounter[SCOUT_RESURRECTIONS] < VAUS_SCOUTS_TOTAL)
                {
                    ++m_aiCounter[SCOUT_RESURRECTIONS];
                    pEnemy->Resurrect((vGridPos + coreVector2(0.0f,3.0f)) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
                }
            }

            pEnemy->DefaultMoveSmooth(vGridPos, 30.0f, 10.0f);

            STATIC_ASSERT(VAUS_SCOUTS_X <= sizeof(m_iScoutOrder)*8u)
        });
    }

    // 
    if(!m_aiCounter[IGNORE_BALL])
    {
        this->SetPosition(coreVector3(pBall->GetPosition().x, this->GetPosition().yz()));
        m_aCompanion[0].SetPosition(coreVector3(m_aCompanion[0].GetPosition().x, pBall->GetPosition().y, m_aCompanion[0].GetPosition().z));
        m_aCompanion[1].SetPosition(coreVector3(m_aCompanion[1].GetPosition().x, pBall->GetPosition().y, m_aCompanion[1].GetPosition().z));
    }

    // 
    if(g_pGame->GetCurMission()->GetCurBoss() == this)
    {
        if(pMission->GetBounceState())
        {
            for(coreUintW j = 0u; j < VIRIDO_PADDLES; ++j)
            {
                if(CONTAINS_BIT(pMission->GetRealState(), j)) continue;
                coreObject3D* pPaddle = pMission->GetPaddle(j);

                const coreVector2 vPos   = pPaddle->GetPosition ().xy();
                const coreFloat   fAngle = pPaddle->GetDirection().xy().Angle();

                const auto nMakeColor = j ? &cOrbBullet::MakeRed : &cOrbBullet::MakeBlue;


                const coreVector2 vRealDir = pPaddle->GetDirection().xy(); 
                const coreFloat fOff = 30.0f;


                for(coreUintW i = 0u; i < (j ? 5u : 7u); ++i)
                {
                    const coreVector2 vDir = coreVector2::Direction(fAngle + 0.1f * I_TO_F(i - (j ? 2u : 3u)));

                    (g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(3.0f+fOff) - vRealDir*fOff, vRealDir)->*nMakeColor)();
                    (g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(0.5f+fOff) - vRealDir*fOff, vRealDir)->*nMakeColor)();
                }
            }
        }
    }


    if(pMission->GetRealState())
    {
        // 
        PHASE_CONTROL_TICKER(3u, 0u, 12.0f)
        {
            if((iTick % 12u) < 4u)
            {
                const coreVector2 vPos = pBall->GetPosition().xy();
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos,  vDir)->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, -vDir)->MakeOrange();
            }
        });

        this->DefaultMoveSmooth(coreVector2(0.0f, 0.95f - 0.95f * (fLimit1 - this->GetCurHealthPct()) * RCP(fLimit1)), 100.0f, 10.0f);
    }


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        coreObject3D& oCurRay = m_aRay[i];

        if(oCurRay.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            const coreFloat fSize = 50.0f;

            const coreObject3D* pPaddle = pMission->GetPaddle(i);

            oCurRay.SetPosition (coreVector3(pPaddle->GetPosition().xy() + pPaddle->GetDirection().xy() * (fSize + 3.0f), 0.0f));
            oCurRay.SetSize     (coreVector3(1.2f, fSize, 1.2f));
            oCurRay.SetDirection(coreVector3(pPaddle->GetDirection().xy(), 0.0f));
            //m_Ray.SetAlpha();
            oCurRay.SetTexOffset(coreVector2(0.4f,0.3f) * -coreFloat(Core::System->GetTotalTime()));   // TODO: fract   
            oCurRay.Move();
        }
    }

    // 
    //g_pEnvironment->SetTargetSide(this->GetPosition().xy() * MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f);
    //g_pEnvironment->SetTargetDirection((this->GetPosition().xy() * coreVector2(MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f, 1.0f)).Normalize());

    //g_pPostProcessing->SetValue(0.8f);
}


// ****************************************************************
// 
coreBool cVausBoss::__ExecuteCompanionAttack(const coreUintW iType, const coreFloat fTime)
{
    //if((iType == 1u) && ((m_aiCounter[SUB_PHASE] % 3) == 1))
    //    return true;

    cViridoMission* pMission  = s_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall     = pMission->GetBall(0u);
    const coreFloat fBallPosY = pBall->GetPosition().y / FOREGROUND_AREA.y;

    // 
    coreFloat fFrom1, fFrom2, fTo1, fTo2;
    switch(m_aiCounter[SUB_PHASE] % 3)
    {
    default:// ASSERT(false)
    case 0: fFrom1 = fBallPosY; fFrom2 = -1.0f; fTo1 = -1.0f; fTo2 = fBallPosY; break;
   // case 1: fFrom1 = 0.9f; fFrom2 = -0.9f; fTo1 = 0.9f; fTo2 = -0.9f; break;
   // case 2: fFrom1 = 0.9f; fFrom2 = -0.9f; fTo1 = 0.9f; fTo2 = -0.9f; break;
    }

    // 
    if(m_aiCounter[SUB_PHASE] % 2u)
    {
        std::swap(fFrom1, fFrom2);
        std::swap(fTo1,   fTo2);
    }

    // 
    switch(iType)
    {
    default: ASSERT(false)
    case 0u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fBallPosY), coreVector2(-0.95f, fFrom1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fBallPosY), coreVector2( 0.95f, fFrom2), fTime);
        break;

    case 1u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fFrom1), coreVector2(-0.95f, fTo1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fFrom2), coreVector2( 0.95f, fTo2), fTime);
        break;

    case 2u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fTo1), coreVector2(-0.95f, fBallPosY), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fTo2), coreVector2( 0.95f, fBallPosY), fTime);
        break;

    case 3u:
        break;
    }

   // PHASE_CONTROL_TICKER(1u, 0u, 10.0f)
   // {
   //     static coreUintW i = 0u;  
   //     if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
   //     i = 1u - i;
   //     //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
   //     {
   //         const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
   //         const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();
   //
   //         g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, vDir)->MakeOrange();
   //     }
   // });

    return false;
}


/*
laser von allen 3 bosse nausgehend
bei phasenwechsel seitenbosse sterben, dropen schilde zum einsammeln (markiert)
boss bringt seinen laser zur mitte
schieﬂt ball mit aller kraft nach unten, muss man ausweichen, ball grˆﬂer schneller
spieler kann schild nehmen und auch ball so schieﬂen (aber nicht aktiv)
laser in mitte (oder seite) blockiert unbeschadet spieler oder paddle


*/