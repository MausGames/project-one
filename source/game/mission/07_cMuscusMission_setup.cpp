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
    STAGE_MAIN
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

            if((m_iStageSub >= 2u) && !STAGE_CLEARED)
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