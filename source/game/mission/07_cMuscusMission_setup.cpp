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
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
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
    // TODO 1: ghost grid (no attack, but disappear with light, last one is true enemy)
    // TODO 1: big ghost with teleport (back to border, follows other player then)
    // TODO 1: badge, kill 1 or 3 hidden enemies
    // TODO 1: coop exploit, spieler stehen einfach ganz links und rechts, was ist wenn immer nur einer ein licht hat, abwechselnd, spieler 2 zuerst, + remove lambda-targeting
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
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 10u, 10u)

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
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetVisualRadius());
                        const coreFloat   fDot  = coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Normalized());

                        if(fDot > 0.96f) ADD_BIT(iLight, i)
                    });
                }
            }

            if(HAS_BIT(iLight, i)) afFade[i] = MIN(afFade[i] + 4.0f * TIME, 1.0f);

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

            if(HAS_BIT(iLight, i))
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                }
            }
        });

        STAGE_WAVE("SIEBENUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // damaging fields hide themselves (d/generation)
    // idea: field chases player and gets visible regularly
    // idea: schräge bewegung von schrägen blöcken
    // idea: snake
    // idea: linien auf allen 4 seiten die sich unsichtbar richtung mitte bewegen (außen-blöcke sichtbar)
    // idea: block in enemy
    // enemies should be aligned to same boundary as blocks
    // grow kann ich nicht machen, der spieler muss die größe einschätzen können, somit muss sowohl größe als auch geschwindigkeit konstant bleiben
    // spieler muss nicht sehen wie jeder block aus bild geht, lenkt nur ab, und ist eh klar wenn intro-block sichtbar bild verlässt
    // TODO 1: badge
    // TODO 1: alles nur von oben kommen lassen, mit seiten gegnern
    // TODO 1: spawn of rotation-blocks too dangerous, can easily hit player
    // TODO 1: high tide in nCreateGenerate
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreFloat fStep        = 0.275f;
        constexpr coreFloat fStepDelay   = 0.35f;
        constexpr coreFloat fSpeed       = 0.8f;
        constexpr coreUintW iTypeMapSize = coreMath::CeilAlign(MUSCUS_GENERATES, 4u) / 4u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(iTypeMapSize + MUSCUS_GENERATES * 3u + 5u)
            STAGE_GET_UINT_ARRAY (aiGenerateTypeMap, iTypeMapSize)
            STAGE_GET_VEC2_ARRAY (avGenerateData,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT_ARRAY(afGenerateTime,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT      (fRotation,         fRotation =  0.5f*PI;)
            STAGE_GET_FLOAT      (fSpawn,            fSpawn    = -1.0f)
            STAGE_GET_UINT       (iSpawnTick)
            STAGE_GET_UINT       (iSpawnCount)
            STAGE_GET_FLOAT      (fSpawnOffset)
        STAGE_GET_END

        coreUint8* aiGenerateType = r_cast<coreUint8*>(aiGenerateTypeMap);

        const auto nCreateGenerate = [&](const coreUint8 iType, const coreFloat fFactor, const coreFloat fOffset, const coreFloat fDelay)
        {
            for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            {
                coreObject3D* pGenerate = (*m_Generate.List())[i];

                if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    this->EnableGenerate(i);

                    aiGenerateType[i] = iType;
                    avGenerateData[i] = coreVector2(fFactor, fOffset);
                    afGenerateTime[i] = -fDelay;

                    return;
                }
            }

            WARN_IF(true) {}
        };

        const auto nMoveGenerate = [](const coreSpline2* pRawPath, const coreVector2 vFactor, const coreVector2 vRawOffset, const coreFloat fTime)
        {
            const coreVector2 vPos = pRawPath->CalcPosition(FMOD(MAX(fTime, 0.0f), pRawPath->GetTotalDistance()));
            return coreVector3(((vPos * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 24u, 31u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 32u, 47u)
            else if(STAGE_SUB(8u)) STAGE_DELAY_START

            iSpawnCount  = 0u;
            fSpawnOffset = (1.0f - fSpawn) / fSpeed;
        }

        fSpawn += TIME * (fSpeed * RCP(pPath1->GetTotalDistance() * 0.5f));
        if(fSpawn >= 1.0f)
        {
            fSpawn -= 1.0f;

            const cPlayer*  pTarget = g_pGame->FindPlayerDual(iSpawnTick % 2u);
            const coreFloat fOffset = pTarget->GetPosition().x / FOREGROUND_AREA.x;
            const coreFloat fHalf   = fStep * 0.5f;

            if(m_iStageSub == 1u)
            {
                // #
                nCreateGenerate(0u, 1.0f, fOffset, 0.0f);
            }
            else if(m_iStageSub == 2u)
            {
                //    #
                // ## #
                if(iSpawnCount % 2u)
                {
                    nCreateGenerate(0u, 1.0f, fOffset - fHalf, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset + fHalf, 0.0f);
                }
                else
                {
                    nCreateGenerate(0u, 1.0f, fOffset, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset, fStepDelay);
                }
            }
            else if(m_iStageSub == 3u)
            {
                // ##  ##  #  ##
                // ## ##  ###  ## ####
                switch(iSpawnCount % 5u)
                {
                default: ASSERT(false)
                case 0u:
                    nCreateGenerate(0u, 1.0f, fOffset - fHalf, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset + fHalf, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset - fHalf, fStepDelay);
                    nCreateGenerate(0u, 1.0f, fOffset + fHalf, fStepDelay);
                    break;
                case 1u:
                    nCreateGenerate(0u, 1.0f, fOffset - fStep, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset,         0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset,         fStepDelay);
                    nCreateGenerate(0u, 1.0f, fOffset + fStep, fStepDelay);
                    break;
                case 2u:
                    nCreateGenerate(0u, 1.0f, fOffset - fStep, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset,         0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset,         fStepDelay);
                    nCreateGenerate(0u, 1.0f, fOffset + fStep, 0.0f);
                    break;
                case 3u:
                    nCreateGenerate(0u, 1.0f, fOffset - fStep, fStepDelay);
                    nCreateGenerate(0u, 1.0f, fOffset,         fStepDelay);
                    nCreateGenerate(0u, 1.0f, fOffset,         0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset + fStep, 0.0f);
                    break;
                case 4u:
                    nCreateGenerate(0u, 1.0f, fOffset - fHalf * 3.0f, 0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset - fHalf,        0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset + fHalf,        0.0f);
                    nCreateGenerate(0u, 1.0f, fOffset + fHalf * 3.0f, 0.0f);
                    break;
                }
            }
            else if(m_iStageSub == 4u)
            {
                // 2 4 5 3
                const coreUint8 iSide = (iSpawnCount % 2u) ? 1u : 3u;

                coreUintW iHole;
                switch(iSpawnCount % 4u)
                {
                default: ASSERT(false)
                case 0u: iHole = 2u; break;
                case 1u: iHole = 4u; break;
                case 2u: iHole = 5u; break;
                case 3u: iHole = 3u; break;
                }

                for(coreUintW i = 0u; i < 8u; ++i)
                {
                    if(i != iHole) nCreateGenerate(iSide, 1.0f, fStep * (I_TO_F(i) - 3.5f), 0.0f);
                }
            }
            else if(m_iStageSub == 5u)
            {
                // 1111 2222
                const coreUint8 iSide = (iSpawnCount % 2u) ? 4u : 0u;

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    nCreateGenerate(0u, 1.0f, fStep * (I_TO_F(i + iSide) - 3.5f), 0.0f);
                }
            }
            else if(m_iStageSub == 6u)
            {
                // 12 12 12 12
                const coreUint8 iSide = (iSpawnCount % 2u) ? 0u : 1u;

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    nCreateGenerate(0u, 1.0f, fStep * (I_TO_F(i * 2u + iSide) - 3.5f), 0.0f);
                }
            }
            else if(m_iStageSub == 7u && !iSpawnCount)
            {
                // rotation
                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    nCreateGenerate(4u, 1.0f, 0.0f, 0.0f);
                }
            }

            iSpawnTick  += 1u;
            iSpawnCount += 1u;   // reset on next sub-stage
        }

        if(m_iStageSub == 1u)
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

            if(STAGE_BEGINNING)
            {
                pHelper->Resurrect();
                nCreateGenerate(0u, 1.0f, 0.0f, 0.0f);
            }

            constexpr coreFloat fDelay = 0.6f;

            if(!HAS_FLAG(pHelper->GetStatus(), HELPER_STATUS_DEAD) && STAGE_TIME_AFTER(fDelay))
            {
                pHelper->SetPosition(coreVector3(LERP(-1.2f, 0.0f, m_fStageTime - fDelay) * FOREGROUND_AREA.x, 0.0f, 0.0f));

                if(STAGE_TIME_AFTER(1.0f + fDelay))
                {
                    pHelper->Kill(false);

                    this->BangGenerate(0u);

                    g_pSpecialEffects->MacroEruptionColorSmall(pHelper->GetPosition(), coreVector2(1.0f,0.0f), COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                }
            }
        }
        else if(m_iStageSub == 7u && iSpawnCount)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(0.0f, 1.0f, MIN(m_fStageSubTime * 0.1f, 1.0f));
        }
        else if(m_iStageSub == 8u)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(1.0f, 0.0f, MIN(m_fStageSubTime * 1.0f, 1.0f));

            for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                this->ShowGenerate(i, 1.0f);

            if(m_fStageSubTime >= 1.5f)
            {
                for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                    this->DisableGenerate(i, true);

                g_pSpecialEffects->MacroExplosionColorBig(coreVector3(0.0f,0.0f,0.0f), COLOR_ENERGY_GREEN);

                STAGE_DELAY_END
            }
        }

        const coreVector2 vRotaDir = coreVector2::Direction(fRotation);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, fSpeed, (i < 2u) ? 2.0f : ((i < 16u) ? 0.0f : ((i < 32u) ? (fSpawnOffset + ((((i - 16u) % 8u) < 4u) ? 0.0f : 1.3f)) : 3.0f)))

            if(i < 16u || i >= 32u)
            {
                const coreFloat fPos = fStep * (((i % 4u) < 2u) ? 0.5f : 1.5f);

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i % 2u) ? 1.0f : -1.0f) * fPos, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) {}
                else if(i <  8u) pEnemy->Rotate180();
                else if(i < 12u) pEnemy->Rotate90 ();
                else if(i < 16u) pEnemy->Rotate270();
                else if(i < 36u) {}
                else if(i < 40u) pEnemy->Rotate180();
                else if(i < 44u) pEnemy->Rotate90 ();
                else if(i < 48u) pEnemy->Rotate270();
            }
            else
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((((i - 16u) % 8u) < 4u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(((i < 24u) ? (I_TO_F((i - 16u) % 4u) + 0.5f) : (I_TO_F(((i - 24u) % 4u) * 2u) - 3.5f)) * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
        });

        coreUintW iRotaIndex = 0u;

        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
        {
            coreObject3D* pGenerate = (*m_Generate.List())[i];
            if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            afGenerateTime[i] += 1.0f * TIME;

            if(aiGenerateType[i] < 4u)
            {
                const coreVector2 vFactor = coreVector2(avGenerateData[i].x, 1.0f);
                const coreVector2 vOffset = coreVector2(avGenerateData[i].y, 0.0f);
                const coreFloat   fTime   = afGenerateTime[i] * fSpeed;

                pGenerate->SetPosition(nMoveGenerate(pPath1, vFactor, vOffset * vFactor, fTime));

                switch(aiGenerateType[i])
                {
                default: ASSERT(false)
                case 0u: break;
                case 1u: pGenerate->SetPosition( pGenerate->GetPosition().RotatedZ90()); break;
                case 2u: pGenerate->SetPosition(-pGenerate->GetPosition());              break;
                case 3u: pGenerate->SetPosition(-pGenerate->GetPosition().RotatedZ90()); break;
                }

                if(fTime <  0.5f)                       this->ShowGenerate   (i, 0.1f);
                if(fTime >= pPath1->GetTotalDistance()) this->DisableGenerate(i, false);
            }
            else if((aiGenerateType[i] == 4u) && (m_iStageSub < 9u))
            {
                const coreFloat fTime   = CLAMP(afGenerateTime[i] * 0.5f - ABS(I_TO_F(iRotaIndex - 5u)) * 0.5f, 0.0f, 1.0f);
                const coreFloat fOffset = (I_TO_F(iRotaIndex - 5u) * fStep + LERPB(1.5f, 0.0f, fTime) * ((iRotaIndex < 5u) ? -1.0f : 1.0f)) * ((m_iStageSub == 8u) ? LERPB(0.0f, 1.0f, CLAMP(1.5f - m_fStageSubTime, 0.0f, 1.0f)) : 1.0f);

                pGenerate->SetPosition (coreVector3(vRotaDir * fOffset * FOREGROUND_AREA, 0.0f));
                pGenerate->SetDirection(coreVector3(vRotaDir,                             0.0f));

                if(!fTime)           this->ShowGenerate(i, 2.0f);
                if(iRotaIndex == 5u) this->ShowGenerate(i, 1.0f);

                iRotaIndex += 1u;
            }

            if(m_iStageSub < 8u)
            {
                cPlayer::TestCollision(PLAYER_TEST_NORMAL, pGenerate, [&](cPlayer* OUTPUT pPlayer, const coreObject3D* pGenerate, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;

                    pPlayer->TakeDamage(5, ELEMENT_GREEN, vIntersection.xy());

                    this->BangGenerate(i);

                    g_pSpecialEffects->CreateSplashColor(pGenerate->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                });
            }
        }

        STAGE_WAVE("ACHTUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all generates
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            this->DisableGenerate(i, false);

        g_pGame->GetHelper(ELEMENT_GREEN)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
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
    // collect lots of shiny pills (pacman, snake)
    // idea: enemy has orb shield or tail you need to collect
    // idea: throw lines of balls form side into field
    // idea: ball lines shot from left or right with gravity down and different x-force
    // in the chain-stage, balls need to be far enough apart to not accidentally spawn ball into player
    // enemy should die with the last ball, not require the player to attack, to remove confusion and delay
    // ball animations need to be in a sequential pattern
    // the combination of collecting stuff and shooting at stuff was a disaster (because of the requirement to collect everything to advance), it was possible to give collecting a meaning, by causing "changes" (e.g. enemy invincibility), but it felt very sluggish (A, B, A, B), also enemies dropping the balls cause players to move into enemies while killing (for speed) which felt forced
    // TODO 1: helper in one of the balls
    // TODO 1: badge, hidden balls 1-N, other color, with special effect and sound
    // TODO 1: pearls fly at enemy and damage him (visibly) (Ikaruga rockets)
    // TODO 1: enemy attacks with pearls at the end, while bullets fly at you (diagonally ?)
    // TODO 1: bouncing pearls (linear, not gravity)
    // TODO 1: big pearl cluster at the end
    // TODO 1: fix 7-pearl loops
    // TODO 1: if chroma-enemies come, spawn them in delay phase
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2( 0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(-0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(1, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(2u + MUSCUS_PEARLS)
            STAGE_GET_FLOAT      (fEnemyMove)
            STAGE_GET_UINT       (iPearlSpawn)
            STAGE_GET_FLOAT_ARRAY(afPearlTime, MUSCUS_PEARLS)
        STAGE_GET_END

        const auto nGetPearl = [this](const coreUintW iIndex)
        {
            ASSERT(iIndex < MUSCUS_PEARLS)

            coreObject3D* pPearl = &m_aPearlRaw[iIndex * 2u];
            return (pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE) && HAS_BIT(m_iPearlActive, iIndex)) ? pPearl : NULL;
        };

        const auto nCreatePearlWave = [this](const coreUintW iCount)
        {
            for(coreUintW i = 0u; i < iCount; ++i)
                this->EnablePearl(i);
        };

        const auto nMovePearl = [](const coreSpline2* pRawPath, const coreVector2 vFactor, const coreVector2 vRawOffset, const coreFloat fTime)
        {
            const coreVector2 vPos = pRawPath->CalcPosition(FMOD(MAX(fTime, 0.0f), pRawPath->GetTotalDistance()));
            return coreVector3(((vPos * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f);
        };

        const coreSet<coreObject3D*>* papList = m_Pearl.List();
        if(std::all_of(papList->begin(), papList->end(), [](const coreObject3D* pPearl) {return !pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE);}))
        {
                 if(STAGE_SUB(1u)) nCreatePearlWave(14u);
            else if(STAGE_SUB(2u)) nCreatePearlWave( 5u);
            else if(STAGE_SUB(3u)) nCreatePearlWave( 5u);
            else if(STAGE_SUB(4u)) nCreatePearlWave(16u);
            else if(STAGE_SUB(5u)) nCreatePearlWave(16u);
            else if(STAGE_SUB(6u)) nCreatePearlWave( 1u);
            else if(STAGE_SUB(7u)) nCreatePearlWave(20u);
            else if(STAGE_SUB(8u)) pSquad1->GetEnemy(0u)->Kill(true);

            if(m_iStageSub == 2u || m_iStageSub == 6u || m_iStageSub == 7u)
                fEnemyMove = 0.0f;

            std::memset(afPearlTime, 0, sizeof(coreFloat) * MUSCUS_PEARLS);
        }

        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            coreObject3D* pPearl = nGetPearl(i);
            if(!pPearl) continue;

            if(m_iStageSub == 1u)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 6.5f) * 0.15f, SIN(afPearlTime[i] * 1.3f + I_TO_F(i) * 0.15f) + LERPB(1.2f, 0.0f, MIN(afPearlTime[i] * 0.5f, 1.0f)));

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else if(m_iStageSub <= 5u)
            {
                const coreSpline2* pPath   = (m_iStageSub <= 3u) ? pPath3 : pPath2;
                const coreVector2  vFactor = coreVector2((m_iStageSub == 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2  vOffset = coreVector2((m_iStageSub <= 3u) ? 0.0f : ((m_iStageSub == 4u) ? -0.5f : (SIN(I_TO_F(i) / 16.0f * (4.0f*PI)) * 0.2f + 0.5f)), 0.0f);
                const coreFloat    fTime   = afPearlTime[i] * 1.2f - I_TO_F(i) * 0.15f;

                pPearl->SetPosition(nMovePearl(pPath, vFactor, vOffset * vFactor, fTime));

                     if(m_iStageSub == 4u) pPearl->SetPosition(-pPearl->GetPosition().RotatedZ90());
                else if(m_iStageSub == 5u) pPearl->SetPosition( pPearl->GetPosition().RotatedZ90());
            }
            else if(m_iStageSub == 6u)
            {
                coreVector2 vPos;
                switch(i)
                {
                default: ASSERT(false)
                case  0u: vPos = coreVector2( 0.3f, 0.0f); break;
                case  1u: vPos = coreVector2( 0.6f, 0.0f); break;
                case  2u: vPos = coreVector2( 0.6f,-0.3f); break;
                case  3u: vPos = coreVector2( 0.3f,-0.3f); break;
                case  4u: vPos = coreVector2( 0.0f,-0.3f); break;
                case  5u: vPos = coreVector2( 0.0f,-0.6f); break;
                case  6u: vPos = coreVector2(-0.3f,-0.6f); break;
                case  7u: vPos = coreVector2(-0.3f,-0.3f); break;
                case  8u: vPos = coreVector2(-0.3f, 0.0f); break;
                case  9u: vPos = coreVector2(-0.6f, 0.0f); break;
                case 10u: vPos = coreVector2(-0.6f, 0.3f); break;
                case 11u: vPos = coreVector2(-0.3f, 0.3f); break;
                case 12u: vPos = coreVector2( 0.0f, 0.3f); break;
                case 13u: vPos = coreVector2( 0.3f, 0.3f); break;
                case 14u: vPos = coreVector2( 0.0f, 0.3f); break;
                case 15u: vPos = coreVector2( 0.0f, 0.0f); break;
                }

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else
            {
                const coreVector2 vPos = coreVector2::Direction((afPearlTime[i] + I_TO_F(i) * (0.2f*PI)) * ((i < 10u) ? -1.0f : 1.0f)) * (LERPS(1.4f, 0.5f, MIN(afPearlTime[i] * 0.5f, 1.0f)) + ((i < 10u) ? 0.2f : 0.0f));

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }

            afPearlTime[i] += 1.0f * TIME;
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
            {
                coreObject3D* pPearl = nGetPearl(j);
                if(!pPearl) continue;

                const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
                if(vDiff.LengthSq() < POW2(4.5f))
                {
                    this->DisablePearl(j, true);
                    g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_YELLOW);

                    if(m_iStageSub == 6u)
                    {
                        iPearlSpawn += 1u;
                        if(iPearlSpawn < 16u) this->EnablePearl(iPearlSpawn);
                    }
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            fEnemyMove = MIN(fEnemyMove + 0.6f * TIME, 1.0f);

            if(m_iStageSub == 1u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                coreVector2 vPos, vDir;

                if(m_iStageSub <= 5u)
                {
                    vPos = LERPS(coreVector2(0.0f,0.9f), coreVector2(-0.9f,0.9f), fEnemyMove);
                    vDir = coreVector2::Direction(LERPS(-1.0f*PI, 1.25f*PI, fEnemyMove));
                }
                else if(m_iStageSub == 6u)
                {
                    vPos = LERPS(coreVector2(-0.9f,0.9f), coreVector2(0.9f,0.9f), fEnemyMove);
                    vDir = coreVector2::Direction(LERPS(1.25f*PI, -3.25f*PI, fEnemyMove));
                }
                else
                {
                    vPos = LERPS(coreVector2(0.9f,0.9f), coreVector2(0.0f,0.9f), fEnemyMove);
                    vDir = coreVector2::Direction(LERPS(-3.25f*PI, -1.0f*PI, fEnemyMove));
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }

            if(fEnemyMove >= 1.0f)
            {
                if(m_iStageSub == 1u)
                {
                }
                else if(m_iStageSub <= 5u)
                {
                    if(STAGE_TICK_LIFETIME(0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(1u).Angle();

                        for(coreUintW j = 5u; j--; )
                        {
                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }
                    }
                }
                else if(m_iStageSub == 6u)
                {
                    if(STAGE_TICK_LIFETIME(0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(0u).Angle();

                        for(coreUintW j = 20u; j--; )
                        {
                            if((s_iTick & 0x01u) == ((j / 2u) & 0x01u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 9.5f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }
                    }
                }
                else
                {
                    if(STAGE_TICK_LIFETIME(16.0f, 0.0f) && ((s_iTick % 16u) < 8u))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    }
                }
            }
        });

        STAGE_WAVE("NEUNUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all pearls
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            this->DisablePearl(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // enemies move and shoot with player
    // schießen auf cooldown der waffe
    // gegner müssen immer gegen schussrichtung schießen um leichte kills zu vermeiden (speziell wenn ihre richtung fix ist, oder eine ihrer pos-achsen)
    // gegner in 3, wenn sie von ner schräge kommen sieht man alle 4 selbst wenn man am rand oder der ecke fliegt, müssen aber weit genug vom rand spawnen um kollisionen zu vermeiden wenn spieler in ecke ist
    // enemies not visible are not allowed to shoot (especially in sub 3)
    // in coop: wenn spieler stirbt soll ziel-spieler gewechselt werden, aber nicht bei wiederbelebung
    // TODO 1: flying directly between two enemies can cause infinity push between two enemies (especially in sub-phase 4)
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.2f,1.2f,1.2f));
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(4u)
            STAGE_GET_VEC2 (vBaseRefPos)
            STAGE_GET_UINT (iPlayerTarget, iPlayerTarget = 1u)
            STAGE_GET_FLOAT(vPlayerLerp)
        STAGE_GET_END

        const cPlayer* pPlayer1 = g_pGame->GetPlayer(0u);
        const cPlayer* pPlayer2 = g_pGame->GetPlayer(1u);
        STATIC_ASSERT(GAME_PLAYERS == 2u)

        const coreBool bCleared = STAGE_CLEARED;

             if(pPlayer1->HasStatus(PLAYER_STATUS_DEAD)) iPlayerTarget = 1u;
        else if(pPlayer2->HasStatus(PLAYER_STATUS_DEAD)) iPlayerTarget = 0u;
        else if(bCleared)                                iPlayerTarget = 1u - iPlayerTarget;

             if(bCleared)      vPlayerLerp = I_TO_F(iPlayerTarget);
        else if(iPlayerTarget) vPlayerLerp = MIN(vPlayerLerp + 1.0f*TIME, 1.0f);
        else                   vPlayerLerp = MAX(vPlayerLerp - 1.0f*TIME, 0.0f);

        const coreVector2 vBasePos   = LERPS(pPlayer1->GetPosition ().xy(), pPlayer2->GetPosition ().xy(), vPlayerLerp) / FOREGROUND_AREA;
        const coreVector2 vBaseDir   = LERPS(pPlayer1->GetDirection().xy(), pPlayer2->GetDirection().xy(), vPlayerLerp).Normalized();
        const coreBool    bBaseShoot = pPlayer1->GetWeapon(0u)->GetCooldown() || pPlayer2->GetWeapon(0u)->GetCooldown();

        if(bCleared)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 19u)

            const coreVector2 vPos = vBasePos;
            const coreVector2 vDir = -vPos.Normalized();

            vBaseRefPos = (vPos + vDir * g_pForeground->RayIntersection(vPos, vDir, 1.4f)) / FOREGROUND_AREA;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vNewPos    = vBasePos;
            coreVector2 vNewRefPos = vBaseRefPos;

            switch(i)
            {
            default: ASSERT(false)

            case  0u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); break;
            case  1u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); break;
            case  2u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); break;
            case  3u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); break;

            case  4u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  5u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  6u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  7u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  8u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case  9u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case 10u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case 11u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;

            case 12u: vNewPos = vNewPos + coreVector2( 0.6f,  0.0f);  vNewRefPos = coreVector2( 0.0f, 1.4f); break;
            case 13u: vNewPos = vNewPos + coreVector2(-0.6f,  0.0f);  vNewRefPos = coreVector2( 0.0f,-1.4f); break;
            case 14u: vNewPos = vNewPos + coreVector2( 0.0f,  0.6f);  vNewRefPos = coreVector2(-1.4f, 0.0f); break;
            case 15u: vNewPos = vNewPos + coreVector2( 0.0f, -0.6f);  vNewRefPos = coreVector2( 1.4f, 0.0f); break;
            case 16u: vNewPos = vNewPos + coreVector2( 0.84f, 0.0f);  vNewRefPos = coreVector2( 0.0f, 1.4f); break;
            case 17u: vNewPos = vNewPos + coreVector2(-0.84f, 0.0f);  vNewRefPos = coreVector2( 0.0f,-1.4f); break;
            case 18u: vNewPos = vNewPos + coreVector2( 0.0f,  0.84f); vNewRefPos = coreVector2(-1.4f, 0.0f); break;
            case 19u: vNewPos = vNewPos + coreVector2( 0.0f, -0.84f); vNewRefPos = coreVector2( 1.4f, 0.0f); break;
            }

            vNewPos = LERPB(vNewRefPos, vNewPos, MIN(0.7f * fLifeTime, 1.0f));

            pEnemy->SetPosition (coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));
            pEnemy->SetDirection(coreVector3(-vBaseDir,                 0.0f));

            if(bBaseShoot && STAGE_LIFETIME_AFTER(0.5f) && STAGE_TICK_LIFETIME(8.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                if(g_pForeground->IsVisiblePoint(vPos, 1.15f)) g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.15f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE("VIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 4u, 5u, 10u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // trade places with others
    // disable bullets of attacking enemies when jumping into them, otherwise unavoidable
    // man kann die position des spielers kontrollieren
    // first enemies should be far enough away to introduce teleport
    // enemies can be in a line to show multi-jump
    // high speed killing with correct rotation -> COOP (double-ended left right, you can start from each side)
    // zu komplexe highspeed-gruppen hatten schlechtes feeling, max zwei rotations-richtungen
    // in highspeed-gruppe gegner müssen weit genug auseinander sein (zur seite), um nicht unabsichtlich mehrere hintereinander anzugreifen
    // vor zweiter highspeed-gruppe, gegner müssen so angeordnet sein, dass man in einer guten ecke startet
    // last two shooting enemies are near each other, to make first teleport risky
    // TODO 1: interface icons ersetzen sich auch (+ continue) -> general icon implementation
    // TODO 1: different teleport-colors for each player ? (yellow + orange ? or ship colors, or weapon colors)
    // TODO 1: badge: follow all paths correctly -> auf COOP achten
    // TODO 1: transformed player should not kill other (transforming) enemy for coop -> IMMORTAL and do custom handling
    // TODO 1: in the bullet-wave phase, maybe move enemies more far away, to force player rotation (same distance like first groups ?)
    // TODO 1: gegner in schussrichtung drehen
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 96u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cWarriorEnemy, 7u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(28u)
            STAGE_GET_VEC2_ARRAY(avPosStorage, 14u)
        STAGE_GET_END

        const auto nTeleportFunc = [](const cEnemy* pEnemy, const coreBool bTransform)
        {
            if(pEnemy->ReachedDeath())
            {
                cPlayer* pPlayer = pEnemy->LastAttacker();
                if(pPlayer->IsDarkShading())
                {
                    const coreVector3 vPos  = pPlayer->GetPosition();
                    const coreVector3 vDiff = pEnemy ->GetPosition() - vPos;
                    const coreUintW   iNum  = F_TO_UI(vDiff.Length() / 1.9f);

                    for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum))), 10.0f, 1u, COLOR_ENERGY_YELLOW);

                    pPlayer->SetPosition(pEnemy->GetPosition());

                    if(bTransform)
                    {
                        pPlayer->TurnIntoEnemy();
                        g_pSpecialEffects->MacroExplosionColorBig(pPlayer->GetPosition(), COLOR_ENERGY_MAGENTA);
                    }
                }
            }
        };

        const auto nReturnFunc = []()
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->IsEnemyLook())
                {
                    pPlayer->TurnIntoPlayer();
                    g_pSpecialEffects->MacroExplosionDarkBig(pPlayer->GetPosition());
                }
            });
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  2u,  9u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 14u, 21u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 10u, 13u)   // # easier in code
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad2,  0u,  0u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad2,  1u,  3u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 22u, 35u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad2,  4u,  4u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 36u, 39u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 40u, 41u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 42u, 45u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 46u, 47u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad2,  5u,  6u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 48u, 95u)

            if(m_iStageSub == 3u)
            {
                pSquad1->GetEnemy(14u)->SetPosition(coreVector3(-0.7f,-0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(15u)->SetPosition(coreVector3(-0.7f, 0.3f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(16u)->SetPosition(coreVector3( 0.7f, 0.3f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(17u)->SetPosition(coreVector3( 0.7f,-0.3f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(18u)->SetPosition(coreVector3(-0.3f,-0.3f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(19u)->SetPosition(coreVector3(-0.3f, 0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(20u)->SetPosition(coreVector3( 0.3f, 0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(21u)->SetPosition(coreVector3( 0.3f,-0.7f,0.0f) * FOREGROUND_AREA3);
            }
            else if(m_iStageSub == 7u)
            {
                pSquad1->GetEnemy(22u)->SetPosition(coreVector3( 0.7f,-0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(23u)->SetPosition(coreVector3( 0.7f, 0.0f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(24u)->SetPosition(coreVector3(-0.7f, 0.0f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(25u)->SetPosition(coreVector3(-0.7f, 0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(26u)->SetPosition(coreVector3( 0.5f, 0.7f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(27u)->SetPosition(coreVector3( 0.5f, 0.2f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(28u)->SetPosition(coreVector3(-0.3f, 0.2f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(29u)->SetPosition(coreVector3(-0.3f, 0.5f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(30u)->SetPosition(coreVector3(-0.0f, 0.5f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(31u)->SetPosition(coreVector3(-0.0f,-0.5f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(32u)->SetPosition(coreVector3( 0.3f,-0.5f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(33u)->SetPosition(coreVector3( 0.3f,-0.2f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(34u)->SetPosition(coreVector3(-0.5f,-0.2f,0.0f) * FOREGROUND_AREA3);
                pSquad1->GetEnemy(35u)->SetPosition(coreVector3(-0.5f,-0.7f,0.0f) * FOREGROUND_AREA3);
            }
            else if((m_iStageSub >= 9u) && (m_iStageSub <= 13u))
            {
                cEnemy* pDummy = pSquad1->GetEnemy(0u);

                coreVector2 vDir;
                switch(m_iStageSub)
                {
                default: ASSERT(false)
                case  9u: vDir = coreVector2( 0.0f, 1.0f); break;
                case 10u: vDir = coreVector2( 0.0f,-1.0f); break;
                case 11u: vDir = coreVector2(-1.0f, 0.0f); break;
                case 12u: vDir = coreVector2( 1.0f, 0.0f); break;
                case 13u: vDir = coreVector2( 0.0f,-1.0f); break;
                }

                for(coreUintW i = 0u; i < 40u; ++i)
                {
                    const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * ((I_TO_F(i) - 19.5f) * 0.055f)) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pDummy, vPos, vDir)->ChangeSize(1.5f);
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 48u) ? 0.8f : 1.0f, (i < 48u) ? 0.0f : (1.0f + 0.1f * I_TO_F(i - 48u)))

            if(i < 14u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 2u) ? ((i % 2u) ? 0.45f : -0.45f) : ((I_TO_F((i - 2u) % 4u) - 1.5f) * 0.45f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  2u) {}
                else if(i <  6u) pEnemy->Rotate90 ();
                else if(i < 10u) pEnemy->Rotate270();
                else if(i < 14u) pEnemy->Rotate180();
            }
            else if(i < 36u)
            {
                coreVector2& vPosStorage = avPosStorage[i % 14u];
                if(STAGE_TAKEOFF) vPosStorage = pEnemy->GetPosition().xy();

                const coreFloat fTime = LERPB(0.0f, 1.0f, MIN(fLifeTime * 2.0f, 1.0f));

                pEnemy->SetPosition      (coreVector3(vPosStorage * fTime, 0.0f));
                pEnemy->DefaultRotateLerp(0.0f*PI, 2.0f*PI, fTime);

                if(STAGE_LIFETIME_AFTER(0.4f))
                {
                    if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                    {
                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                    }
                }
                else
                {
                    if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                    }
                }
            }
            else if(i < 48u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 40u || (i >= 42u && i < 46u)) ? ((I_TO_F(i % 4u) - 1.5f) * 0.3f) : ((i % 2u) ? -0.8f : -0.5f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 40u) pEnemy->Rotate180();
                else if(i < 42u) {}
                else if(i < 46u) pEnemy->Rotate90 ();
                else if(i < 48u) pEnemy->Rotate270();
            }
            else
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,-1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 48u) * 8u) % 11u) - 5.0f) * 0.2f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            nTeleportFunc(pEnemy, false);
        });

        const coreBool bTransform = (m_iStageSub >= 13u) && (pSquad2->GetNumEnemiesAlive() < g_pGame->GetPlayers());

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i < 4u) ? -0.7f : ((i < 5u) ? 0.9f : ((i % 2u) ? -0.3f : 0.3f)), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i < 1u) {}
            else if(i < 2u) pEnemy->Rotate90 ();
            else if(i < 3u) pEnemy->Rotate180();
            else if(i < 4u) pEnemy->Rotate270();
            else if(i < 7u) {}

            nTeleportFunc(pEnemy, bTransform);

            if(STAGE_LIFETIME_AFTER(0.3f) && STAGE_TICK_LIFETIME(5.0f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = pEnemy->AimAtPlayerSideRev().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.5f);
                }
            }

            if(pEnemy->ReachedDeath())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cViewBullet>([&](cViewBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
                });
            }
        });

        if((m_iStageSub < 5u) && STAGE_TICK_FREE(6.0f, 0.0f))
        {
            cEnemy* pDummy = pSquad1->GetEnemy(0u);

            const coreVector2 vPos = coreVector2(1.0f,0.0f) * FOREGROUND_AREA * -1.2f;
            const coreVector2 vDir = coreVector2(1.0f,0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pDummy, vPos.Rotated90(), vDir.Rotated90())->ChangeSize(1.5f);
        }

        if(STAGE_CLEARED) nReturnFunc();

        STAGE_WAVE("EINUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // corpses remain on the field
    // enemies should not get attached to the side they are spawning from to reduce issues (steep angles or out-of-view kills) (but why are last two groups different ? though it looks better)
    // first bullet wave needs an opening at the bottom, as player will most likely start there
    // moving enemy waves need to have good flow when killing one after another, and distinct paths (when all waves would spawn together)
    // corners should not be safe-spots
    // every bullet in the corpse-phase should be removed in the middle (though inverting it or twisting it would be allowed)
    // TODO 1: for coop create two safe-spots
    // TODO 1: enemies moving outside creating badges
    // TODO 1: effekte beim einschlagen in die wand, und beim einschlagen in legion, und beim wiederbelebt werden
    // TODO 1: wave where middle bullets faster than outer bullets, or left-right faster than up-down, then invert
    // TODO 1: helfer is in mitte ?
    // TODO 1: similar to zelda water boss https://youtu.be/HRQXOCU8OWA?t=1005
    // TODO 1: tunnel to follow
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 1.2f,-0.8f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2( 1.2f, 0.8f), coreVector2(-1.0f,0.0f));
            pPath2->AddNode(coreVector2(-1.2f,-0.8f), coreVector2(-1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(-0.8f,-1.2f), coreVector2(0.0f, 1.0f));
            pPath3->AddNode(coreVector2( 0.0f, 0.8f), coreVector2(1.0f, 0.0f));
            pPath3->AddNode(coreVector2( 0.8f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));   // increase distance between first and last enemy
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(120u + 8u)
            STAGE_GET_VEC2_ARRAY (avChildMove,  40u)
            STAGE_GET_FLOAT_ARRAY(afChildAngle, 40u)
            STAGE_GET_UINT       (iStageMini)
            STAGE_GET_FLOAT      (fStageMiniTime)
            STAGE_GET_VEC4       (vLegionPos)
            STAGE_GET_FLOAT      (fLegionTime)
            STAGE_GET_UINT       (iLegionCount)
        STAGE_GET_END

        const coreFloat fStageMiniTimeBefore = fStageMiniTime;

        const auto nStageMiniTickFunc = [&](const coreFloat fSpeed, const coreFloat fOffset)
        {
            return ((s_iTick = F_TO_UI(fStageMiniTime * fSpeed - fOffset) - 1u) != coreUint16(F_TO_UI(fStageMiniTimeBefore * fSpeed - fOffset) - 1u));
        };

        const auto nStageMiniChangeFunc = [&](const coreUint32 iStage)
        {
            iStageMini     = iStage;
            fStageMiniTime = 0.0f;
        };

        if(m_iStageSub < 5u)
        {
            coreUint8 iNumAlive = 0u;
            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(pEnemy->GetCurHealth()) iNumAlive += 1u;
            });

            if(!iNumAlive)
            {
                     if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
                else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
                else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)
                else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
                else if(STAGE_SUB(5u)) {}
            }
        }
        else
        {
            fStageMiniTime += 1.0f * TIME;

            if((iStageMini == 0u) && (fStageMiniTime >= 1.0f))
            {
                nStageMiniChangeFunc(1u);
            }
            else if((iStageMini == 10u) && (fStageMiniTime >= 1.0f))
            {
                nStageMiniChangeFunc(11u);

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    avChildMove [i] = pEnemy->GetPosition ().xy();
                    afChildAngle[i] = pEnemy->GetDirection().xy().Angle();
                });
            }
            else if((iStageMini == 11u) && (fStageMiniTime >= 3.0f))
            {
                nStageMiniChangeFunc(12u);

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST);
                    pEnemy->SetMaxHealth(10);
                    pEnemy->SetCurHealth(10);
                    pEnemy->RefreshColor(1.0f);
                });
            }
        }

        coreVector2 vLegionTarget = coreVector2(0.0f,0.0f);
        if(iStageMini == 12u)
        {
            fLegionTime -= 1.0f * TIME;

            if(fLegionTime <= 0.0f)
            {
                const coreVector2 vTarget = iLegionCount ? g_pGame->FindPlayerDual(((iLegionCount - 1u) / 2u) % 2u)->GetPosition().xy() : coreVector2(0.0f, 0.2f * FOREGROUND_AREA.y);
                const coreVector2 vDiff   = vTarget - vLegionPos.zw();

                vLegionPos.xy(vLegionPos.zw());
                vLegionPos.zw(vLegionPos.zw() + vDiff.Normalized() * MIN(vDiff.Length(), 20.0f));

                fLegionTime  += 2.0f;
                iLegionCount += 1u;
            }

            vLegionTarget = LERPH3(vLegionPos.xy(), vLegionPos.zw(), CLAMP(2.0f - fLegionTime, 0.0f, 1.0f));
        }

        const coreMatrix3 mRota = coreMatrix3::Rotation(1.0f * TIME);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.25f * I_TO_F(i % 10u))

            if(iStageMini == 0u)
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                    g_pSpecialEffects->MacroDestructionDark(pEnemy);

                    //   3
                    // 1   0
                    //   2
                    const coreVector2 vSide   = (i < 20u) ? coreVector2((i < 10u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 30u) ? -1.0f : 1.0f);
                    const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F(i % 10u) - 4.5f) * 0.2f) + vSide;

                    avChildMove[i] = (vTarget * FOREGROUND_AREA * 1.05f - pEnemy->GetPosition().xy()).Normalized();
                }

                if(pEnemy->GetCurHealth())
                {
                    const coreSpline2* pPath = (i < 10u) ? pPath1 : ((i < 20u) ? pPath2 : ((i < 30u) ? pPath3 : pPath4));

                    STAGE_REPEAT(pPath->GetTotalDistance())

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                    pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    coreVector2& vMove = avChildMove[i];
                    if(!vMove.IsNull())
                    {
                        coreVector2       vNewPos = pEnemy->GetPosition().xy() + vMove * (100.0f * TIME);
                        const coreVector2 vNewDir = coreVector2::Direction(pEnemy->GetDirection().xy().Angle() + (10.0f * TIME));

                             if((vNewPos.x < -FOREGROUND_AREA.x * 1.05f) && (vMove.x < 0.0f)) {vNewPos *= -FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x); vMove = coreVector2(0.0f,0.0f);}
                        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.05f) && (vMove.x > 0.0f)) {vNewPos *=  FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x); vMove = coreVector2(0.0f,0.0f);}
                             if((vNewPos.y < -FOREGROUND_AREA.y * 1.05f) && (vMove.y < 0.0f)) {vNewPos *= -FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y); vMove = coreVector2(0.0f,0.0f);}
                        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.05f) && (vMove.y > 0.0f)) {vNewPos *=  FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y); vMove = coreVector2(0.0f,0.0f);}

                        pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                        pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
                    }
                }
            }
            else if(iStageMini >= 11u)
            {
                const coreFloat   fDelay  = I_TO_F((i * 7u) % 40u);
                const coreVector2 vTarget = vLegionTarget + coreVector2::Direction(GA * fDelay) * LERPB(0.0f, 1.0f, (fDelay + 1.0f) / 40.0f) * 15.0f;

                if(iStageMini == 11u)
                {
                    const coreFloat   fTime   = CLAMP(1.0f - 1.0f * (fStageMiniTime - 0.05f * fDelay), 0.0f, 1.0f);
                    const coreVector2 vNewPos = LERPB(vTarget, avChildMove[i], fTime);
                    const coreVector2 vNewDir = coreVector2::Direction(afChildAngle[i] + LERPB(1.0f*PI, 0.0f*PI, fTime));

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
                }
                else
                {
                    const coreVector2 vNewPos = vTarget;
                    const coreVector2 vNewDir = pEnemy->GetDirection().xy() * mRota;

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

                    if(nStageMiniTickFunc(1.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = (s_iTick % 2u) ? (vPos - vLegionTarget).Normalized() : pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            }
        });

        cEnemy* pDummy = pSquad1->GetEnemy(0u);

        if(iStageMini == 1u)
        {
            if(nStageMiniTickFunc(1.0f, 0.0f))
            {
                if(s_iTick < 6u)
                {
                    for(coreUintW i = 0u; i < 80u; ++i)
                    {
                        if((s_iTick < 4u) ? ((i >= ((s_iTick + 2u) % 4u) * 20u) && (i < (((s_iTick + 2u) % 4u) + 1u) * 20u)) : ((s_iTick % 2u) == ((i / 5u) % 2u))) continue;

                        const coreVector2 vSide   = (i < 40u) ? coreVector2((i < 20u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 60u) ? -1.0f : 1.0f);
                        const coreVector2 vTarget = (vSide.Rotated90() * ((I_TO_F(i % 20u) - 9.5f) * 0.1f) + vSide) * 1.1f;
                        const coreFloat   fSpeed  = 0.6f * vTarget.Length();

                        const coreVector2 vPos = vTarget * FOREGROUND_AREA;
                        const coreVector2 vDir = -vPos.Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
                else nStageMiniChangeFunc(2u);
            }
        }
        else if(iStageMini == 2u)
        {
            if(nStageMiniTickFunc(30.0f, 0.0f))
            {
                if(s_iTick < 160u)
                {
                    const coreVector2 vSide   = StepRotated90(((s_iTick / 20u) + 1u) % 4u);
                    const coreVector2 vTarget = (vSide.Rotated90() * ((I_TO_F(s_iTick % 20u) - 9.5f) * ((s_iTick < 80u) ? -0.1f : 0.1f)) + vSide) * 1.1f;
                    const coreFloat   fSpeed  = 1.2f * vTarget.Length();

                    const coreVector2 vPos = vTarget * FOREGROUND_AREA;
                    const coreVector2 vDir = -vPos.Normalized();

                                      g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos,             vDir)            ->ChangeSize(1.3f);
                    if(s_iTick < 80u) g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos * -1.0f,     vDir * -1.0f)    ->ChangeSize(1.3f);
                                 else g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos.Rotated90(), vDir.Rotated90())->ChangeSize(1.3f);
                }
                else nStageMiniChangeFunc(10u);
            }
        }

        if(iStageMini < 12u)
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([](cQuadBullet* OUTPUT pBullet)
            {
                if(pBullet->GetPosition().LengthSq() < POW2(2.0f))
                    pBullet->Deactivate(true);
            });
        }

        STAGE_WAVE("ZWEIUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });
STAGE_START_HERE
    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 10u})
    {
        STAGE_BOSS(m_Geminga, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // ################################################################
}