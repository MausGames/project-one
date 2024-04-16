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
    // damaging fields hide themselves (d/generation)
    // idea: field chases player and gets visible regularly
    // idea: schräge bewegung von schrägen blöcken
    // idea: snake
    // idea: linien auf allen 4 seiten die sich unsichtbar richtung mitte bewegen (außen-blöcke sichtbar)
    // idea: block in enemy
    // enemies should be aligned to same boundary as blocks
    // grow kann ich nicht machen, der spieler muss die größe einschätzen können, somit muss sowohl größe als auch geschwindigkeit konstant bleiben
    // spieler muss nicht sehen wie jeder block aus bild geht, lenkt nur ab, und ist eh klar wenn intro-block sichtbar bild verlässt
    // TODO: badge
    // TODO: alles nur von oben kommen lassen, mit seiten gegnern
    // TODO: spawn of rotation-blocks too dangerous, can easily hit player
    // TODO: high tide in nCreateGenerate
    STAGE_MAIN({TAKE_ALWAYS, 0u})
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

        const auto nMoveGenerate = [](const coreSpline2* pRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat fTime)
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
                cPlayer::TestCollision(PLAYER_TEST_NORMAL, pGenerate, [&](cPlayer* OUTPUT pPlayer, const coreObject3D* pGenerate, const coreVector3& vIntersection, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;

                    pPlayer->TakeDamage(5, ELEMENT_GREEN, vIntersection.xy());

                    this->BangGenerate(i);

                    g_pSpecialEffects->CreateSplashColor(pGenerate->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                });
            }
        }

        STAGE_WAVE("EINUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all generates
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            this->DisableGenerate(i, false);

        g_pGame->GetHelper(ELEMENT_GREEN)->Kill(false);

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
    STAGE_MAIN({TAKE_ALWAYS, 1u})
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
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetCollisionRadius());
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
                if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                }
            }
        });

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
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 16u, 17u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 18u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 22u, 23u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 24u, 25u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 26u, 27u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 28u, 29u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 30u, 31u)

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
    // collect lots of shiny pills (pacman, snake)
    // idea: enemy has orb shield or tail you need to collect
    // idea: throw lines of balls form side into field
    // idea: ball lines shot from left or right with gravity down and different x-force
    // in the chain-stage, balls need to be far enough apart to not accidentally spawn ball into player
    // enemy should die with the last ball, not require the player to attack, to remove confusion and delay
    // ball animations need to be in a sequential pattern
    // the combination of collecting stuff and shooting at stuff was a disaster (because of the requirement to collect everything to advance), it was possible to give collecting a meaning, by causing "changes" (e.g. enemy invincibility), but it felt very sluggish (A, B, A, B), also enemies dropping the balls cause players to move into enemies while killing (for speed) which felt forced
    // TODO: helper in one of the balls
    // TODO: badge, hidden balls 1-N, other color, with special effect and sound
    // TODO: pearls fly at enemy and damage him (visibly) (Ikaruga rockets)
    // TODO: enemy attacks with pearls at the end, while bullets fly at you (diagonally ?)
    // TODO: bouncing pearls (linear, not gravity)
    // TODO: big pearl cluster at the end
    // TODO: fix 7-pearl loops
    // TODO: if chroma-enemies come, spawn them in delay phase
    STAGE_MAIN({TAKE_ALWAYS, 8u})
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

        const auto nMovePearl = [](const coreSpline2* pRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat fTime)
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

        STAGE_WAVE("ACHTUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all pearls
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            this->DisablePearl(i, false);

        STAGE_FINISH_NOW
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