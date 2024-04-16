///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cGame::cGame(const sGameOptions oOptions, const coreInt32* piMissionList, const coreUintW iNumMissions)noexcept
: m_BulletManagerPlayer (TYPE_BULLET_PLAYER)
, m_BulletManagerEnemy  (TYPE_BULLET_ENEMY)

, m_BulletManagerPlayerTop (TYPE_BULLET_PLAYER)

, m_Interface           ((oOptions.iType != GAME_TYPE_SOLO) ? GAME_PLAYERS : 1u)
, m_pRepairEnemy        (NULL)
, m_piMissionList       (piMissionList)
, m_iNumMissions        (iNumMissions)
, m_pCurMission         (NULL)
, m_iCurMissionIndex    (coreUintW(-1))
, m_fTimeInOut          (0.0f)
, m_fMusicFade          (0.0f)
, m_fMusicSpeed         (0.0f)
, m_fMusicVolume        (1.0f)
, m_fHitDelay           (0.0f)
, m_fVanishDelay        (0.0f)
, m_bDefeatDelay        (false)
, m_iContinues          ((g_bDemoVersion || !g_pSave->GetHeader().oProgress.bFirstPlay) ? GAME_CONTINUES : 0u)
, m_iDepthLevel         (0u)
, m_iDepthDebug         (0u)
, m_iOutroType          (GAME_OUTRO_MISSION)
, m_iOutroSub           (0u)
, m_bVisibleCheck       (false)
, m_iRepairMove         (0xFFu)
, m_Options             (oOptions)
, m_iVersion            (0u)
, m_iStatus             (0u)
{
    ASSERT(m_piMissionList && (m_iNumMissions <= MISSIONS))

#if defined(_P1_DEBUG_RANDOM_)

    // 
    if(!this->IsMulti() && (CORE_RAND_RUNTIME & 0x01u))
        m_aPlayer[0].Configure(PLAYER_SHIP_DEF);
    else

#endif

    // configure first player
    m_aPlayer[0].Configure  (g_bCheatP1 ? PLAYER_SHIP_P1 : PLAYER_SHIP_ATK);
    m_aPlayer[0].EquipShield(oOptions.aiShield[0]);
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS;  ++i) m_aPlayer[0].EquipWeapon (i, oOptions.aaiWeapon [0][i]);
    for(coreUintW i = 0u; i < PLAYER_EQUIP_SUPPORTS; ++i) m_aPlayer[0].EquipSupport(i, oOptions.aaiSupport[0][i]);

    if(this->IsMulti())
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF);
        m_aPlayer[1].EquipShield(oOptions.aiShield[1]);
        for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS;  ++i) m_aPlayer[1].EquipWeapon (i, oOptions.aaiWeapon [1][i]);
        for(coreUintW i = 0u; i < PLAYER_EQUIP_SUPPORTS; ++i) m_aPlayer[1].EquipSupport(i, oOptions.aaiSupport[1][i]);

        // 
        m_aPlayer[0].SetInput(&g_aGameInput[0]);
        m_aPlayer[1].SetInput(&g_aGameInput[1]);

        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // 
    for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
        m_aHelper[i].Configure(ELEMENT_WHITE + i);

    // 
    m_Interface .UpdateLayout();
    m_Interface .UpdateEnabled();
    m_CombatText.UpdateLayout();

    // 
    m_pRepairEnemy = new cRepairEnemy();   // # because of enemy-registration

    // load first mission
    m_pCurMission = new cNoMission();

    // 
    cHelper::GlobalReset();

    // 
    g_pSave->SaveFile();

#if defined(_CORE_DEBUG_)

    // 
    for(coreUintW i = 0u; i < iNumMissions; ++i)
        for(coreUintW j = i+1u; j < iNumMissions; ++j)
            ASSERT((m_piMissionList[i] != m_piMissionList[j]) ||
                   (m_piMissionList[i] == cNoMission::ID))

#endif
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // 
    this->__ClearAll(false);

    // delete last mission
    SAFE_DELETE(m_pCurMission)

    // 
    SAFE_DELETE(m_pRepairEnemy)

    // 
    g_pWindscreen->ClearAdds(true);

    // 
    g_pPostProcessing->Reset();

    // 
    if(g_pEnvironment->GetOldBackground())
    {
        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (ENVIRONMENT_DEFAULT_SPEED);
    }
    else
    {
        g_pEnvironment->SetTargetDirectionLerp(ENVIRONMENT_DEFAULT_DIRECTION, 5.0f);
        g_pEnvironment->SetTargetSideLerp     (ENVIRONMENT_DEFAULT_SIDE,      5.0f);
        g_pEnvironment->SetTargetSpeedLerp    (ENVIRONMENT_DEFAULT_SPEED,     5.0f);
    }

    // 
    g_pSave->SaveFile();

    // 
    this->CancelFadeMusic();
    
    if(g_bDemoVersion && (g_MusicPlayer.GetCurMusic() != g_MusicPlayer.GetMusicName("menu.ogg")))   // TODO 1: condition for finale in demo
        g_MusicPlayer.Stop();
    
}


// ****************************************************************
// render the game
void cGame::Render()
{
    if(g_bTiltMode)
    {
        
        const auto nFlipFunc = []()
        {
            if(g_CurConfig.Game.iMirrorMode)
            {
                if(IsHorizontal(g_pPostProcessing->GetDirection())) c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._22 *= -1.0f;
                                                               else c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._11 *= -1.0f;
            }
        };
        
        coreVector3 vCamPos = CAMERA_POSITION;
        if(g_fShiftMode)
        {
            const coreVector3 vShake = coreVector3(g_pPostProcessing->GetPosition() * 80.0f, 0.0f);
            vCamPos += this->CalculateCamShift() * 0.5f + vShake;
        }
        
        const coreVector3 vCamOri = MapToAxis(CAMERA_ORIENTATION, g_pPostProcessing->GetDirection());
        
        Core::Graphics->SetCamera(vCamPos, CAMERA_DIRECTION, vCamOri);   // do not reset at the end
            
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 51.0f, 500.0f);    // invoke transform-update
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);    // TODO 1: optimize
        nFlipFunc();
        if(g_CurConfig.Game.iMirrorMode) glCullFace(GL_FRONT);

        __DEPTH_GROUP_BOTTOM
        {
            // 
            m_EnemyManager.RenderBottom();
            m_pCurMission->RenderBottom();

            // 
            m_CrashManager.Render();
        
            //g_pSpecialEffects->RenderBottom();
        }

        __DEPTH_GROUP_SHIP   // # 1
        {
            // render all players
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].Render();

            // 
            for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                m_aHelper[i].Render();

            // 
            m_Tracker.Render();

            // render all enemies
            m_EnemyManager.Render();
        }

        __DEPTH_GROUP_UNDER
        {
            
            
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 1.0f, 500.0f);   // TODO 1: to make it possible for bullets flying towards screen
        nFlipFunc();

            m_EnemyManager.RenderUnder();
            m_pCurMission->RenderUnder();
            
            DEPTH_PUSH_DOUBLE
            
            g_pSpecialEffects->RenderBottom();
            
        m_BulletManagerEnemy.Render();

            // render low-priority bullet manager
            m_BulletManagerPlayer.Render();
            m_BulletManagerPlayer.RenderAfter();

            DEPTH_PUSH

            // 
            m_ShieldManager.Render();

            // 
            m_ExhaustManager.Render();

            // render underlying objects
            //if(!g_bTiltMode) m_EnemyManager.RenderUnder();
            //if(!g_bTiltMode) m_pCurMission->RenderUnder();

        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);
        nFlipFunc();

            DEPTH_PUSH

            glDepthMask(false);
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderBefore();

                // 
                m_Tracker.RenderBefore();
            }
            glDepthMask(true);

            // 
            for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                m_aHelper[i].RenderBefore();
        }

        __DEPTH_GROUP_SHIP   // # 2
        {
            // apply deferred outline-layer
            g_pOutline->Apply();

        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 1.0f, 500.0f);   // TODO 1: to make it possible for bullets flying towards screen
        nFlipFunc();
            
            m_BulletManagerPlayerTop.Render();
            m_BulletManagerPlayerTop.RenderAfter();

        m_BulletManagerEnemy.RenderAfter();
            
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);
        nFlipFunc();
        }

        __DEPTH_GROUP_OVER
        {
            DEPTH_PUSH

            // 
            m_ItemManager.Render();

            // render overlying objects
            m_EnemyManager.RenderOver();
            m_pCurMission->RenderOver();

            // TODO 1: push oder GL_DEPTH_TEST ?? rauf schieben ?? 
            glDepthMask(false);
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderMiddle();

                // 
                m_Tracker.RenderMiddle();
            }
            glDepthMask(true);
        }

        __DEPTH_GROUP_TOP
        {
            DEPTH_PUSH_DOUBLE
            
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 1.0f, 500.0f);   // TODO 1: to make it possible for ENEMIES and special effects near screen
        nFlipFunc();

            // render special-effects
            g_pSpecialEffects->Render();

            // render high-priority bullet manager
            //if(!g_bTiltMode) m_BulletManagerEnemy.Render();
            //if(!g_bTiltMode) m_BulletManagerEnemy.RenderAfter();

            // render top objects
            m_EnemyManager.RenderTop();
            m_pCurMission->RenderTop();
            
        Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);
        nFlipFunc();

            glDisable(GL_DEPTH_TEST);
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderAfter();

                // 
                for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                    m_aHelper[i].RenderAfter();

                // 
                m_Tracker.RenderAfter();
            }
            glEnable(GL_DEPTH_TEST);
        }

        __DEPTH_RESET
        
        

        nFlipFunc();
        if(g_CurConfig.Game.iMirrorMode) glCullFace(GL_BACK);

    }
    else
    {
/*
        coreVector3 vShift = coreVector3(0.0f,0.0f,0.0f);
        for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
        {
            vShift += m_aPlayer[i].GetPosition();
        }
        vShift /= I_TO_F(this->GetNumPlayers());

        Core::Graphics->SetCamera(CAMERA_POSITION + vShift * 0.02f, CAMERA_DIRECTION, CAMERA_ORIENTATION);

        coreVector2 vTarget = coreVector2(0.0f,0.0f);
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            vTarget += pPlayer->GetPosition().xy();
        });

        g_pEnvironment->SetTargetSide(vTarget * (0.05f * BLENDH3(MIN1(g_pGame->GetTimeTable()->GetTimeEvent() * 0.5f)) * RCP(I_TO_F(g_pGame->GetNumPlayers()))), 10.0f);
        //g_pEnvironment->SetTargetSideNow(vTarget * (0.1f * BLENDH3(MIN1(g_pGame->GetTimeTable()->GetTimeEvent() * 0.5f)) * RCP(I_TO_F(g_pGame->GetNumPlayers()))));
*/
        
        __DEPTH_GROUP_BOTTOM
        {
            // 
            m_EnemyManager.RenderBottom();
            m_pCurMission->RenderBottom();

            // 
            m_CrashManager.Render();

            g_pSpecialEffects->RenderBottom();
        }

        __DEPTH_GROUP_SHIP   // # 1
        {
            // render all players
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].Render();

            // 
            for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                m_aHelper[i].Render();

            // 
            m_Tracker.Render();

            // render all enemies
            m_EnemyManager.Render();
        }

        __DEPTH_GROUP_UNDER
        {
            DEPTH_PUSH_DOUBLE

            // render low-priority bullet manager
            m_BulletManagerPlayer.Render();
            m_BulletManagerPlayer.RenderAfter();

            DEPTH_PUSH

            // 
            m_ShieldManager.Render();

            // 
            m_ExhaustManager.Render();

            // render underlying objects
            m_pCurMission->RenderUnder();   
            m_EnemyManager.RenderUnder();   

            DEPTH_PUSH

            glDepthMask(false);
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderBefore();

                // 
                m_Tracker.RenderBefore();
            }
            glDepthMask(true);

            // 
            for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                m_aHelper[i].RenderBefore();
        }

        __DEPTH_GROUP_SHIP   // # 2
        {
            // apply deferred outline-layer
            g_pOutline->Apply();
        }

        __DEPTH_GROUP_OVER
        {
            DEPTH_PUSH

            // 
            m_ItemManager.Render();

            // render overlying objects
            m_EnemyManager.RenderOver();
            m_pCurMission->RenderOver();

            // TODO 1: push oder GL_DEPTH_TEST ?? rauf schieben ?? 
            //glDepthMask(false);   because of pulse weapon
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderMiddle();

                // 
                m_Tracker.RenderMiddle();
            }
            //glDepthMask(true);
        }

        __DEPTH_GROUP_TOP
        {
            DEPTH_PUSH_DOUBLE

            // render special-effects
            g_pSpecialEffects->Render();

            // render high-priority bullet manager
            m_BulletManagerEnemy.Render();
            m_BulletManagerEnemy.RenderAfter();
            
            if(m_BulletManagerPlayerTop.GetNumBulletsTypedEst<cRayBullet>() ||
               m_BulletManagerPlayerTop.GetNumBulletsTypedEst<cPulseBullet>())
            {
                DEPTH_PUSH
                m_BulletManagerPlayerTop.Render();
                m_BulletManagerPlayerTop.RenderAfter();
            }

            // render top objects
            m_EnemyManager.RenderTop();
            m_pCurMission->RenderTop();

            glDisable(GL_DEPTH_TEST);
            {
                // 
                for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                    m_aPlayer[i].RenderAfter();

                // 
                for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
                    m_aHelper[i].RenderAfter();

                // 
                m_Tracker.RenderAfter();
            }
            glEnable(GL_DEPTH_TEST);
        }

        __DEPTH_RESET
    }
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // handle intro and outro animation
    if(!this->__HandleIntro()) return;
    if(!this->__HandleOutro()) return;

    // 
    m_pCurMission->MoveAlways();

    if(SPECIAL_FROZEN)
    {
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            //m_aPlayer[i].MoveFrozen();
            m_aPlayer[i].Move();
        
        this->__HandleDefeat();
        return;
    }

    // 
    m_TimeTable.Update();

    //if(m_TimeTable.GetTimeEvent() >= 6.3f)   // GAME_INTRO_INTERFACE
    //    m_Interface.SetVisible(true);

    // 
    cHelper::GlobalUpdate();

    // 
    m_EnemyManager.MoveBefore();
    m_pCurMission->MoveBefore();

    // move all players
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Move();

    // move all helpers
    for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
        m_aHelper[i].Move();

    // 
    m_Tracker.Move();

    // 
    m_pCurMission->MoveMiddle();   // # swapped
    m_EnemyManager.MoveMiddle();

    // move the bullet managers
    m_BulletManagerPlayer.Move();
    m_BulletManagerEnemy .Move();
    
    m_BulletManagerPlayerTop.Move();

    // 
    m_EnemyManager.MoveAfter();
    m_pCurMission->MoveAfter();

    // 
    m_ItemManager  .Move();
    m_ShieldManager.Move();

    // 
    m_CrashManager.Move();

    // 
    m_ExhaustManager.Move();

    // handle default object collisions
    this->__HandleCollisions();   // TODO 1: do all collisions here (virtual funcs), for consistency, e.g. bullet collisions are done before move when handled in mission/boss 

    // 
    this->__HandleDefeat();
    
    m_fHitDelay   .UpdateMax(-20.0f, 0.0f);
    m_fVanishDelay.UpdateMax(-20.0f, 0.0f);
    
    //if(HAS_FLAG(m_iStatus, GAME_STATUS_PLAY)) g_pEnvironment->SetTargetSide(m_aPlayer[0].GetPosition().xy() * 0.03f, 10.0f);
}


// ****************************************************************
// render the overlay separately
void cGame::RenderOverlay()
{
    // render interface and combat text
    m_Interface .Render();
    m_CombatText.Render();
}


// ****************************************************************
// move the overlay separately
void cGame::MoveOverlay()
{
    // move interface and combat text
    m_Interface .Move();
    m_CombatText.Move();
}


// ****************************************************************
// 
void cGame::MoveAlways()
{
    // 
    if(m_fMusicFade)
    {
        m_fMusicFade.UpdateMax(-m_fMusicSpeed, 0.0f);
        if(!m_fMusicFade) g_MusicPlayer.Stop();
    }

    g_MusicPlayer.SetVolume((m_fMusicFade ? BLENDH3(m_fMusicFade.ToFloat()) : 1.0f) * g_pMenu->GetVolume() * m_fMusicVolume * MUSIC_VOLUME);
}


// ****************************************************************
// load new active mission
void cGame::LoadMissionID(const coreInt32 iID, const coreUint8 iTakeFrom, const coreUint8 iTakeTo)
{
    for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
    {
        if(!m_aHelper[i].HasStatus(HELPER_STATUS_DEAD))
            g_pSpecialEffects->CreateSplashColor(m_aHelper[i].GetPosition(), SPECIAL_SPLASH_TINY, m_aHelper[i].GetColor3());
    }
    
    // 
    this->__ClearAll(false);

    // hold old mission (to keep resources valid)
    ASSERT(m_pCurMission)
    const cMission* pOldMission = m_pCurMission;

    // create new mission
    switch(iID)
    {
    default: ASSERT(false)
    case cNoMission     ::ID: m_pCurMission = new cNoMission     (); break;
    case cViridoMission ::ID: m_pCurMission = new cViridoMission (); break;
    case cNevoMission   ::ID: m_pCurMission = new cNevoMission   (); break;
    case cHarenaMission ::ID: m_pCurMission = new cHarenaMission (); break;
    case cRutilusMission::ID: m_pCurMission = new cRutilusMission(); break;
    case cGeluMission   ::ID: m_pCurMission = new cGeluMission   (); break;
    case cCalorMission  ::ID: m_pCurMission = new cCalorMission  (); break;
    case cMuscusMission ::ID: m_pCurMission = new cMuscusMission (); break;
    case cAterMission   ::ID: m_pCurMission = new cAterMission   (); break;
    case cIntroMission  ::ID: m_pCurMission = new cIntroMission  (); break;
    case cBonus1Mission ::ID: m_pCurMission = new cBonus1Mission (); break;
    case cBonus2Mission ::ID: m_pCurMission = new cBonus2Mission (); break;
    case cErrorMission  ::ID: m_pCurMission = new cErrorMission  (); break;
    case cDemoMission   ::ID: m_pCurMission = new cDemoMission   (); break;
    }

    // delete possible old mission
    SAFE_DELETE(pOldMission)

    // 
    m_iCurMissionIndex = std::find(m_piMissionList, m_piMissionList + m_iNumMissions, iID) - m_piMissionList;
    ASSERT(m_iCurMissionIndex < m_iNumMissions)

    if(iID != cNoMission::ID)
    {
        // setup the mission
        m_pCurMission->Setup(iTakeFrom, iTakeTo);

        // set initial status
        m_iStatus = GAME_STATUS_LOADING;

        // 
        m_bDefeatDelay = false;

        if((m_Options.iKind == GAME_KIND_ALL) || (m_Options.iKind == GAME_KIND_MISSION))
        {
            // 
            m_pCurMission->Open();
        }
    }

    Core::Log->Info("Mission (%s) created", m_pCurMission->GetName());
}


// ****************************************************************
// 
void cGame::LoadMissionIndex(const coreUintW iIndex, const coreUint8 iTakeFrom, const coreUint8 iTakeTo)
{
    ASSERT(iIndex < m_iNumMissions)
    this->LoadMissionID(m_piMissionList[iIndex], iTakeFrom, iTakeTo);
}


// ****************************************************************
// 
void cGame::LoadNextMission()
{
    // 
    this->CloseMission();

    if((m_Options.iKind == GAME_KIND_ALL) && (m_iCurMissionIndex + 1u < m_iNumMissions))
    {
        // 
        this->LoadMissionIndex(m_iCurMissionIndex + 1u);
    }
    else
    {
        ASSERT(HAS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

        // 
        REMOVE_FLAG(m_iStatus, GAME_STATUS_OUTRO)
        ADD_FLAG   (m_iStatus, GAME_STATUS_FINISHED)
    }
}


// ****************************************************************
// 
void cGame::CloseMission()
{
    if(m_pCurMission->GetID() != cNoMission::ID)
    {
        if((m_Options.iKind == GAME_KIND_ALL) || (m_Options.iKind == GAME_KIND_MISSION))
        {
            // 
            m_pCurMission->Close();
        }
    }
}


// ****************************************************************
// 
void cGame::StartIntro()
{
    ASSERT(!HAS_FLAG(m_iStatus, GAME_STATUS_PLAY))
    ASSERT(!HAS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

    // 
    ADD_FLAG(m_iStatus, GAME_STATUS_INTRO)

    // 
    m_fTimeInOut = GAME_INTRO_OFFSET;

    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        // 
        m_aPlayer[i].Kill(false);
        m_aPlayer[i].Resurrect();
        m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);   // TODO 1: handle combination with gameplay code

        // 
        m_aPlayer[i].ActivateNormalShading();

        // 
        const coreFloat fSide = this->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;
        m_aPlayer[i].SetPosition(coreVector3(fSide, -140.0f, 0.0f));

        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }
}


// ****************************************************************
// 
void cGame::StartOutro(const coreUint8 iType, const coreUint8 iSub)
{
    ASSERT( HAS_FLAG(m_iStatus, GAME_STATUS_PLAY))
    ASSERT(!HAS_FLAG(m_iStatus, GAME_STATUS_INTRO))

    // 
    REMOVE_FLAG(m_iStatus, GAME_STATUS_PLAY)
    ADD_FLAG   (m_iStatus, GAME_STATUS_OUTRO)

    // 
    m_fTimeInOut = 0.0f;
    m_iOutroType = iType;
    m_iOutroSub  = iSub;

    // 
    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
        m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
    
    if(m_iOutroType == GAME_OUTRO_ENDING_NORMAL)
    {
        for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
            m_aPlayer[i].AddStatus(PLAYER_STATUS_KEEP_RANGE);
    }

    // 
    m_Interface .SetVisible(false);
    m_CombatText.SetVisible(false);

    // 
    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_MISSION_END(m_pCurMission->GetID()));

    // 
    if((m_iOutroSub >= 1u) && (m_iOutroSub < 10u))
    {
        m_Tracker.Resurrect();
        m_Tracker.EnableWind();
        m_Tracker.SetPosition(coreVector3((m_iOutroSub == 1u) ? -0.5f : 0.8f, -1.5f, 0.0f) * FOREGROUND_AREA3);
    }
    else if((m_iOutroSub >= 11u) && (m_iOutroSub < 20u))
    {
        this->GetHelper(ELEMENT_WHITE)->Resurrect(false);
    }
}


// ****************************************************************
// 
void cGame::FadeMusic(const coreFloat fSpeed)
{
    if(!m_fMusicFade)
    {
        // 
        m_fMusicFade  = 1.0f;
        m_fMusicSpeed = fSpeed;
    }
}


// ****************************************************************
// 
void cGame::CancelFadeMusic()
{
    // 
    if(m_fMusicFade) g_MusicPlayer.Stop();

    //
    m_fMusicFade = 0.0f;
}


// ****************************************************************
// 
void cGame::PlayHitSound(const coreVector3 vPosition)
{
    if(!m_fHitDelay)
    {
        m_fHitDelay = 1.0f;
        g_pSpecialEffects->PlaySound(vPosition, 1.0f, 1.0f, SOUND_BULLET_HIT);
    }
}


// ****************************************************************
// 
void cGame::PlayReflectSound(const coreVector3 vPosition)
{
    if(!m_fHitDelay)
    {
        m_fHitDelay = 1.0f;
        g_pSpecialEffects->PlaySound(vPosition, 1.0f, 1.0f, SOUND_BULLET_REFLECT);
    }
}


// ****************************************************************
// 
void cGame::PlayVanishSound(const coreVector3 vPosition)
{
    if(!m_fVanishDelay)
    {
        m_fVanishDelay = 1.0f;
        g_pSpecialEffects->PlaySound(vPosition, 1.0f, 1.0f, SOUND_BULLET_VANISH);
    }
}


// ****************************************************************
// 
void cGame::UseContinue()
{
    ASSERT(HAS_FLAG(m_iStatus, GAME_STATUS_DEFEATED))

    const coreUintW iMissionIndex = m_iCurMissionIndex;
    const coreUintW iSegmentIndex = m_pCurMission->GetCurSegmentIndex();

    // 
    this->LoadMissionID(m_pCurMission->GetID(), iSegmentIndex, m_pCurMission->GetTakeTo());

    // 
    REMOVE_FLAG(m_iStatus, GAME_STATUS_DEFEATED)   // # already removed
    ADD_FLAG   (m_iStatus, GAME_STATUS_CONTINUE)
    ADD_FLAG   (m_iStatus, GAME_STATUS_QUICK)
    ADD_FLAG   (m_iStatus, GAME_STATUS_NAMELESS)

    // 
    ASSERT(m_iContinues)
    m_iContinues -= 1u;

    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        // 
        m_aPlayer[i].GetDataTable ()->RevertSegment(iMissionIndex, iSegmentIndex);
        m_aPlayer[i].GetScoreTable()->RevertSegment(iMissionIndex, iSegmentIndex);

        // 
        m_aPlayer[i].GetDataTable()->EditCounterTotal  ()                            ->iContinuesUsed += 1u;
        m_aPlayer[i].GetDataTable()->EditCounterMission(iMissionIndex)               ->iContinuesUsed += 1u;
        m_aPlayer[i].GetDataTable()->EditCounterSegment(iMissionIndex, iSegmentIndex)->iContinuesUsed += 1u;
    }

    // 
    m_TimeTable.RevertSegment(iMissionIndex, iSegmentIndex);

    // 
    g_pSave->EditGlobalStats      ()                            ->iContinuesUsed += 1u;
    g_pSave->EditLocalStatsArcade ()                            ->iContinuesUsed += 1u;
    g_pSave->EditLocalStatsMission(iMissionIndex)               ->iContinuesUsed += 1u;
    g_pSave->EditLocalStatsSegment(iMissionIndex, iSegmentIndex)->iContinuesUsed += 1u;
}


// ****************************************************************
// 
void cGame::UseNext()
{
    // 
    ADD_FLAG(m_iStatus, GAME_STATUS_QUICK)
}


// ****************************************************************
// 
void cGame::UseRestart()
{
    // 
    ADD_FLAG(m_iStatus, GAME_STATUS_QUICK)
    ADD_FLAG(m_iStatus, GAME_STATUS_NAMELESS)
}


// ****************************************************************
// 
void cGame::DisableMissionName()
{
    ADD_FLAG(m_iStatus, GAME_STATUS_NAMELESS)
}


// ****************************************************************
// 
void cGame::RepairPlayer()
{
    if(HAS_FLAG(m_iStatus, GAME_STATUS_DEFEATED)) return;

    if(m_pRepairEnemy->GetPlayer())
    {
        cPlayer* pPlayer = m_pRepairEnemy->GetPlayer();

        // 
        pPlayer->Resurrect();

        // 
        pPlayer->SetPosition  (m_pRepairEnemy->GetPosition());
        pPlayer->SetDesaturate(PLAYER_DESATURATE);
        pPlayer->StartFeeling (PLAYER_FEEL_TIME_REPAIR, 0u);

        pPlayer->SetCurHealth(1);
        pPlayer->SetCurShield(pPlayer->GetMaxShield() / 5);
        pPlayer->AddStatus   (PLAYER_STATUS_REPAIRED);

        // 
        g_pSpecialEffects->PlaySound(m_pRepairEnemy->GetPosition(), 1.0f, 1.0f, SOUND_PLAYER_EXPLOSION);
        g_pSpecialEffects->RumblePlayer(pPlayer, SPECIAL_RUMBLE_BIG, 500u);

        // 
        pPlayer->GetDataTable()->EditCounterTotal  ()->iRepairsUsed += 1u;
        pPlayer->GetDataTable()->EditCounterMission()->iRepairsUsed += 1u;
        pPlayer->GetDataTable()->EditCounterSegment()->iRepairsUsed += 1u;

        // 
        g_pSave->EditGlobalStats      ()->iRepairsUsed += 1u;
        g_pSave->EditLocalStatsArcade ()->iRepairsUsed += 1u;
        g_pSave->EditLocalStatsMission()->iRepairsUsed += 1u;
        g_pSave->EditLocalStatsSegment()->iRepairsUsed += 1u;

        // 
        m_pRepairEnemy->Kill(false);
    }

    // 
    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        if(m_aPlayer[i].HasStatus(PLAYER_STATUS_DEAD))
        {
            m_aPlayer[i].Resurrect();

            m_aPlayer[i].SetDesaturate(PLAYER_DESATURATE);
            m_aPlayer[i].StartFeeling (PLAYER_FEEL_TIME_REPAIR, 0u);

            m_aPlayer[i].SetCurHealth(1);
            m_aPlayer[i].AddStatus   (PLAYER_STATUS_REPAIRED);

            m_CombatText.DrawText(Core::Language->GetString("TEXT_LUCKY"), m_aPlayer[i].GetPosition(), COLOR_MENU_INSIDE);

            m_bDefeatDelay = false;
        }
    }
}


// ****************************************************************
// 
void cGame::ChangeDepthLevel(const coreUint8 iLevelNear, const coreUint8 iLevelFar)const
{
    // 
    ASSERT(iLevelNear < iLevelFar)
    glDepthRange(0.05f * I_TO_F(iLevelNear), 0.05f * I_TO_F(iLevelFar));
}

void cGame::PushDepthLevel(const coreUint8 iLevels)
{
    // 
    ASSERT(m_iDepthLevel >= (m_iDepthDebug & ~BIT(7u)) + iLevels)
    m_iDepthLevel -= iLevels;

    // 
    this->ChangeDepthLevel(m_iDepthLevel, m_iDepthLevel + iLevels);
}

void cGame::PushDepthLevelShip()
{
    // 
    ASSERT(m_iDepthDebug & BIT(7u))
    __DEPTH_GROUP_SHIP
}

void cGame::PopDepthLevel(const coreUint8 iLevels)
{
    // 
    ASSERT(m_iDepthLevel < (m_iDepthDebug & ~BIT(7u)) + 10u)
    m_iDepthLevel += iLevels;

    // 
    this->ChangeDepthLevel(m_iDepthLevel, m_iDepthLevel + iLevels);
}


// ****************************************************************
// 
coreVector3 cGame::CalculateCamShift()const
{
    coreVector3 vShift = coreVector3(0.0f,0.0f,0.0f);
    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        vShift += m_aPlayer[i].GetPosition();
    }
    vShift /= I_TO_F(this->GetNumPlayers());

    return vShift * g_fShiftMode * 1.5f;
}


// ****************************************************************
// 
void cGame::PrefetchBoss()
{
    // 
    m_BulletManagerEnemy.PrefetchBullet<cOrbBullet>     ();
    m_BulletManagerEnemy.PrefetchBullet<cConeBullet>    ();
    m_BulletManagerEnemy.PrefetchBullet<cWaveBullet>    ();
    m_BulletManagerEnemy.PrefetchBullet<cSpearBullet>   ();
    m_BulletManagerEnemy.PrefetchBullet<cTriangleBullet>();
    m_BulletManagerEnemy.PrefetchBullet<cFlipBullet>    ();
    m_BulletManagerEnemy.PrefetchBullet<cQuadBullet>    ();
    m_BulletManagerEnemy.PrefetchBullet<cViewBullet>    ();
}


// ****************************************************************
// 
RETURN_NONNULL cPlayer* cGame::FindPlayerSide(const coreVector2 vPosition)
{
    STATIC_ASSERT(GAME_PLAYERS == 2u)

    // 
    if(m_aPlayer[1].HasStatus(PLAYER_STATUS_DEAD)) return &m_aPlayer[0];
    if(m_aPlayer[0].HasStatus(PLAYER_STATUS_DEAD)) return &m_aPlayer[1];

    // 
    ASSERT(vPosition.x)
    return &m_aPlayer[(vPosition.x > 0.0f) ? 1u : 0u];
}


// ****************************************************************
// 
RETURN_NONNULL cPlayer* cGame::FindPlayerDual(const coreUintW iIndex)
{
    STATIC_ASSERT(GAME_PLAYERS == 2u)

    // 
    if(m_aPlayer[1].HasStatus(PLAYER_STATUS_DEAD)) return &m_aPlayer[0];
    if(m_aPlayer[0].HasStatus(PLAYER_STATUS_DEAD)) return &m_aPlayer[1];

    // 
    ASSERT(iIndex < GAME_PLAYERS)
    return &m_aPlayer[iIndex];
}


// ****************************************************************
// 
coreBool cGame::IsAlone()const
{
    STATIC_ASSERT(GAME_PLAYERS == 2u)

    if(m_aPlayer[1].HasStatus(PLAYER_STATUS_DEAD)) return true;
    if(m_aPlayer[0].HasStatus(PLAYER_STATUS_DEAD)) return true;

    return false;
}


// ****************************************************************
// 
coreUint8 cGame::CalcMedal(const coreFloat fTime, const coreFloat* pfMedalGoal)
{
    ASSERT(pfMedalGoal && (pfMedalGoal[0] < pfMedalGoal[1]) && (pfMedalGoal[1] < pfMedalGoal[2]) && (pfMedalGoal[2] < pfMedalGoal[3]) && (pfMedalGoal[3] < pfMedalGoal[4]))

    // 
         if(fTime <= pfMedalGoal[0]) return MEDAL_DARK;
    else if(fTime <= pfMedalGoal[1]) return MEDAL_PLATINUM;
    else if(fTime <= pfMedalGoal[2]) return MEDAL_GOLD;
    else if(fTime <= pfMedalGoal[3]) return MEDAL_SILVER;
    else                             return MEDAL_BRONZE;
}


// ****************************************************************
// 
coreFloat cGame::CalcMedalTime(const coreFloat fTime, const coreFloat* pfMedalGoal)
{
    ASSERT(pfMedalGoal && (pfMedalGoal[0] < pfMedalGoal[1]) && (pfMedalGoal[1] < pfMedalGoal[2]) && (pfMedalGoal[2] < pfMedalGoal[3]) && (pfMedalGoal[3] < pfMedalGoal[4]))

    // 
         if(fTime <= pfMedalGoal[0]) return pfMedalGoal[0] - fTime;
    else if(fTime <= pfMedalGoal[1]) return pfMedalGoal[1] - fTime;
    else if(fTime <= pfMedalGoal[2]) return pfMedalGoal[2] - fTime;
    else if(fTime <= pfMedalGoal[3]) return pfMedalGoal[3] - fTime;
    else                             return 0.0f;
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusTime(const coreFloat fTime, const coreFloat* pfMedalGoal)
{
    ASSERT(pfMedalGoal && (pfMedalGoal[0] < pfMedalGoal[1]) && (pfMedalGoal[1] < pfMedalGoal[2]) && (pfMedalGoal[2] < pfMedalGoal[3]) && (pfMedalGoal[3] < pfMedalGoal[4]))

    // 
    return F_TO_UI(LERP(20000.0f, 1000.0f, MIN(fTime * RCP(pfMedalGoal[4]), 1.0f)));
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusMedal(const coreUint8 iMedal)
{
    // 
    switch(iMedal)
    {
    default: ASSERT(false)
    case MEDAL_DARK:     return 10000u;
    case MEDAL_PLATINUM: return  5000u;
    case MEDAL_GOLD:     return  3000u;
    case MEDAL_SILVER:   return  2000u;
    case MEDAL_BRONZE:   return  1000u;
    case MEDAL_NONE:     return     0u;
    }
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusBadge(const coreUint8 iBadge)
{
    // 
    switch(iBadge)
    {
    default: ASSERT(false)
    case BADGE_ACHIEVEMENT: return 0u;
    case BADGE_HARD:        return 5u;
    case BADGE_NORMAL:      return 5u;
    case BADGE_EASY:        return 5u;
    }
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusSurvive(const coreUint32 iDamageTaken, const coreBool bWasDead)
{
    // 
    //if(!bWasDead)
    {
        // 
        switch(iDamageTaken)
        {
        case 0u: return 200000u;
        case 1u: return 100000u;
        case 2u: return  50000u;
        case 3u: return  30000u;
        case 4u: return  20000u;
        default: return  10000u - MIN(50u * (iDamageTaken - LIVES), 9900u);
        }

        STATIC_ASSERT(LIVES == 5u)
    }

    //return 0u;
}


// ****************************************************************
// handle intro animation
coreBool cGame::__HandleIntro()
{
    if(HAS_FLAG(m_iStatus, GAME_STATUS_LOADING))
    {
        // do not start while game resources are still loading
        if(Core::Manager::Resource->IsLoading()) return false;   // mission
        REMOVE_FLAG(m_iStatus, GAME_STATUS_LOADING)
    
        if(!HAS_FLAG(m_iStatus, GAME_STATUS_QUICK)) this->FadeMusic(0.7f);
    }

    if(HAS_FLAG(m_iStatus, GAME_STATUS_INTRO))
    {
        if(Core::Manager::Resource->IsLoading()) return false;   // background

        // 
        const coreFloat fOldTime = m_fTimeInOut;
        m_fTimeInOut.Update(1.0f);

        if(m_fTimeInOut >= GAME_INTRO_DURATION)
        {
            ASSERT(!HAS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

            // end intro and start actual game
            REMOVE_FLAG(m_iStatus, GAME_STATUS_INTRO)
            ADD_FLAG   (m_iStatus, GAME_STATUS_PLAY)

            // re-enable player controls
            for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
                m_aPlayer[i].RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);

            // 
            m_Interface .SetVisible(true);
            m_CombatText.SetVisible(true);

            // 
            g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_MISSION_START(m_pCurMission->GetID()));
        }
        else
        {
            // create spline for intro animation (YZ)
            static const coreSpline2 s_Spline = []()
            {
                coreSpline2 oSpline(3u);

                oSpline.AddNode(coreVector2(-140.0f,-10.0f), coreVector2( 1.0f, 0.0f));
                oSpline.AddNode(coreVector2(  10.0f, 10.0f), coreVector2(-1.0f,-1.0f).Normalized());
                oSpline.AddNode(coreVector2( -30.0f,  0.0f), coreVector2(-1.0f, 0.0f));
                oSpline.Refine();

                return oSpline;
            }();

            this->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                // calculate new player position and rotation
                const coreFloat   fTime = CLAMP01((i ? 0.0f : 0.08f) + m_fTimeInOut / GAME_INTRO_DURATION);
                const coreVector2 vPos  = s_Spline.CalcPositionLerp(LERPB(0.0f, 1.0f,    fTime));
                const coreVector2 vDir  = coreVector2::Direction   (LERPS(0.0f, 4.0f*PI, fTime));

                // 
                if((pPlayer->GetPosition().y < -FOREGROUND_AREA.y) && (vPos.x >= -FOREGROUND_AREA.y))
                {
                    g_pSpecialEffects->CreateBlowColor(pPlayer->GetPosition(), coreVector3(0.0f,1.0f,0.0f), SPECIAL_BLOW_SMALL, pPlayer->GetEnergyColor());
                    if(!i) g_pSpecialEffects->PlaySound(this->IsMulti() ? LERP(m_aPlayer[0].GetPosition(), m_aPlayer[1].GetPosition(), 0.5f) : pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_FLY);
                }

                // fly player animated into the game field
                pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, vPos));
                pPlayer->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                pPlayer->SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
                pPlayer->SetThrust     (LERPB(1.0f, 0.0f, fTime));
            });

            // 
            if(InBetween(0.65f, fOldTime, m_fTimeInOut) && (!g_MusicPlayer.IsPlaying() || m_fMusicFade))   // # beware of intro/loop and game-destruction on segment-change
            {
                const coreChar* pcName = m_pCurMission->GetMusicName();
                if(pcName && pcName[0])
                {
                    this->CancelFadeMusic();

                    g_MusicPlayer.SelectName(pcName);
                    g_MusicPlayer.Play();
                }
            }
        }
    }

    return true;
}


// ****************************************************************
// handle outro animation
coreBool cGame::__HandleOutro()
{
    if(HAS_FLAG(m_iStatus, GAME_STATUS_OUTRO) || HAS_FLAG(m_iStatus, GAME_STATUS_FINISHED))
    {
        // 
        const coreFloat fOldTime = m_fTimeInOut;
        m_fTimeInOut.Update(1.0f);

        this->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            if((m_iOutroType == GAME_OUTRO_MISSION) || (m_iOutroType == GAME_OUTRO_SEGMENT) || (m_iOutroType == GAME_OUTRO_BEGINNING))
            {
                // calculate new player position and rotation
                const coreFloat   fTime = MAX((i ? -0.16f : 0.0f) + m_fTimeInOut, 0.0f);
                const coreFloat   fPos  = MIN(pPlayer->GetPosition().y + 90.0f * fTime * TIME, 1000.0f);
                const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 2.0f*PI, 0.6f * fTime));

                // fly player animated out of the game field
                pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, fPos, pPlayer->GetPosition().z));
                pPlayer->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                pPlayer->SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
                pPlayer->SetThrust     ((fTime < 0.2f) ? LERPB(0.0f, 0.7f, fTime / 0.2f) : LERPB(0.7f, 0.3f, fTime - 0.2f));

                // 
                if(g_pForeground->IsVisiblePoint(pPlayer->GetPosition().xy()) && InBetween(i ? 0.16f : 0.0f, fOldTime, m_fTimeInOut))
                {
                    if(!i) g_pSpecialEffects->PlaySound(this->IsMulti() ? LERP(m_aPlayer[0].GetPosition(), m_aPlayer[1].GetPosition(), 0.5f) : pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_FLY);
                }
            }
            else
            {
                if(m_fTimeInOut >= 13.0f) pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }
        });

        if((m_iOutroSub == 1u) || (m_iOutroSub == 2u))
        {
            // 
            const coreFloat   fTime = MAX(m_fTimeInOut - 2.5f, 0.0f);
            const coreFloat   fPos  = MIN(m_Tracker.GetPosition().y + 90.0f * fTime * TIME, 1000.0f);
            const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 2.0f*PI, 0.6f * fTime));

            // 
            m_Tracker.SetPosition   (coreVector3(m_Tracker.GetPosition().x, fPos, m_Tracker.GetPosition().z));
            m_Tracker.SetDirection  (coreVector3(0.0f,1.0f,0.0f));
            m_Tracker.SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
            m_Tracker.SetThrust     ((fTime < 0.2f) ? LERPB(0.0f, 0.7f, fTime / 0.2f) : LERPB(0.7f, 0.3f, fTime - 0.2f));

            // 
            if(InBetween(2.5f, fOldTime, m_fTimeInOut))
            {
                g_pSpecialEffects->PlaySound(m_Tracker.GetPosition(), 1.0f, 0.8f, SOUND_EFFECT_FLY);
            }
        }
        else if(m_iOutroSub == 11u)
        {
            cHelper* pHelper = this->GetHelper(ELEMENT_WHITE);

            const coreFloat fTime = STEP(2.0f, 4.0f, m_fTimeInOut);
            if(fTime >= 1.0f) pHelper->Kill(false);

            // 
            pHelper->SetPosition(coreVector3(0.0f, LERP(-1.2f, 1.2f, fTime), 0.0f) * FOREGROUND_AREA3);
        }
        else if(m_iOutroSub == 12u)
        {
            cHelper* pHelper = this->GetHelper(ELEMENT_WHITE);

            const coreFloat fTime = STEP(2.0f, 4.0f, m_fTimeInOut);
            if(fTime >= 1.0f) pHelper->Kill(false);

            // 
            pHelper->SetPosition(coreVector3(LERP(-1.2f, 1.2f, fTime), 0.0f, 0.0f) * FOREGROUND_AREA3);
        }
        else if(m_iOutroSub == 13u)
        {
            cHelper* pHelper = this->GetHelper(ELEMENT_WHITE);

            // 
            static const coreSpline2 s_Spline = []()
            {
                coreSpline2 oSpline(2u);

                oSpline.AddNode(coreVector2( 1.2f, 0.8f), coreVector2(-1.0f,0.0f));
                oSpline.AddNode(coreVector2(-1.2f,-0.8f), coreVector2(-1.0f,0.0f));
                oSpline.Refine();

                return oSpline;
            }();

            const coreFloat fTime = STEP(2.0f, 4.5f, m_fTimeInOut);
            if(fTime >= 1.0f) pHelper->Kill(false);

            // 
            pHelper->SetPosition(coreVector3(s_Spline.CalcPositionLerp(fTime), 0.0f) * FOREGROUND_AREA3);
        }
        else if(m_iOutroSub == 14u)
        {
            cHelper* pHelper = this->GetHelper(ELEMENT_WHITE);

            // 
            static const coreSpline2 s_Spline = []()
            {
                coreSpline2 oSpline(3u);

                oSpline.AddNode(coreVector2(-0.8f, 1.3f), coreVector2(0.0f,-1.0f));
                oSpline.AddNode(coreVector2( 0.0f,-0.8f), coreVector2(1.0f, 0.0f));
                oSpline.AddNode(coreVector2( 0.8f, 1.3f), coreVector2(0.0f, 1.0f));
                oSpline.Refine();

                return oSpline;
            }();

            const coreFloat fTime = STEP(2.0f, 5.0f, m_fTimeInOut);
            if(fTime >= 1.0f) pHelper->Kill(false);

            // 
            pHelper->SetPosition(coreVector3(s_Spline.CalcPositionLerp(fTime), 0.0f) * FOREGROUND_AREA3);
        }
    }

    return true;
}


// ****************************************************************
// 
void cGame::__HandleDefeat()
{
    if(HAS_FLAG(m_iStatus, GAME_STATUS_PLAY))
    {
        if(m_bDefeatDelay)
        {
            // 
            REMOVE_FLAG(m_iStatus, GAME_STATUS_PLAY)
            ADD_FLAG   (m_iStatus, GAME_STATUS_DEFEATED)

            if(m_pRepairEnemy->GetPlayer())
            {
                // 
                g_pSpecialEffects->CreateSplashDark(m_pRepairEnemy->GetPosition(), SPECIAL_SPLASH_TINY);
                m_pRepairEnemy->Kill(false);
            }

            // 
            g_MusicPlayer.Stop();
        }
        else
        {
            coreBool bAllDefeated = true;

            // 
            for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
            {
                cPlayer* pPlayer = &m_aPlayer[i];

                // 
                const coreBool bDefeated = pPlayer->HasStatus(PLAYER_STATUS_DEAD);
                bAllDefeated = bAllDefeated && bDefeated;

                // 
                g_pPostProcessing->SetSaturation(i, bDefeated ? 0.0f : (1.0f - MIN(pPlayer->GetDesaturate(), 1.0f)));

                if(this->IsMulti() && bDefeated && !m_pRepairEnemy->GetPlayer())
                {
                    // 
                    m_pRepairEnemy->AssignPlayer(pPlayer);
                    m_pRepairEnemy->Resurrect();
                }
            }

            if(bAllDefeated)
            {
                // 
                m_bDefeatDelay = true;

                // 
                m_BulletManagerPlayer.ClearBullets(true);

                m_BulletManagerPlayerTop.ClearBullets(true);
            }

            if(m_pRepairEnemy->GetPlayer() && m_pRepairEnemy->ReachedDeath())
            {
                this->RepairPlayer();
            }
        }
    }
}


// ****************************************************************
// handle default object collisions
void cGame::__HandleCollisions()
{
    // (# before enemy-bullet collision) 
    m_ShieldManager.GetEffect(SHIELD_EFFECT_INVINCIBLE)->ForEachShield([this](coreObject3D* OUTPUT pShield, const cEnemy* pOwner)
    {
        // 
        if(!pOwner || pOwner->HasStatus(ENEMY_STATUS_GHOST_BULLET) || !pOwner->GetType()) return;

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, pShield, [this](cBullet* OUTPUT pBullet, const coreObject3D* pShield, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            // 
            if(m_bVisibleCheck && !g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            if(bFirstHit)
            {
                if(pBullet->HasStatus(BULLET_STATUS_ACTIVE) && !pBullet->HasStatus(BULLET_STATUS_GHOST))
                {
                    // 
                    const coreVector2 vDiff = (vIntersection.xy() - pBullet->GetFlyDir() * MAX(pBullet->GetCollisionRadius() * 2.0f, pBullet->GetSpeed() * TIME)) - pShield->GetPosition().xy();
                    pBullet->Reflect(pShield, vIntersection.xy(), vDiff.Normalized());

                    // 
                    this->PlayReflectSound(vIntersection);
                }
            }
        });
    });
    
    
    // 
    cPlayer::TestCollision(PLAYER_TEST_ALL, TYPE_ENEMY, [this](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        m_pCurMission->CollPlayerEnemy(pPlayer, pEnemy, vIntersection, bFirstHit);

        if(bFirstHit)
        {
            if(pPlayer->IsNormal())
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_GHOST) && !pEnemy->HasStatus(ENEMY_STATUS_GHOST_PLAYER))
                {
                    if(true || pEnemy->HasStatus(ENEMY_STATUS_DAMAGING))
                    {
                        if(pEnemy->GetLifeTime() >= 1.0f || pEnemy->HasStatus(ENEMY_STATUS_NODELAY))   // TODO 1: modifiers are not applied (vielleicht egal, weil es eher wichtig für den anfang einer sub-gruppe is ?)

                        // 
                        pPlayer->TakeDamage(15, ELEMENT_NEUTRAL, vIntersection.xy());
                    }
                    else
                    {
                        // 
                        const coreVector2 vDiff = pPlayer->GetOldPos() - pEnemy->GetPosition().xy();
                        pPlayer->ApplyForce  (vDiff.Normalized() * 100.0f);
                        pPlayer->SetInterrupt(PLAYER_INTERRUPT);

                        // 
                        g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 50.0f, 10u, coreVector3(1.0f,1.0f,1.0f));
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                        g_pSpecialEffects->PlaySound(vIntersection, 1.0f, 1.0f, SOUND_EFFECT_DUST);
                    }
                }
            }
        }
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_ALL, TYPE_BULLET_ENEMY, [this](cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        m_pCurMission->CollPlayerBullet(pPlayer, pBullet, vIntersection, bFirstHit);

        if(bFirstHit)
        {
            if(pPlayer->IsNormal())
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_GHOST) && !pBullet->HasStatus(BULLET_STATUS_GHOST))
                {
                    // 
                    pPlayer->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy());
                    //pBullet->Deactivate(true, vIntersection.xy());
                    //pBullet->AddStatus(BULLET_STATUS_GHOST);
                }
            }
        }
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [this](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(m_bVisibleCheck && !g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

        // 
        if(pEnemy->GetID() != cRepairEnemy::ID) m_pCurMission->CollEnemyBullet(pEnemy, pBullet, vIntersection, bFirstHit);
        
        // TODO 1: fix all cases where bullet is made ghost in collision-callback (e.g. handle penetration, rumble, sound, ...)

        if(bFirstHit)
        {
            if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST_BULLET) && !pBullet->HasStatus(BULLET_STATUS_GHOST))
            {
                // 
                const coreInt32 iTaken = pEnemy->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()), false);

                if(iTaken)
                {
                    if(pBullet->HasStatus(BULLET_STATUS_PENETRATE))
                    {
                        // 
                        pBullet->SetDamage(pBullet->GetDamage() - iTaken);
                        ASSERT(pBullet->GetDamage() >= 0)

                        // 
                        if(!pBullet->GetDamage()) pBullet->Deactivate(true, vIntersection.xy());
                    }
                    else
                    {
                        // 
                        pBullet->Deactivate(true, vIntersection.xy());
                    }

                    // 
                    if(!pEnemy->ReachedDeath() && (pBullet->GetID() != cFinalBullet::ID)) this->PlayHitSound(vIntersection);   // TODO 1: alle anderen stellen, wo PlayHitSound aufgerufen wird, ignorieren ReachedDeath
                }

                if(pBullet->HasStatus(BULLET_STATUS_ACTIVE))
                {
                    // prevent an already killed but immortal enemy from reflecting bullets (in the same frame)
                    // TODO 1: this does not work for boss body-parts (e.g. messier rings, zeroth any)
                    if(!pEnemy->ReachedDeath())
                    {
                        if(pEnemy->HasStatus(ENEMY_STATUS_DEACTIVATE))
                        {
                            // 
                            pBullet->Deactivate(true, vIntersection.xy());
                        }
                        else
                        {
                            // 
                            const coreVector2 vDiff = (vIntersection.xy() - pBullet->GetFlyDir() * MAX(pBullet->GetCollisionRadius() * 2.0f, pBullet->GetSpeed() * TIME)) - pEnemy->GetPosition().xy();
                            pBullet->Reflect(pEnemy, vIntersection.xy(), vDiff.Normalized());

                            // 
                            this->PlayReflectSound(vIntersection);
                        }
                    }
                }
            }
        }
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_ITEM, TYPE_BULLET_PLAYER, [this](const cItem* pItem, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(m_bVisibleCheck && !g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

        if(bFirstHit)
        {
            if(pBullet->HasStatus(BULLET_STATUS_ACTIVE))
            {
                // 
                pBullet->Reflect(pItem, vIntersection.xy());

                // 
                this->PlayReflectSound(vIntersection);
            }
        }
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_ALL, TYPE_ITEM, [](cPlayer* OUTPUT pPlayer, cItem* OUTPUT pItem, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(pItem->GetLifeTime() < 1.0f) return;

        // 
        pItem->Collect(pPlayer);
    });
}


// ****************************************************************
// 
void cGame::__ClearAll(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Kill(bAnimated);

    // 
    for(coreUintW i = 0u; i < GAME_HELPERS; ++i)
        m_aHelper[i].Kill(bAnimated);

    // 
    m_Tracker.Kill(bAnimated);

    // 
    m_EnemyManager       .ClearEnemies(bAnimated);
    m_BulletManagerPlayer.ClearBullets(bAnimated);
    m_BulletManagerEnemy .ClearBullets(bAnimated);
    m_ItemManager        .ClearItems  (bAnimated);
    m_ShieldManager      .ClearShields(bAnimated);
    m_CrashManager       .ClearCrashes(bAnimated);
    
    m_BulletManagerPlayerTop.ClearBullets(bAnimated);

    // 
    this->HideHelpers();

    // 
    if(m_pRepairEnemy->GetPlayer())
    {
        if(bAnimated) g_pSpecialEffects->MacroExplosionDarkSmall(m_pRepairEnemy->GetPosition());
        m_pRepairEnemy->Kill(false);
    }
}