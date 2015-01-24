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
cBullet::cBullet()noexcept
{
    // set initial status
    m_iStatus = BULLET_STATUS_READY;
}


// ****************************************************************
// destructor
cBullet::~cBullet()
{
}


// ****************************************************************
// render and move the bullet
void cBullet::Render()
{
    if(!(m_iStatus & BULLET_STATUS_ACTIVE)) return;

    // render the 3d-object
    coreObject3D::Render();
}


// ****************************************************************
// render and move the bullet
void cBullet::Move()
{
    if(!(m_iStatus & BULLET_STATUS_ACTIVE)) return;


    this->SetPosition(this->GetPosition() + this->GetDirection() * Core::System->GetTime() * 10.0f);


    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// prepare and start up bullet
void cBullet::Activate(const int& iType, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // activate bullet and remove readiness
    if(m_iStatus & BULLET_STATUS_ACTIVE) return;
    m_iStatus |=  BULLET_STATUS_ACTIVE;
    m_iStatus &= ~BULLET_STATUS_READY;

    // reset bullet properties
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));

    // enable collision
    this->ChangeType(iType);
}


// ****************************************************************
// shut down bullet
void cBullet::Deactivate(const bool& bAnimated)
{
    // deactivate bullet (will be cleaned up by bullet manager)
    if(!(m_iStatus & BULLET_STATUS_ACTIVE)) return;
    m_iStatus &= ~BULLET_STATUS_ACTIVE;

    // disable collision
    this->ChangeType(0);
}


// ****************************************************************
// constructor
cBulletManager::sBulletSetGen::sBulletSetGen()noexcept
: oBulletActive (BULLET_SET_INIT_SIZE)
{
}


// ****************************************************************
// constructor
cBulletManager::cBulletManager()noexcept
{
}


// ****************************************************************
// destructor
cBulletManager::~cBulletManager()
{
    // delete all bullet sets
    FOR_EACH(it, m_apBulletSet)
        SAFE_DELETE(it->second)

    // clear memory
    m_apBulletSet.clear();
}


// ****************************************************************
// render the bullet mangager
void cBulletManager::Render()
{
    // render all bullet sets
    FOR_EACH(it, m_apBulletSet)
        it->second->oBulletActive.Render();
}


// ****************************************************************
// move the bullet manager
void cBulletManager::Move()
{
    // loop through all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &it->second->oBulletActive;

        // move the bullet set
        pBulletActive->MoveNormal();

        // loop through all bullets
        FOR_EACH_DYN(et, *pBulletActive->List())
        {
            coreObject3D* pBullet = (*et);

            // check current bullet status
            if(!(pBullet->GetStatus() & BULLET_STATUS_ACTIVE))
            {
                // clean up bullet and make ready again
                pBullet->SetStatus(pBullet->GetStatus() & BULLET_STATUS_READY);
                DYN_REMOVE(et, *pBulletActive->List())
            }
            else DYN_KEEP(et)
        }
    }
}


// ****************************************************************
// remove all bullets
void cBulletManager::ClearBullets()
{
    // loop trough all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &it->second->oBulletActive;

        // deactivate each active bullet
        FOR_EACH(et, *pBulletActive->List())
            s_cast<cBullet*>(*et)->Deactivate(true);

        // clear list
        pBulletActive->Clear();
    }
}


// ****************************************************************
// constructor
cRay::cRay()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_ray.md3");
    this->DefineProgram("effect_energy_program");

    // set color value
    this->SetColor3(coreVector3(0.9f,0.8f,0.4f));
}


// ****************************************************************
// destructor
cRay::~cRay()
{
}


// ****************************************************************
// constructor
cOrb::cOrb()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineProgram("effect_energy_program");

    // set color value
    this->SetColor3(coreVector3(0.09f,0.387f,0.9f));
}


// ****************************************************************
// destructor
cOrb::~cOrb()
{
}