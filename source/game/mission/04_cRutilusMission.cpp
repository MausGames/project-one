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
cRutilusMission::cRutilusMission()noexcept
: m_avTeleporterPrev  {}
, m_iTeleporterActive (2u)
, m_aiMoveFlip        {}
, m_fAnimation        (0.0f)
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
        m_aTeleporter[i].SetColor3    (RUTILUS_TELEPORTER_COLOR(i) * 1.2f);
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
void cRutilusMission::EnableTeleporter(const coreUintW iIndex)
{
    ASSERT(iIndex < RUTILUS_TELEPORTER)
    coreObject3D& oTeleporter = m_aTeleporter[iIndex];

    // 
    WARN_IF(oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oTeleporter.ChangeType(TYPE_RUTILUS_TELEPORTER);

    // 
    oTeleporter.SetEnabled(CORE_OBJECT_ENABLE_ALL);
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
    oTeleporter.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&oTeleporter);

    // 
    if(bAnimated) cRutilusMission::__LineEffect(iIndex);
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
    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
    {
        const coreObject3D& oTeleporter = m_aTeleporter[i];
        if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_avTeleporterPrev[i] = m_aTeleporter[i].GetPosition().xy();
    }

    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        sGameInput* pInput = c_cast<sGameInput*>(pPlayer->GetInput());

        // 
        if(pInput->vMove.IsNull()) m_aiMoveFlip[i] = 0u;

        // 
        if(m_aiMoveFlip[i]) pInput->vMove = MapToAxis(pInput->vMove, UnpackDirection(m_aiMoveFlip[i]));
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
        oTeleporter.SetAlpha    (m_iTeleporterActive ? 1.0f : 0.5f);
        oTeleporter.SetTexSize  (coreVector2(0.075f,0.125f) * oTeleporter.GetSize().xy());
        oTeleporter.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.6f));
        oTeleporter.Move();
    }

    if(m_iTeleporterActive == 1u)
    {
        // 
        const auto nTeleportFunc = [this](const coreObject3D* pObject, const coreVector2& vMove, coreVector2* OUTPUT vNewPos, coreVector2* OUTPUT vNewDir, coreVector2* OUTPUT vNewMoveDir)
        {
            ASSERT(pObject && vNewPos && vNewDir && vNewMoveDir)

            // 
            if(!m_aTeleporter[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) return false;
            STATIC_ASSERT(RUTILUS_TELEPORTER == 2u)

            // 
            const coreVector2 vMoveDir = vMove.IsNull() ? coreVector2(0.0f,0.0f) : vMove.Normalized();

            // 
            for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            {
                const coreVector2 vPos1 = m_aTeleporter[i].GetPosition ().xy();
                const coreVector2 vDir1 = m_aTeleporter[i].GetDirection().xy();

                const coreVector2 vFullMove1 = vMove - (vPos1 - m_avTeleporterPrev[i]);

                const coreVector2 vDiff     = pObject->GetPosition().xy() - vPos1;
                const coreVector2 vDiffTo   = vDiff + vMoveDir * pObject->GetCollisionRadius();
                const coreVector2 vDiffFrom = vDiff - vFullMove1;

                const coreFloat fOffset  = coreVector2::Dot(vDiff,     vDir1.Rotated90());
                const coreFloat fDotTo   = coreVector2::Dot(vDiffTo,   vDir1);
                const coreFloat fDotFrom = coreVector2::Dot(vDiffFrom, vDir1);

                // 
                if((ABS(fOffset) <= m_aTeleporter[i].GetSize().x) && (SIGN(fDotTo) != SIGN(fDotFrom)))
                {
                    const coreVector2 vPos2 = m_aTeleporter[1u - i].GetPosition ().xy();
                    const coreVector2 vDir2 = m_aTeleporter[1u - i].GetDirection().xy();

                    const coreMatrix2 mRota = coreMatrix3::Rotation(vDir1).m12().Transposed() *
                                              coreMatrix3::Rotation(vDir2).m12();

                    const coreVector2 vHit1 = fOffset * vDir1.Rotated90() + vPos1;
                    const coreVector2 vHit2 = fOffset * vDir2.Rotated90() + vPos2;

                    const coreVector2 vFullMove2 = vMove * mRota - (vPos2 - m_avTeleporterPrev[1u - i]);

                    // 
                    (*vNewPos)     = vHit2 + vFullMove2;
                    (*vNewDir)     = pObject->GetDirection().xy() * mRota;
                    (*vNewMoveDir) = vMoveDir                     * mRota;

                    // 
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vHit1, 0.0f), 5.0f, 3u, RUTILUS_TELEPORTER_COLOR(i));
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vHit2, 0.0f), 5.0f, 3u, RUTILUS_TELEPORTER_COLOR(1u - i));

                    return true;
                }
            }

            return false;
        };

        // 
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            sGameInput* pInput = c_cast<sGameInput*>(pPlayer->GetInput());

            // 
            if(pPlayer->IsRolling()) return;

            // 
            if(m_aiMoveFlip[i]) pInput->vMove = MapToAxis(pInput->vMove, UnpackDirection(m_aiMoveFlip[i]).InvertedX());

            // 
            coreVector2 vNewPos, vNewDir, vNewMoveDir;
            if(nTeleportFunc(pPlayer, pPlayer->GetMove(), &vNewPos, &vNewDir, &vNewMoveDir))
            {
                // 
                if(!vNewMoveDir.IsNull()) m_aiMoveFlip[i] = PackDirection(MapToAxis(vNewMoveDir, pInput->vMove.InvertedX()));

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
            coreVector2 vNewPos, vNewDir, vNewMoveDir;
            if(nTeleportFunc(pEnemy, pEnemy->GetMove(), &vNewPos, &vNewDir, &vNewMoveDir))
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
            coreVector2 vNewPos, vNewDir, vNewMoveDir;
            if(nTeleportFunc(pBullet, pBullet->GetFlyMove(), &vNewPos, &vNewDir, &vNewMoveDir))
            {
                // 
                if(pBullet->GetFlyTime() >= 3.0f) pBullet->Deactivate(true);

                // 
                pBullet->SetFade  (0.0f);
                pBullet->SetFlyDir(vNewMoveDir);
                pBullet->Move();   // # full move, for fade

                // 
                pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
                pBullet->coreObject3D::Move();
            }
        };
        g_pGame->GetBulletManagerPlayer()->ForEachBullet(nTeleportBulletFunc);
        g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nTeleportBulletFunc);
    }
}


// ****************************************************************
// 
void cRutilusMission::__LineEffect(const coreUintW iIndex)const
{
    ASSERT(iIndex < RUTILUS_TELEPORTER)
    const coreObject3D& oTeleporter = m_aTeleporter[iIndex];

    // 
    const coreVector3 vPos   = oTeleporter.GetPosition ();
    const coreVector3 vDir   = oTeleporter.GetDirection().RotatedZ90();
    const coreVector3 vColor = RUTILUS_TELEPORTER_COLOR(iIndex);

    // 
    for(coreUintW j = 60u; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (2.0f * I_TO_F(j - 30u)), 10.0f, 1u, vColor);
}