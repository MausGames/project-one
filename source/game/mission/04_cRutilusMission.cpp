///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO: effect if player has overridden control 


// ****************************************************************
// constructor
cRutilusMission::cRutilusMission()noexcept
: m_avTeleporterPos {}
, m_iMoveFlip       (0u)
, m_fAnimation      (0.0f)
{
    // 
    m_apBoss[0] = &m_Quaternion;
    m_apBoss[1] = &m_Saros;
    m_apBoss[2] = &m_Messier;

    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
    {
        m_aTeleporter[i].DefineModel  ("object_teleporter.md3");
        m_aTeleporter[i].DefineTexture(0u, "effect_energy.png");
        m_aTeleporter[i].DefineProgram("effect_energy_flat_invert_program");
        m_aTeleporter[i].SetColor3    ((i ? COLOR_ENERGY_BLUE : COLOR_ENERGY_ORANGE) * 1.2f);
        m_aTeleporter[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// destructor
cRutilusMission::~cRutilusMission()
{
    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) this->DisableTeleporter(i, false);
}


// ****************************************************************
// 
void cRutilusMission::EnableTeleporter(const coreUintW iIndex, const coreVector2& vPosition)
{
    ASSERT(iIndex < RUTILUS_TELEPORTER)
    coreObject3D& oTeleporter = m_aTeleporter[iIndex];

    // 
    WARN_IF(oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oTeleporter.ChangeType(TYPE_RUTILUS_TELEPORTER);

    // 
    oTeleporter.SetPosition (coreVector3(vPosition,0.0f));
    oTeleporter.SetSize     (coreVector3(0.0f,0.0f,0.0f));
    oTeleporter.SetDirection(coreVector3(0.0f,1.0f,0.0f));
    oTeleporter.SetAlpha    (0.0f);
    oTeleporter.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&oTeleporter);
}


// ****************************************************************
// 
void cRutilusMission::DisableTeleporter(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < RUTILUS_TELEPORTER)
    coreObject3D& oTeleporter = m_aTeleporter[iIndex];

    // 
    if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oTeleporter.ChangeType(0);

    // 
    if(!bAnimated)
    {
        oTeleporter.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&oTeleporter);
    }
}


// ****************************************************************
// 
void cRutilusMission::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) m_aTeleporter[i].Render();
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aTeleporter[i]);
}


// ****************************************************************
// 
void cRutilusMission::__MoveOwnBefore()
{
    
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
    {
        coreObject3D& oTeleporter = m_aTeleporter[i];
        if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
        
        m_avTeleporterPos[i] = m_aTeleporter[i].GetPosition().xy();
    }
    
    
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        // 
        const coreVector2 vInputMove = pPlayer->GetInput()->vMove;
        if(vInputMove.IsNull()) m_iMoveFlip = 0u;

        // 
        if(m_iMoveFlip) c_cast<sGameInput*>(pPlayer->GetInput())->vMove = cRutilusMission::__GetFlipDirection(vInputMove, m_iMoveFlip);
    });
    
    
}


// ****************************************************************
// 
void cRutilusMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
    {
        coreObject3D& oTeleporter = m_aTeleporter[i];
        if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(oTeleporter.GetType()) oTeleporter.SetAlpha(MIN(oTeleporter.GetAlpha() + 5.0f*Core::System->GetTime(), 1.0f));
                             else oTeleporter.SetAlpha(MAX(oTeleporter.GetAlpha() - 5.0f*Core::System->GetTime(), 0.0f));

        // 
        if(!oTeleporter.GetAlpha()) this->DisableTeleporter(i, false);

        // 
        oTeleporter.SetTexSize  (coreVector2(oTeleporter.GetSize().x * 0.075f, 0.125f));
        oTeleporter.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.6f));  
        oTeleporter.Move();
    }
    
    
    
    
    // 
    const auto nTeleportFunc = [this](const coreObject3D* pObject, const coreVector2 vMove, coreVector2* OUTPUT vNewPos, coreVector2* OUTPUT vNewDir)
    {
        ASSERT(vNewPos && vNewDir)

        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
        {
            coreObject3D& oTeleporter = m_aTeleporter[i];
            if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreVector2 vMoveDir1 = vMove.IsNull() ? coreVector2(0.0f,0.0f) : vMove.Normalized();
            const coreVector2 vMoveDir2 = vMove.IsNull() ? pObject->GetDirection().xy() : vMove.Normalized();
            
            const coreVector2 vTeleporterMove = (oTeleporter.GetPosition().xy() - m_avTeleporterPos[i]);

            const coreVector2 vPos1 = pObject->GetPosition().xy() + 1.0f*vMoveDir1 * pObject->GetCollisionRadius(); // max tip
            const coreVector2 vPos2 = pObject->GetPosition().xy() - vMove       +vTeleporterMove*2.0f; // min center, center to center  (2.0f* to account for rotation (heuristic))

            const coreVector2 vDiff1 = vPos1 - oTeleporter.GetPosition().xy();
            const coreVector2 vDiff2 = vPos2 - oTeleporter.GetPosition().xy();

            const coreFloat fDot1 = coreVector2::Dot(vDiff1,                   oTeleporter.GetDirection().xy());
            const coreFloat fDot2 = coreVector2::Dot(vDiff2,                   oTeleporter.GetDirection().xy());
            const coreFloat fDist = coreVector2::Dot((vDiff1 + vDiff2) * 0.5f, oTeleporter.GetDirection().xy().Rotated90());   // # approximation

            if((SIGN(fDot1) != SIGN(fDot2)) && (ABS(fDist) <= oTeleporter.GetSize().x))
            {
                coreObject3D& oOther = m_aTeleporter[1u - i];
                STATIC_ASSERT(RUTILUS_TELEPORTER == 2u)

                const coreMatrix2 mRota  = coreMatrix3::Rotation(oTeleporter.GetDirection().xy()).m12().Inverted() *
                                           coreMatrix3::Rotation(oOther     .GetDirection().xy()).m12();

                (*vNewPos) = vDiff1   * mRota + oOther.GetPosition().xy();
                (*vNewDir) = vMoveDir2 * mRota;
                
                const coreVector2 vTeleporterMove2 = (oOther.GetPosition().xy() - m_avTeleporterPos[1u - i]);
                (*vNewPos) += (vMove * mRota  - vTeleporterMove2*2.0f) * 1.0f;

                g_pSpecialEffects->CreateSplashColor(coreVector3(fDist * oTeleporter.GetDirection().xy().Rotated90() + oTeleporter.GetPosition().xy(), 0.0f), 5.0f, 3u, oTeleporter.GetColor3() / 1.2f);
                g_pSpecialEffects->CreateSplashColor(coreVector3(fDist * oOther     .GetDirection().xy().Rotated90() + oOther     .GetPosition().xy(), 0.0f), 5.0f, 3u, oOther     .GetColor3() / 1.2f);

                return true;
            }
        }

        return false;
    };

    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        
        // 
        if(m_iMoveFlip) c_cast<sGameInput*>(pPlayer->GetInput())->vMove = cRutilusMission::__GetFlipDirection(pPlayer->GetInput()->vMove, 4u - m_iMoveFlip);

        if(pPlayer->IsRolling()) return;

        coreVector2 vNewPos, vNewDir;
        if(nTeleportFunc(pPlayer, pPlayer->GetMove(), &vNewPos, &vNewDir))
        {
            if(!pPlayer->GetMove().IsNull())
            {
                const coreUint8 iNewMoveFlip = cRutilusMission::__GetFlipType(pPlayer->GetMove().Normalized(), vNewDir);

                m_iMoveFlip = (m_iMoveFlip + iNewMoveFlip) % 4u;

                vNewDir = cRutilusMission::__GetFlipDirection(pPlayer->GetDirection().xy(), iNewMoveFlip);
            }

            // 
            pPlayer->SetPosition (coreVector3(vNewPos, 0.0f));
            pPlayer->SetDirection(coreVector3(vNewDir, 0.0f));
            pPlayer->coreObject3D::Move();
        }
    });

    // 
    g_pGame->GetEnemyManager()->ForEachEnemy([&](cEnemy* OUTPUT pEnemy)
    {
        // 
        coreVector2 vNewPos, vNewDir;
        //if(nTeleportFunc(pEnemy, (pEnemy->GetMove().IsNull() ? (pEnemy->GetDirection().xy() * 30.0f * Core::System->GetTime()) : pEnemy->GetMove()), &vNewPos, &vNewDir))
        if(nTeleportFunc(pEnemy, pEnemy->GetMove(), &vNewPos, &vNewDir))
        {
            // 
            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
            pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
            pEnemy->coreObject3D::Move();
        }
    });

    // 
    const auto nTeleportBulletFunc = [&](cBullet* OUTPUT pBullet)
    {
        // 
        coreVector2 vNewPos, vNewDir;
        if(nTeleportFunc(pBullet, pBullet->GetFlyMove(), &vNewPos, &vNewDir))
        {
            // 
            if(pBullet->GetFlyTime() >= 2.0f) pBullet->Deactivate(true);

            // 
            pBullet->SetFade  (0.0f);
            pBullet->SetFlyDir(vNewDir);
            pBullet->Move();   // # full move

            // 
            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
            pBullet->coreObject3D::Move();
        }
    };
    g_pGame->GetBulletManagerPlayer()->ForEachBullet(nTeleportBulletFunc);
    g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nTeleportBulletFunc);
}


// ****************************************************************
// 
coreUint8 cRutilusMission::__GetFlipType(const coreVector2& vOldDir, const coreVector2& vNewDir)
{
    ASSERT(vOldDir.IsNormalized() && vNewDir.IsNormalized())

    const coreFloat fDotA = coreVector2::Dot(vOldDir, vNewDir);

    if(fDotA >  0.5f) return 0u;
    if(fDotA < -0.5f) return 2u;

    const coreFloat fDotB = coreVector2::Dot(vOldDir, vNewDir.Rotated90());

    return (fDotB < 0.0f) ? 1u : 3u;
}


// ****************************************************************
// 
coreVector2 cRutilusMission::__GetFlipDirection(const coreVector2& vDir, const coreUint8 iType)
{
    switch(iType)
    {
    default: ASSERT(false)
    case 0u: return  vDir;
    case 1u: return  vDir.Rotated90();
    case 2u: return -vDir;
    case 3u: return -vDir.Rotated90();
    }
}