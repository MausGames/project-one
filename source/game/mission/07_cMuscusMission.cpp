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
cMuscusMission::cMuscusMission()noexcept
: m_Generate       (MUSCUS_GENERATES)
, m_GenerateWave   (MUSCUS_GENERATES)
, m_afGenerateTime {}
, m_afGenerateBang {}
, m_afGenerateView {}
, m_iGenerateTest  (0u)
, m_iGenerateHit   (0u)
, m_Pearl          (MUSCUS_PEARLS)
, m_PearlWave      (MUSCUS_PEARLS)
, m_iPearlActive   (0u)
, m_iPearlHidden   (0u)
, m_iPearlPitch    (0u)
, m_fSunValue      (0.0f)
, m_fSunAnimation  (0.0f)
, m_iDiamondIndex  (UINT8_MAX)
, m_fDiamondSpin   (0.0f)
, m_afStrikeTime   {}
, m_apStrikePlayer {}
, m_apStrikeTarget {}
, m_iStrikeState   (0u)
, m_fStrikeTicker  (0.0f)
, m_fAnimation     (0.0f)
, m_bStory         (!HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiState, STATE_STORY_MUSCUS))
{
    // 
    m_apBoss[0] = &m_Geminga;

    // 
    m_Generate    .DefineProgram("effect_energy_flat_invert_inst_program");
    m_GenerateWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < MUSCUS_GENERATES_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pGenerate = &m_aGenerateRaw[i];
            pGenerate->DefineModel  ("object_cube_top.md3");
            pGenerate->DefineVolume ("object_cube_volume.md3");
            pGenerate->DefineTexture(0u, "effect_energy.png");
            pGenerate->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_invert_program");

            // set object properties
            pGenerate->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.15f);
            pGenerate->SetColor3           (COLOR_ENERGY_GREEN * 0.7f);
            pGenerate->SetTexSize          (coreVector2(1.0f,1.0f) * 0.5f);
            pGenerate->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_GenerateWave.BindObject(pGenerate);
                 else m_Generate    .BindObject(pGenerate);
        }
    }

    // 
    m_Pearl    .DefineProgram("effect_energy_flat_inst_program");
    m_PearlWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pPearl = &m_aPearlRaw[i];
            pPearl->DefineModel  ("object_sphere.md3");
            pPearl->DefineTexture(0u, "effect_energy.png");
            pPearl->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_program");

            // set object properties
            pPearl->SetTexSize(coreVector2(1.5f,3.0f) * 0.6f);
            pPearl->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_PearlWave.BindObject(pPearl);
                 else m_Pearl    .BindObject(pPearl);
        }
    }

    // 
    for(coreUintW i = 0u; i < MUSCUS_ZOMBIES; ++i)
    {
        m_aZombie[i].DefineModel  ("object_tetra_top.md3");
        m_aZombie[i].DefineTexture(0u, "effect_energy.png");
        m_aZombie[i].DefineProgram("effect_energy_flat_spheric_program");
        m_aZombie[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
        m_aZombie[i].SetColor3    (COLOR_ENERGY_CYAN * 0.8f);
        m_aZombie[i].SetTexSize   (coreVector2(1.0f,1.0f) * 0.5f);
        m_aZombie[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Sun.DefineModel  ("object_sphere.md3");
    m_Sun.DefineTexture(0u, "effect_energy.png");
    m_Sun.DefineProgram("effect_energy_flat_invert_program");
    m_Sun.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 3.5f);
    m_Sun.SetTexSize   (coreVector2(1.0f,1.0f) * 3.5f);
    m_Sun.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSunWave); ++i)
    {
        m_aSunWave[i].DefineModel  ("object_tetra_top.md3");
        m_aSunWave[i].DefineTexture(0u, "effect_energy.png");
        m_aSunWave[i].DefineProgram("effect_energy_flat_spheric_program");
        m_aSunWave[i].SetTexSize   (coreVector2(1.0f,1.0f) * 0.5f);
        m_aSunWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Diamond.DefineModel  ("object_cube_top.md3");
    m_Diamond.DefineTexture(0u, "effect_energy.png");
    m_Diamond.DefineProgram("effect_energy_flat_invert_program");
    m_Diamond.SetColor3 (COLOR_ENERGY_ORANGE * 0.7f);
    m_Diamond.SetTexSize(coreVector2(1.0f,1.0f) * 0.4f);
    m_Diamond.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        m_aStrikeSpline[i].Reserve(2u);
        m_aStrikeSpline[i].AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.0f),  2.0f);
        m_aStrikeSpline[i].AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.0f), 10.0f);
    }

    // 
    g_pGlow->BindList(&m_Generate);
    g_pGlow->BindList(&m_GenerateWave);
    g_pGlow->BindList(&m_Pearl);
    g_pGlow->BindList(&m_PearlWave);
}


// ****************************************************************
// destructor
cMuscusMission::~cMuscusMission()
{
    // 
    g_pGlow->UnbindList(&m_Generate);
    g_pGlow->UnbindList(&m_GenerateWave);
    g_pGlow->UnbindList(&m_Pearl);
    g_pGlow->UnbindList(&m_PearlWave);

    // 
    for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i) this->DisableGenerate(i, false);
    for(coreUintW i = 0u; i < MUSCUS_PEARLS;    ++i) this->DisablePearl   (i, false);
    for(coreUintW i = 0u; i < MUSCUS_ZOMBIES;   ++i) this->DisableZombie  (i, false);
    this->DisableSun(false);
}


// ****************************************************************
// 
void cMuscusMission::EnableGenerate(const coreUintW iIndex)
{
    ASSERT(iIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[iIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[iIndex];

    // 
    WARN_IF(pGenerate->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableGenerate(iIndex, false);

    // 
    m_afGenerateTime[iIndex] = 0.0f;
    m_afGenerateBang[iIndex] = 0.0f;
    m_afGenerateView[iIndex] = 0.0f;

    // 
    ADD_BIT   (m_iGenerateTest, iIndex)
    REMOVE_BIT(m_iGenerateHit,  iIndex)
    STATIC_ASSERT(MUSCUS_GENERATES <= sizeof(m_iGenerateTest)*8u)
    STATIC_ASSERT(MUSCUS_GENERATES <= sizeof(m_iGenerateHit) *8u)

    // 
    pGenerate->SetAlpha  (0.0f);
    pGenerate->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave    ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cMuscusMission::DisableGenerate(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[iIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[iIndex];

    // 
    if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(iIndex == m_iDiamondIndex) this->EndDiamond(false);

    // 
    m_afGenerateTime[iIndex] = -1.0f;

    // 
    if(!bAnimated)
    {
        pGenerate->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave    ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMuscusMission::EnablePearl(const coreUintW iIndex)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl    .List())[iIndex];
    coreObject3D* pWave  = (*m_PearlWave.List())[iIndex];

    // 
    WARN_IF(pPearl->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisablePearl(iIndex, false);

    // 
    ADD_BIT   (m_iPearlActive, iIndex)
    REMOVE_BIT(m_iPearlHidden, iIndex)
    STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_iPearlActive)*8u)
    STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_iPearlHidden)*8u)

    // 
    m_afStrikeTime  [iIndex] = 0.0f;
    m_apStrikeTarget[iIndex] = NULL;
    // # do not reset strike-player

    // 
    pPearl->SetAlpha  (0.0f);
    pPearl->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cMuscusMission::DisablePearl(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl    .List())[iIndex];
    coreObject3D* pWave  = (*m_PearlWave.List())[iIndex];

    // 
    if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iPearlActive, iIndex)

    // 
    m_apStrikeTarget[iIndex] = NULL;   // only

    // 
    if(!bAnimated)
    {
        pPearl->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMuscusMission::EnableZombie(const coreUintW iIndex)
{
    ASSERT(iIndex < MUSCUS_ZOMBIES)
    coreObject3D& oZombie = m_aZombie[iIndex];

    // 
    WARN_IF(oZombie.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableZombie(iIndex, false);

    // 
    oZombie.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&oZombie);
}


// ****************************************************************
// 
void cMuscusMission::DisableZombie(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < MUSCUS_ZOMBIES)
    coreObject3D& oZombie = m_aZombie[iIndex];

    // 
    if(!oZombie.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oZombie.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&oZombie);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(oZombie.GetPosition(), 25.0f, 10u, COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cMuscusMission::EnableSun()
{
    // 
    WARN_IF(m_Sun.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableSun(false);

    // 
    m_fSunValue     = 0.0f;
    m_fSunAnimation = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_Sun);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSunWave); ++i) nInitFunc(&m_aSunWave[i]);
}


// ****************************************************************
// 
void cMuscusMission::DisableSun(const coreBool bAnimated)
{
    // 
    if(!m_Sun.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(pObject);
    };
    nExitFunc(&m_Sun);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSunWave); ++i) nExitFunc(&m_aSunWave[i]);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(m_Sun.GetPosition(), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cMuscusMission::StartDiamond(const coreUintW iIndex)
{
    WARN_IF(m_iDiamondIndex != UINT8_MAX) return;

    ASSERT(iIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[iIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[iIndex];

    // 
    m_iDiamondIndex = iIndex;
    m_fDiamondSpin  = 0.0f;

    // 
    ASSERT(pGenerate->IsEnabled(CORE_OBJECT_ENABLE_ALL))
    pGenerate->SetColor3(COLOR_ENERGY_ORANGE * 0.7f);
    pWave    ->SetColor3(COLOR_ENERGY_ORANGE * 0.7f);

    // 
    m_Diamond.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Diamond);
}


// ****************************************************************
// 
void cMuscusMission::EndDiamond(const coreBool bAnimated)
{
    if(m_iDiamondIndex == UINT8_MAX) return;

    ASSERT(m_iDiamondIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[m_iDiamondIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[m_iDiamondIndex];

    // 
    m_iDiamondIndex = UINT8_MAX;
    m_fDiamondSpin  = 0.0f;

    // 
    pGenerate->SetColor3(COLOR_ENERGY_GREEN * 0.7f);
    pWave    ->SetColor3(COLOR_ENERGY_GREEN * 0.7f);

    // 
    m_Diamond.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Diamond);
}


// ****************************************************************
// 
coreFloat cMuscusMission::RetrievePearlPitch()
{
    const coreUint8 iValue = m_iPearlPitch;
    if(++m_iPearlPitch >= 5u) m_iPearlPitch = 0u;

    switch(iValue)
    {
    default: UNREACHABLE
    case 0u: return 0.9f;
    case 1u: return 0.95f;
    case 2u: return 1.0f;
    case 3u: return 1.05f;
    case 4u: return 1.1f;
    }
}


// ****************************************************************
// 
void cMuscusMission::StrikeAttack(const coreUintW iIndex, cPlayer* pPlayer, const cShip* pTarget)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    const coreObject3D* pPearl = (*m_Pearl.List())[iIndex];

    ASSERT(pPlayer && pTarget && HAS_BIT(m_iPearlActive, iIndex))

    // 
    REMOVE_BIT(m_iPearlHidden, iIndex)

    // 
    const coreVector2 vDiff = pTarget->GetPosition().xy() - pPearl->GetPosition().xy();
    const coreFloat   fLen  = LERPB(8.0f, 1.0f, STEP(0.0f, 0.8f * FOREGROUND_AREA.x, vDiff.Length()));

    // 
    const coreVector2 vDirIn  = vDiff.Normalized();
    const coreVector2 vDirOut = (-vDirIn + vDirIn.Rotated90() * (Core::Rand->Float(0.4f, 1.0f) * ((iIndex & 0x01u) ? -1.0f : 1.0f))) * (Core::Rand->Float(2.5f, 3.5f) * fLen);

    // 
    m_aStrikeSpline[iIndex].EditNodePosition(0u, pPearl ->GetPosition().xy());
    m_aStrikeSpline[iIndex].EditNodePosition(1u, pTarget->GetPosition().xy());
    m_aStrikeSpline[iIndex].EditNodeTangent (0u, vDirOut);
    m_aStrikeSpline[iIndex].EditNodeTangent (1u, vDirIn);

    // 
    m_afStrikeTime  [iIndex] = 0.0f;
    m_apStrikePlayer[iIndex] = pPlayer;
    m_apStrikeTarget[iIndex] = pTarget;
}


// ****************************************************************
// 
void cMuscusMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < MUSCUS_ZOMBIES; ++i) m_aZombie[i].Render();
    for(coreUintW i = 0u; i < MUSCUS_ZOMBIES; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aZombie[i]);
}


// ****************************************************************
// 
void cMuscusMission::__RenderOwnOver()
{
    DEPTH_PUSH

    glDepthMask(false);   // TODO 1: glDisable(GL_DEPTH_TEST); in over ???
    {
        // 
        m_GenerateWave.Render();
    }
    glDepthMask(true);

    // 
    m_Generate.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Generate);

    DEPTH_PUSH

    // 
    m_Diamond.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Diamond);

    glDepthMask(false);   // TODO 1: glDisable(GL_DEPTH_TEST); in over ???
    {
        // 
        m_PearlWave.Render();
    }
    glDepthMask(true);

    // 
    m_Pearl.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Pearl);
}


// ****************************************************************
// 
void cMuscusMission::__RenderOwnTop()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSunWave); ++i) m_aSunWave[i].Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    m_Sun.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Sun);
}


// ****************************************************************
// 
void cMuscusMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_iGenerateHit = 0u;

    // 
    for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
    {
        coreObject3D* pGenerate = (*m_Generate    .List())[i];
        coreObject3D* pWave     = (*m_GenerateWave.List())[i];
        if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afGenerateBang[i].UpdateMax(-1.0f, 0.0f);

        if(m_afGenerateTime[i] >= 0.0f)
        {
            // 
            m_afGenerateTime[i].UpdateMax(-1.0f, 0.0f);

            // 
                 if(m_afGenerateBang[i]) m_afGenerateView[i] = 1.0f;
            else if(m_afGenerateTime[i]) m_afGenerateView[i].UpdateMin( 3.0f, 1.0f);
            else                         m_afGenerateView[i].UpdateMax(-3.0f, 0.0f);
        }
        else
        {
            // 
            m_afGenerateView[i].UpdateMax(-3.0f, 0.0f);

            // 
            if(!m_afGenerateView[i]) this->DisableGenerate(i, false);
        }

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);
        const coreFloat fBang   = BLENDB(1.0f - m_afGenerateBang[i]);

        // 
        pGenerate->SetSize     (coreVector3(5.0f,5.0f,5.0f) * LERP(1.2f, 1.0f, fBang));
        pGenerate->SetAlpha    (BLENDH3(m_afGenerateView[i]));
        pGenerate->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pGenerate->GetPosition ());
        pWave->SetSize     (pGenerate->GetSize     () * LERP(1.0f, 1.6f, fBang));
        pWave->SetDirection(pGenerate->GetDirection());
        pWave->SetAlpha    (pGenerate->GetAlpha    () * LERP(1.0f, 0.0f, fBang));
        pWave->SetTexOffset(pGenerate->GetTexOffset());

        // 
        if(HAS_BIT(m_iGenerateTest, i))
        {
            // 
            cPlayer::TestCollision(this->IsGenerateDiamond(i) ? PLAYER_TEST_ALL : PLAYER_TEST_NORMAL, pGenerate, [&](cPlayer* OUTPUT pPlayer, const coreObject3D* pGenerate, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                // 
                ADD_BIT(m_iGenerateHit, i)

                // 
                if(!this->IsGenerateDiamond(i)) pPlayer->TakeDamage(5, ELEMENT_GREEN, vIntersection.xy());

                // 
                this->BangGenerate(i);

                // 
                for(coreUintW j = 0u; j < MUSCUS_GENERATES; ++j)
                    this->ShowGenerate(j, 1.0f);

                // 
                g_pSpecialEffects->CreateSplashColor(pGenerate->GetPosition(), SPECIAL_SPLASH_SMALL, pGenerate->GetColor3() / 0.7f);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(pGenerate->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
            });
        }
    }

    // 
    m_Generate    .MoveNormal();
    m_GenerateWave.MoveNormal();

    // 
    m_iStrikeState = 0u;

    // 
    m_fStrikeTicker.Update(60.0f);
    const coreUintW iStrikeTick = F_TO_UI(m_fStrikeTicker);
    m_fStrikeTicker = FRACT(m_fStrikeTicker);

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        coreObject3D* pPearl = (*m_Pearl    .List())[i];
        coreObject3D* pWave  = (*m_PearlWave.List())[i];
        if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(m_apStrikeTarget[i])
        {
            const coreVector2 vOldPos = pPearl->GetPosition().xy();

            // 
            m_aStrikeSpline[i].EditNodePosition(1u, m_apStrikeTarget[i]->GetPosition().xy());
            m_afStrikeTime [i].UpdateMin(1.0f, 1.0f);

            // 
            pPearl->SetPosition(coreVector3(m_aStrikeSpline[i].CalcPositionLerp(m_afStrikeTime[i]), 0.0f));

            if(iStrikeTick)
            {
                const coreVector3 vColor = COLOR_ENERGY_WHITE * LERP(0.5f, 1.0f, m_afStrikeTime[i]);
                const coreVector2 vPos1  = LERP(pPearl->GetPosition().xy(), vOldPos, 0.5f);
                const coreVector2 vPos2  = pPearl->GetPosition().xy();

                g_pSpecialEffects->CreateSplashDot(coreVector3(vPos1, SPECIAL_DEEP), 0.0f, 1u, vColor);
                g_pSpecialEffects->CreateSplashDot(coreVector3(vPos2, SPECIAL_DEEP), 0.0f, 1u, vColor);
            }

            if(m_afStrikeTime[i] >= 1.0f)
            {
                // 
                this->DisablePearl(i, true);
                g_pSpecialEffects->CreateBlowColor(pPearl->GetPosition(), coreVector3(-this->GetStrikeDir(i), 0.0f), 100.0f, 10u, COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);

                // 
                ADD_BIT(m_iStrikeState, i)
                STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_iStrikeState)*8u)
            }
        }

        // 
        if(HAS_BIT(m_iPearlActive, i)) pPearl->SetAlpha(MIN1(pPearl->GetAlpha() + 10.0f*TIME));
                                  else pPearl->SetAlpha(MAX0(pPearl->GetAlpha() - 10.0f*TIME));

        // 
        if(!pPearl->GetAlpha()) this->DisablePearl(i, false);

        // 
        if(HAS_BIT(m_iPearlHidden, i)) pPearl->SetAlpha(0.0f);

        // 
        const coreFloat fOffset = I_TO_F(MUSCUS_PEARLS - i) * (1.0f/7.0f);
        const coreFloat fValue  = FRACT(7.0f * m_fAnimation + fOffset);

        // 
        pPearl->SetSize     (coreVector3(2.4f,2.4f,2.4f) * pPearl->GetAlpha());
        pPearl->SetColor3   (m_apStrikeTarget[i] ? (COLOR_ENERGY_WHITE * 0.6f) : (COLOR_ENERGY_YELLOW * 0.7f));
        pPearl->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pPearl->GetPosition ());
        pWave->SetSize     (pPearl->GetSize     () * LERPH3(0.0f, 2.0f, fValue));
        pWave->SetDirection(pPearl->GetDirection());
        pWave->SetColor3   (pPearl->GetColor3   ());
        pWave->SetAlpha    (pPearl->GetAlpha    () * LERPH3(0.0f, 1.0f, 1.0f - fValue));
        pWave->SetTexOffset(pPearl->GetTexOffset());
    }

    // 
    m_Pearl    .MoveNormal();
    m_PearlWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < MUSCUS_ZOMBIES; ++i)
    {
        coreObject3D& oZombie = m_aZombie[i];
        if(!oZombie.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        oZombie.SetTexOffset(coreVector2(1.0f,1.0f) * -0.5f * m_fAnimation);
        oZombie.Move();
    }

    // 
    if(m_Sun.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_fSunAnimation.UpdateMod(m_fSunValue * 0.2f, 10.0f);

        const coreVector3 vColor = LERPH3(COLOR_ENERGY_WHITE * 0.1f, COLOR_ENERGY_ORANGE, m_fSunValue);

        // 
        m_Sun.SetColor3   (vColor);
        m_Sun.SetTexOffset(coreVector2(0.0f, -m_fSunAnimation));
        m_Sun.Move();

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSunWave); ++i)
        {
            const coreVector2 vDir = coreVector2::Direction((m_fSunAnimation + I_TO_F(i) / 9.0f) * (2.0f*PI));

            // 
            m_aSunWave[i].SetPosition (m_Sun.GetPosition ());
            m_aSunWave[i].SetSize     (m_Sun.GetSize     () * LERP(0.0f, 1.5f, m_fSunValue));
            m_aSunWave[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aSunWave[i].SetColor3   (m_Sun.GetColor3   ());
            m_aSunWave[i].SetAlpha    (m_Sun.GetAlpha    () * 0.8f);
            m_aSunWave[i].SetTexOffset(m_Sun.GetTexOffset());
            m_aSunWave[i].Move();
        }
    }

    // 
    if(m_Diamond.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        ASSERT(m_iDiamondIndex != UINT8_MAX)
        coreObject3D* pGenerate = (*m_Generate.List())[m_iDiamondIndex];

        // 
        if(m_fDiamondSpin) m_fDiamondSpin.UpdateMin(3.0f, 2.0f);

        // 
        const coreVector2 vDir   = coreVector2::Direction(m_fAnimation * (4.0f*PI));
        const coreFloat   fScale = STEP(1.0f, 2.0f, m_fDiamondSpin);

        // 
        m_Diamond.SetPosition (pGenerate->GetPosition ());
        m_Diamond.SetSize     (pGenerate->GetSize     () * LERPB(1.0f, 1.5f, fScale) * 0.5f);
        m_Diamond.SetDirection(coreVector3(vDir, 0.0f));
        m_Diamond.SetAlpha    (pGenerate->GetAlpha    () * LERPB(1.0f, 0.0f, fScale));
        m_Diamond.SetTexOffset(pGenerate->GetTexOffset());
        m_Diamond.Move();
    }
}