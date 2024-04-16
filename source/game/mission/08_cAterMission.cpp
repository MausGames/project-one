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
cAterMission::cAterMission()noexcept
: m_pInnerMission (NULL)
, m_iNextID       (-1)
, m_bTurfState    (false)
, m_bSecret       (false)
, m_iCredits      ((g_pGame->GetKind() == GAME_KIND_ALL) ? 0u : (g_pSave->GetHeader().oProgress.aiAdvance[MISSION_ATER] - 6u))
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_Eigengrau;

    // 
    this->LoadInnerMission(cNoMission::ID);
}


// ****************************************************************
// destructor
cAterMission::~cAterMission()
{
    // 
    SAFE_DELETE(m_pInnerMission)
}


// ****************************************************************
// 
void cAterMission::RequestInnerMission(const coreInt32 iID)
{
    ASSERT(m_iNextID == -1)
    m_iNextID = iID;
}


// ****************************************************************
// 
void cAterMission::LoadInnerMission(const coreInt32 iID)
{
    // hold old mission (to keep resources valid)
    const cMission* pOldMission = m_pInnerMission;

    // create new mission
    switch(iID)
    {
    default: UNREACHABLE
    case cNoMission     ::ID: m_pInnerMission = new cNoMission     (); break;
    case cViridoMission ::ID: m_pInnerMission = new cViridoMission (); break;
    case cNevoMission   ::ID: m_pInnerMission = new cNevoMission   (); break;
    case cHarenaMission ::ID: m_pInnerMission = new cHarenaMission (); break;
    case cRutilusMission::ID: m_pInnerMission = new cRutilusMission(); break;
    case cGeluMission   ::ID: m_pInnerMission = new cGeluMission   (); break;
    case cCalorMission  ::ID: m_pInnerMission = new cCalorMission  (); break;
    case cMuscusMission ::ID: m_pInnerMission = new cMuscusMission (); break;
    }

    // delete possible old mission
    SAFE_DELETE(pOldMission)

    Core::Log->Info("Inner Mission (%s) created", m_pInnerMission->GetName());
}


// ****************************************************************
// 
void cAterMission::TransformPlayers()
{
    cPlayer* pPlayer = g_pGame->GetPlayer(0u);

    // 
    pPlayer->Configure  (PLAYER_SHIP_P1);
    pPlayer->EquipWeapon(0u, cFinalWeapon::ID);
    pPlayer->EquipShield(0);
    pPlayer->SetScale   (1.5f / PLAYER_SIZE_FACTOR);
    pPlayer->SetRainbow (true);
    pPlayer->AddStatus  (PLAYER_STATUS_KEEP_RANGE);

    if(g_pGame->IsMulti())
    {
        cPlayer* pOther = g_pGame->GetPlayer(1u);

        // 
        pOther->EquipWeapon(0u, cFinalWeapon::ID);
        pOther->EquipShield(0);
        pOther->SetScale   (1.5f / PLAYER_SIZE_FACTOR);
        pOther->SetRainbow (true);
        pOther->AddStatus  (PLAYER_STATUS_KEEP_RANGE);
    }

    // 
    g_pGame->GetInterface()->UpdateEnabled();

    STATIC_ASSERT(GAME_PLAYERS == 2u)
}


// ****************************************************************
// 
void cAterMission::__RenderOwnBottom()
{
    if(m_pInnerMission) m_pInnerMission->RenderBottom();

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Turf.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cAterMission::__RenderOwnUnder()
{
    if(m_pInnerMission) m_pInnerMission->RenderUnder();
}


// ****************************************************************
// 
void cAterMission::__RenderOwnOver()
{
    if(m_pInnerMission) m_pInnerMission->RenderOver();
}


// ****************************************************************
// 
void cAterMission::__RenderOwnTop()
{
    if(m_pInnerMission) m_pInnerMission->RenderTop();
}


// ****************************************************************
// 
void cAterMission::__MoveOwnBefore()
{
    if(m_pInnerMission) m_pInnerMission->MoveBefore();
}


// ****************************************************************
// 
void cAterMission::__MoveOwnMiddle()
{
    if(m_pInnerMission) m_pInnerMission->MoveMiddle();
}


// ****************************************************************
// 
void cAterMission::__MoveOwnAfter()
{
    if(m_pInnerMission) m_pInnerMission->MoveAfter();

    if(m_iNextID > -1)
    {
        this->LoadInnerMission(m_iNextID);
        m_iNextID = -1;
    }

    if(m_Turf.IsActive())
    {
        const coreFloat fOld = m_Turf.CalcPercent();

        g_pGame->GetBulletManagerPlayer()->ForEachBullet([this](const cBullet* pBullet)
        {
            m_Turf.DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, SNOW_TYPE_ADD);
            m_Turf.DrawPoint(pBullet->GetPosition().xy(),                                4.0f, SNOW_TYPE_ADD);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBullet([this](const cBullet* pBullet)
        {
            if(pBullet->GetID() != cRayBullet::ID) return;

            m_Turf.DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, SNOW_TYPE_REMOVE);
            m_Turf.DrawPoint(pBullet->GetPosition().xy(),                                4.0f, SNOW_TYPE_REMOVE);
        });

        const coreFloat fNew = m_Turf.CalcPercent();

        if(m_bTurfState)
        {
            if((fOld > 0.1f) && (fNew <= 0.1f))
            {
                m_Turf.DrawAll(SNOW_TYPE_REMOVE);
                m_Turf.Flash();

                m_bTurfState = false;
            }
        }
        else
        {
            if((fOld < 0.5f) && (fNew >= 0.5f))
            {
                m_Turf.DrawAll(SNOW_TYPE_ADD);
                m_Turf.Flash();

                m_bTurfState = true;
            }
        }
    }

    // 
    m_Turf.Move();
}