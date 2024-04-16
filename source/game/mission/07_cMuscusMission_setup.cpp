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
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cMossBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        g_pEnvironment->SetTargetSpeed(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        g_pGame->GetInterface()->ShowMission(this);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u, 2u, 3u, 4u, 5u})
    {
        if(STAGE_BEGINNING)
        {
            cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

            pBackground->GetHeadlight()->ResetFlicker();
            pBackground->SetEnableLightning(false);
            pBackground->SetEnableHeadlight(true);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // ghost enemies only move when nobody is looking
    // gegner müssen "mehr" machen wenn sie im schatten sind, damit sich die dunkel-phase lohnt
    // kontinuierliche geschosse erzeugen eine sich windende schlange, ist aber leicht zu erraten
    // kleine wellen von aktueller position ist wesentlich subtiler, und extrem erschreckend wenn das licht aus ist
    // geschosse sollen nur navigation erschweren, wenn ich auf der linie des gegners bin sollt ich ihn angreifen können, ohne dass mir ein geschoss im weg steht
    // waiting time after the light goes out
    // 0.3f distances
    // dicker gegner am ende kann überraschen
    // wenn gegner außerhalb des lichts von anfang an sichtbar sind, wird die ganze spannung weggenommen (die mechanik wird nicht voll ausgereizt), wenn sie unsichtbar sind muss man ihre position aber erraten können (geschosse)
    // wenn gegner sich anders verhalten innerhalb und außerhalb licht, kommt das nicht zur geltung (zu kurz), da innerhalb des lichts auch gleich getötet wird
    // flucht vor licht (und spieler) ist nervig wenn man versucht auf die gegner zu zielen
    // mehrere verfolgende gegner konvergieren zu stark auf den gleichen pfad
    // TODO: ghost grid (no attack, but disappear with light, last one is true enemy)
    // TODO: big ghost with teleport (back to border, follows other player then)
    // TODO: badge, kill 1 or 3 hidden enemies
    // TODO: coop exploit, spieler stehen einfach ganz links und rechts, was ist wenn immer nur einer ein licht hat, abwechselnd, spieler 2 zuerst, + remove lambda-targeting
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 11u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure((i == 10) ? 120 : 10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });

            pSquad1->GetEnemy(6u)->SetPosition (coreVector3(0.0f, 0.9f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(6u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        });

        STAGE_GET_START(14u)
            STAGE_GET_FLOAT_ARRAY(afFade, 11u)
            STAGE_GET_UINT64     (iLight)
            STAGE_GET_FLOAT      (fBlind)
        STAGE_GET_END

        const auto nSpawnPosFunc = []()
        {
            coreVector2 vPos = coreVector2(0.0f,0.0f);
            coreUintW   iNum = 0u;

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                vPos -= pPlayer->GetPosition().xy();
                iNum += 1u;
            });

            return (vPos * RCP(I_TO_F(iNum))).Processed(SIGN);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 10u, 10u)

            if(((m_iStageSub == 3u) || (m_iStageSub == 7u)) && !STAGE_CLEARED)
            {
                const coreBool bBlackout = (m_iStageSub == 7u);
                if(bBlackout) fBlind = 6.0f;

                d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker(bBlackout ? 1u : 0u);
            }
        }

        if(fBlind)
        {
            fBlind = MAX(fBlind - 1.0f * TIME, 0.0f);
            if(!fBlind) d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker(0u);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if((i == 6u) && STAGE_LIFETIME_BEFORE(1.2f)) return;

            if(i < 2u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, i ? 0.9f : -0.9f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->Rotate90();
            }
            else if(i < 6u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, I_TO_F(4u - i) * 0.3f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                if(STAGE_TAKEOFF)
                {
                    const coreVector2 vPos = nSpawnPosFunc() * FOREGROUND_AREA * 0.9f;
                    const coreVector2 vDir = -vPos.Normalized();

                    pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }

                const cPlayer* pPlayer = pEnemy->NearestPlayerDual(i % 2u);

                pEnemy->DefaultMoveTarget(pPlayer->GetPosition().xy(), 30.0f, 2.0f);
            }

            if(!fBlind)
            {
                if((ABS(pEnemy->GetPosition().x) < FOREGROUND_AREA.x * 1.0f) &&
                   (ABS(pEnemy->GetPosition().y) < FOREGROUND_AREA.y * 1.0f))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetCollisionRadius());
                        const coreFloat   fDot  = coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Normalized());

                        if(fDot > 0.96f) ADD_BIT(iLight, i)
                    });
                }
            }

            if(CONTAINS_BIT(iLight, i)) afFade[i] = MIN(afFade[i] + 4.0f * TIME, 1.0f);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * ((i == 10u) ? 1.9f : 1.1f) * LERPS(2.0f, 1.0f, afFade[i]));
            pEnemy->SetAlpha(LERPS(0.0f, 1.0f, afFade[i]));

            if(STAGE_TICK_LIFETIME((i == 10u) ? 3.0f : 2.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 4u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f + 22.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
                }
            }

            if(CONTAINS_BIT(iLight, i))
            {
                if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                }
            }
        });

        STAGE_WAVE("EINUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_WAVE("ZWEIUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_WAVE("DREIUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Orlac, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u, 7u, 8u, 9u, 10u, 11u})
    {
        if(STAGE_BEGINNING)
        {
            cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

            pBackground->GetHeadlight()->ResetFlicker();
            pBackground->SetEnableLightning(true);
            pBackground->SetEnableHeadlight(false);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // automatic forward movement
    // rückwärts fliegen (dem spieler zeit geben) (beschleunigung interpolieren ??, aber sehr kurz ??)
    // stillstand (in mitte) (rotation + dodge muss genutzt werden), gegner kommen auf ihn zu, geschosse kommen von diagonale (visibility)
    //   - bewegung geht in die mitte (erst Y, dann X), links-rechts in coop, weil gut sichtbar welche gegner welchen spieler dann angreifen
    // erste welle startet nach 2 turns
    // (automatic right-turn on wall-collision ?)
    // schiff hat kleine explosionen und beginnt zu brennen -> orangener helfer kommt am ende raus
    // man kann nicht strafen
    
    // zwei gegner weit auseinander von einer seite fliegen gegenüber (einer wird durch direktes anfliegen getötet, der andere durch 90grad abwarten)
    // durchgezogene linien
    // beim rückwärtsfliegen kann man wand zum stabilisieren ausnutzen -> gegner die am rand (mittig) spawnen und dort bleiben und angreifen (aber bei coop geht dann ecke ecke, vielleicht ein stückchen weg vom rand)
    
    // zwei von oben
    // zwei von rechts
    // zwei von links
    // fünf von rechts
    // fünf von links
    // von allen 8 seiten einzeln (oder im doppel, einzeln hätt aber mehr kontrast ?), muster, start oben rechts, endet links
    // <flip> + wait (r-type remainer)
    // 
    // geschosse von beiden seiten der gegner, gehen mit gegner mit um "wand" zu bauen
    // pacifist ? -> attack on old position when spawning ?   ODER MITTE
    // shared damage ?
    // TODO: general solution to properly override direction
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 32u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
                
                if(i < 16u)
                {
                    pEnemy->SetPosition(coreVector3((I_TO_F(i % 2u) - 1.5f) * 0.25f - 0.5f, 1.2f, 0.0f) * FOREGROUND_AREA3);
    
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                    
                         if(i <  4u/2u) {}
                    else if(i <  8u/2u) pEnemy->Rotate180();
                    else if(i < 12u/2u) pEnemy->Rotate90 ();
                    else if(i < 16u/2u) pEnemy->Rotate270();
                }
                else
                {
                    const coreVector2 vDir = UnpackDirection((((i - 16u) / 2u) * 3u) % 8u);
                    const coreVector2 vTan = vDir.Rotated90();
                    
                    pEnemy->SetPosition(coreVector3(((vDir * 2.0f).Processed(CLAMP, -1.0f, 1.0f) * 1.2f + vTan * ((i % 2u) ? -0.1f : 0.1f)) * FOREGROUND_AREA, 0.0f));
                    
                    pEnemy->SetDirection(coreVector3(-vDir, 0.0f));
                }
            });
        });

        //STAGE_GET_START(48u)
        //    STAGE_GET_VEC2_ARRAY(avTarget, 24u)
        //STAGE_GET_END
#if 1
        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1))
        {
            if(!bFirstHit) return;

            const coreUintW i = pSquad1->GetIndex(pEnemy);

            coreUint8 iGroupNum;
            if(i < 16u) iGroupNum = 2u;
                   else iGroupNum = 2u;

            for(coreUintW j = coreMath::FloorAlign(i, iGroupNum), je = coreMath::CeilAlign(i + 1u, iGroupNum); j < je; ++j)
            {
                if(i != j) pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
            }
        });
#endif
        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1, 16u, 17u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1, 18u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1, 22u, 23u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1, 24u, 25u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1, 26u, 27u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1, 28u, 29u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 30u, 31u)

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                if(m_iStageSub == 1u)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);

                    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(pPlayer->GetPosition(), COLOR_FIRE_ORANGE);
                    // TODO: smoke explosion and smoke should come out 1 second before
                }
                else if(m_iStageSub == 14u)
                {
                    pPlayer->SetDirection(-pPlayer->GetDirection());

                    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(pPlayer->GetPosition(), COLOR_FIRE_ORANGE);
                }
            });
        }

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            const coreFloat   fSign      = (m_iStageSub >= 14u) ? -1.0f : 1.0f;
            const coreVector2 vEffectPos = pPlayer->GetPosition().xy() - pPlayer->GetDirection().xy() * (2.0f * fSign);

            //coreVector2 vNewPos = pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * (/*pPlayer->CalcMoveSpeed()*/50.0f * TIME * 0.7f * fSign);
            coreVector2 vNewPos = pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * (pPlayer->CalcMoveSpeed() * TIME * fSign);

            const coreVector4 vArea = pPlayer->GetArea();

            vNewPos.x = CLAMP(vNewPos.x, vArea.x, vArea.z);
            vNewPos.y = CLAMP(vNewPos.y, vArea.y, vArea.w);

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            g_pSpecialEffects->CreateSplashFire (coreVector3(vEffectPos, 0.0f),  5.0f, 2u, COLOR_FIRE_ORANGE);
            g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), 10.0f, 1u, COLOR_FIRE_ORANGE);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 25.0f);
            

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.3f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.3f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.3f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.3f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            
            //if(STAGE_LIFETIME_POINT(0.0f)) 
                //avTarget[i] = pEnemy->NearestPlayerSide()->GetPosition().xy();

            if(STAGE_TICK_LIFETIME(20.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = -pEnemy->GetDirection().xy();  
                //const coreVector2 vDir = (avTarget[i] - vPos).Normalized();
                //const coreVector2 vDir = StepRotated90(i % 4u);
                const coreVector2 vTan = vDir.Rotated90() * ((i % 2u) ? -1.0f : 1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, pEnemy, vPos, vTan)->ChangeSize(1.4f);   // fast enough to not run into it from behind
            }

            if(pEnemy->ReachedDeath())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
                });
            }
        });

        STAGE_WAVE("SECHSUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        STAGE_WAVE("SIEBENUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        STAGE_WAVE("ACHTUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    STAGE_BOSS(m_Geminga, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 12u, 13u, 14u, 15u, 16u, 17u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 12u})
    {
        STAGE_WAVE("HUNDERTUNDEINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_WAVE("HUNDERTUNDZWEI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_WAVE("HUNDERTUNDDREI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Nagual, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}