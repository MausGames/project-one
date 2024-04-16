///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

cRotaCache   cBullet    ::s_RotaCache = {};
coreObject3D cMineBullet::s_Wave      = {};


// ****************************************************************
// constructor
cBullet::cBullet()noexcept
: m_iDamage    (0)
, m_fSpeed     (0.0f)
, m_pOwner     (NULL)
, m_fDepth     (0.0f)
, m_fAnimation (0.0f)
, m_iElement   (ELEMENT_NEUTRAL)
, m_fFlyTime   (0.0f)
, m_vFlyDir    (coreVector2(0.0f,0.0f))
{
    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // set initial status
    m_iStatus = BULLET_STATUS_READY;
}


// ****************************************************************
// move the bullet
void cBullet::Move()
{
    // 
    m_fFlyTime.Update(1.0f);

    // call individual move routine
    this->__MoveOwn();

    // deactivate bullet when leaving the defined area
    if(!IN_FOREGROUND_AREA(this->GetPosition(), BULLET_AREA_FACTOR) && (m_fFlyTime >= 1.0f))
        this->Deactivate(false);

    // move the 3d-object
    this->coreObject3D::Move();
}


// ****************************************************************
// prepare and start up bullet
void cBullet::Activate(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType)
{
    // activate bullet and remove readiness
    if(CONTAINS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    ADD_FLAG   (m_iStatus, BULLET_STATUS_ACTIVE)
    REMOVE_FLAG(m_iStatus, BULLET_STATUS_READY)

    // save damage, speed and owner
    m_iDamage = iDamage;
    m_fSpeed  = fSpeed * BULLET_SPEED_FACTOR;
    m_pOwner  = pOwner;

    // 
    m_fFlyTime = 0.0f;
    m_vFlyDir  = vDirection;

    // reset bullet properties
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
    this->SetAlpha    (1.0f);

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // enable collision
    this->ChangeType(iType);
}


// ****************************************************************
// shut down bullet
void cBullet::Deactivate(const coreBool bAnimated, const coreVector2& vImpact)
{
    // deactivate bullet (will be cleaned up by bullet manager)
    if(!CONTAINS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    REMOVE_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)

    // 
    if(bAnimated) this->__ImpactOwn(vImpact);

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // disable collision
    this->ChangeType(0);
}

void cBullet::Deactivate(const coreBool bAnimated)
{
    // 
    this->Deactivate(bAnimated, this->GetPosition().xy());
}


// ****************************************************************
// 
void cBullet::Reflect(const coreObject3D* pObject)
{
    // 
    this->Reflect((this->GetPosition().xy() - pObject->GetPosition().xy()).Normalized());
}

void cBullet::Reflect(const coreVector2& vNormal)
{
    // 
    m_vFlyDir = coreVector2::Reflect(m_vFlyDir, vNormal);
}


// ****************************************************************
// 
void cBullet::_EnableDepth(const coreProgramPtr& pProgram)const
{
    if(!pProgram.IsUsable()) return;

    // 
    pProgram->Enable();
    pProgram->SendUniform("u_v1Depth", m_fDepth);
}

void cBullet::_EnableDepth()const
{
    // 
    this->_EnableDepth(this->GetProgram());
}


// ****************************************************************
// constructor
cBulletManager::sBulletSetGen::sBulletSetGen()noexcept
: oBulletActive (BULLET_SET_INIT)
, iCurBullet    (0u)
{
}


// ****************************************************************
// constructor
cBulletManager::cBulletManager(const coreInt32 iType)noexcept
: m_apBulletSet {}
, m_iType       (iType)
{
    // 
    Core::Manager::Object->TestCollision(m_iType, [](coreObject3D*, coreObject3D*, coreVector3, coreBool) {});
    // TODO   
}


// ****************************************************************
// destructor
cBulletManager::~cBulletManager()
{
    // 
    m_Outline.ClearLists();

    // 
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
        SAFE_DELETE(m_apBulletSet[i])
}


// ****************************************************************
// render the bullet manager
void cBulletManager::Render()
{
    // loop through all bullet sets
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[i]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[i]->oBulletActive;

        // call individual preceding render routines
        FOR_EACH(it, *pBulletActive->List())
            d_cast<cBullet*>(*it)->__RenderOwnBefore();

        // render bullet set
        pBulletActive->Render();

        // call individual subsequent render routines
        FOR_EACH(it, *pBulletActive->List())
            d_cast<cBullet*>(*it)->__RenderOwnAfter();
    }

    // 
    m_Outline.Apply();
}


// ****************************************************************
// move the bullet manager
void cBulletManager::Move()
{
    coreVector2 vPrevPos    = coreVector2(1000.0f,1000.0f);
    coreFloat   fPrevRadius = 0.0f;
    coreFloat   fDepth      = 0.0f;

    // loop through all bullet sets
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[i]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[i]->oBulletActive;

        // loop through all bullets
        FOR_EACH_DYN(it, *pBulletActive->List())
        {
            cBullet* pBullet = d_cast<cBullet*>(*it);

            // check current bullet status
            if(!CONTAINS_FLAG(pBullet->GetStatus(), BULLET_STATUS_ACTIVE))
            {
                // clean up bullet and make ready again
                pBullet->SetStatus(pBullet->GetStatus() | BULLET_STATUS_READY);
                DYN_REMOVE(it, *pBulletActive->List())
            }
            else
            {
                // calculate properties between current and previous bullet
                const coreFloat fLengthSq   = (vPrevPos    - pBullet->GetPosition().xy()).LengthSq();
                const coreFloat fFullRadius = (fPrevRadius + pBullet->GetCollisionRadius());
                vPrevPos    = pBullet->GetPosition().xy();
                fPrevRadius = pBullet->GetCollisionRadius();

                // increase depth (half if not near each other)
                fDepth += BULLET_DEPTH_FACTOR * ((fLengthSq < POW2(fFullRadius)) ? 1.0f : 0.5f);

                // 
                pBullet->m_fDepth = fDepth;
                DYN_KEEP(it)
            }
        }

        // move the bullet set (after deletions)
        pBulletActive->MoveNormal();
    }
}


// ****************************************************************
// remove all bullets
void cBulletManager::ClearBullets(const coreBool bAnimated)
{
    // loop trough all bullet sets
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[i]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[i]->oBulletActive;

        // deactivate all active bullets
        FOR_EACH(it, *pBulletActive->List())
            d_cast<cBullet*>(*it)->Deactivate(bAnimated);
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
    this->SetTexSize          (coreVector2(0.4f,0.2f) * 0.7f);
    this->SetCollisionModifier(coreVector3(1.0f,0.333f,1.0f));   // model with offset
}


// ****************************************************************
// 
void cRayBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    if(m_iElement == ELEMENT_WHITE)
    {
        g_pSpecialEffects->CreateSplashDark(coreVector3(vImpact, 0.0f), 5.0f, 1u);
    }
    else
    {
        g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 1u, this->GetColor3());
    }
}


// ****************************************************************
// move the ray bullet
void cRayBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.Update(0.4f);
    this->SetTexOffset(coreVector2(0.35f, m_fAnimation));
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
    this->SetTexSize          (coreVector2(0.4f,0.2f));
    this->SetCollisionModifier(coreVector3(1.0f,0.333f,1.0f));   // model with offset
}


// ****************************************************************
// move the pulse bullet
void cPulseBullet::__MoveOwn()
{
    // 
    m_fSpeed += 2.5f * BULLET_SPEED_FACTOR * Core::System->GetTime();

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
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
void cOrbBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the orb bullet
void cOrbBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
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
// 
void cConeBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the cone bullet
void cConeBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
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
    this->SetTexSize(coreVector2(1.1f,0.25f) * 0.2f);
}


// ****************************************************************
// 
void cWaveBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the wave bullet
void cWaveBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.Update(0.06f);
    this->SetTexOffset(coreVector2(0.3f, m_fAnimation));
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
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
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
{
    // load object resources
    this->DefineModel  ("bullet_mine.md3");
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");

    // set object properties
    this->SetColor3(coreVector3(1.0f,0.0f,0.0f));
}


// ****************************************************************
// 
void cMineBullet::GlobalInit()
{
    // 
    s_Wave.DefineModel  (Core::Manager::Object->GetLowQuad());
    s_Wave.DefineTexture(0u, "effect_wave.png");
    s_Wave.DefineProgram("effect_decal_single_program");
}


// ****************************************************************
// 
void cMineBullet::GlobalExit()
{
    // 
    s_Wave.Undefine();
}


// ****************************************************************
// 
void cMineBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cMineBullet::__RenderOwnBefore()
{
    glDepthMask(false);
    {
        const coreFloat fValue = LERPB(0.0f, 1.0f, FRACT(m_fAnimation * 0.5f));

        // 
        s_Wave.SetPosition(this->GetPosition());
        s_Wave.SetSize    (coreVector3(coreVector2(15.0f,15.0f) * (0.25f + 0.75f*fValue), 1.0f));
        s_Wave.SetAlpha   (1.0f - fValue);
        s_Wave.Move();
        s_Wave.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// move the mine bullet
void cMineBullet::__MoveOwn()
{
    // 
    m_fSpeed = MAX(m_fSpeed * FrictionFactor(3.5f), 0.0f);

    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update animation
    m_fAnimation.Update(1.5f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation), 0.0f));
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
void cRocketBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// move the rocket bullet
void cRocketBullet::__MoveOwn()
{
    // 
    m_fSpeed += 2.0f * BULLET_SPEED_FACTOR * Core::System->GetTime();
    if(m_fSpeed > 300.0f) this->Deactivate(true);

    // 
    const cEnemy* pEnemy = g_pGame->GetEnemyManager()->FindEnemy(this->GetPosition().xy());
    if(pEnemy)
    {
        const coreVector2 vDiffNorm = (pEnemy->GetPosition().xy() - this->GetPosition().xy()).Normalized();
        const coreVector2 vNewDir   = (m_vFlyDir + vDiffNorm * (0.05f * m_fSpeed * Core::System->GetTime())).Normalized();

        m_vFlyDir = vNewDir;
    }

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.Update(10.0f);
    this->SetColor3(coreVector3(0.0f, 0.6f + 0.4f * SIN(PI*m_fAnimation), 0.0f));

    // 
    if(Core::System->GetTime()) g_pSpecialEffects->CreateSplashSmoke(this->GetPosition() - this->GetDirection() * 4.5f, 5.0f, 1u, coreVector3(1.0f,1.0f,1.0f));
}


// ****************************************************************
// constructor
cSpearBullet::cSpearBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_spear.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetTexSize(coreVector2(0.5f,0.2f));
}


// ****************************************************************
// 
void cSpearBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the spear bullet
void cSpearBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.Update(0.2f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cTriangleBullet::cTriangleBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_triangle.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetTexSize(coreVector2(0.5f,0.2f));
}


// ****************************************************************
// 
void cTriangleBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the triangle bullet
void cTriangleBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update animation
    m_fAnimation.Update(0.2f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * 10.0f), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cFlipBullet::cFlipBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_spear.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");

    // set object properties
    this->SetTexSize(coreVector2(0.4f,0.2f));
}


// ****************************************************************
// 
void cFlipBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the flip bullet
void cFlipBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update animation
    m_fAnimation.Update(-0.2f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * 50.0f), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cQuadBullet::cQuadBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_quad.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetTexSize(coreVector2(0.5f,0.2f));
}


// ****************************************************************
// 
void cQuadBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the quad bullet
void cQuadBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update animation
    m_fAnimation.Update(0.2f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * 10.0f), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}


// ****************************************************************
// constructor
cChromaBullet::cChromaBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_quad.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetTexSize(coreVector2(0.5f,0.2f));
}


// ****************************************************************
// 
void cChromaBullet::__ImpactOwn(const coreVector2& vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cChromaBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + m_vFlyDir * (m_fSpeed * Core::System->GetTime()), 0.0f));

    // update animation
    m_fAnimation.Update(0.2f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * 10.0f), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));
}