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
// setup the Muscus mission
void cMuscusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cMossBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // ghost enemies only move when nobody is looking
    // gegner flüchten aus licht (nach 1s richtung langer achse seitlich, transparent werdend, spawnen dann von der seite)
    // lampe geht aus für wenige sekunden (flimmer -> aus -> flimmer -> an)
    // invert, enemies are attracted by light (+attack), and stay still in dark (may be confusing)
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 18u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);

                
                
                pEnemy->SetPosition(coreVector3(I_TO_F(i% 3u - 1u) * 0.5f, 1.2f, 0.0f) * FOREGROUND_AREA3);

                     if(i < 3u) pEnemy->Rotate90 ();
                else if(i < 6u) pEnemy->Rotate270();
            });
        });

        STAGE_GET_START(18u * 2u + 1u)
            STAGE_GET_FLOAT_ARRAY(afSpeed,  18u)
            STAGE_GET_FLOAT_ARRAY(afEscape, 18u)
            STAGE_GET_FLOAT      (fDisabled)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 17u)

            if(!STAGE_CLEARED) d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker((m_iStageSub == 3u) ? 1u : 0u);       
            
            if(m_iStageSub == 3u) fDisabled = 5.0f;
        }
        
        
        if(fDisabled)
        {
            fDisabled = MAX(fDisabled - 1.0f * Core::System->GetTime(), 0.0f);
            if(!fDisabled)
            {
                d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker(0u);       
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreFloat fOldSpeed = afSpeed[i];
            //afSpeed[i] = MIN(afSpeed[i] + 1.5f * Core::System->GetTime(), 1.0f);
            afSpeed[i] += 1.5f * Core::System->GetTime();
            const coreFloat fRealSpeed = (0.6f - 0.4f * COS(afSpeed[i] * 4.0f)) * MIN(afSpeed[i], 1.0f) * 1.6f;
            
            coreVector2 vEscape = coreVector2(0.0f,0.0f);

            if(!fDisabled)
            {
                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetCollisionRadius());
                    const coreFloat   fDot  = coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Normalized());

                    if(fDot > 0.9f)
                    {
                        afSpeed[i] = 0.0f;
                        vEscape += vDiff.Rotated90() * -SIGN(coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Rotated90()));
                        //vEscape += vDiff;
                    }
                });
            }


            if(afSpeed[i])
            {
                const cPlayer* pPlayer = pEnemy->NearestPlayerDual(i % 2u);       
                pEnemy->DefaultMoveTarget(pPlayer->GetPosition().xy(), 20.0f * fRealSpeed, 20.0f);
                
                //afEscape[i] = 0.0f
                afEscape[i] = 30.0f;
            }
            else
            {
                //afEscape[i] += 30.0f*2.0f * Core::System->GetTime();
                afEscape[i] = MAX(afEscape[i] - 30.0f * Core::System->GetTime(), 0.0f);
                //if(afEscape[i] >= 15.0f*4.0f)
                {
                    //pEnemy->DefaultMoveForward(vEscape.Normalized(), afEscape[i]);
                    //pEnemy->SetPosition(pEnemy->GetPosition().Processed(CLAMP, -FOREGROUND_AREA.x, FOREGROUND_AREA.x));
                }
                
                if(STAGE_TICK_FREE(10.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();    

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(1.25f);
                }
            }
            
            
            
            
            const coreFloat fActive = MIN(afSpeed[i], 1.0f);
            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * LERPS(1.0f, 1.2f, fActive));
            pEnemy->SetAlpha(LERPS(1.0f, 0.3f, fActive));
            
            if((FRACT(fOldSpeed  * 4.0f / (2.0f*PI)) >
                FRACT(afSpeed[i] * 4.0f / (2.0f*PI))) && afSpeed[i]     && false)
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.2f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos,        vDir)->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.5f);
            }

            if(afSpeed[i])
            {
                if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
        });

        STAGE_WAVE("EINUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Orlac, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Geminga, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Nagual, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}