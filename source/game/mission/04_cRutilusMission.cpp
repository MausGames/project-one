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
cRutilusMission::cRutilusMission()noexcept
: m_avTeleporterPrev  {}
, m_avTeleporterRota  {}
, m_iTeleporterActive (2u)
, m_Plate             (RUTILUS_PLATES_RAWS)
, m_afPlateTime       {}
, m_avPlateData       {}
, m_iPlateRotated     (0u)
, m_fAreaTime         (0.0f)
, m_fAreaScale        (1.0f)
, m_fSafeTime         (0.0f)
, m_Wave              (RUTILUS_WAVES_RAWS)
, m_afWaveTime        {}
, m_vWavePos          (HIDDEN_POS)
, m_iWaveDir          (0u)
, m_iWaveActive       (0u)
, m_iWaveType         (0u)
, m_aiMoveFlip        {}
, m_fAnimation        (0.0f)
{
    // 
    m_apBoss[0] = &m_Messier;

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
    m_Plate.DefineProgram("object_plate_inst_program");
    {
        for(coreUintW i = 0u; i < RUTILUS_PLATES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pPlate = &m_aPlateRaw[i];
            pPlate->DefineModel  (Core::Manager::Object->GetLowQuad());
            pPlate->DefineTexture(0u, "effect_arrow.png");
            pPlate->DefineTexture(1u, "menu_background_black.png");
            pPlate->DefineProgram("object_plate_program");

            // set object properties
            pPlate->SetColor3 ((i % 2u) ? COLOR_ENERGY_BLUE : COLOR_ENERGY_RED);
            pPlate->SetAlpha  (0.7f);
            pPlate->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Plate.BindObject(pPlate);
        }
    }

    // 
    for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i)
    {
        // 
        m_aArea[i].DefineModel  ("object_sphere.md3");
        m_aArea[i].DefineTexture(0u, "effect_energy.png");
        m_aArea[i].DefineProgram(i ? "effect_energy_flat_spheric_program" : "effect_energy_flat_program");
        m_aArea[i].SetColor3    (COLOR_ENERGY_PURPLE * (i ? 1.0f : 0.6f));
        m_aArea[i].SetTexSize   (coreVector2(2.4f,0.4f) * 5.0f);
        m_aArea[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Safe.DefineModel  ("object_sphere.md3");
    m_Safe.DefineTexture(0u, "effect_energy.png");
    m_Safe.DefineProgram("effect_energy_spheric_program");   // # not flat to draw over area
    m_Safe.SetColor3    (COLOR_ENERGY_PURPLE * 1.0f);
    m_Safe.SetTexSize   (coreVector2(2.4f,0.4f) * 5.0f);
    m_Safe.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Wave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < RUTILUS_WAVES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pWave = &m_aWaveRaw[i];
            pWave->DefineTexture(0u, "effect_energy.png");
            pWave->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pWave->SetColor3 (COLOR_ENERGY_RED * 0.8f);
            pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Wave.BindObject(pWave);
        }
    }

    // 
    m_apWaveModel[0] = Core::Manager::Resource->Get<coreModel>("object_cube_top.md3");
    m_apWaveModel[1] = Core::Manager::Resource->Get<coreModel>("object_tube_open.md3");

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
    for(coreUintW i = 0u; i < RUTILUS_PLATES;     ++i) this->DisablePlate     (i, false);
    this->DisableArea(false);
    this->DisableSafe(false);
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
    ASSERT(m_iTeleporterActive == 2u)

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
    m_iTeleporterActive = 2u;

    // 
    oTeleporter.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&oTeleporter);

    // 
    if(bAnimated) cRutilusMission::__TeleporterEffect(iIndex);
}


// ****************************************************************
// 
void cRutilusMission::EnablePlate(const coreUintW iIndex, const coreFloat fOffsetFrom, const coreFloat fOffsetTo, const coreFloat fScaleFrom, const coreFloat fScaleTo)
{
    ASSERT(iIndex < RUTILUS_PLATES)
    coreObject3D& oPlate = m_aPlateRaw[iIndex];

    // 
    WARN_IF(oPlate.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afPlateTime[iIndex] = 0.0f;
    m_avPlateData[iIndex] = coreVector4(fOffsetFrom, fOffsetTo, fScaleFrom, fScaleTo);

    // 
    oPlate.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cRutilusMission::DisablePlate(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < RUTILUS_PLATES)
    coreObject3D& oPlate = m_aPlateRaw[iIndex];

    // 
    if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_avPlateData[iIndex].w != 0.0f) this->SetPlateScale(iIndex, 0.0f);

    // 
    if(!bAnimated) oPlate.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cRutilusMission::EnableArea()
{
    coreObject3D& oArea = m_aArea[0];

    // 
    WARN_IF(oArea.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_fAreaTime  = 1.0f;
    m_fAreaScale = 1.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) nInitFunc(&oArea + i);
}


// ****************************************************************
// 
void cRutilusMission::DisableArea(const coreBool bAnimated)
{
    coreObject3D& oArea = m_aArea[0];

    // 
    if(!oArea.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_fAreaTime > 0.0f) m_fAreaTime = -1.0f;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(pObject);
        };
        for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) nExitFunc(&oArea + i);
    }
}


// ****************************************************************
// 
void cRutilusMission::EnableSafe()
{
    // 
    WARN_IF(m_Safe.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_fSafeTime = 1.0f;

    // 
    m_Safe.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Safe);
}


// ****************************************************************
// 
void cRutilusMission::DisableSafe(const coreBool bAnimated)
{
    // 
    if(!m_Safe.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_fSafeTime > 0.0f) m_fSafeTime = -1.0f;

    if(!bAnimated)
    {
        // 
        m_Safe.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Safe);
    }
}


// ****************************************************************
// 
void cRutilusMission::EnableWave(const coreUint8 iType)
{
    coreObject3D& oWave = m_aWaveRaw[0];

    // 
    WARN_IF(oWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iWaveActive, RUTILUS_WAVES)
    m_vWavePos  = HIDDEN_POS;
    m_iWaveDir  = 0u;
    m_iWaveType = iType;

    // 
    const auto nInitFunc = [this](coreObject3D* OUTPUT pObject, const coreFloat fOffset, const coreUintW iIndex)
    {
        m_afWaveTime[iIndex] = fOffset;

        pObject->DefineModel (m_iWaveType ? m_apWaveModel[1] : m_apWaveModel[0]);
        pObject->SetPosition (coreVector3(HIDDEN_POS, 0.0f));
        pObject->SetAlpha    (0.0f);
        pObject->SetTexSize  (m_iWaveType ? coreVector2(0.6f,2.4f) : coreVector2(0.9f,0.9f));
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
    DEPTH_PUSH

    // 
    m_Plate.Render();   // # depth test with each other

    // 
    m_Safe.Render();

    //
    for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) m_aArea[i].Render();

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Wave.Render();
    }
    glEnable(GL_DEPTH_TEST);
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
        m_avTeleporterPrev[i] = m_aTeleporter[i].GetPosition ().xy();
        m_avTeleporterRota[i] = m_aTeleporter[i].GetDirection().xy();
    }

    // 
    g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
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
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
    {
        coreObject3D& oTeleporter = m_aTeleporter[i];
        if(!oTeleporter.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        oTeleporter.SetAlpha    (m_iTeleporterActive ? 1.0f : 0.5f);
        oTeleporter.SetTexSize  (coreVector2(0.075f,0.125f) * oTeleporter.GetSize().xy());
        oTeleporter.SetTexOffset(coreVector2(0.0f, m_fAnimation * -3.0f));
        oTeleporter.Move();
    }

    // 
    for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
    {
        coreObject3D& oPlate = m_aPlateRaw[i];
        if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afPlateTime[i].UpdateMin(RUTILUS_PLATE_SPEED, 1.0f);
        const coreFloat fOffset = LERPB(    m_avPlateData[i].x,      m_avPlateData[i].y,  m_afPlateTime[i]);
        const coreFloat fScale  = LERPB(ABS(m_avPlateData[i].z), ABS(m_avPlateData[i].w), m_afPlateTime[i]);

        // 
        if(fScale <= 0.0f) this->DisablePlate(i, false);

        // 
        const coreVector2 vSize       = coreVector2(fScale, (m_avPlateData[i].w <= 0.0f) ? 1.0f : fScale);
        const coreVector2 vRealOffset = HAS_BIT(m_iPlateRotated, i) ? coreVector2(fOffset, 0.0f) : coreVector2(0.0f, fOffset);
        const coreVector2 vRealSize   = HAS_BIT(m_iPlateRotated, i) ? vSize.yx()                 : vSize;
        const coreFloat   fHeight     = 0.001f * (I_TO_F(RUTILUS_PLATES - i - 1u));

        // 
        oPlate.SetPosition (coreVector3(vRealOffset, fHeight) * (FOREGROUND_AREA.y * 2.2f));
        oPlate.SetSize     (coreVector3(vRealSize,   1.0f)    * (FOREGROUND_AREA.y * 2.2f));
        oPlate.SetTexSize  (vRealSize * 4.0f);
        oPlate.SetTexOffset((coreVector2(0.0f, FRACT(m_fAnimation * 2.0f)) - vRealSize * 2.0f));
    }

    // 
    m_Plate.MoveNormal();

    // 
    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);

        if(!pPlayer->IsRolling())
        {
            const coreVector2 vPos = pPlayer->GetPosition().xy();

            for(coreUintW j = 0u; j < RUTILUS_PLATES; ++j)
            {
                const coreObject3D& oPlate = m_aPlateRaw[j];
                if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vSize = oPlate.GetCollisionRange().xy();
                const coreVector2 vDiff = MapToAxis(oPlate.GetPosition().xy() - vPos, oPlate.GetDirection().xy());

                if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                {
                    pPlayer->SetDirection(oPlate.GetDirection());
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                    break;
                }
            }
        }
    });

    // 
    if(m_aArea[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreFloat fScale;
        if(m_fAreaTime > 0.0f)
        {
            // 
            m_fAreaTime.UpdateMin(2.0f, 2.0f);
            fScale = LERPB(0.0f, 1.0f, m_fAreaTime - 1.0f);
        }
        else
        {
            // 
            m_fAreaTime.UpdateMax(-1.0f/0.6f, -2.0f);
            fScale = ParaLerp(1.0f, 1.5f, 0.0f, -m_fAreaTime - 1.0f);

            // 
            if(m_fAreaTime <= -2.0f) this->DisableArea(false);
        }

        // 
        m_aArea[0].SetSize     (coreVector3(1.0f,1.0f,1.0f) * RUTILUS_AREA_SIZE * fScale * m_fAreaScale);
        m_aArea[0].SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
        m_aArea[0].Move();
    }

    // 
    if(m_aArea[1].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_aArea[1].SetPosition (m_aArea[0].GetPosition());
        m_aArea[1].SetSize     (m_aArea[0].GetSize    () * 0.8f);
        m_aArea[1].SetTexSize  (m_aArea[0].GetTexSize ());
        m_aArea[1].SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.5f));
        m_aArea[1].Move();
    }
    STATIC_ASSERT(RUTILUS_AREAS == 2u)

    // 
    if(m_Safe.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreFloat fScale;
        if(m_fSafeTime > 0.0f)
        {
            // 
            m_fSafeTime.UpdateMin(2.0f, 2.0f);
            fScale = LERPB(0.0f, 1.0f, m_fSafeTime - 1.0f);
        }
        else
        {
            // 
            m_fSafeTime.UpdateMax(-1.0f/0.6f, -2.0f);
            fScale = ParaLerp(1.0f, 1.5f, 0.0f, -m_fSafeTime - 1.0f);

            // 
            if(m_fSafeTime <= -2.0f) this->DisableSafe(false);
        }

        // 
        m_Safe.SetSize     (coreVector3(1.0f,1.0f,1.0f) * RUTILUS_SAFE_SIZE * fScale);
        m_Safe.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
        m_Safe.Move();
    }

    // 
    for(coreUintW i = 0u; i < RUTILUS_WAVES; ++i)
    {
        coreObject3D& oWave = m_aWaveRaw[i];
        if(!oWave.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector2 vDir   = StepRotated45(GET_BITVALUE(m_iWaveDir, 2u, 0u));
        const coreFloat   fSign  = HAS_BIT(m_iWaveDir, 2u) ? -1.0f : 1.0f;
        const coreBool    bDelay = HAS_BIT(m_iWaveDir, 3u);

        // 
        m_afWaveTime[i].Update(0.5f * fSign);
        if((m_afWaveTime[i] < 0.0f) || (m_afWaveTime[i] > 1.0f))
        {
            // 
            m_afWaveTime[i] -= fSign;
            SET_BIT(m_iWaveActive, i, HAS_BIT(m_iWaveActive, RUTILUS_WAVES))

            // 
            if(bDelay) oWave.SetPosition(coreVector3(m_vWavePos, 0.0f));

            // 
            oWave.SetDirection(coreVector3(m_iWaveType ? vDir : (coreVector2::Direction((I_TO_F(i) / I_TO_F(RUTILUS_WAVES_RAWS)) * (-0.5f*PI))), 0.0f));
        }

        // 
        if(!bDelay) oWave.SetPosition(coreVector3(m_vWavePos, 0.0f));

        // 
        oWave.SetSize (m_iWaveType ? coreVector3(20.0f * m_afWaveTime[i], 80.0f, 1.0f) : (coreVector3(1.0f,1.0f,1.0f) * 30.0f * m_afWaveTime[i]));
        oWave.SetAlpha(MIN(1.0f - m_afWaveTime[i], 6.0f * m_afWaveTime[i], 1.0f) * (HAS_BIT(m_iWaveActive, i) ? 1.0f : 0.0f));
    }

    // 
    if(!m_iWaveActive) this->DisableWave(false);

    // 
    m_Wave.MoveNormal();

    // 
    if(HAS_BIT(m_iWaveActive, RUTILUS_WAVES))
    {
        const coreVector2 vDir  = StepRotated45(GET_BITVALUE(m_iWaveDir, 2u, 0u)).Rotated90();
        const coreFloat   fSign = HAS_BIT(m_iWaveDir, 2u) ? 1.0f : -1.0f;

        coreFloat fPower = 0.0f;
        coreFloat fLimit = 0.0f;
        const auto nGravityFunc = [&](cBullet* OUTPUT pBullet)
        {
            const coreVector2 vDiff = m_vWavePos - pBullet->GetPosition().xy();
            const coreVector2 vPush = vDir * SIGN(coreVector2::Dot(vDiff, vDir)) * fSign;
            const coreVector2 vMove = pBullet->GetFlyDir() * pBullet->GetSpeed() + vPush * (fPower * TIME * (60.0f/16.0f));

            pBullet->SetFlyDir(vMove.Normalized());
            pBullet->SetSpeed (vMove.Length());

            if(pBullet->GetFlyTime() >= fLimit)
                pBullet->Deactivate(true);
        };

        fPower = HAS_BIT(m_iWaveDir, 2u) ? 12.0f/*18.0f*/ : 12.0f;   // TODO 1: 12 for boss, 18 for wave ?
        fLimit = 3.0f;
        g_pGame->GetBulletManagerPlayer()->ForEachBullet(nGravityFunc);

        fPower = HAS_BIT(m_iWaveDir, 2u) ? 0.15f : 0.1f;
        fLimit = 6.0f;
        g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nGravityFunc);
    }

    if(m_iTeleporterActive == 1u)
    {
        // 
        const auto nTeleportFunc = [this](const coreObject3D* pObject, const coreVector2 vMove, coreVector2* OUTPUT vNewPos, coreVector2* OUTPUT vNewDir, coreVector2* OUTPUT vNewMoveDir)
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
                
                if((vPos1 - m_avTeleporterPrev[i]).LengthSq() >= POW2(50.0f)) continue;
                
                
                const coreVector2 vDiff     = pObject->GetPosition().xy() - vPos1;
                
                const coreFloat fAngle1 = (m_avTeleporterRota[i].IsNull() ? vDir1 : m_avTeleporterRota[i]).Angle();
                const coreFloat fAngle2 = (vDir1).Angle();
                
                const coreVector2 A = vDiff;
                const coreVector2 B = MapToAxis(vDiff, coreVector2::Direction(AngleDiff(fAngle2, fAngle1)));

                const coreVector2 vFullMove1 = vMove - (vPos1 - m_avTeleporterPrev[i]) - (B - A);
                

                const coreVector2 vDiffTo   = vDiff;// + vMoveDir * pObject->GetCollisionRadius();
                const coreVector2 vDiffFrom = vDiff - vFullMove1;

                //const coreFloat fOffset  = coreVector2::Dot(vDiff,     vDir1.Rotated90());   // falscher offset
                const coreFloat fDotTo   = coreVector2::Dot(vDiffTo,   vDir1);
                const coreFloat fDotFrom = coreVector2::Dot(vDiffFrom, vDir1);
                
                const coreFloat fTotal = ABS(fDotTo) + ABS(fDotFrom) + CORE_MATH_PRECISION;
                const coreFloat fOffset2  = LERP(coreVector2::Dot(vDiffFrom, vDir1.Rotated90()), coreVector2::Dot(vDiffTo, vDir1.Rotated90()), (ABS(fDotFrom) / fTotal));

                // 
                if((ABS(fOffset2) <= m_aTeleporter[i].GetSize().x) && (SIGN(fDotTo) != SIGN(fDotFrom)))
                {
                    const coreVector2 vPos2 = m_aTeleporter[1u - i].GetPosition ().xy();
                    const coreVector2 vDir2 = m_aTeleporter[1u - i].GetDirection().xy();

                    const coreMatrix2 mRota = coreMatrix3::Rotation(vDir1.InvertedX()).m12().Transposed() *
                                              coreMatrix3::Rotation(vDir2.InvertedX()).m12();

                    const coreVector2 vHit1 = fOffset2 * vDir1.Rotated90() + vPos1;
                    const coreVector2 vHit2 = fOffset2 * vDir2.Rotated90() + vPos2;

                    const coreVector2 vFullMove2 = vMove * mRota - (vPos2 - m_avTeleporterPrev[1u - i]) - (B - A) * mRota;

                    // 
                    (*vNewPos)     = vHit2 + vFullMove2 * (ABS(fDotTo) / fTotal);
                    (*vNewDir)     = pObject->GetDirection().xy() * mRota;
                    (*vNewMoveDir) = vMoveDir                     * mRota;

                    // 
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vHit1, 0.0f), 5.0f, 3u, RUTILUS_TELEPORTER_COLOR(i));
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vHit2, 0.0f), 5.0f, 3u, RUTILUS_TELEPORTER_COLOR(1u - i));

                    return true;
                }
            }
            
            // TODO 1: kollision mit enemy-bullet auf anderer seite war möglich
            // TODO 1: geschosse können durchfliege wenn nah dran, oder auch auf anderer seite, wenn man grad rauskam, auch von gegnern
            // TODO 1: schiffe werden leicht rotiert wenn winkel-differenz beider teleporter durch frame-step immer leicht abweicht, da der step nicht ausgeglichen wird, passiert nicht wenn winkel-differenz konstant ist (e.g. immer 180 grad)

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
            if(pEnemy->WasTeleporting()) return;
            
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
                if(pBullet->GetFlyTime() >= 3.0f) pBullet->Deactivate(false);

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
        
        // TODO 1: zerstörte bullet sollten keinen effekt am ausgang erzeugen
        // TODO 1: gegner bullets sollten immer zerstört werden
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
    const coreFloat   fLen   = oTeleporter.GetCollisionRange().x;
    const coreVector3 vColor = RUTILUS_TELEPORTER_COLOR(iIndex);

    // 
    const coreVector3 vStart = vPos + vDir * fLen;
    const coreVector3 vDiff  = vPos - vDir * fLen - vStart;
    const coreUintW   iNum   = MAX(F_TO_UI(fLen * 2.0f / 1.9f), 2u);

    // 
    for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vStart + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, vColor);
}