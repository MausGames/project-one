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
// setup the Calor mission
void cCalorMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // evade being attacked
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                //pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
                
                
                pEnemy->SetPosition(coreVector3((I_TO_F(i) - 2.5f) * 0.2f, 0.3f, 0.0f) * FOREGROUND_AREA3);
            });
        });


        STAGE_GET_START(24u)
            STAGE_GET_VEC2_ARRAY (avTargetPos, 6u)
            STAGE_GET_FLOAT_ARRAY(afEnergy,    6u)
            STAGE_GET_FLOAT_ARRAY(afRecover,   6u)
        STAGE_GET_END


        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(STAGE_TAKEOFF)
            {
                avTargetPos[i] = pEnemy->GetPosition().xy();
            }

            const cBullet* pBullet = g_pGame->GetBulletManagerPlayer()->FindBullet(pEnemy->GetPosition().xy());
            if(pBullet)
            {
                const coreVector2 vDiff = avTargetPos[i] - pBullet->GetPosition().xy();
                const coreFloat   fLen  = vDiff.Length();
                
                if(fLen < 20.0f)
                {
                    coreVector2 vCurPos = avTargetPos[i] + vDiff.Normalized() * MAX(20.0f - fLen, 0.0f);
                    
                    //vCurPos.x = CLAMP(vCurPos.x, -1.0f * FOREGROUND_AREA.x, 1.0f * FOREGROUND_AREA.x);
                    //vCurPos.y = CLAMP(vCurPos.y, -1.0f * FOREGROUND_AREA.y, 1.0f * FOREGROUND_AREA.y);
                    
                    avTargetPos[i].x = vCurPos.x;
                }
            }
                    
                    
                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                    {
                        if(pEnemy == pEnemy2) return;

                        const coreVector2 vDiff  = avTargetPos[i] - avTargetPos[j];
                        const coreFloat   fPower = POW2(pEnemy->GetCollisionRadius() * 2.0f) - vDiff.LengthSq();

                        if((fPower > 0.0f) && !vDiff.IsNull())
                        {
                            constexpr coreVector2 vBase = coreVector2(0.0f,0.0f);

                            const coreFloat A = (avTargetPos[i] - vBase).LengthSq();
                            const coreFloat B = (avTargetPos[j] - vBase).LengthSq();

                            const coreVector2 vPush = vDiff.Normalized() * (fPower * Core::System->GetTime());

                            if(A > B) avTargetPos[i] = avTargetPos[i] + vPush;
                                 else avTargetPos[j] = avTargetPos[j] - vPush;
                        }
                    });

//sind grau und werden farbig nachdem sie ausgewichen sind
// fixe bewegungen wie die wellen bei moses, teilen saich auf, abhängig von schussrichtung(!) und gehen dnan wieder zusammen, reagieren erneut mit leichtem delay, sind ghost wärend grau
// twist, reagieren mehrmals auf geshcosse
// reagieren auf explosionen
// schießen wärend sie farbe haben, an alte position vom spieler
// spawn like a wave

            avTargetPos[i].x = CLAMP(avTargetPos[i].x, -1.0f * FOREGROUND_AREA.x, 1.0f * FOREGROUND_AREA.x);
            avTargetPos[i].y = CLAMP(avTargetPos[i].y, -1.0f * FOREGROUND_AREA.y, 1.0f * FOREGROUND_AREA.y);

            pEnemy->DefaultMoveSmooth(coreVector2(avTargetPos[i].x, pEnemy->GetPosition().y) / FOREGROUND_AREA, 30.0f, 20.0f);
            
        });

        STAGE_WAVE("SECHSUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Fenrir, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Shelob, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Zeroth, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}