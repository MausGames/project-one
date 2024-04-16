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
cNevoMission::cNevoMission()noexcept
: m_Bomb        (NEVO_BOMBS)
, m_abBombGone  {}
, m_Blast       (NEVO_BLASTS)
, m_BlastLine   (NEVO_BLASTS * NEVO_LINES)
, m_afBlastTime {}
, m_vForce      (coreVector2(0.0f,0.0f))
, m_vImpact     (coreVector2(0.0f,0.0f))
, m_bClamp      (false)
, m_bOverdraw   (false)
, m_fAnimation  (0.0f)
{
    // 
    m_apBoss[0] = &m_Nautilus;
    m_apBoss[1] = &m_Amemasu;
    m_apBoss[2] = &m_Leviathan;

    // 
    m_Bomb.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_BOMBS_RAWS; ++i)
        {
            // load object resources
            cLodObject* pBomb = &m_aBombRaw[i];
            pBomb->DefineModelHigh("object_bomb_high.md3");
            pBomb->DefineModelLow ("object_bomb_low.md3");
            pBomb->DefineTexture  (0u, "ship_enemy.png");
            pBomb->DefineProgram  ("object_ship_glow_program");

            // set object properties
            pBomb->SetColor3 (COLOR_ENERGY_YELLOW);
            pBomb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Bomb.BindObject(pBomb);
        }
    }

    // 
    m_Blast    .DefineProgram("effect_energy_flat_inst_program");
    m_BlastLine.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_BLASTS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (NEVO_LINES + 1u);

            // load object resources
            coreObject3D* pBlast = &m_aBlastRaw[i];
            pBlast->DefineModel  (iType ? "object_tube_open.md3" : "object_sphere.md3");
            pBlast->DefineTexture(0u, "effect_energy.png");
            pBlast->DefineProgram("effect_energy_flat_program");

            // set object properties
            pBlast->SetSize   (iType ? coreVector3(0.0f,80.0f,0.0f) : coreVector3(5.0f,5.0f,5.0f));
            pBlast->SetColor3 (COLOR_ENERGY_ORANGE);   // # not fire
            pBlast->SetTexSize(iType ? coreVector2(0.6f,2.4f) : coreVector2(4.0f,4.0f));
            pBlast->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BlastLine.BindObject(pBlast);
                 else m_Blast    .BindObject(pBlast);
        }
    }

    // 
    m_Container.DefineModelHigh("object_container_high.md3");
    m_Container.DefineModelLow ("object_container_low.md3");
    m_Container.DefineTexture  (0u, "ship_enemy.png");
    m_Container.DefineProgram  ("object_ship_program");
    m_Container.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
    m_Container.SetColor3      (COLOR_SHIP_GREY);
    m_Container.SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);

    // 
    cShadow::GetGlobalContainer()->BindList(&m_Bomb);
    g_pGlow->BindList(&m_Blast);
    g_pGlow->BindList(&m_BlastLine);
}


// ****************************************************************
// destructor
cNevoMission::~cNevoMission()
{
    // 
    cShadow::GetGlobalContainer()->UnbindList(&m_Bomb);
    g_pGlow->UnbindList(&m_Blast);
    g_pGlow->UnbindList(&m_BlastLine);

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS;  ++i) this->DisableBomb (i, false);
    for(coreUintW i = 0u; i < NEVO_BLASTS; ++i) this->DisableBlast(i, false);

    // 
    this->DisableContainer(false);
}


// ****************************************************************
// 
void cNevoMission::EnableBomb(const coreUintW iIndex, const coreBool bGrow)
{
    ASSERT(iIndex < NEVO_BOMBS)
    coreObject3D& oBomb = m_aBombRaw[iIndex];

    // 
    WARN_IF(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oBomb.ChangeType(TYPE_NEVO_BOMB);

    // 
    oBomb.SetSize   (coreVector3(1.0f,1.0f,1.0f) * (bGrow ? 0.0f : NEVO_BOMB_SIZE));
    oBomb.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableBomb(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_BOMBS)
    coreObject3D& oBomb = m_aBombRaw[iIndex];

    // 
    if(!oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oBomb.ChangeType(0);

    // 
    m_abBombGone[iIndex] = true;

    // 
    oBomb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalColorSmall(oBomb.GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cNevoMission::EnableBlast(const coreUintW iIndex)
{
    // 
    ASSERT(iIndex < NEVO_BLASTS)
    coreObject3D* pBlast = (*m_Blast    .List())[iIndex];
    coreObject3D* pLine  = (*m_BlastLine.List())[iIndex*NEVO_LINES];

    // 
    WARN_IF(pBlast->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afBlastTime[iIndex] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBlast);
    for(coreUintW i = 0u; i < NEVO_LINES; ++i) nInitFunc(pLine + i);
}


// ****************************************************************
// 
void cNevoMission::DisableBlast(const coreUintW iIndex, const coreBool bAnimated)
{
    // 
    ASSERT(iIndex < NEVO_BLASTS)
    coreObject3D* pBlast = (*m_Blast    .List())[iIndex];
    coreObject3D* pLine  = (*m_BlastLine.List())[iIndex*NEVO_LINES];

    // 
    if(!pBlast->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(!bAnimated || (m_afBlastTime[iIndex] < 1.0f))
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        nExitFunc(pBlast);
        for(coreUintW i = 0u; i < NEVO_LINES; ++i) nExitFunc(pLine + i);
    }
}


// ****************************************************************
// 
void cNevoMission::EnableContainer(const coreVector2& vPosition)
{
    // 
    if(m_Container.GetType()) return;
    m_Container.ChangeType(TYPE_NEVO_CONTAINER);

    // 
    m_Container.SetPosition(coreVector3(vPosition, 0.0f));

    // 
    m_Container.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    cShadow::GetGlobalContainer()->BindObject(&m_Container);

    // 
    m_bClamp    = false;
    m_bOverdraw = false;
}


// ****************************************************************
// 
void cNevoMission::DisableContainer(const coreBool bAnimated)
{
    // 
    if(!m_Container.GetType()) return;
    m_Container.ChangeType(0);

    // 
    m_Container.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    cShadow::GetGlobalContainer()->UnbindObject(&m_Container);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalDarkBig(m_Container.GetPosition());
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnOver()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_BlastLine.Render();
        m_Blast    .Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    FOR_EACH(it, *m_Bomb.List()) d_cast<cLodObject*>(*it)->ActivateModelDefault();
    {
        // 
        m_Bomb.Render();
    }
    FOR_EACH(it, *m_Bomb.List()) d_cast<cLodObject*>(*it)->ActivateModelLowOnly();

    // 
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Bomb);

    // 
    if(m_bOverdraw) DEPTH_PUSH
               else DEPTH_PUSH_SHIP

    // 
    m_Container.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Container);
}


// ****************************************************************
// 
void cNevoMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    std::memset(m_abBombGone, 0, sizeof(m_abBombGone));

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
    {
        coreObject3D& oBomb = m_aBombRaw[i];
        if(!oBomb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        oBomb.SetSize  (coreVector3(1.0f,1.0f,1.0f) * MIN(oBomb.GetSize().x + 20.0f * Core::System->GetTime(), NEVO_BOMB_SIZE));
        oBomb.SetColor3(COLOR_ENERGY_YELLOW * (0.5f + 0.5f * COS(12.0f*PI * m_afBlastTime[i])));
    }

    // 
    m_Bomb.MoveNormal();

    // 
    for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
    {
        coreObject3D* pBlast = (*m_Blast.List())[i];
        if(!pBlast->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector2 vBaseDir = m_aBombRaw[i].GetDirection().xy();
        const coreVector2 vBasePos = m_aBombRaw[i].GetPosition ().xy();

        // 
        m_afBlastTime[i].Update(1.0f);
        const coreFloat fTime = m_afBlastTime[i];

        // 
        if(fTime >= 1.0f) this->DisableBomb (i, true);
        if(fTime >= 2.0f) this->DisableBlast(i, false);
        STATIC_ASSERT(NEVO_BOMBS == NEVO_BLASTS)

        // 
        const coreBool  bWarn  = (fTime < 1.0f);
        const coreFloat fLerp  = 1.0f - POW(fTime - 1.0f, 9.0f);
        const coreFloat fBlend = CLAMP((fTime - 1.0f) * 20.0f, 0.0f, 1.0f);
        const coreFloat fSize  = bWarn ? (2.0f)                  : (LERP(1.0f, 4.0f, fLerp) * fBlend);
        const coreFloat fAlpha = bWarn ? (0.6f * (1.0f - fTime)) : (LERP(0.0f, 1.0f, fLerp) * fBlend);

        // 
        pBlast->SetPosition (coreVector3(vBasePos, 0.0f));
        pBlast->SetSize     (coreVector3(2.0f,2.0f,2.0f) * fSize);
        pBlast->SetAlpha    (bWarn ? 0.0f : fAlpha);
        pBlast->SetTexOffset(coreVector2(0.0f, -2.5f * m_fAnimation).Processed(FRACT));

        for(coreUintW j = 0u; j < NEVO_LINES; ++j)
        {
            coreObject3D* pLine = (*m_BlastLine.List())[i*NEVO_LINES + j];

            // 
            const coreVector2 vDir = MapStepRotated90(vBaseDir, j);
            const coreVector2 vPos = vBasePos + vDir * pLine->GetSize().y;

            // 
            pLine->SetPosition (coreVector3(vPos, 0.0f));
            pLine->SetDirection(coreVector3(vDir, 0.0f));
            pLine->SetSize     (coreVector3(fSize, pLine->GetSize().y, fSize));
            pLine->SetAlpha    (fAlpha);
            pLine->SetTexOffset((coreVector2(2.4f,2.4f) * m_fAnimation * (bWarn ? 0.35f : -1.0f)).Processed(FRACT));   // TODO: move outside  
        }
    }

    // 
    m_Blast    .MoveNormal();
    m_BlastLine.MoveNormal();

    // 
    const auto nPlayerPushFunc = [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        const coreVector2 vDiff = pPlayer->GetOldPos() - pObject->GetPosition().xy();
        pPlayer->ApplyForce(vDiff.Normalized() * 100.0f);

        // 
        g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_WHITE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
    };

    // 
    const auto nBulletReflectFunc = [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pObject, vIntersection.xy());
    };

    // 
    //cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, TYPE_NEVO_BOMB, nPlayerPushFunc);
    //Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_NEVO_BOMB, nBulletReflectFunc);

    if(m_Container.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreVector2 vNewPos = m_Container.GetPosition().xy();

        // 
        if(!m_vForce.IsNull())
        {
            vNewPos  += m_vForce * Core::System->GetTime();
            m_vForce *= FrictionFactor(2.5f);
        }

        // 
        m_vImpact = coreVector2(0.0f,0.0f);

        if(m_bClamp)
        {
            // 
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; if(m_vForce.x < 0.0f) m_vImpact = coreVector2(-FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x =  ABS(m_vForce.x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; if(m_vForce.x > 0.0f) m_vImpact = coreVector2( FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x = -ABS(m_vForce.x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; if(m_vForce.y < 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x, -FOREGROUND_AREA.y * 1.1f); m_vForce.y =  ABS(m_vForce.y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; if(m_vForce.y > 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x,  FOREGROUND_AREA.y * 1.1f); m_vForce.y = -ABS(m_vForce.y);}
        }

        // 
        m_Container.SetPosition(coreVector3(vNewPos, m_Container.GetPosition().z));
        m_Container.Move();

        // 
        m_Container.ActivateModelLowOnly();
        {
            // 
            cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &m_Container, nPlayerPushFunc);
            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_Container, nBulletReflectFunc);
        }
        m_Container.ActivateModelDefault();
    }
}