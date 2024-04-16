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
, m_Wave              (RUTILUS_WAVES_RAWS)
, m_afWaveTime        {}
, m_iWaveActive       (0u)
, m_iWaveDir          (0u)
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

    // 
    m_Wave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < RUTILUS_WAVES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pWave = &m_aWaveRaw[i];
            //pWave->DefineModel  ("object_tube_open.md3");
            pWave->DefineModel  ("object_sphere.md3");
            pWave->DefineTexture(0u, "effect_energy.png");
            pWave->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pWave->SetColor3 (COLOR_ENERGY_RED * 0.7f);
            //pWave->SetTexSize(coreVector2(0.6f,2.4f));
            pWave->SetTexSize(coreVector2(0.6f,2.4f) * 5.0f);
            pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Wave.BindObject(pWave);
        }
    }

    // 
    g_pGlow->BindList(&m_Wave);
}


// ****************************************************************
// destructor
cRutilusMission::~cRutilusMission()
{
    // 
    g_pGlow->UnbindList(&m_Wave);

    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) this->DisableTeleporter(i, false);
    this->DisableWave(false);
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
    if(bAnimated) cRutilusMission::__TeleporterEffect(iIndex);
}


// ****************************************************************
// 
void cRutilusMission::EnableWave()
{
    coreObject3D& oWave = m_aWaveRaw[0];

    // 
    WARN_IF(oWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iWaveActive, RUTILUS_WAVES)
    m_iWaveDir = 0u;

    // 
    const auto nInitFunc = [this](coreObject3D* OUTPUT pObject, const coreFloat fOffset, const coreUintW iIndex)
    {
        m_afWaveTime[iIndex] = fOffset;

        pObject->SetAlpha    (0.0f);
        pObject->SetTexOffset(coreVector2(0.0f, fOffset));
        pObject->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    };
    for(coreUintW i = 0u; i < RUTILUS_WAVES; ++i) nInitFunc(&oWave + i, I_TO_F(i) / I_TO_F(RUTILUS_WAVES), i);
}


// ****************************************************************
// 
void cRutilusMission::DisableWave(const coreBool bAnimated)
{
    coreObject3D& oWave = m_aWaveRaw[0];

    // 
    if(!oWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iWaveActive, RUTILUS_WAVES)

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        for(coreUintW i = 0u; i < RUTILUS_WAVES; ++i) nExitFunc(&oWave + i);
    }
}


// ****************************************************************
// 
void cRutilusMission::__RenderOwnBottom()
{
    // 
    m_Wave.Render();
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

    // 
    for(coreUintW i = 0u; i < RUTILUS_WAVES; ++i)
    {
        coreObject3D& oWave = m_aWaveRaw[i];
        if(!oWave.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector2 vDir  = CONTAINS_BIT(m_iWaveDir, 0u) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);
        const coreFloat   fSign = CONTAINS_BIT(m_iWaveDir, 1u) ? -1.0f : 1.0f;

        // 
        //m_afWaveTime[i].Update(0.7f * fSign);
        m_afWaveTime[i].Update(0.5f * fSign);
        if((m_afWaveTime[i] < 0.0f) || (m_afWaveTime[i] > 1.0f))
        {
            // 
            m_afWaveTime[i] -= fSign;
            SET_BIT(m_iWaveActive, i, CONTAINS_BIT(m_iWaveActive, RUTILUS_WAVES))

            // 
            oWave.SetDirection(coreVector3(vDir, 0.0f));
        }

        // 
        oWave.SetPosition(m_aWaveRaw[0].GetPosition());
        //oWave.SetSize    (coreVector3(15.0f * m_afWaveTime[i], 80.0f, 1.0f));
        oWave.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 30.0f * m_afWaveTime[i]);
        oWave.SetAlpha   (MIN(1.0f - m_afWaveTime[i], 6.0f * m_afWaveTime[i], 1.0f) * (CONTAINS_BIT(m_iWaveActive, i) ? 1.0f : 0.0f));
    }

    // 
    if(!m_iWaveActive) this->DisableWave(false);

    // 
    m_Wave.MoveNormal();

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
            if(m_aiMoveFlip[i]) pInput->vMove = MapToAxisInv(pInput->vMove, UnpackDirection(m_aiMoveFlip[i]));

            // 
            coreVector2 vNewPos, vNewDir, vNewMoveDir;
            if(nTeleportFunc(pPlayer, pPlayer->GetMove(), &vNewPos, &vNewDir, &vNewMoveDir))
            {
                // 
                if(!vNewMoveDir.IsNull()) m_aiMoveFlip[i] = PackDirection(MapToAxisInv(vNewMoveDir, pInput->vMove));

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
void cRutilusMission::__TeleporterEffect(const coreUintW iIndex)const
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