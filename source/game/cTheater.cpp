//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cTheater::cTheater()noexcept
: m_fSpawnTimer (0.0f)
, m_iCurType    (0u)
, m_bCurSide    (false)
{
}


// ****************************************************************
// render the theater
void cTheater::Render()
{
    // 
    FOR_EACH(it, m_aMute)
        it->pEnemy->Render();
}


// ****************************************************************
// move the theater
void cTheater::Move()
{
    // 
    if(!g_pGame && (g_pEnvironment->GetBackground()->GetID() != cCloudBackground::ID))
    {
        // 
        m_fSpawnTimer.Update(1.0f);
        if(m_fSpawnTimer >= THEATER_SPAWN_TIME)
        {
            m_fSpawnTimer -= THEATER_SPAWN_TIME - Core::Rand->Float(THEATER_SPAWN_RAND);

            // 
            m_iCurType = (m_iCurType - 1u + Core::Rand->Int(1,6)) % 7u + 1u;
            m_bCurSide = !m_bCurSide;

            // 
            const coreVector3 vPos = coreVector3(Core::Rand->Float(0.2f,0.9f) * (m_bCurSide ? 1.0f : -1.0f) * FOREGROUND_AREA.x, 0.95f * THEATER_AREA_FACTOR * FOREGROUND_AREA.y, 0.0f);
            const coreVector2 vDir = coreVector2(0.0f,-1.0f);

            // 
            switch(m_iCurType)
            {
            default: ASSERT(false)
            case cScoutEnemy  ::ID: this->AddMute<cScoutEnemy>  (vPos, vDir); break;
            case cWarriorEnemy::ID: this->AddMute<cWarriorEnemy>(vPos, vDir); break;
            case cStarEnemy   ::ID: this->AddMute<cStarEnemy>   (vPos, vDir); break;
            case cArrowEnemy  ::ID: this->AddMute<cArrowEnemy>  (vPos, vDir); break;
            case cMinerEnemy  ::ID: this->AddMute<cMinerEnemy>  (vPos, vDir); break;
            case cFreezerEnemy::ID: this->AddMute<cFreezerEnemy>(vPos, vDir); break;
            case cCinderEnemy ::ID: this->AddMute<cCinderEnemy> (vPos, vDir); break;
            }
        }
    }
    else m_fSpawnTimer = 0.0f;

    // 
    FOR_EACH_DYN(it, m_aMute)
    {
        cEnemy*            pEnemy = it->pEnemy;;
        const coreVector3& vPos   = pEnemy->GetPosition();
        const coreVector2& vDir   = it->vDirection;

        // 
        pEnemy->SetPosition(coreVector3(vPos.xy() + vDir * (45.0f * Core::System->GetTime()), vPos.z));

        // 
        if((pEnemy->GetPosition().x < -FOREGROUND_AREA.x * THEATER_AREA_FACTOR) ||
           (pEnemy->GetPosition().x >  FOREGROUND_AREA.x * THEATER_AREA_FACTOR) ||
           (pEnemy->GetPosition().y < -FOREGROUND_AREA.y * THEATER_AREA_FACTOR) ||
           (pEnemy->GetPosition().y >  FOREGROUND_AREA.y * THEATER_AREA_FACTOR))
        {
            DYN_REMOVE(it, m_aMute)
        }
        else
        {
            pEnemy->Move();
            DYN_KEEP(it)
        }
    }
}