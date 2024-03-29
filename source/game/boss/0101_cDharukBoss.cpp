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
// counter identifier
#define DUPLICATE_STATUS  (0u)
#define CURRENT_SIDE      (1u)
#define CURRENT_ITERATION (2u)
#define BOOMERANG_TARGET  (3u)
#define FLIP_SAVE         (4u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cDharukBoss::cDharukBoss()noexcept
: m_DuplicateTrail (DHARUK_DUPLICATE_RAWS)
, m_Boomerang      (DHARUK_BOOMERANGS)
, m_BoomerangTrail (DHARUK_BOOMERANGS * DHARUK_TRAILS)
, m_iPackedDir     (0u)
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_dharuk_high.md3");
    this->DefineModelLow ("ship_boss_dharuk_low.md3");

    // set object properties
    this->SetSize(coreVector3(3.4f,3.4f,3.4f));

    // configure the boss
    this->Configure(2800, COLOR_SHIP_RED);

    // create duplicate object
    m_Duplicate.DefineModelHigh("ship_boss_dharuk_high.md3");
    m_Duplicate.DefineModelLow ("ship_boss_dharuk_low.md3");
    m_Duplicate.DefineTexture  (0u, "effect_energy.png");
    m_Duplicate.DefineProgram  ("effect_energy_blink_invert_program");
    m_Duplicate.SetSize        (this->GetSize());
    m_Duplicate.Configure      (500, COLOR_ENERGY_RED * 0.8f);
    m_Duplicate.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL);

    // create duplicate trail list
    m_DuplicateTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_DUPLICATE_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = (i % DHARUK_TRAILS) + 1u;

            // load object resources
            coreObject3D* pDuplicate = &m_aDuplicateRaw[i];
            pDuplicate->DefineModel  ("ship_boss_dharuk_low.md3");
            pDuplicate->DefineTexture(0u, "effect_energy.png");
            pDuplicate->DefineProgram("effect_energy_invert_program");

            // set object properties
            pDuplicate->SetSize   (this->GetSize());
            pDuplicate->SetColor3 (COLOR_ENERGY_RED * (0.15f + 0.15f * I_TO_F(iType)));
            pDuplicate->SetAlpha  (0.15f + 0.2f * I_TO_F(iType));
            pDuplicate->SetEnabled((i < DHARUK_TRAILS) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_DuplicateTrail.BindObject(pDuplicate);
        }
    }

    // create boomerang lists
    m_Boomerang     .DefineProgram("effect_energy_invert_inst_program");
    m_BoomerangTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_BOOMERANGS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (DHARUK_TRAILS + 1u);

            // load object resources
            coreObject3D* pBoomerang = &m_aBoomerangRaw[i];
            pBoomerang->DefineModel  ("object_boss_dharuk_boomerang.md3");
            pBoomerang->DefineTexture(0u, "effect_energy.png");
            pBoomerang->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBoomerang->SetSize   (coreVector3(1.4f,1.4f,1.4f) * 1.1f);
            pBoomerang->SetColor3 (COLOR_ENERGY_RED * (iType ? (0.15f + 0.15f * I_TO_F(iType)) : 0.8f));
            pBoomerang->SetTexSize(coreVector2(1.5f,1.5f));
            pBoomerang->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BoomerangTrail.BindObject(pBoomerang);
                 else m_Boomerang     .BindObject(pBoomerang);
        }
    }
}


// ****************************************************************
// 
void cDharukBoss::__ResurrectOwn()
{
    // 
    g_pGlow->BindList(&m_DuplicateTrail);
    g_pGlow->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_BoomerangTrail);
}


// ****************************************************************
// 
void cDharukBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableDuplicate(bAnimated);

    // 
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
        this->__DisableBoomerang(i, bAnimated);

    // 
    g_pGlow->UnbindList(&m_DuplicateTrail);
    g_pGlow->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_BoomerangTrail);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_DuplicateTrail.Render();
        m_BoomerangTrail.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_Boomerang.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Boomerang);
}


// ****************************************************************
// 
void cDharukBoss::__MoveOwn()
{
    // 
    const coreVector2 vOldBasePos   = this->GetPosition   ().xy();
    const coreFloat   fOldBaseAngle = this->GetDirection  ().xy().Angle();
    const coreFloat   fOldBaseOri   = this->GetOrientation().xz().Angle();   // # not y

    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // ################################################################
    // ################################################################

    // 
    coreBool bChange = false;
    this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), 1.5f, &bChange), 0.0f));

    // 
    m_Duplicate.SetPosition   (-this->GetPosition   ());
    m_Duplicate.SetDirection  (-this->GetDirection  ());
    m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
    m_Duplicate.SetTexOffset  (coreVector2(0.0f, m_fAnimation));

    // 
    if(m_Duplicate.ReachedDeath())
    {
        g_pGame->GetItemManager()->AddItem<cFragmentItem>(m_Duplicate.GetPosition().xy());
        this->__DisableDuplicate(true);
    }

    // 
    if(TIME)
    {
        const coreFloat fRcpTime = RCP(TIME);

        // 
        const coreVector2 vCurBasePos   = this->GetPosition   ().xy();
        const coreFloat   fCurBaseAngle = this->GetDirection  ().xy().Angle();
        const coreFloat   fCurBaseOri   = this->GetOrientation().xz().Angle();   // # not y
        const coreVector2 vOffsetPos    =          (vCurBasePos -  vOldBasePos)   * fRcpTime * 0.05f;
        const coreFloat   fOffsetAngle  = AngleDiff(fCurBaseAngle, fOldBaseAngle) * fRcpTime * 0.055f;
        const coreFloat   fOffsetOri    = AngleDiff(fCurBaseOri,   fOldBaseOri)   * fRcpTime * 0.06f;

        for(coreUintW i = 0u, ie = (m_aiCounter[DUPLICATE_STATUS] ? DHARUK_DUPLICATE_RAWS : DHARUK_TRAILS); i < ie; ++i)
        {
            // 
            const coreFloat fSide   = (i < DHARUK_TRAILS) ? 1.0f : -1.0f;
            const coreFloat fFactor = I_TO_F(DHARUK_TRAILS - (i % DHARUK_TRAILS));

            // 
            const coreVector2 vNewPos =                        vCurBasePos   - vOffsetPos   * fFactor;
            const coreVector2 vNewDir = coreVector2::Direction(fCurBaseAngle - fOffsetAngle * fFactor);
            const coreVector2 vNewOri = coreVector2::Direction(fCurBaseOri   - fOffsetOri   * fFactor);

            // 
            m_aDuplicateRaw[i].SetPosition   (coreVector3(fSide * vNewPos,   0.0f));
            m_aDuplicateRaw[i].SetDirection  (coreVector3(fSide * vNewDir,   0.0f));
            m_aDuplicateRaw[i].SetOrientation(coreVector3(fSide * vNewOri.x, 0.0f, vNewOri.y));
            m_aDuplicateRaw[i].SetTexOffset  (coreVector2(0.0f, m_fAnimation));
            m_aDuplicateRaw[i].Move();
        }
    }

    // 
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
    {
        coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
        if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vFly = this->__DecodeDirection(i);

        // 
        coreVector2       vNewPos = pBoomerang->GetPosition().xy() + vFly * FOREGROUND_AREA * (1.5f * TIME);
        const coreVector2 vNewDir = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos.x + vNewPos.y) * (vFly.x + vFly.y));

        // 
        vNewPos = this->__RepeatPosition(vNewPos, 1.35f, &bChange);
        if(bChange)
        {
            // 
            const coreVector2 vBossPos   = m_aiCounter[CURRENT_ITERATION] ? this->GetPosition().xy() : this->GetPosition().xy().InvertedY();
            const coreVector2 vPlayerPos = g_pGame->FindPlayerSide(vNewPos)->GetPosition().xy();

            // 
            const coreVector2 vTarget = m_aiCounter[BOOMERANG_TARGET] ? vBossPos : vPlayerPos;
            const coreVector2 vFlyAbs = vFly.Processed(ABS);
            vNewPos = vNewPos * vFlyAbs + vTarget * vFlyAbs.yx();

            // 
            g_pSpecialEffects->CreateBlowColor(coreVector3(vNewPos, 0.0f), coreVector3(vFly, 0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_RED);
        }

        // 
        pBoomerang->SetPosition (coreVector3(vNewPos, 0.0f));
        pBoomerang->SetDirection(coreVector3(vNewDir, 0.0f));
        pBoomerang->SetAlpha    (MIN(pBoomerang->GetAlpha() + 10.0f * TIME, 1.0f));
        pBoomerang->SetTexOffset(coreVector2(0.0f, m_fAnimation));

        for(coreUintW j = 0u; j < DHARUK_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BoomerangTrail.List())[i*DHARUK_TRAILS + j];

            // 
            const coreVector2 vNewPos2 = pBoomerang->GetPosition().xy() - vFly * FOREGROUND_AREA * (0.06f * I_TO_F(DHARUK_TRAILS - j));
            const coreVector2 vNewDir2 = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos2.x + vNewPos2.y) * (vFly.x + vFly.y));

            // 
            pTrail->SetPosition (coreVector3(vNewPos2, 0.0f));
            pTrail->SetDirection(coreVector3(vNewDir2, 0.0f));
            pTrail->SetAlpha    (pBoomerang->GetAlpha() * (0.15f + 0.2f * I_TO_F(j + 1u)));
            pTrail->SetTexOffset(coreVector2(0.0f, m_fAnimation));
        }
    }

    // 
    m_Boomerang     .MoveNormal();
    m_BoomerangTrail.MoveNormal();

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_DHARUK_BOOMERANG, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBoomerang, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(5, ELEMENT_RED, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_RED);
    });
}


// ****************************************************************
// 
void cDharukBoss::__EnableDuplicate()
{
    // 
    if(m_aiCounter[DUPLICATE_STATUS] != 0) return;
    m_aiCounter[DUPLICATE_STATUS] = 1;

    // 
    m_Duplicate.Resurrect();

    // 
    m_Duplicate.SetPosition   (-this->GetPosition   ());
    m_Duplicate.SetDirection  (-this->GetDirection  ());
    m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
    m_Duplicate.SetAlpha      (0.0f);

    // 
    for(coreUintW i = DHARUK_TRAILS; i < DHARUK_DUPLICATE_RAWS; ++i)
    {
        m_aDuplicateRaw[i].SetAlpha  (0.0f);
        m_aDuplicateRaw[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }

    // 
    g_pSpecialEffects->MacroExplosionColorBig(m_Duplicate.GetPosition(), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__DisableDuplicate(const coreBool bAnimated)
{
    // 
    if(m_aiCounter[DUPLICATE_STATUS] == 0) return;
    m_aiCounter[DUPLICATE_STATUS] = 0;

    // 
    m_Duplicate.Kill(false);

    // 
    for(coreUintW i = DHARUK_TRAILS; i < DHARUK_DUPLICATE_RAWS; ++i)
        m_aDuplicateRaw[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorBig(m_Duplicate.GetPosition(), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__EnableBoomerang(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    WARN_IF(pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBoomerang->ChangeType(TYPE_DHARUK_BOOMERANG);

    // 
    this->__EncodeDirection(iIndex, vDirection);

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(vPosition, 0.0f));
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nInitFunc(pTrail + i);

    // 
    g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPosition, 0.0f), vDirection, COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__DisableBoomerang(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBoomerang->ChangeType(0);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nExitFunc(pTrail + i);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pBoomerang->GetPosition(), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__RepeatPosition(const coreVector2 vPosition, const coreFloat fThreshold, coreBool* OUTPUT pbChange)
{
    const coreVector2 vThreshVec = fThreshold * FOREGROUND_AREA;

    // 
    coreVector2 vRepeatPos = vPosition;
    while(vRepeatPos.x < -vThreshVec.x) vRepeatPos.x += 2.0f * vThreshVec.x;
    while(vRepeatPos.x >  vThreshVec.x) vRepeatPos.x -= 2.0f * vThreshVec.x;
    while(vRepeatPos.y < -vThreshVec.y) vRepeatPos.y += 2.0f * vThreshVec.y;
    while(vRepeatPos.y >  vThreshVec.y) vRepeatPos.y -= 2.0f * vThreshVec.y;

    // 
    ASSERT(pbChange)
    (*pbChange) = (vRepeatPos.x != vPosition.x) || (vRepeatPos.y != vPosition.y);

    return vRepeatPos;
}

coreVector2 cDharukBoss::__RepeatPosition(const coreVector2 vPosition, const coreFloat fThreshold)
{
    coreBool bChange;
    return this->__RepeatPosition(vPosition, fThreshold, &bChange);
}


// ****************************************************************
// 
void cDharukBoss::__EncodeDirection(const coreUintW iIndex, const coreVector2 vDirection)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    if(vDirection.x + vDirection.y > 0.0f)
         ADD_BIT   (m_iPackedDir, 0u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 0u + 2u*iIndex)

    // 
    if(coreMath::IsNear(vDirection.y, 0.0f))
         ADD_BIT   (m_iPackedDir, 1u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 1u + 2u*iIndex)

    STATIC_ASSERT(DHARUK_BOOMERANGS*2u <= sizeof(m_iPackedDir)*8u)
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__DecodeDirection(const coreUintW iIndex)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    const coreFloat P = HAS_BIT(m_iPackedDir, 0u + 2u*iIndex) ? 1.0f : -1.0f;
    const coreBool  X = HAS_BIT(m_iPackedDir, 1u + 2u*iIndex);

    // 
    return coreVector2((X) ? P : 0.0f, (!X) ? P : 0.0f);
}