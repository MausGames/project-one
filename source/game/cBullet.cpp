///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

cRotaCache   cBullet      ::s_RotaCache = {};
coreObject3D cDebrisBullet::s_Wave      = {};
coreObject3D cMineBullet  ::s_Wave      = {};


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
, m_fAnimSpeed (1.0f)
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
    
    const coreBool bVisible = g_pForeground->IsVisibleObject(this);

    // deactivate bullet when leaving the visible area
    if((m_fFlyTime >= 0.5f) && !HAS_FLAG(m_iStatus, BULLET_STATUS_IMMORTAL) && !bVisible)
        this->Deactivate(false);
    
    if(HAS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE))
    {
        this->SetEnabled(bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
    }
    
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
    
    m_fAnimSpeed = 1.0f;

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
void cBullet::Deactivate(const coreBool bAnimated, const coreVector2 vImpact, const coreVector2 vForce)
{
    // deactivate bullet (will be cleaned up by bullet manager)
    if(!HAS_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)) return;
    REMOVE_FLAG(m_iStatus, BULLET_STATUS_ACTIVE)

    // 
    if(bAnimated && (!HAS_FLAG(m_iStatus, BULLET_STATUS_IMMORTAL) || this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)))
        this->__ImpactOwn(vImpact, vForce);

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // disable collision
    this->ChangeType(0);
    //this->SetSize(this->GetSize() * coreVector3(1.5f,1.0f,1.0f)); // TODO 1: nur fuer ray ?
    //this->coreObject3D::Move();
}

void cBullet::Deactivate(const coreBool bAnimated, const coreVector2 vImpact)
{
    // 
    this->Deactivate(bAnimated, vImpact, coreVector2(0.0f,0.0f));
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
    ASSERT(pObject && pObject->GetVolume()->GetNumClusters())

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
    const coreVector2 vNormal = vForceNormal.IsNull() ? ((vPeak - m_vFlyDir * 3.0f) - pObject->GetPosition().xy()).Normalized(-m_vFlyDir) : vForceNormal;
    if(coreVector2::Dot(m_vFlyDir, vNormal) >= 0.0f) return;

    // reflect bullet
    ASSERT(vNormal.IsNormalized())
    m_vFlyDir = coreVector2::Reflect(m_vFlyDir, vNormal);

    // set corrected position
    this->SetPosition(coreVector3(vPeak - m_vFlyDir * fHitProj, 0.0f));

    // call individual reflect routine
    this->__ReflectOwn();

    // move the 3d-object
    this->coreObject3D::Move();   // for direction (and other) changes

    // 
    this->AddStatus(BULLET_STATUS_REFLECTED);
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
    }
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[m_aiOrder[i]]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[m_aiOrder[i]]->oBulletActive;

        // render bullet set
        pBulletActive->Render();
    }

    // 
    m_Outline.Apply();
}


void cBulletManager::RenderAfter()
{
    for(coreUintW i = 0u; i < BULLET_SET_COUNT; ++i)
    {
        if(!m_apBulletSet[m_aiOrder[i]]) continue;
        coreBatchList* pBulletActive = &m_apBulletSet[m_aiOrder[i]]->oBulletActive;

        // call individual subsequent render routines
        FOR_EACH(it, *pBulletActive->List())
            d_cast<cBullet*>(*it)->__RenderOwnAfter();
        
        /*
        FOR_EACH(it, *pBulletActive->List())
        {        
            if((*it)->GetPosition().z > 5.0f)
            {
                const coreFloat fHeight = (*it)->GetPosition().z;
    
                (*it)->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f * 4.0f);
                (*it)->SetAlpha(1.0f - STEPH3(0.0f, 100.0f, fHeight));
                
    
                
                d_cast<cBullet*>(*it)->_EnableDepth(g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT_BULLET_THICK)->GetProgramSingle());
                g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT_BULLET_THICK)->ApplyObject(*it);
    
                
                (*it)->SetPosition(coreVector3((*it)->GetPosition().xy(), fHeight));
        
                (*it)->SetSize(coreVector3(0.0f,0.0f,0.0f));
                (*it)->SetAlpha(0.0f);
            }
        }*/
    }
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
                
                //pBullet->ChangeType(0);
    
                // clean up bullet and make ready again
                pBullet->AddStatus(BULLET_STATUS_READY);
                DYN_REMOVE(it, *pBulletActive->List())
            }
            else
            {
                if((m_aiOrder[i] == cTiltBullet::ID) || (m_aiOrder[i] == cFinalBullet::ID))
                {
                    pBullet->m_fDepth = (STEP(-350.0f, 100.0f, pBullet->GetPosition().z)) * 800.0f + (1.0f - STEP(0.0f, 250.0f, pBullet->GetPosition().xy().Length())) * 300.0f; DYN_KEEP(it)
                    continue;
                }   // TODO 1: improve

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
        {
            cBullet* pBullet = d_cast<cBullet*>(*it);

            const coreVector2 vImpact = pBullet->GetPosition().xy();
            const coreVector2 vForce  = vImpact.IsNull() ? coreVector2(0.0f,1.0f) : vImpact.Normalized();

            pBullet->Deactivate(bAnimated, vImpact, vForce);
        }
    }
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

    // 
    this->SetCollisionModifier(coreVector3(1.07f,1.0f,3.0f));
}


// ****************************************************************
// 
void cRayBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 20.0f, ABS(m_iDamage), this->GetColor3());
}


// ****************************************************************
// 
void cRayBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, this->GetPosition().z));   // move to tip, to account for resetting the length
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // 
    m_fFade = 0.0f;
    this->SetSize (coreVector3(0.0f,0.0f,0.0f));
    this->SetAlpha(0.0f);

    // 
    g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 10.0f, ABS(m_iDamage), this->GetColor3());
}


// ****************************************************************
// move the ray bullet
void cRayBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), this->GetPosition().z));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    
    const coreFloat fRelSpeed = m_fSpeed / (8.0f*BULLET_SPEED_FACTOR);

    // update animation
    //m_fAnimation.UpdateMod(0.4f * fRelSpeed * m_fAnimSpeed, 1.0f);
    //this->SetTexOffset(coreVector2(0.35f, 0.0f + 0.0f*m_fAnimation + 0.0f*FRACT(coreFloat(Core::System->GetTotalTime()) * 0.1f)));

    
    // update fade
    m_fFade.Update(1.5f * fRelSpeed);
    const coreFloat fLen = MIN1(12.0f * m_fFade) * (0.3f + 0.7f * fRelSpeed);
    const coreFloat fWave = 1.0f + 0.25f * SIN(m_fFade * 30.0f + 1.5f*PI);
    this->SetSize (coreVector3(3.7f * fWave, 3.7f * fLen, 3.7f * fWave) * 0.52f * m_fScale);
    this->SetAlpha(MIN1(15.0f * m_fFade));

    
    
    this->SetTexSize(coreVector2(0.4f, 0.2f * fLen) * 0.01f);
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

    // 
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,3.0f));
}


// ****************************************************************
// 
void cPulseBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 10.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cPulseBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, this->GetPosition().z));   // move to tip, to account for resetting the length
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
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), this->GetPosition().z));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    const coreFloat fRelSpeed = m_fSpeed / (3.0f*BULLET_SPEED_FACTOR);
    
    // update animation
    m_fAnimation.UpdateMod(0.4f * fRelSpeed * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f * fRelSpeed);
    this->SetSize (coreVector3(1.0f, 2.5f * MIN1(12.0f * m_fFade)     * fRelSpeed, 1.0f) * 1.3f * m_fScale);
    this->SetAlpha(MIN1(15.0f * m_fFade));
}


// ****************************************************************
// constructor
cSurgeBullet::cSurgeBullet()noexcept
: m_fScale (1.0f)
{
    // load object resources
    this->DefineModel  ("bullet_wave.md3");
    this->DefineVolume ("bullet_wave_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");

    // 
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,3.0f));
}


// ****************************************************************
// 
void cSurgeBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 10.0f, 3u, this->GetColor3());
}


// ****************************************************************
// 
void cSurgeBullet::__ReflectOwn()
{
    // 
    this->SetPosition (coreVector3(this->GetPosition().xy() + m_vFlyDir * this->GetCollisionRange().y, this->GetPosition().z));   // move to tip, to account for resetting the length
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
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), this->GetPosition().z));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.06f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.3f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetSize (coreVector3(1.5f, 1.5f * MIN1(12.0f * m_fFade), 1.5f) * 2.35f * m_fScale);
    this->SetAlpha(MIN1(15.0f * m_fFade));
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

    // 
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,3.0f));
}


// ****************************************************************
// 
void cTeslaBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 10.0f, 3u, this->GetColor3());
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
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), this->GetPosition().z));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(-0.2f * m_fAnimSpeed, -1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetSize(coreVector3(2.5f,2.5f,2.5f) * MIN1(12.0f * m_fFade) * m_fScale);
    this->SetAlpha(MIN1(15.0f * m_fFade));

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
cFinalBullet::cFinalBullet()noexcept
: m_fScale    (1.0f)
, m_vFlyDir3D (coreVector3(0.0f,0.0f,0.0f))
{
    // load object resources
    this->DefineModel  ("object_cube_top.md3");
    this->DefineVolume ("bullet_final_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");
}


// ****************************************************************
// 
void cFinalBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 100.0f, 5u, this->GetColor3(), false, false, 2.0f, 0.7f);
}


// ****************************************************************
// move the final bullet
void cFinalBullet::__MoveOwn()
{
    this->SetTexSize(coreVector2(0.5f,0.5f) * 0.3f);
    
    
    // fly around

    this->SetPosition(this->GetPosition() + m_vFlyDir3D * (m_fSpeed * TIME));
    
    
    const coreFloat fRelSpeed = m_fSpeed / (16.0f*BULLET_SPEED_FACTOR);

    // update animation
    m_fAnimation.UpdateMod(0.4f * fRelSpeed * m_fAnimSpeed, 1.0f);
    m_fAnimation = FMOD(coreFloat(Core::System->GetTotalTime()) * 0.2f, 5.0f);
    this->SetTexOffset(coreVector2(0.35f, -0.2f * m_fAnimation));

    const coreFloat fRange = STEP(-500.0f, 0.0f, this->GetPosition().z) + CORE_MATH_PRECISION;
    
    // update fade
    m_fFade.Update(1.5f * fRelSpeed);
    this->SetSize(coreVector3(3.7f, 3.7f, 3.7f) * 0.8f * m_fScale * MIN1(12.0f * m_fFade) * fRange);
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * RCP(fRange));
    
    const coreVector2 vOri = coreVector2::Direction(m_fFade * 6.0f);
    this->SetDirection(coreVector3(vOri, 0.0f));
    

    const coreFloat fHeight = this->GetPosition().z;
    this->SetAlpha(MIN1(15.0f * m_fFade) * (STEPH3(-390.0f, -310.0f, fHeight) - STEPH3(50.0f, 150.0f, fHeight)) * 0.8f);
}


// ****************************************************************
// constructor
cOrbBullet::cOrbBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");
}


// ****************************************************************
// 
void cOrbBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the orb bullet
void cOrbBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cConeBullet::cConeBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_cone.md3");
    this->DefineVolume ("bullet_cone_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");
}


// ****************************************************************
// 
void cConeBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
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
    m_fAnimation.UpdateMod(0.15f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cWaveBullet::cWaveBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_wave.md3");
    this->DefineVolume ("bullet_wave_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cWaveBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
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
    m_fAnimation.UpdateMod(0.06f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.3f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cSpearBullet::cSpearBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_spear.md3");
    this->DefineVolume ("bullet_spear_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cSpearBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
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
    m_fAnimation.UpdateMod(0.18f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cTriangleBullet::cTriangleBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_triangle.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cTriangleBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the triangle bullet
void cTriangleBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 8.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (3.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 0.625f)));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cFlipBullet::cFlipBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_spear.md3");
    this->DefineVolume ("bullet_flip_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");
}


// ****************************************************************
// 
void cFlipBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the flip bullet
void cFlipBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(-0.2f * m_fAnimSpeed, -8.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (9.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(FRACT(m_fAnimation * 0.625f), 0.0f));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cQuadBullet::cQuadBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_quad.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cQuadBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
}


// ****************************************************************
// move the quad bullet
void cQuadBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 8.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (3.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 0.625f)));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cViewBullet::cViewBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_view.md3");
    this->DefineVolume ("bullet_view_volume.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_direct_program");
}


// ****************************************************************
// 
void cViewBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            10.0f, 3u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 3u, this->GetColor3());
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
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cCardBullet::cCardBullet()noexcept
{
    // load object resources
    this->DefineModel  ("object_cube_top.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_invert_program");
}


// ****************************************************************
// 
void cCardBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f),                            25.0f, 10u, this->GetColor3());
                   else g_pSpecialEffects->CreateBlowColor  (coreVector3(vImpact, 0.0f), coreVector3(vForce, 0.0f), 50.0f, 10u, this->GetColor3());
}


// ****************************************************************
// move the card bullet
void cCardBullet::__MoveOwn()
{
    // 
    m_fSpeed = m_fSpeed - 40.0f * TIME;

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 8.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (9.0f*PI)), 0.0f));
    this->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 0.625f)));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cDebrisBullet::cDebrisBullet()noexcept
{
    // load object resources
    this->DefineModel  ("ship_meteor.md3");
    this->DefineTexture(0u, "environment_stone_diff.png");
    this->DefineTexture(1u, "environment_stone_norm.png");
    this->DefineProgram("object_meteor_program");

    // 
    this->SetColor3(COLOR_SHIP_YELLOW / COLOR_SHIP_YELLOW.Max());
}


// ****************************************************************
// 
void cDebrisBullet::GlobalInit()
{
    // 
    s_Wave.DefineModel  (Core::Manager::Object->GetLowQuad());
    s_Wave.DefineTexture(0u, "effect_wave.png");
    s_Wave.DefineProgram("effect_decal_single_program");
}


// ****************************************************************
// 
void cDebrisBullet::GlobalExit()
{
    // 
    s_Wave.Undefine();
}


// ****************************************************************
// 
void cDebrisBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), 25.0f, 5u, COLOR_ENERGY_YELLOW);
}


// ****************************************************************
// 
void cDebrisBullet::__RenderOwnBefore()
{
    glDisable(GL_DEPTH_TEST);
    {
        const coreFloat fValue = BLENDB(FRACT(m_fAnimation * 0.5f));

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
// move the debris bullet
void cDebrisBullet::__MoveOwn()
{
    // fly around
    this->SetPosition(coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));

    // update animation
    m_fAnimation.UpdateMod(1.5f * m_fAnimSpeed, 20.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (-0.3f*PI)), 0.0f));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
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
void cMineBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    if(this->GetSize().x >= 3.0f) g_pSpecialEffects->MacroExplosionPhysicalColorBig  (this->GetPosition(), COLOR_FIRE_ORANGE);
                             else g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cMineBullet::__RenderOwnBefore()
{
    glDisable(GL_DEPTH_TEST);
    {
        const coreFloat fValue = BLENDB(FRACT(m_fAnimation * 0.5f));

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
    m_fAnimation.UpdateMod(1.5f * m_fAnimSpeed, 20.0f);
    this->SetDirection(coreVector3(s_RotaCache.Direction(m_fAnimation * (0.3f*PI)), 0.0f));
    this->SetColor3   (coreVector3(0.5f + 0.5f * SIN(m_fAnimation * (1.0f*PI)), 0.0f, 0.0f));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cRocketBullet::cRocketBullet()noexcept
: m_pTarget (NULL)
{
    // load object resources
    this->DefineModel  ("bullet_rocket.md3");
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_glow_program");
}


// ****************************************************************
// 
void cRocketBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
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
    if(m_pTarget)
    {
        if(m_fFlyTime < 3.0f)
        {
            const coreVector2 vAim    = (m_pTarget->GetPosition().xy() - this->GetPosition().xy()).Normalized();
            const coreVector2 vNewDir = SmoothAim(m_vFlyDir, vAim, 0.06f * m_fSpeed);

            m_vFlyDir = vNewDir;

            this->AddStatus(BULLET_STATUS_IMMORTAL);
        }
        else
        {
            this->RemoveStatus(BULLET_STATUS_IMMORTAL);
        }
    }

    const coreFloat fFadePrev = m_fFade;

    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(10.0f * m_fAnimSpeed, 2.0f);
    this->SetColor3(COLOR_SHIP_BLUE * (0.6f + 0.4f * SIN(m_fAnimation * (1.0f*PI))));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));

    // 
    if(F_TO_UI(fFadePrev * 60.0f) != F_TO_UI(m_fFade * 60.0f))
    {
        g_pSpecialEffects->CreateSplashSmoke(this->GetPosition() - this->GetDirection() * 4.5f, 5.0f, 1u, coreVector3(1.0f,1.0f,1.0f));
    }
}


// ****************************************************************
// constructor
cGrowBullet::cGrowBullet()noexcept
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_spheric_program");
}


// ****************************************************************
// 
void cGrowBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    const coreBool bBig = (m_iDamage == 1);

    // 
    for(coreUintW i = 0u; i < 8u; ++i)
    {
        const coreVector2 vPos = vImpact + StepRotated45(i) * this->GetVisualRadius();
        const coreVector2 vDir = vPos.Normalized();

        // 
        if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f),                          10.0f, bBig ? 9u : 3u, this->GetColor3());
                       else g_pSpecialEffects->CreateBlowColor  (coreVector3(vPos, 0.0f), coreVector3(vDir, 0.0f), 50.0f, bBig ? 9u : 3u, this->GetColor3());
    }

    if(bBig)
    {
        // 
        for(coreUintW i = 0u; i < 8u; ++i)
        {
            const coreVector2 vPos = vImpact + StepRotated45X(i) * this->GetVisualRadius();
            const coreVector2 vDir = vPos.Normalized();

            // 
            if(vForce.IsNull()) g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f),                          10.0f, 9u, this->GetColor3());
                           else g_pSpecialEffects->CreateBlowColor  (coreVector3(vPos, 0.0f), coreVector3(vDir, 0.0f), 50.0f, 9u, this->GetColor3());
        }
    }
}


// ****************************************************************
// move the grow bullet
void cGrowBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (coreVector3(this->GetPosition().xy() + this->GetFlyMove(), 0.0f));
    this->SetDirection(coreVector3(m_vFlyDir, 0.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    this->SetAlpha(MIN1(20.0f * m_fFade));
}


// ****************************************************************
// constructor
cTiltBullet::cTiltBullet()noexcept
: m_vFlyDir3D (coreVector3(0.0f,0.0f,0.0f))
{
    // load object resources
    this->DefineModel  ("bullet_orb.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_bullet_program");
    
    //this->DefineModel  ("bullet_quad.md3");
    //this->DefineTexture(0u, "effect_energy.png");
    //this->DefineProgram("effect_energy_bullet_direct_program");
    
    //this->DefineModel  ("object_cube_rota.md3");

    // set object properties
    //this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR);
}


// ****************************************************************
// 
void cTiltBullet::__ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)
{
    // 
    g_pSpecialEffects->CreateSplashDark(coreVector3(vImpact, this->GetPosition().z), 10.0f, 6u);
}


// ****************************************************************
// 
void cTiltBullet::__RenderOwnAfter()
{
    if(this->GetPosition().z > 5.0f)
    {
        const coreFloat fHeight = this->GetPosition().z;
        
       // this->SetPosition(coreVector3(this->GetPosition().xy(), 0.0f));
        
        this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f * 4.0f);// * (1.0f - STEPH3(0.0f, 60.0f, fHeight)));
        this->SetAlpha(1.0f - STEPH3((m_vFlyDir3D.z > 0.0f) ? 0.0f : 50.0f, 100.0f, fHeight));
        
        this->_EnableDepth();
        this->Render();
        
        
                this->_EnableDepth(g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT_BULLET_THICK)->GetProgramSingle());
                g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT_BULLET_THICK)->ApplyObject(this);
        
        this->SetPosition(coreVector3(this->GetPosition().xy(), fHeight));

        this->SetSize(coreVector3(0.0f,0.0f,0.0f));
        this->SetAlpha(0.0f);
    }
}


// ****************************************************************
// move the tilt bullet
void cTiltBullet::__MoveOwn()
{
    // fly around
    this->SetPosition (this->GetPosition() + m_vFlyDir3D * (m_fSpeed * TIME));
    //this->SetDirection(coreVector3(m_vFlyDir, 0.0f));
    //this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // update animation
    m_fAnimation.UpdateMod(0.2f * m_fAnimSpeed, 1.0f);
    this->SetTexOffset(coreVector2(0.0f, m_fAnimation));

    // update fade
    m_fFade.Update(1.0f);
    //this->SetAlpha(MIN1(20.0f * m_fFade));

    const coreVector2 vDiff = (this->GetPosition() + ABS(this->GetPosition().z) * m_vFlyDir3D / ABS(m_vFlyDir3D.z)).xy() - g_pGame->GetPlayer(0u)->GetPosition().xy();
    
    const coreFloat fNear = MAX0((1.0f - STEPH3(6.0f, 8.0f, vDiff.Length())) - ((m_vFlyDir3D.z > 0.0f) ? STEPH3(0.0f, 10.0f, this->GetPosition().z) : (1.0f - STEPH3(-10.0f, 0.0f, this->GetPosition().z))));
    //const coreFloat fNear2 = 1.0f - STEPH3(6.0f, 50.0f, (this->GetPosition() - g_pGame->GetPlayer(0u)->GetPosition()).Length());

    this->SetColor3(LERP(COLOR_ENERGY_WHITE * 0.1f, COLOR_ENERGY_GREEN, fNear));
    
    if(g_pGame->IsMulti())
    {
        const coreVector2 vDiff2 = (this->GetPosition() + ABS(this->GetPosition().z) * m_vFlyDir3D / ABS(m_vFlyDir3D.z)).xy() - g_pGame->GetPlayer(1u)->GetPosition().xy();
        const coreFloat   fNear2 = MAX0((1.0f - STEPH3(6.0f, 8.0f, vDiff2.Length())) - ((m_vFlyDir3D.z > 0.0f) ? STEPH3(0.0f, 10.0f, this->GetPosition().z) : (1.0f - STEPH3(-10.0f, 0.0f, this->GetPosition().z))));

        this->SetColor3(LERP(this->GetColor3(), COLOR_ENERGY_YELLOW, fNear2));
    }
    
    if(this->GetPosition().z > 100.0f) this->Deactivate(false);

    if(this->GetPosition().z > 5.0f)
    {
        this->SetSize(coreVector3(0.0f,0.0f,0.0f));
        this->SetAlpha(0.0f);
    }
    else
    {
        this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f * 4.0f);
        this->SetAlpha(MIN1(8.0f * m_fFade) * STEPH3(EIGENGRAU_DEPTH, EIGENGRAU_DEPTH + 40.0f, this->GetPosition().z));
    }
}