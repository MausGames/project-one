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
// setup the Rutilus mission
void cRutilusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // teleport everything
    // eckige portale mit richtung die sich auch drehen können (90grad)
    // normal, bewegung, drehung
    // gegner in der mitte, portal oben und unten
    // portale nebeneinander, gegner fliegen in kreis durch portale
    // geschoss-welle ausweichen mit portalen
    // gegner fliegen zwischen portalen in infinity []***[], dann invertiert |**[]  []**|, dann rotiert |*>[]*<*[]*>|
    // vergrößerung durch portale
    // portale werden am ende zu gegnern
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 28u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_ORANGE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1, 10u, 11u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1, 14u, 21u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 26u, 26u)
            else if(STAGE_SUB(12u)) STAGE_RESSURECT(pSquad1, 27u, 27u)
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                this->EnableTeleporter(i, coreVector2(i ? 0.5f : -0.5f, 0.0f) * FOREGROUND_AREA);
        }

        if(STAGE_TIME_BEFORE(1.2f))
        {
            const coreFloat x = (SIN(m_fStageTime / 1.2f * PI*0.5f) * 2.0f - 1.0f) * MAX(1.2f - m_fStageTime, 0.0f);   
            const coreFloat y = SIN(m_fStageTime / 1.2f * PI) * 0.4f;

            m_aTeleporter[0].SetPosition(coreVector3(x, y, 0.0f) * FOREGROUND_AREA3);
            m_aTeleporter[0].SetSize(coreVector3(1.0f,1.0f,1.0f));

            m_aTeleporter[1].SetPosition(-m_aTeleporter[0].GetPosition());
            m_aTeleporter[1].SetSize(coreVector3(1.0f,1.0f,1.0f));

        }
        else if(STAGE_TIME_BEFORE(2.2f))
        {
            const coreFloat fSize = LERPS(0.0f, 1.0f, m_fStageTime - 1.2f) * 1.05f * FOREGROUND_AREA.x;

            m_aTeleporter[0].SetPosition(coreVector3(0.0f,0.0f,0.0f));
            m_aTeleporter[0].SetSize(coreVector3(fSize, 1.0f, 1.0f));

            m_aTeleporter[1].SetPosition(coreVector3(0.0f,0.0f,0.0f));
            m_aTeleporter[1].SetSize(coreVector3(fSize, 1.0f, 1.0f));
        }
        else if(STAGE_TIME_BEFORE(3.2f))
        {
            for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            {
                const coreFloat fHeight = LERPS(0.0f, 1.0f, m_fStageTime - 2.2f) * (i ? 0.3f : -0.3f);
                //const coreFloat fHeight = LERPS(0.0f, 1.0f, m_fStageTime - 2.2f) * (i ? 0.7f : -0.7f);
                m_aTeleporter[i].SetPosition(coreVector3(0.0f, fHeight, 0.0f) * FOREGROUND_AREA3);
            }
            m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
            m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        }
        else
        {
            //for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            //{
            //    const coreFloat fHeight = SIN((m_fStageTime - 3.2f)*0.1f * 2.0f*PI) * 0.5f * LERPS(0.0f, 1.0f, MIN((m_fStageTime - 3.2f)*0.1f, 1.0f)) + (i ? 0.3f : -0.3f);
            //    m_aTeleporter[i].SetPosition(coreVector3(0.0f, fHeight, 0.0f) * FOREGROUND_AREA3);
            //}
            m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
            m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        }
#if 0

        // EIGHT
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            //const coreFloat fHeight = SIN(m_fStageTime*0.15f * 2.0f*PI + I_TO_F(i)*0.5f*PI) * 0.6f;
            m_aTeleporter[i].SetPosition(coreVector3(0.4f, 0.0f, 0.0f) * FOREGROUND_AREA3);
            m_aTeleporter[i].SetPosition(coreVector3(0.7f, 0.0f, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3( 1.0f,0.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3(-1.0f,0.0f,0.0f).Normalized());
        m_aTeleporter[0].SetPosition(-m_aTeleporter[0].GetPosition());
        // EIGHT


        // SIXT
            const coreVector2 vDir = coreVector2::Direction(m_fStageTime*0.09f * 2.0f*PI);
            m_aTeleporter[0].SetDirection(coreVector3(vDir, 0.0f));
            m_aTeleporter[0].SetPosition(coreVector3(vDir.Rotated90() * 0.53f,0.0f) * FOREGROUND_AREA3);
            m_aTeleporter[1].SetDirection(coreVector3(-vDir, 0.0f));
            m_aTeleporter[1].SetPosition(coreVector3(-vDir.Rotated90() * 0.53f,0.0f) * FOREGROUND_AREA3);
        m_aTeleporter[0].SetSize     (coreVector3(0.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(0.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));


            m_aTeleporter[0].SetPosition(coreVector3(vDir.Rotated90() * 0.73f,0.0f) * FOREGROUND_AREA3);
            m_aTeleporter[1].SetPosition(coreVector3(-vDir.Rotated90() * 0.73f,0.0f) * FOREGROUND_AREA3);
        m_aTeleporter[0].SetSize     (coreVector3(0.7f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(0.7f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        // SIXT

        // FIRST
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreFloat fHeight = SIN(m_fStageTime*0.1f * 2.0f*PI + I_TO_F(i)*0.5f*PI*0.0f) * 0.5f + (i ? 0.3f : -0.3f);
            m_aTeleporter[i].SetPosition(coreVector3(0.0f, fHeight, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3(0.0f,1.0f,0.0f));
        m_aTeleporter[1].SetDirection(coreVector3(0.0f,1.0f,0.0f));
        // FIRST
        
        // SECOND
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreFloat fHeight = SIN(m_fStageTime*0.1f * 2.0f*PI + I_TO_F(i)*1.0f*PI) * 0.8f;
            m_aTeleporter[i].SetPosition(coreVector3(fHeight, i ? 0.5f : -0.5f, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(0.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(0.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetSize     (coreVector3(1.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.5f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
        m_aTeleporter[0].SetDirection(coreVector3(-1.0f, 0.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 1.0f, 0.0f,0.0f).Normalized());
        // SECOND
        
        // THIRD
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreFloat fHeight = SIN(m_fStageTime*0.15f * 2.0f*PI + I_TO_F(i)*0.5f*PI) * 0.6f;
            m_aTeleporter[i].SetPosition(coreVector3(fHeight, 0.0f, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(0.4f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(0.4f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3( 1.0f, 0.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 0.0f, 1.0f,0.0f).Normalized());
        m_aTeleporter[0].SetPosition(m_aTeleporter[0].GetPosition().RotatedZ90());
        // THIRD


        // FIFTH
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreVector2 vDir = coreVector2::Direction(m_fStageTime*0.05f * 2.0f*PI + I_TO_F(i)*0.5f*PI);
            m_aTeleporter[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aTeleporter[i].SetPosition(coreVector3(0.0f,0.0f,0.0f));
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        // FIFTH

        // SEVENTH
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            //const coreFloat fHeight = SIN(m_fStageTime*0.15f * 2.0f*PI + I_TO_F(i)*0.5f*PI) * 0.6f;
            m_aTeleporter[i].SetPosition(coreVector3(1.0f, 0.0f, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3( 0.0f,-1.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 1.0f, 0.0f,0.0f).Normalized());
        m_aTeleporter[0].SetPosition(m_aTeleporter[0].GetPosition().RotatedZ90());
        // SEVENTH

        // FOURTH
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreFloat fHeight = SIN(m_fStageTime*0.15f * 2.0f*PI + I_TO_F(i)*0.5f*PI) * 0.6f;
            m_aTeleporter[i].SetPosition(coreVector3(0.0f, fHeight, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3( 1.0f, 0.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 0.0f, 1.0f,0.0f).Normalized());
        m_aTeleporter[0].SetPosition(m_aTeleporter[0].GetPosition().RotatedZ90());
        // FOURTH
        

        // NINTH
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            const coreFloat fHeight = SIN(m_fStageTime*0.05f * 2.0f*PI) * 1.0f;
            m_aTeleporter[i].SetPosition(coreVector3(0.0f, fHeight, 0.0f) * FOREGROUND_AREA3);
        }
        m_aTeleporter[0].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[1].SetSize     (coreVector3(1.05f * FOREGROUND_AREA.x, 1.0f, 1.0f));
        m_aTeleporter[0].SetDirection(coreVector3( 1.0f, 0.0f,0.0f).Normalized());
        m_aTeleporter[1].SetDirection(coreVector3( 0.0f,-1.0f,0.0f).Normalized());
        m_aTeleporter[0].SetPosition(m_aTeleporter[0].GetPosition().RotatedZ90());
        // NINTH
        
#endif


        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i < 6u) ? ((I_TO_F(i) - 2.5f) * 0.2f) : (I_TO_F(i % 2u) * 0.2f - 0.85f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i >= 6u)
            {
                     if(i <  8u) pEnemy->Rotate270();
                else if(i < 10u) pEnemy->Rotate270()->InvertY();
                else if(i < 12u) pEnemy->Rotate90 ()->InvertY();
                else if(i < 14u) pEnemy->Rotate90 ();
            }

            if(STAGE_TAKEOFF)
            {
                /*
                pEnemy->SetPosition (coreVector3(((I_TO_F(i % 8u) / 7.0f) - 0.5f) * 1.4f, 0.9f,0.0f) * FOREGROUND_AREA3);
               // pEnemy->SetPosition (coreVector3(((I_TO_F(i) / 7.0f) - 0.5f) * 0.5f, 0.9f,0.0f) * FOREGROUND_AREA3);
                    if(i < 8u) pEnemy->Rotate90();
                    else pEnemy->Rotate270();
                    
                    const coreVector2 vDir = coreVector2::Direction((I_TO_F(i)/8.0f) * 2.0f*PI);
                    pEnemy->SetPosition (coreVector3(vDir * 0.5f, 0.0f) * FOREGROUND_AREA3);
                    */
            }
            /*
                const coreFloat fCurAngle = pEnemy->GetPosition().xy().Angle();
                const coreVector2 vDir = coreVector2::Direction(fCurAngle - Core::System->GetTime() * 0.3f);
                pEnemy->SetPosition (coreVector3(vDir * 0.5f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->SetDirection(coreVector3(vDir.Rotated90(), 0.0f));
            
            //pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 20.0f);
             */


            if(STAGE_TICK_LIFETIME(1.4f, 0.0f))
            {
                if(i < 6u)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
                else
                {
                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    const coreFloat   fBase = pEnemy->AimAtPlayerSide().Angle();

                    for(coreUintW j = 5u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 6.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            }
        });

        STAGE_WAVE("SECHSUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all teleporters
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            this->DisableTeleporter(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Quaternion, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Saros, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Messier, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}