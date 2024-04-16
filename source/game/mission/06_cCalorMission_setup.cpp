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
    // twist, reagieren mehrmals auf geshcosse
    // reagieren auf explosionen
    // schießen wärend sie farbe haben, an alte position vom spieler
    // spawn like a wave
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();


                pEnemy->SetPosition(coreVector3((I_TO_F(i % 6u) - 2.5f) * 0.1f, I_TO_F(i / 6u) * 0.1f, 0.0f) * FOREGROUND_AREA3);

                pEnemy->SetPosition(coreVector3(0.0f,0.0f,0.0f) * FOREGROUND_AREA3);
            });
        });


        STAGE_GET_START(36u * 6u)
            STAGE_GET_VEC2_ARRAY (avSourcePos, 36u)
            STAGE_GET_VEC2_ARRAY (avTargetPos, 36u)
            STAGE_GET_FLOAT_ARRAY(afEnergy,    36u)
            STAGE_GET_FLOAT_ARRAY(afRecover,   36u)
        STAGE_GET_END


        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(STAGE_TAKEOFF)
            {
                avSourcePos[i] = avTargetPos[i] = pEnemy->GetPosition().xy();
            }

            const cBullet* pBullet = g_pGame->GetBulletManagerPlayer()->FindBullet(pEnemy->GetPosition().xy());
            if(pBullet)
            {
                const coreVector2 vDiff = avTargetPos[i] - (pBullet->GetPosition().xy() + pBullet->GetFlyMove());
                const coreFloat   fLen  = vDiff.Length();

                //if(fLen < 20.0f)
                if(fLen < 10.0f)
                {
                    const coreFloat A = 50.0f;
                    const coreFloat B = 3.0f;

                    const coreVector2 vDiff2 = coreVector2::Direction(I_TO_F(i) / 36.0f * 2.0f*PI);

                    //const coreFloat fPower = CLAMP(MAX(1.0f - fLen/20.0f, 0.0f) * A + afEnergy[i], 0.0f, A / B) * MAX(1.0f - afRecover[i] * 3.0f, 0.0f);
                    //coreVector2 vCurPos = avTargetPos[i] + fPower * (vDiff2 * 2.0f);//(vDiff.Normalized() * LERP(pBullet->GetFlyDir().Rotated90().Processed(ABS), coreVector2(1.0f,1.0f), 0.00f)).Normalized();
                    const coreFloat fPower = afEnergy[i] ? 0.0f : ((A / B) * (I_TO_F((i % 3u)+1u) * 0.5f));
                    coreVector2 vCurPos = avTargetPos[i] + fPower * vDiff2;//(vDiff.Normalized() * LERP(pBullet->GetFlyDir().Rotated90().Processed(ABS), coreVector2(1.0f,1.0f), 0.00f)).Normalized();

                    afEnergy[i] -= fPower * B;

                    avTargetPos[i] = vCurPos;
                }
            }

            if(afEnergy[i])
            {
                const coreFloat fPrevRecover = afRecover[i];

                afRecover[i] += 1.0f * Core::System->GetTime();
                if(afRecover[i] >= 1.0f)
                {
                    avTargetPos[i] = avSourcePos[i];

                    afEnergy [i] = 0.0f;
                    afRecover[i] = 0.0f;
                }

                if((fPrevRecover < 0.3f) && (afRecover[i] >= 0.3f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }

            if(afEnergy[i])
            {
                if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                }
            }


            pEnemy->DefaultMoveSmooth(coreVector2(avTargetPos[i]) / FOREGROUND_AREA, 150.0f, 12.0f);
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