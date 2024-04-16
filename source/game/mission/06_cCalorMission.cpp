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
cCalorMission::cCalorMission()noexcept
: m_Load           (CALOR_LOADS)
, m_pLoadOwner     (NULL)
, m_afLoadPower    {}
, m_Star           (CALOR_STARS)
, m_StarChain      (CALOR_STARS * CALOR_CHAINS)
, m_apStarOwner    {}
, m_avStarOffset   {}
, m_iStarState     (0u)
, m_fSwingStart    (0.0f)
, m_afSwingValue   {}
, m_apCatchObject  {}
, m_avCatchPos     {}
, m_avCatchDir     {}
, m_fCatchTransfer (0.0f)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Zeroth;

    // 
    m_Load.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_LOADS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pLoad = &m_aLoadRaw[i];
            pLoad->DefineModel  ("object_sphere.md3");
            pLoad->DefineTexture(0u, "effect_energy.png");
            pLoad->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLoad->SetTexSize(coreVector2(3.0f,3.0f));
            pLoad->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Load.BindObject(pLoad);
        }
    }

    // 
    m_LoadCopy.DefineTexture(0u, "effect_energy.png");
    m_LoadCopy.DefineProgram("effect_energy_invert_program");
    m_LoadCopy.SetColor3    (COLOR_ENERGY_RED);
    m_LoadCopy.SetTexSize   (coreVector2(3.0f,3.0f));
    m_LoadCopy.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Star     .DefineProgram("effect_energy_flat_invert_inst_program");
    m_StarChain.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_STARS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (CALOR_CHAINS + 1u);

            // load object resources
            coreObject3D* pStar = &m_aStarRaw[i];
            pStar->DefineModel  (iType ? "object_sphere.md3" : "object_star.md3");
            pStar->DefineTexture(0u, "default_black.png");
            pStar->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pStar->SetSize   ((iType ? coreVector3(0.7f,0.7f,0.7f) : coreVector3(1.5f,1.5f,1.5f)) * 1.3f);
            pStar->SetColor3 (COLOR_ENERGY_WHITE * 0.8f);
            pStar->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_StarChain.BindObject(pStar);
                 else m_Star     .BindObject(pStar);
        }
    }

    // 
    m_Boulder.DefineModelHigh("ship_meteor.md3");
    m_Boulder.DefineModelLow ("ship_meteor.md3");
    m_Boulder.DefineTexture  (0u, "environment_stone_diff.png");
    m_Boulder.DefineTexture  (1u, "environment_stone_norm.png");
    m_Boulder.DefineProgram  ("object_meteor_blink_program");
    m_Boulder.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
    m_Boulder.Configure      (50, coreVector3(1.0f,1.0f,1.0f));
    m_Boulder.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_GHOST | ENEMY_STATUS_WORTHLESS);

    // 
    g_pGlow->BindList(&m_Load);
    g_pGlow->BindList(&m_Star);
    g_pGlow->BindList(&m_StarChain);
}


// ****************************************************************
// destructor
cCalorMission::~cCalorMission()
{
    // 
    m_Snow.Disable(0.0f);

    // 
    m_Boulder.Kill(false);

    // 
    g_pGlow->UnbindList(&m_Load);
    g_pGlow->UnbindList(&m_Star);
    g_pGlow->UnbindList(&m_StarChain);

    // 
    this->DisableLoad(false);
    for(coreUintW i = 0u; i < CALOR_STARS; ++i) this->DisableStar(i, false);
}


// ****************************************************************
// 
void cCalorMission::EnableLoad(const cShip* pOwner)
{
    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    WARN_IF(oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pLoadOwner     = pOwner;
    m_afLoadPower[0] = 0.0f;
    m_afLoadPower[1] = 0.0f;
    m_afLoadPower[2] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nInitFunc(&oLoad + i);
    nInitFunc(&m_LoadCopy);

    // 
    m_LoadCopy.DefineModel(pOwner->GetModelHigh());
    g_pGlow->BindObject(&m_LoadCopy);
}


// ****************************************************************
// 
void cCalorMission::DisableLoad(const coreBool bAnimated)
{
    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pLoadOwner = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [&](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nExitFunc(&oLoad + i);
        nExitFunc(&m_LoadCopy);

        // 
        m_LoadCopy.DefineModel(NULL);
        g_pGlow->UnbindObject(&m_LoadCopy);
    }
}


// ****************************************************************
// 
void cCalorMission::EnableStar(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vOffset)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    WARN_IF(pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apStarOwner [iIndex] = pOwner;
    m_avStarOffset[iIndex] = vOffset;

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
            pObject->SetPosition(coreVector3(HIDDEN_POS + 1.0f, 0.0f));             
    };
    nInitFunc(pStar);
    for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nInitFunc(pChain + i);
}


// ****************************************************************
// 
void cCalorMission::DisableStar(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar  = (*m_Star     .List())[iIndex];
    coreObject3D* pChain = (*m_StarChain.List())[iIndex*CALOR_CHAINS];

    // 
    if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apStarOwner[iIndex] = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        nExitFunc(pStar);
        for(coreUintW i = 0u; i < CALOR_CHAINS; ++i) nExitFunc(pChain + i);
    }
}

// ****************************************************************
// 
void cCalorMission::StartSwing()
{
    // 
    m_iStarState  = BITLINE(CALOR_STARS);
    m_fSwingStart = 0.0f;

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(m_apStarOwner[i])
        {
            const coreObject3D* pStar = (*m_Star.List())[i];
            m_afSwingValue[i] = (pStar->GetPosition().xy() - m_apStarOwner[i]->GetPosition().xy()).Angle();
        }
    }
}


// ****************************************************************
// 
void cCalorMission::CatchObject(const coreUintW iIndex, cShip* pObject)
{
    ASSERT(iIndex < CALOR_STARS)
    coreObject3D* pStar = (*m_Star.List())[iIndex];

    // 
    ASSERT(m_apStarOwner[iIndex])
    const coreVector2 vDir = (pStar->GetPosition().xy() - m_apStarOwner[iIndex]->GetPosition().xy()).Normalized();

    // 
    ASSERT(!m_apCatchObject[iIndex])
    m_apCatchObject[iIndex] = pObject;
    m_avCatchPos   [iIndex] = MapToAxisInv(pObject->GetPosition ().xy() - pStar->GetPosition().xy(), vDir);
    m_avCatchDir   [iIndex] = MapToAxisInv(pObject->GetDirection().xy(),                             vDir);
}

void cCalorMission::UncatchObject(const coreUintW iIndex)
{
    ASSERT(iIndex < CALOR_STARS)

    // 
    m_apCatchObject[iIndex] = NULL;
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Snow.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_StarChain.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_StarChain);

    DEPTH_PUSH

    // 
    m_Star.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Star);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_LoadCopy.Render();
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    m_Load.Render();
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnMiddle()
{
    // 
    m_fSwingStart   .UpdateMin( 0.2f, 1.0f);
    m_fCatchTransfer.UpdateMax(-1.0f, 0.0f);

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        coreObject3D* pStar = (*m_Star.List())[i];
        if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afSwingValue[i].UpdateMod(7.0f * m_fSwingStart, 2.0f*PI);

        // 
        const cShip* pOwner = m_apStarOwner[i];
        if(pOwner)
        {
            cPlayer* pPlayer = g_pGame->GetPlayer(i);

            // 
            if(pPlayer == pOwner)
            {
                if(HAS_BIT(m_iStarState, i))
                {
                    const coreVector2 vDir = coreVector2::Direction(m_afSwingValue[i]) * coreVector2(i ? -1.0f : 1.0f, 1.0f);

                    // 
                    pStar->SetPosition (coreVector3(pPlayer->GetPosition().xy() + vDir * CALOR_CHAIN_CONSTRAINT1, 0.0f));
                    pStar->SetDirection(coreVector3(vDir, 0.0f));

                    // 
                    pPlayer->SetSpeed(m_fSwingStart);
                }
                else
                {
                    const coreVector2 vBase = pPlayer->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pPlayer->GetDirection().xy());
                    const coreVector2 vDiff = vBase - pStar->GetPosition().xy();

                    // 
                    pPlayer->SetPosition(coreVector3(pStar->GetPosition().xy() + vDiff.Normalized() * MIN(vDiff.Length(), CALOR_CHAIN_CONSTRAINT1), 0.0f));
                }
            }
            else
            {
                const coreVector2 vBase = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());
                const coreVector2 vDiff = vBase - pStar->GetPosition().xy();

                // 
                pStar->SetDirection(coreVector3(vDiff.Normalized(), 0.0f));
            }

            // 
            const coreVector2 vBase = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());   // # again
            const coreVector2 vDiff = vBase - pStar->GetPosition().xy();
            const coreVector2 vDir  = vDiff.Normalized();

            // 
            if(pPlayer == pOwner)
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_DEAD))
                {
                    // 
                    Core::Manager::Object->TestCollision(TYPE_ENEMY, pStar, [&](cEnemy* OUTPUT pEnemy, const coreObject3D* pStar, const coreVector3 vIntersection, const coreBool bFirstHit)
                    {
                        if(m_apCatchObject[i] || pEnemy->HasStatus(ENEMY_STATUS_BOSS) || pEnemy->IsChild()) return;

                        if(pEnemy == m_apCatchObject[1u - i])
                        {
                            // 
                            if(!m_fCatchTransfer)
                            {
                                m_fCatchTransfer = 0.5f;

                                // 
                                this->UncatchObject(1u - i);
                                this->CatchObject(i, pEnemy);
                            }
                        }
                        else
                        {
                            // 
                            this->CatchObject(i, pEnemy);
                        }
                    });
                }

                if(m_apCatchObject[i])
                {
                    cEnemy* pCopy = d_cast<cEnemy*>(m_apCatchObject[i]);

                    // 
                    pCopy->SetPosition (coreVector3(MapToAxis(m_avCatchPos[i], -vDir), 0.0f) + pStar->GetPosition());
                    pCopy->SetDirection(coreVector3(MapToAxis(m_avCatchDir[i], -vDir), 0.0f));

                    // 
                    Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pCopy, [](cBullet* OUTPUT pBullet, const cEnemy* pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
                    {
                        pBullet->Deactivate(true, vIntersection.xy());
                    });

                    // 
                    Core::Manager::Object->TestCollision(TYPE_ENEMY, pCopy, [&](cEnemy* OUTPUT pEnemy, cEnemy* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
                    {
                        if(!m_apCatchObject[i]) return;

                        // 
                        const coreBool bEnemyBig  = (pEnemy ->GetMaxHealth() >= 10);
                        const coreBool bObjectBig = (pObject->GetMaxHealth() >= 10);

                        if(!bEnemyBig || bObjectBig)
                        {
                            const coreBool bOther = (pEnemy == m_apCatchObject[1u - i]);

                            // 
                            pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                            pEnemy->TakeDamage  (1000, ELEMENT_NEUTRAL, vIntersection.xy(), bOther ? g_pGame->GetPlayer(1u - i) : pPlayer);

                            // 
                            if(bOther) this->UncatchObject(1u - i);
                        }

                        if(!bObjectBig || bEnemyBig)
                        {
                            // 
                            pObject->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                            pObject->TakeDamage  (1000, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer);

                            // 
                            this->UncatchObject(i);
                        }
                    });
                }
            }
            else
            {
                if(m_apCatchObject[i])
                {
                    cShip* pCopy = m_apCatchObject[i];

                    // 
                    pCopy->SetPosition (coreVector3(MapToAxis(m_avCatchPos[i], -vDir), 0.0f) + pStar->GetPosition());
                    pCopy->SetDirection(coreVector3(MapToAxis(m_avCatchDir[i], -vDir), 0.0f));
                }
            }

            STATIC_ASSERT(CALOR_STARS == 2u)
        }
    }

    // 
    m_Star.MoveNormal();
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_Snow.Move();

    // 
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i)
    {
        coreObject3D& oLoad = m_aLoadRaw[i];
        if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vDir    = coreVector2::Direction((I_TO_F(i) / I_TO_F(CALOR_LOADS)) * (2.0f*PI)).InvertedX();
        const coreFloat   fScale  = CLAMP(m_afLoadPower[0] - I_TO_F(i), 0.0f, 1.0f);
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/12.0f);

        // 
        const cShip* pOwner = m_pLoadOwner;
        if(pOwner)
        {
            // 
            oLoad.SetPosition(coreVector3(pOwner->GetPosition().xy() + vDir * (pOwner->GetVisualRadius() + 3.5f), 0.0f));
            oLoad.SetAlpha   (1.0f);
        }
        else
        {
            // 
            oLoad.SetPosition(coreVector3(oLoad.GetPosition().xy() + vDir * (50.0f * TIME), 0.0f));
            oLoad.SetAlpha   (MAX(oLoad.GetAlpha() - 3.0f * TIME, 0.0f));

            // 
            if(!i && !oLoad.GetAlpha()) this->DisableLoad(false);
        }

        // 
        oLoad.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.7f * fScale);
        oLoad.SetColor3   (COLOR_ENERGY_RED * ((fScale >= 1.0f) ? 1.0f : 0.5f));
        oLoad.SetTexOffset(coreVector2(0.0f, FRACT(-2.0f * m_fAnimation + fOffset)));
    }

    // 
    m_Load.MoveNormal();

    // 
    if(m_pLoadOwner)
    {
        // 
        m_LoadCopy.SetPosition   (m_pLoadOwner->GetPosition   ());
        m_LoadCopy.SetSize       (m_pLoadOwner->GetSize       ());
        m_LoadCopy.SetDirection  (m_pLoadOwner->GetDirection  ());
        m_LoadCopy.SetOrientation(m_pLoadOwner->GetOrientation());
        m_LoadCopy.SetTexOffset  (coreVector2(FRACT(1.4f * m_fAnimation), 0.0f));   // TODO 1: fix model tex coords

        // 
        if(F_TO_UI(m_afLoadPower[0]) != F_TO_UI(m_afLoadPower[1]))
        {
            const coreUintW iIndex = F_TO_UI(m_afLoadPower[1]);
            ASSERT(iIndex < CALOR_LOADS)

            g_pSpecialEffects->CreateSplashColor(m_aLoadRaw[iIndex].GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
        }
    }
    m_LoadCopy.SetAlpha(m_afLoadPower[2] * m_aLoadRaw[0].GetAlpha());
    m_LoadCopy.Move();

    // 
    m_afLoadPower[0].UpdateMax(-0.6f, FLOOR(m_afLoadPower[0]));
    m_afLoadPower[1] = m_afLoadPower[0];
    m_afLoadPower[2].UpdateMax(-1.0f, 0.0f);

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        const coreObject3D* pStar = (*m_Star.List())[i];
        if(!pStar->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apStarOwner[i];
        if(pOwner)
        {
            const cPlayer* pPlayer = g_pGame->GetPlayer(i);

            // 
            const coreVector2 vBase    = pOwner->GetPosition().xy() + MapToAxis(m_avStarOffset[i], pOwner->GetDirection().xy());
            const coreVector2 vDiff    = vBase - pStar->GetPosition().xy();
            const coreVector2 vDir     = vDiff.Normalized();
            const coreFloat   fLen     = vDiff.Length();
            const coreFloat   fTension = STEPH3(CALOR_CHAIN_CONSTRAINT2 - 5.0f, CALOR_CHAIN_CONSTRAINT2, (pPlayer == pOwner) ? fLen : 0.0f);

            // 
            for(coreUintW j = 0u; j < CALOR_CHAINS; ++j)
            {
                coreObject3D* pChain = (*m_StarChain.List())[i*CALOR_CHAINS + j];

                // 
                const coreFloat   fOffset = MIN(I_TO_F(j) * 2.5f + 4.7f, fLen);
                const coreVector2 vPos    = vBase - vDir * fOffset;

                // 
                pChain->SetPosition(coreVector3(vPos, 0.0f));
                pChain->SetColor3  (LERP(COLOR_ENERGY_WHITE * 0.8f, COLOR_ENERGY_RED * 0.8f, fTension));
                pChain->SetAlpha   (STEPH3(1.7f, 2.2f, fLen - fOffset));
                pChain->SetEnabled (pChain->GetAlpha() ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            }
        }
    }

    // 
    m_StarChain.MoveNormal();
}