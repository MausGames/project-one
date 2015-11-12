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
: m_iDamage    (0)
, m_fSpeed     (0.0f)
, m_pOwner     (NULL)
, m_fAnimation (0.0f)
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
void cBullet::Activate(const coreInt32& iDamage, const coreFloat& fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32& iType)
{
    // activate bullet and remove readiness
    if(CONTAINS_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    ADD_VALUE   (m_iStatus, BULLET_STATUS_ACTIVE)
    REMOVE_VALUE(m_iStatus, BULLET_STATUS_READY)

    // save damage, speed and owner
    m_iDamage = iDamage;
    m_fSpeed  = fSpeed * BULLET_SPEED_FACTOR;
    m_pOwner  = pOwner;

    // reset animation value
    m_fAnimation = Core::Rand->Float(1.0f);

    // reset bullet properties
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
    this->SetAlpha    (1.0f);

    // enable collision
    this->ChangeType(iType);
}


// ****************************************************************
// shut down bullet
void cBullet::Deactivate(const coreBool& bAnimated)
{
    // deactivate bullet (will be cleaned up by bullet manager)
    if(!CONTAINS_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    REMOVE_VALUE(m_iStatus, BULLET_STATUS_ACTIVE)

    // 
    if(bAnimated) this->__ImpactOwn();

    // disable collision
    this->ChangeType(0);
}


// ****************************************************************
// constructor
cBulletManager::sBulletSetGen::sBulletSetGen()noexcept
: oBulletActive (BULLET_SET_INIT_SIZE)
, bShadow       (false)
{
}


// ****************************************************************
// constructor
cBulletManager::cBulletManager(const coreInt32& iType)noexcept
: m_iType     ((iType))
, m_iPriority ((iType == TYPE_BULLET_PLAYER) ? PRIO_PLAYER : PRIO_ENEMY)
{
}


// ****************************************************************
// destructor
cBulletManager::~cBulletManager()
{
    // 
    g_aaOutline[m_iPriority][STYLE_FULL]  .ClearLists();
    g_aaOutline[m_iPriority][STYLE_DIRECT].ClearLists();

    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &(*it)->oBulletActive;

        // remove bullet set from glow
        g_pGlow->UnbindList(pBulletActive);

        // 
        if((*it)->bShadow) cShadow::GetGlobalContainer()->UnbindList(pBulletActive);

        // delete bullet set
        SAFE_DELETE(*it)
    }

    // clear memory
    m_apBulletSet.clear();
}


// ****************************************************************
// render the bullet manager
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
void cBulletManager::ClearBullets(const coreBool& bAnimated)
{
    // loop trough all bullet sets
    FOR_EACH(it, m_apBulletSet)
    {
        coreBatchList* pBulletActive = &(*it)->oBulletActive;

        // deactivate each active bullet
        FOR_EACH(et, *pBulletActive->List())
            s_cast<cBullet*>(*et)->Deactivate(bAnimated);

        // clear list
        pBulletActive->Clear();
    }
}


// ****************************************************************
// constructor
cRayBullet::cRayBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_ray.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetCollisionModifier(coreVector3(1.0f,0.333f,1.0f));   // model with offset
    this->SetTexSize          (coreVector2(0.4f,0.2f));
}


// ****************************************************************
// move the ray bullet
void cRayBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(0.4f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cPulseBullet::cPulseBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_ray.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetCollisionModifier(coreVector3(1.0f,0.333f,1.0f));   // model with offset
    this->SetTexSize          (coreVector2(0.4f,0.2f));
}


// ****************************************************************
// move the pulse bullet
void cPulseBullet::__MoveOwn()
{
    // 
    m_fSpeed += 2.5f * BULLET_SPEED_FACTOR * Core::System->GetTime();

    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(0.4f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cOrbBullet::cOrbBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");

    // set object properties
    this->SetTexSize(coreVector2(0.4f,0.4f));
}


// ****************************************************************
// 
void cOrbBullet::__ImpactOwn()
{
    // 
    g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the orb bullet
void cOrbBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(-0.2f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cConeBullet::cConeBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_cone.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");

    // set object properties
    this->SetTexSize(coreVector2(0.5f,0.2f));
}


// ****************************************************************
// move the cone bullet
void cConeBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(0.2f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cWaveBullet::cWaveBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_wave.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetTexSize(coreVector2(0.1f,0.25f));
}


// ****************************************************************
// move the wave bullet
void cWaveBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(0.15f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cTeslaBullet::cTeslaBullet()noexcept
: m_fLightningTime (1.0f)
, m_bLightningSide (false)
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_spheric_program");

    // set object properties
    this->SetTexSize(coreVector2(0.45f,0.45f));
}


// ****************************************************************
// move the tesla bullet
void cTeslaBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update texture animation
    m_fAnimation.Update(-0.2f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // 
    m_fLightningTime.Update(10.0f);
    if(m_fLightningTime >= 1.0f)
    {
        m_fLightningTime -= 1.0f;

        // 
        m_bLightningSide = !m_bLightningSide;
        coreVector2 vDir = coreVector2::Rand();
                    vDir = coreVector2(m_bLightningSide ? ABS(vDir.x) : -ABS(vDir.x), vDir.y);

        // 
        g_pSpecialEffects->CreateLightning(this, vDir, 7.0f, SPECIAL_LIGHTNING_SMALL, coreVector3(1.0f,1.0f,1.0f), coreVector2(1.0f,1.0f), 0.0f);
    }
}


// ****************************************************************
// constructor
cMineBullet::cMineBullet()noexcept
: m_vFlyDir (coreVector2(0.0f,0.0f))
{
    // load object resources
    this->DefineModel  ("bullet_mine.md3");
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");

    // set object properties
    this->SetColor3(coreVector3(1.0f,0.0f,0.0f));



    m_Wave.DefineModel  (Core::Manager::Object->GetLowModel()); 
    m_Wave.DefineTexture(0u, "effect_wave.png"); 
    m_Wave.DefineProgram("effect_decal_program"); 
}


// ****************************************************************
// 
void cMineBullet::__ImpactOwn()
{
    // 
    g_pSpecialEffects->MacroExplosionPhysicalSmall(this->GetPosition());
}


// ****************************************************************
// 
void cMineBullet::__RenderOwnBefore()
{
    glDepthMask(false);

    const coreFloat fValue = LERPB(0.0f, 1.0f, FRACT(m_fAnimation * 0.5f));  

    m_Wave.SetPosition(this->GetPosition());    
    m_Wave.SetSize    (coreVector3(coreVector2(15.0f,15.0f) * (0.25f+0.75f*fValue), 1.0f));   
    m_Wave.SetAlpha(1.0f - fValue);
    m_Wave.Move();   
    m_Wave.Render();   

    glDepthMask(true);
}


// ****************************************************************
// move the mine bullet
void cMineBullet::__MoveOwn()
{
    // 
    //m_fSpeed = MAX(m_fSpeed - 2.0f * BULLET_SPEED_FACTOR * Core::System->GetTime(), 0.0f);  
    m_fSpeed = MAX(m_fSpeed * (1.0f - 3.5f*Core::System->GetTime()), 0.0f);

    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // 
    m_fAnimation.Update(1.5f);
    this->SetDirection(coreVector3(coreVector2::Direction(m_fAnimation), 0.0f));
    this->SetColor3   (coreVector3(0.5f + 0.5f * SIN(PI*m_fAnimation), 0.0f, 0.0f));
}


// ****************************************************************
// constructor
cRocketBullet::cRocketBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_rocket.md3");
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");

    // set object properties
    this->SetColor3(coreVector3(0.0f,1.0f,0.0f));
}


// ****************************************************************
// 
void cRocketBullet::__ImpactOwn()
{
    // 
    g_pSpecialEffects->MacroExplosionPhysicalSmall(this->GetPosition());
}


// ****************************************************************
// move the rocket bullet
void cRocketBullet::__MoveOwn()
{

    m_fSpeed += 2.0f * BULLET_SPEED_FACTOR * Core::System->GetTime();
    if(m_fSpeed > 300.0f) this->Deactivate(true);


    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetDirection().xy() * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // 
    m_fAnimation.Update(10.0f);

    g_pSpecialEffects->CreateSplashSmoke(this->GetPosition() - this->GetDirection() * 4.5f, 5.0f, 1u);

   // static coreUint8 iHalf = 0u;
   // if(++iHalf >= 3u) iHalf = 0u;
   // if(!iHalf) g_pSpecialEffects->CreateSplashColor(this->GetPosition() - this->GetDirection() * 4.5f, 25.0f, 1u, coreVector3(0.5f,0.5f,0.5f));


    /**/
    const cEnemy* pEnemy = g_pGame->GetEnemyManager()->FindEnemy(this->GetPosition().xy());

    if(pEnemy)
    {
        const coreVector2 vDiffNorm = (pEnemy->GetPosition().xy() - this->GetPosition().xy()).Normalize();
        const coreVector2 vNewDir   = (this->GetDirection().xy() + vDiffNorm * (0.05f * m_fSpeed * Core::System->GetTime())).Normalize();
    
        this->SetDirection(coreVector3(vNewDir, 0.0f));/**/
    }
    this->SetColor3   (coreVector3(0.0f, 0.6f + 0.4f * SIN(PI*m_fAnimation), 0.0f));
    
}