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
// constructor
cTheater::cTheater()noexcept
: m_fSpawnTimer (0.0f)
, m_iSpawnCount (0u)
, m_fCurRow     (0.0f)
, m_bCurSide    (false)
, m_iCurType    (0u)
{
}


// ****************************************************************
// destructor
cTheater::~cTheater()
{
    // 
    this->ClearMutes(false);
}


// ****************************************************************
// render the theater
void cTheater::Render()
{
    // 
    FOR_EACH(it, m_aMute)
        it->pEnemy->Render();

    if(!STATIC_ISVALID(g_pGame))
    {
        // apply deferred outline-layer
        g_pOutline->Apply();

        // render special-effects
        g_pSpecialEffects->Render(true);
    }
}


// ****************************************************************
// move the theater
void cTheater::Move()
{
    // 
    if(!STATIC_ISVALID(g_pGame) && (g_pEnvironment->GetBackground()->GetID() != cCloudBackground::ID))
    {
        // 
        m_fSpawnTimer.Update(1.0f);
        if(m_fSpawnTimer >= THEATER_SPAWN_TIME)
        {
            // 
            if(m_iSpawnCount == 0)
            {
                m_fCurRow  = Core::Rand->Float(-0.8f,0.8f);
                m_bCurSide = !m_bCurSide;
            }

            // 
            if(++m_iSpawnCount >= THEATER_SPAWN_COUNT + Core::Rand->Int(THEATER_SPAWN_COUNT_RAND))
            {
                m_fSpawnTimer -= THEATER_SPAWN_TIME - Core::Rand->Float(THEATER_SPAWN_TIME_RAND);
                m_iSpawnCount  = 0;
            }
            else
            {
                m_fSpawnTimer -= Core::Rand->Float(0.26f,0.36f);
            }

            // 
            m_iCurType = (m_iCurType - 1u + Core::Rand->Int(1,6)) % 7u + 1u;

            // 
            const coreFloat   fSide = m_bCurSide ? -1.0f : 1.0f;
            const coreFloat   fRow  = m_fCurRow + Core::Rand->Float(-0.2f,0.2f);
            const coreVector3 vPos  = coreVector3(fSide * -0.95f * THEATER_AREA_FACTOR * FOREGROUND_AREA.x, fRow * FOREGROUND_AREA.y, 0.0f);
            const coreVector2 vDir  = coreVector2(fSide, 0.0f);

            // 
            switch(m_iCurType)
            {
            default: ASSERT(false)
            case cScoutEnemy  ::ID: this->AddMute<cScoutEnemy>  (vPos, vDir, COLOR_SHIP_BLUE);   break;
            case cWarriorEnemy::ID: this->AddMute<cWarriorEnemy>(vPos, vDir, COLOR_SHIP_YELLOW); break;
            case cStarEnemy   ::ID: this->AddMute<cStarEnemy>   (vPos, vDir, COLOR_SHIP_RED);    break;
            case cArrowEnemy  ::ID: this->AddMute<cArrowEnemy>  (vPos, vDir, COLOR_SHIP_ORANGE); break;
            case cMinerEnemy  ::ID: this->AddMute<cMinerEnemy>  (vPos, vDir, COLOR_SHIP_CYAN);   break;
            case cFreezerEnemy::ID: this->AddMute<cFreezerEnemy>(vPos, vDir, COLOR_SHIP_ICE);    break;
            case cCinderEnemy ::ID: this->AddMute<cCinderEnemy> (vPos, vDir, COLOR_SHIP_GREY);   break;
            }
        }
    }
    else m_fSpawnTimer = 0.0f;

    // 
    const coreBool    bInteract = (Core::Input->GetLastMouse() != CORE_INPUT_INVALID_MOUSE);
    const coreVector2 vMousePos = MapToAxis((Core::Input->GetMousePosition() * Core::System->GetResolution() / g_vGameResolution) * (FOREGROUND_AREA * 2.2f), g_pPostProcessing->GetDirection());

    // 
    FOR_EACH_DYN(it, m_aMute)
    {
        cEnemy*            pEnemy = it->pEnemy;
        const coreVector3& vPos   = pEnemy->GetPosition();
        const coreVector2& vDir   = it->vDirection;

        // 
        pEnemy->SetPosition(coreVector3(vPos.xy() + vDir * (35.0f * Core::System->GetTime()), vPos.z));

        // 
        if(!g_pForeground->IsVisiblePoint(vPos.xy(), THEATER_AREA_FACTOR))
        {
            this->__KillMute(&(*it), false);
            DYN_REMOVE(it, m_aMute)
        }
        else if(bInteract && ((vMousePos - vPos.xy()).LengthSq() < POW2(pEnemy->GetCollisionRadius() * 1.1f)))
        {
            this->__KillMute(&(*it), true);
            DYN_REMOVE(it, m_aMute)
        }
        else
        {
            pEnemy->Move();
            DYN_KEEP(it)
        }
    }
}


// ****************************************************************
// 
void cTheater::ClearMutes(const coreBool bAnimated)
{
    // 
    FOR_EACH(it, m_aMute)
        this->__KillMute(&(*it), bAnimated);

    // 
    m_aMute.clear();
}


// ****************************************************************
// 
void cTheater::__KillMute(sMute* OUTPUT pMute, const coreBool bAnimated)
{
    cEnemy* pEnemy = pMute->pEnemy;

    // 
    pEnemy->SetBaseColor(coreVector3(0.0f,0.0f,0.0f), true);   // # always use colored explosions
    pEnemy->Kill(bAnimated);
    SAFE_DELETE(pEnemy)
}