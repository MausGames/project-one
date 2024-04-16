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
, m_fFade      (0.0f)
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
    if((m_fFlyTime >= 0.5f) && !HAS_FLAG(m_iStatus, BULLET_STATUS_IMMORTAL) && !g_pForeground->IsVisibleObject(this))
        this->Deactivate(false);

    // move the 3d-object
    this->coreObject3D::Move();
}


// ****************************************************************
// prepare and start up bullet
void cBullet::Activate(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection, const coreInt32 iType)
{
    // activate bullet and remove readiness
    if(HAS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    m_iStatus = BULLET_STATUS_ACTIVE;   // # reset status

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
void cBullet::Deactivate(const coreBool bAnimated, const coreVector2 vImpact)
{
    // deactivate bullet (will be cleaned up by bullet manager)
    if(!HAS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    REMOVE_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)

    // 
    if(bAnimated && (!HAS_FLAG(m_iStatus, BULLET_STATUS_IMMORTAL) || g_pForeground->IsVisibleObject(this)))
        this->__ImpactOwn(vImpact);

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
void cBullet::Reflect(const coreObject3D* pObject, const coreVector2 vIntersection, const coreVector2 vForceNormal)
{
    if(coreVector2::Dot(m_vFlyDir, vForceNormal) >= 0.0f) return;
    this->__Reflect(pObject, vIntersection, vForceNormal, 0.0f);
}

void cBullet::Reflect(const coreObject3D* pObject, const coreVector2 vIntersection, const coreFloat fSharpness)
{
    if(coreVector2::Dot(m_vFlyDir, this->GetPosition().xy() - vIntersection - m_vFlyDir * MAX(this->GetCollisionRadius(), m_fSpeed * TIME)) >= 0.0f) return;
    this->__Reflect(pObject, vIntersection, coreVector2(0.0f,0.0f), fSharpness);
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
// 
void cBullet::__Reflect(const coreObject3D* pObject, const coreVector2 vIntersection, const coreVector2 vForceNormal, const coreFloat fSharpness)
{
    ASSERT(pObject )//&& pObject->GetVolume()->GetNumClusters())

    // increase intersection precision
    coreVector2 vHit = vIntersection;
    {
        coreFloat fHitDistance = 0.0f;
        coreUint8 iHitCount    = 1u;

        // shoot ray into fly direction
        const coreVector2 vRayPos = vHit - m_vFlyDir * MAX(this->GetCollisionRadius() * 2.0f, m_fSpeed * TIME);
        if(Core::Manager::Object->TestCollision(pObject, coreVector3(vRayPos, 0.0f), coreVector3(m_vFlyDir, 0.0f), &fHitDistance, &iHitCount))
        {
            vHit = vRayPos + m_vFlyDir * fHitDistance;
        }
        else
        {
            // move ray further towards object
            const coreVector2 vTowardsDir = (pObject->GetPosition().xy() - vHit).Normalized();
            if(Core::Manager::Object->TestCollision(pObject, coreVector3(vHit, 0.0f), coreVector3(vTowardsDir, 0.0f), &fHitDistance, &iHitCount))
            {
                // shoot ray again into fly direction
                const coreVector2 vNewRayPos = vRayPos + vTowardsDir * fHitDistance;
                if(Core::Manager::Object->TestCollision(pObject, coreVector3(vNewRayPos, 0.0f), coreVector3(m_vFlyDir, 0.0f), &fHitDistance, &(iHitCount = 1)))   // reset
                {
                    vHit = vNewRayPos + m_vFlyDir * fHitDistance;
                }
                else
                {
                    vHit = vHit + vTowardsDir * fHitDistance;
                }
            }
        }
    }

    // calculate projected intersection (for error correction)
    const coreVector2 vHitDiff = vHit - this->GetPosition().xy();
    const coreFloat   fHitProj = coreVector2::Dot(vHitDiff, m_vFlyDir);
    const coreVector2 vPeak    = this->GetPosition().xy() + m_vFlyDir * fHitProj;

    // calculate reflection normal (approximation, sharp)
    const coreVector2 vNormal = vForceNormal.IsNull() ? ((vPeak - 0.0f*m_vFlyDir * fSharpness) - pObject->GetPosition().xy()).Normalized(-m_vFlyDir) : vForceNormal;
    if(coreVector2::Dot(m_vFlyDir, vNormal) >= 0.0f) return;

    // reflect bullet
    ASSERT(vNormal.IsNormalized())
    m_vFlyDir = vNormal;//coreVector2::Reflect(m_vFlyDir, vNormal);

    // set corrected position
    this->SetPosition(coreVector3(vPeak - m_vFlyDir * fHitProj, 0.0f));

    // call individual reflect routine
    this->__ReflectOwn();

    // move the 3d-object
    this->coreObject3D::Move();   // for direction (and other) changes
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
, m_aiOrder     {}
{
    // 
    this->ResetOrder();
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
        if(!m_apBulletSet[m_aiOrder[i]]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[m_aiOrder[i]]->oBulletActive;

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
    coreVector2 vPrevPos    = HIDDEN_POS;
    coreFloat   fPrevRadius = 0.0f;
    coreFloat   fDepth      = 0.0f;

    // loop through all bullet sets
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[m_aiOrder[i]]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[m_aiOrder[i]]->oBulletActive;

        // loop through all bullets
        FOR_EACH_DYN(it, *pBulletActive->List())
        {
            cBullet* pBullet = d_cast<cBullet*>(*it);

            // check current bullet status
            if(!pBullet->HasStatus(BULLET_STATUS_ACTIVE))
            {
                // clean up bullet and make ready again
                pBullet->AddStatus(BULLET_STATUS_READY);
                DYN_REMOVE(it, *pBulletActive->List())
            }
            else
            {
                // calculate properties between current and previous bullet
                const coreFloat fLengthSq   = (vPrevPos    - pBullet->GetPosition().xy()).LengthSq();
                const coreFloat fFullRadius = (fPrevRadius + pBullet->GetVisualRadius() + 2.0f * OUTLINE_THICKNESS);
                vPrevPos    = pBullet->GetPosition().xy();
                fPrevRadius = pBullet->GetVisualRadius();

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

    // 
    this->ResetOrder();
}


// ****************************************************************
// 
void cBulletManager::OverrideOrder(const coreUint8* piNewOrder, const coreUintW iSize)
{
    ASSERT(piNewOrder && (iSize < BULLET_SET_COUNT))

#if defined(_CORE_DEBUG_)

    // 
    for(coreUintW i = 0u; i < iSize; ++i)
        for(coreUintW j = i+1u; j < iSize; ++j)
            ASSERT((piNewOrder[i] != piNewOrder[j]) &&
                   (piNewOrder[i] < BULLET_SET_COUNT))

#endif

    // 
    for(coreUintW i = 0u, j = 0u; i < BULLET_SET_COUNT; ++i)
        if(!std::memchr(piNewOrder, i, iSize)) m_aiOrder[j++] = i;

    // 
    for(coreUintW i = 0u; i < iSize; ++i)
        m_aiOrder[BULLET_SET_COUNT - 1u - i] = piNewOrder[i];

    STATIC_ASSERT(sizeof(*piNewOrder) == 1u)
}


// ****************************************************************
// 
void cBulletManager::ResetOrder()
{
    // 
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
        m_aiOrder[i] = i;
}


// ****************************************************************
// constructor
cRayBullet::cRayBullet()noexcept
: m_fScale (1.0f)
{
    // load object resources
    this->DefineModel  ("bullet_ray.md3");
    this->DefineVolume ("bullet_ray_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cRayBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 1u, this->GetColor3());
}


// ****************************************************************
// 
void cRayBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, 0.0f));   // move to tip, to account for resetting the length
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // 
    m_fFade = 0.0f;
    this->SetSize (coreVector3(0.0f,0.0f,0.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// move the ray bullet
void cRayBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.4f, 1.0f);
    this->SetTexOffset(coreVector2(0.35f, m_fAnimation));

    // update fade
    m_fFade.Update(1.5f);
    this->SetSize (coreVector3(3.7f, 3.7f * MIN(12.0f * m_fFade, 1.0f)     * (m_fSpeed / (6.0f*BULLET_SPEED_FACTOR)), 3.7f) * 0.5f * m_fScale);
    this->SetAlpha(MIN(15.0f * m_fFade, 1.0f));
}


// ****************************************************************
// constructor
cPulseBullet::cPulseBullet()noexcept
: m_fScale (1.0f)
{
    // load object resources
    this->DefineModel  ("bullet_pulse.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cPulseBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cPulseBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, 0.0f));   // move to tip, to account for resetting the length
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // 
    m_fFade = 0.0f;
    this->SetSize (coreVector3(0.0f,0.0f,0.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// move the pulse bullet
void cPulseBullet::__MoveOwn()
{
    // 
    m_fSpeed += 2.5f * BULLET_SPEED_FACTOR * TIME;

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.4f, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetSize (coreVector3(1.0f, 2.5f * MIN(12.0f * m_fFade, 1.0f)     * (m_fSpeed / (3.0f*BULLET_SPEED_FACTOR)), 1.0f) * 1.3f * m_fScale);
    this->SetAlpha(MIN(15.0f * m_fFade, 1.0f));
}


// ****************************************************************
// constructor
cSurgeBullet::cSurgeBullet()noexcept
: m_fScale (1.0f)
{
    // load object resources
    this->DefineModel  ("bullet_wave.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cSurgeBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cSurgeBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, 0.0f));   // move to tip, to account for resetting the length
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // 
    m_fFade = 0.0f;
    this->SetSize (coreVector3(0.0f,0.0f,0.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// move the surge bullet
void cSurgeBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.06f, 1.0f);
    this->SetTexOffset(coreVector2(0.3f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetSize (coreVector3(1.5f, 1.5f * MIN(12.0f * m_fFade, 1.0f), 1.5f) * 2.35f * m_fScale);
    this->SetAlpha(MIN(15.0f * m_fFade, 1.0f));
}


// ****************************************************************
// constructor
cTeslaBullet::cTeslaBullet()noexcept
: m_fScale         (1.0f)
, m_fLightningTime (1.0f)
, m_bLightningSide (false)
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_spheric_program");
}


// ****************************************************************
// 
void cTeslaBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cTeslaBullet::__ReflectOwn()
{
    // 
    m_fFade = 0.0f;
    this->SetSize (coreVector3(0.0f,0.0f,0.0f));
    this->SetAlpha(0.0f);
}


// ****************************************************************
// move the tesla bullet
void cTeslaBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(-0.2f, -1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetSize(coreVector3(2.5f,2.5f,2.5f) * MIN(12.0f * m_fFade, 1.0f) * m_fScale);
    this->SetAlpha(MIN(15.0f * m_fFade, 1.0f));

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
cOrbBullet::cOrbBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");

    // set object properties
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cOrbBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cOrbBullet::__ReflectOwn()
{
}


// ****************************************************************
// move the orb bullet
void cOrbBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cConeBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cConeBullet::__ReflectOwn()
{
    // 
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
}


// ****************************************************************
// move the cone bullet
void cConeBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cWaveBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cWaveBullet::__ReflectOwn()
{
    // 
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
}


// ****************************************************************
// move the wave bullet
void cWaveBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.06f, 1.0f);
    this->SetTexOffset(coreVector2(0.3f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
    // TODO 1: make sure the player cannot be hit by the transparent rear of the bullet (e.g. when he moves into it)
}


// ****************************************************************
// 
void cSpearBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cSpearBullet::__ReflectOwn()
{
    // 
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
}


// ****************************************************************
// move the spear bullet
void cSpearBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cTriangleBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cTriangleBullet::__ReflectOwn()
{
}


// ****************************************************************
// move the triangle bullet
void cTriangleBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 2.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (3.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cFlipBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cFlipBullet::__ReflectOwn()
{
}


// ****************************************************************
// move the flip bullet
void cFlipBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(-0.2f, -2.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (15.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cQuadBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cQuadBullet::__ReflectOwn()
{
}


// ****************************************************************
// move the quad bullet
void cQuadBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 2.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (3.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
}


// ****************************************************************
// constructor
cViewBullet::cViewBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_view.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // set object properties
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
    // TODO 1: make sure the player cannot be hit by the transparent rear of the bullet (e.g. when he moves into it) (does the view-bullet also has this problem ?)
}


// ****************************************************************
// 
void cViewBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 5.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cViewBullet::__ReflectOwn()
{
    // 
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
}


// ****************************************************************
// move the view bullet
void cViewBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
    this->SetColor3           (coreVector3(1.0f,0.0f,0.0f));
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
void cMineBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    if(this->GetSize().x >= 3.0f) g_pSpecialEffects->MacroExplosionPhysicalColorBig  (this->GetPosition(), COLOR_FIRE_ORANGE);
                             else g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cMineBullet::__ReflectOwn()
{
}


// ****************************************************************
// 
void cMineBullet::__RenderOwnBefore()
{
    glDisable(GL_DEPTH_TEST);
    {
        const coreFloat fValue = LERPB(0.0f, 1.0f, FRACT(m_fAnimation * 0.5f));

        // 
        s_Wave.SetPosition(this->GetPosition());
        s_Wave.SetSize    (coreVector3(coreVector2(7.5f,7.5f) * (0.25f + 0.75f*fValue) * this->GetSize().x, 1.0f));
        s_Wave.SetAlpha   (1.0f - fValue);
        s_Wave.Move();
        s_Wave.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the mine bullet
void cMineBullet::__MoveOwn()
{
    // 
    m_fSpeed = MAX(m_fSpeed * FrictionFactor(3.5f), 0.0f);

    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(1.5f, 20.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (0.3f*PI)), 0.0f));
    this->SetColor3   (coreVector3(0.5f + 0.5f * SIN(m_fAnimation * (1.0f*PI)), 0.0f, 0.0f));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));
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
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
    this->SetColor3           (coreVector3(0.0f,1.0f,0.0f));
}


// ****************************************************************
// 
void cRocketBullet::__ImpactOwn(const coreVector2 vImpact)
{
    // 
    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cRocketBullet::__ReflectOwn()
{
    // 
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
}


// ****************************************************************
// move the rocket bullet
void cRocketBullet::__MoveOwn()
{
    // 
    m_fSpeed += 2.0f * BULLET_SPEED_FACTOR * TIME;
    if(m_fSpeed > 300.0f) this->Deactivate(true);

    // 
    const cEnemy* pEnemy = g_pGame->GetEnemyManager()->FindEnemy(this->GetPosition().xy());
    if(pEnemy)
    {
        const coreVector2 vAim    = (pEnemy->GetPosition().xy() - this->GetPosition().xy()).Normalized();
        const coreVector2 vNewDir = SmoothAim(m_vFlyDir, vAim, 0.05f * m_fSpeed);

        m_vFlyDir = vNewDir;
    }

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(10.0f, 2.0f);
    this->SetColor3(coreVector3(0.0f, 0.6f + 0.4f * SIN(m_fAnimation * (1.0f*PI)), 0.0f));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN(20.0f * m_fFade, 1.0f));

    // 
    if(TIME) g_pSpecialEffects->CreateSplashSmoke(this->GetPosition() - this->GetDirection() * 4.5f, 5.0f, 1u, coreVector3(1.0f,1.0f,1.0f));
    // TODO 1: timer 
}