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
: m_iDamage (0)
, m_pOwner  (NULL)
{
    // set initial status
    m_iStatus = BULLET_STATUS_READY;
}


// ****************************************************************
// render and move the bullet
void cBullet::Move()
{
    // call individual move routine
    this->__MoveOwn();

    // deactivate bullet when leaving the defined area
    if((this->GetPosition().x < -FOREGROUND_AREA.x * BULLET_AREA_FACTOR) || (this->GetPosition().x > FOREGROUND_AREA.x * BULLET_AREA_FACTOR) ||
       (this->GetPosition().y < -FOREGROUND_AREA.y * BULLET_AREA_FACTOR) || (this->GetPosition().y > FOREGROUND_AREA.y * BULLET_AREA_FACTOR))
        this->Deactivate(false);

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// prepare and start up bullet
void cBullet::Activate(const int& iDamage, cShip* pOwner, const int& iType, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // activate bullet and remove readiness
    if(CONTAINS_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    ADD_VALUE   (m_iStatus, BULLET_STATUS_ACTIVE)
    REMOVE_VALUE(m_iStatus, BULLET_STATUS_READY)

    // save damage value and owner
    m_iDamage = iDamage;
    m_pOwner  = pOwner;

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
    if(!CONTAINS_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    REMOVE_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)

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
        SAFE_DELETE(*it)

    // clear memory
    m_apBulletSet.clear();
}


// ****************************************************************
// render the bullet mangager
void cBulletManager::Render()
{
    // loop through all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &(*it)->oBulletActive;

        // call individual preceding render routines
        FOR_EACH(et, *pBulletActive->List())
            s_cast<cBullet*>(*et)->__RenderOwnBefore();

        // render bullet set
        (*it)->oBulletActive.Render();

        // call individual subsequent render routines
        FOR_EACH(et, *pBulletActive->List())
            s_cast<cBullet*>(*et)->__RenderOwnAfter();
    }
}


// ****************************************************************
// move the bullet manager
void cBulletManager::Move()
{
    // loop through all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &(*it)->oBulletActive;

        // loop through all bullets
        FOR_EACH_DYN(et, *pBulletActive->List())
        {
            coreObject3D* pBullet = (*et);

            // check current bullet status
            if(!CONTAINS_VALUE(pBullet->GetStatus(), BULLET_STATUS_ACTIVE))
            {
                // clean up bullet and make ready again
                pBullet->SetStatus(pBullet->GetStatus() | BULLET_STATUS_READY);
                DYN_REMOVE(et, *pBulletActive->List())
            }
            else DYN_KEEP(et)
        }

        // move the bullet set (after deletions)
        pBulletActive->MoveNormal();
    }
}


// ****************************************************************
// remove all bullets
void cBulletManager::ClearBullets()
{
    // loop trough all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &(*it)->oBulletActive;

        // deactivate each active bullet
        FOR_EACH(et, *pBulletActive->List())
            s_cast<cBullet*>(*et)->Deactivate(true);

        // clear list
        pBulletActive->Clear();
    }
}


// ****************************************************************
// constructor
cRayBullet::cRayBullet()noexcept
: m_fAnimation (Core::Rand->Float(1.0f))
{
    // load object resources
    this->DefineModel  ("bullet_ray.md3");
    this->DefineTexture(0, "effect_energy.png");
    this->DefineProgram("effect_energy_direct_program");

    // set object properties
    this->SetSize             (coreVector3(2.5f,2.5f,  2.5f));
    this->SetCollisionModifier(coreVector3(1.0f,0.333f,1.0f));   // model with offset
    this->SetColor3           (coreVector3(0.9f,0.8f,  0.4f));
    this->SetTexSize          (coreVector2(0.4f,0.2f));

    // coreVector3(1.0f, 0.4f, 0.0f) orange
}


// ****************************************************************
// move the ray bullet
void cRayBullet::__MoveOwn()
{
    // fly around (size modified)
    this->SetPosition(this->GetPosition() + this->GetDirection() * (45.0f * this->GetSize().y * Core::System->GetTime()));

    // update texture animation
    m_fAnimation.Update(0.4f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cOrbBullet::cOrbBullet()noexcept
: m_fAnimation (Core::Rand->Float(1.0f))
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0, "effect_energy.png");
    this->DefineProgram("effect_energy_program");

    // set object properties
    this->SetSize   (coreVector3(1.6f,  1.6f,  1.6f));
    this->SetColor3 (coreVector3(0.090f,0.387f,0.900f));
    this->SetTexSize(coreVector2(0.4f,  0.4f));

    // coreVector3(0.3f, 0.7f, 0.3f)*0.8f green
    // coreVector3(0.9f, 0.25f, 0.25f)*0.8f red
    // coreVector3(1.0f, 0.3f, 0.0f) orange
    // coreVector3(0.4f, 0.15f, 1.0f) violet
}


// ****************************************************************
// move the orb bullet
void cOrbBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(this->GetPosition() + this->GetDirection() * (30.0f * Core::System->GetTime()));

    // update texture animation
    m_fAnimation.Update(-0.2f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}