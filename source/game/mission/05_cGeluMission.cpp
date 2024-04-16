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
cGeluMission::cGeluMission()noexcept
: m_Fang          (GELU_FANGS)
, m_Way           (GELU_WAYS)
, m_WayArrow      (GELU_WAYS)
, m_iWayActive    (0u)
, m_iWayVisible   (0u)
, m_Orb           (GELU_ORBS)
, m_afOrbTime     {}
, m_Line          (GELU_LINES)
, m_afLineTime    {}
, m_iLineMode     (0u)
, m_avOldPos      {}
, m_abCrushImmune {}
, m_iCrushState   (0u)
, m_fAnimation    (0.0f)
{
    // 
    m_apBoss[0] = &m_Chol;

    // 
    m_Fang.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_FANGS_RAWS; ++i)
        {
            // load object resources
            cLodObject* pFang = &m_aFangRaw[i];
            pFang->DefineModelHigh("object_fang.md3");
            pFang->DefineModelLow ("object_fang.md3");
            pFang->DefineTexture  (0u, "ship_enemy.png");
            pFang->DefineProgram  ("object_ship_glow_program");

            // set object properties
            pFang->SetSize             (coreVector3(1.0f,1.0f,1.0f) * 9.0f);
            pFang->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.05f);
            pFang->SetColor3           (coreVector3(0.5f,0.5f,0.5f));
            pFang->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Fang.BindObject(pFang);
        }
    }

    // 
    m_Way     .DefineProgram("effect_energy_flat_spheric_inst_program");
    m_WayArrow.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_WAYS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pWay = &m_aWayRaw[i];
            pWay->DefineModel  (iType ? "object_arrow.md3" : "object_cube_top.md3");
            pWay->DefineTexture(0u, "effect_energy.png");
            pWay->DefineProgram(iType ? "effect_energy_flat_invert_program" : "effect_energy_flat_spheric_program");

            // set object properties
            pWay->SetSize             (coreVector3(1.0f,1.0f,1.0f) * (iType ? 2.0f : 7.0f));
            pWay->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.15f);
            pWay->SetColor3           (COLOR_ENERGY_MAGENTA * 0.8f);
            pWay->SetTexSize          (coreVector2(1.0f,1.0f) * (iType ? 0.4f : 0.7f));
            pWay->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_WayArrow.BindObject(pWay);
                 else m_Way     .BindObject(pWay);
        }
    }

    // 
    m_Orb.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_ORBS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pOrb = &m_aOrbRaw[i];
            pOrb->DefineModel  ("object_sphere.md3");
            pOrb->DefineTexture(0u, "effect_energy.png");
            pOrb->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pOrb->SetColor3 (COLOR_ENERGY_CYAN * 0.8f);
            pOrb->SetTexSize(coreVector2(4.0f,4.0f) * 0.9f);
            pOrb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Orb.BindObject(pOrb);
        }
    }

    // 
    m_Line.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_LINES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pLine = &m_aLineRaw[i];
            pLine->DefineModel  ("object_tube_open.md3");
            pLine->DefineTexture(0u, "effect_energy.png");
            pLine->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLine->SetColor3 (COLOR_ENERGY_CYAN * 0.5f);
            pLine->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Line.BindObject(pLine);
        }
    }

    // 
    g_pGlow->BindList(&m_Way);
    g_pGlow->BindList(&m_WayArrow);
    g_pGlow->BindList(&m_Orb);
    g_pGlow->BindList(&m_Line);
}


// ****************************************************************
// destructor
cGeluMission::~cGeluMission()
{
    // 
    g_pGlow->UnbindList(&m_Way);
    g_pGlow->UnbindList(&m_WayArrow);
    g_pGlow->UnbindList(&m_Orb);
    g_pGlow->UnbindList(&m_Line);

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i) this->DisableFang(i, false);
    for(coreUintW i = 0u; i < GELU_WAYS;  ++i) this->DisableWay (i, false);
    for(coreUintW i = 0u; i < GELU_ORBS;  ++i) this->DisableOrb (i, false);
    for(coreUintW i = 0u; i < GELU_LINES; ++i) this->DisableLine(i, false);
}


// ****************************************************************
// 
void cGeluMission::EnableFang(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_FANGS)
    cLodObject& oFang = m_aFangRaw[iIndex];

    // 
    WARN_IF(oFang.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_avOldPos[iIndex] = coreVector2(0.0f,0.0f);
    STATIC_ASSERT(GELU_FANGS <= GELU_POSITIONS)

    // 
    oFang.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    oFang.SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableFang(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_FANGS)
    cLodObject& oFang = m_aFangRaw[iIndex];

    // 
    if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oFang.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionDarkSmall(oFang.GetPosition());
}


// ****************************************************************
// 
void cGeluMission::EnableWay(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < GELU_WAYS)
    coreObject3D* pWay   = (*m_Way     .List())[iIndex];
    coreObject3D* pArrow = (*m_WayArrow.List())[iIndex];

    // 
    WARN_IF(pWay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iWayActive,  iIndex)
    ADD_BIT(m_iWayVisible, iIndex)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayActive) *8u)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayVisible)*8u)

    // 
    m_avOldPos[iIndex] = coreVector2(0.0f,0.0f);
    STATIC_ASSERT(GELU_WAYS <= GELU_POSITIONS)

    // 
    pWay  ->SetPosition (coreVector3(vPosition,  0.0f));
    pWay  ->SetDirection(coreVector3(vDirection, 0.0f));
    pWay  ->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    pArrow->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableWay(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_WAYS)
    coreObject3D* pWay   = (*m_Way     .List())[iIndex];
    coreObject3D* pArrow = (*m_WayArrow.List())[iIndex];

    // 
    if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iWayActive, iIndex)

    // 
    if(!bAnimated)
    {
        pWay  ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pArrow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cGeluMission::EnableOrb(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_ORBS)
    coreObject3D& oOrb = m_aOrbRaw[iIndex];

    // 
    WARN_IF(oOrb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afOrbTime[iIndex] = 1.0f;

    // 
    oOrb.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableOrb(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_ORBS)
    coreObject3D& oOrb = m_aOrbRaw[iIndex];

    // 
    if(!oOrb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(m_afOrbTime[iIndex] > 0.0f)
    {
        // 
        m_afOrbTime[iIndex] = -1.0f;

        // 
        if(bAnimated) g_pSpecialEffects->CreateSplashColor(oOrb.GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_CYAN);
    }

    // 
    if(!bAnimated) oOrb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

}


// ****************************************************************
// 
void cGeluMission::EnableLine(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_LINES)
    coreObject3D& oLine = m_aLineRaw[iIndex];

    // 
    WARN_IF(oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afLineTime[iIndex] = 1.0f;

    // 
    oLine.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableLine(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_LINES)
    coreObject3D& oLine = m_aLineRaw[iIndex];

    // 
    if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(m_afLineTime[iIndex] > 0.0f)
    {
        // 
        m_afLineTime[iIndex] = -1.0f;

        // 
        if(bAnimated) for(coreUintW j = 10u; j--; ) g_pSpecialEffects->CreateSplashColor(oLine.GetPosition() + oLine.GetDirection() * (2.0f * (I_TO_F(j) - 4.5f)), 10.0f, 1u, COLOR_ENERGY_CYAN);
    }

    // 
    if(!bAnimated) oLine.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    // 
    m_Line.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Line);

    DEPTH_PUSH

    // 
    m_Orb .Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Orb);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    if(m_Fang.GetCurEnabled()) cLodObject::RenderHighList(&m_Fang);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Fang);

    // 
    m_Way.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Way);

    DEPTH_PUSH

    // 
    m_WayArrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_WayArrow);
}


// ****************************************************************
// 
void cGeluMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);
    
    
    
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        oFang.SetPosition(coreVector3(oFang.GetPosition().xy(), -0.1f));   // make sure player can be inside pyramid geometry
        // needs to be before collision handling
    }
    
    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        coreVector2 vNewPos = pPlayer->GetPosition().xy();

        const coreVector2 vFangRange = m_aFangRaw[0].GetCollisionRange().xy();
        if(vFangRange.IsNull()) return;

        coreFloat afDistance[4] = {};
        coreUint8 iHitIndex     = 255u;

        for(coreUintW j = 0u; j < GELU_FANGS; ++j)
        {
            const cLodObject& oFang = m_aFangRaw[j];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreVector2 vFangPos = oFang.GetPosition().xy();
            const coreVector2 vShift   = (pPlayer->GetOldPos() - vFangPos) / vFangRange * 0.02f;

            coreVector2 vFangMove = vFangPos - m_avOldPos[j];
            if(vFangMove.LengthSq() > vFangRange.LengthSq()) vFangMove = coreVector2(0.0f,0.0f);   // handle teleportation

            const coreVector2 vRayPos = pPlayer->GetOldPos() + vFangMove + vShift;

            for(coreUintW k = 0u; k < 4u; ++k)
            {
                const coreVector2 vRayDir = StepRotated90(k);

                coreFloat fHitDistance = 0.0f;
                coreUint8 iHitCount    = 1u;
                if(Core::Manager::Object->TestCollision(&oFang, coreVector3(vRayPos, 0.0f), coreVector3(vRayDir, 0.0f), &fHitDistance, &iHitCount))
                {
                    if(iHitCount & 0x01u)
                    {
                        iHitIndex = j;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vFangPos;
                        if(IsHorizontal(vDiff)) vNewPos.x = vFangPos.x + SIGN(vDiff.x) * vFangRange.x;
                                           else vNewPos.y = vFangPos.y + SIGN(vDiff.y) * vFangRange.y;

                        //ASSERT(k == 0u)  triggered while moving within blocks, maybe edge case when moving along the tilted edge
                        break;
                    }
                    else
                    {
                        const coreFloat fNewDistance = fHitDistance + coreVector2::Dot(vFangMove + vShift - pPlayer->GetMove(), vRayDir) - CORE_MATH_PRECISION;
                        if(fNewDistance < afDistance[k])
                        {
                            afDistance[k] = fNewDistance;
                        }
                    }
                }
            }
        }

        coreBool bIntersect = false;
        coreBool bCrush     = false;
        for(coreUintW j = 0u; j < GELU_FANGS; ++j)
        {
            const cLodObject& oFang = m_aFangRaw[j];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            if(InBetween(pPlayer->GetPosition().xy(), oFang.GetPosition().xy() - vFangRange, oFang.GetPosition().xy() + vFangRange))
            {
                bIntersect = true;
                if((iHitIndex != 255u) && (iHitIndex != j)) bCrush = true;
            }
        }

        if(pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) m_abCrushImmune[i] = true;

        if(bIntersect && !m_abCrushImmune[i])
        {
            vNewPos.y += afDistance[0];
            vNewPos.x -= afDistance[1];
            vNewPos.y -= afDistance[2];
            vNewPos.x += afDistance[3];

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            if(bCrush || !InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))
            {
                if(pPlayer->IsNormal())
                {
                    pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vNewPos);
                    if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;
                }
            }
        }
        else if(!bIntersect && m_abCrushImmune[i])
        {
            if(pPlayer->IsNormal() || !HAS_BIT(m_iCrushState, 1u))
            {
                m_abCrushImmune[i] = false;
            }
        }
    });
    
    
    

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        const cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_avOldPos[i] = oFang.GetPosition().xy();
    }

    // 
    m_Fang.MoveNormal();


    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const auto nBulletFangCollFunc = [&](cBullet* OUTPUT pBullet)
        {
            if(InBetween(pBullet->GetPosition().xy(), FOREGROUND_AREA * -1.05f, FOREGROUND_AREA * 1.05f))
            {
                const coreVector2 vDiff = pBullet->GetPosition().xy() - oFang.GetPosition().xy();

                if(InBetween(vDiff, -oFang.GetCollisionRange().xy(), oFang.GetCollisionRange().xy()))
                {
                    pBullet->Deactivate(true);
                    if(m_iStageSub == 19u) this->DisableFang(i, true);   // TODO 1: HACK!!!
                }
            }
        };
        g_pGame->GetBulletManagerPlayer()->ForEachBullet(nBulletFangCollFunc);   // TODO 1: noch immer falsch, geschosse gehen rein, weil mittelpunkt des geschosses verwendet wird
    }
    
    
    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        coreVector2 vNewPos = pPlayer->GetPosition().xy();

        const coreVector2 vWayRange = (*m_Way.List())[0]->GetCollisionRange().xy();
        if(vWayRange.IsNull()) return;

        coreFloat afDistance[4] = {};
        coreUint8 iHitIndex     = 255u;

        for(coreUintW j = 0u; j < GELU_WAYS; ++j)
        {
            const coreObject3D* pWay = (*m_Way.List())[j];
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayVisible, j)) continue;

            const coreVector2 vFangPos = pWay->GetPosition().xy();
            const coreVector2 vShift   = (pPlayer->GetOldPos() - vFangPos) / vWayRange * 0.02f;

            coreVector2 vFangMove = vFangPos - m_avOldPos[j];
            if(vFangMove.LengthSq() > vWayRange.LengthSq()) vFangMove = coreVector2(0.0f,0.0f);   // handle teleportation

            const coreVector2 vRayPos = pPlayer->GetOldPos() + vFangMove + vShift;

            for(coreUintW k = 0u; k < 4u; ++k)
            {
                const coreVector2 vRayDir = StepRotated90(k);

                coreFloat fHitDistance = 0.0f;
                coreUint8 iHitCount    = 1u;
                if(Core::Manager::Object->TestCollision(pWay, coreVector3(vRayPos, 0.0f), coreVector3(vRayDir, 0.0f), &fHitDistance, &iHitCount))
                {
                    if(iHitCount & 0x01u)
                    {
                        iHitIndex = j;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vFangPos;
                        if(IsHorizontal(vDiff)) vNewPos.x = vFangPos.x + SIGN(vDiff.x) * vWayRange.x;
                                           else vNewPos.y = vFangPos.y + SIGN(vDiff.y) * vWayRange.y;

                        // ASSERT(k == 0u) same as for fangs
                        break;
                    }
                    else
                    {
                        const coreFloat fNewDistance = fHitDistance + coreVector2::Dot(vFangMove + vShift - pPlayer->GetMove(), vRayDir) - CORE_MATH_PRECISION;
                        if(fNewDistance < afDistance[k])
                        {
                            afDistance[k] = fNewDistance;
                        }
                    }
                }
            }
        }

        coreBool bIntersect = false;
        coreBool bCrush     = false;
        for(coreUintW j = 0u; j < GELU_WAYS; ++j)
        {
            const coreObject3D* pWay = (*m_Way.List())[j];
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayVisible, j)) continue;

            if(InBetween(pPlayer->GetPosition().xy(), pWay->GetPosition().xy() - vWayRange, pWay->GetPosition().xy() + vWayRange))
            {
                bIntersect = true;
                if((iHitIndex != 255u) && (iHitIndex != j)) bCrush = true;
            }
        }

        if(pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) m_abCrushImmune[i] = true;

        if(bIntersect && !m_abCrushImmune[i])
        {
            vNewPos.y += afDistance[0];
            vNewPos.x -= afDistance[1];
            vNewPos.y -= afDistance[2];
            vNewPos.x += afDistance[3];

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            if(bCrush || !InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))
            {
                if(pPlayer->IsNormal())
                {
                    pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vNewPos);
                    if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;
                }
            }
        }
        else if(!bIntersect && m_abCrushImmune[i])
        {
            if(pPlayer->IsNormal() || !HAS_BIT(m_iCrushState, 1u))
            {
                m_abCrushImmune[i] = false;
            }
        }
    });
    
    

    // 
    for(coreUintW i = 0u; i < GELU_WAYS; ++i)
    {
        coreObject3D* pWay   = (*m_Way     .List())[i];
        coreObject3D* pArrow = (*m_WayArrow.List())[i];
        if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        if(!HAS_BIT(m_iWayActive, i))
        {
            // 
            const coreVector2 vPos    = pWay->GetPosition ().xy();
            const coreVector2 vDir    = pWay->GetDirection().xy();
            const coreVector2 vNewPos = vPos + vPos.Normalized() * coreVector2(1.0f,0.5f) * (TIME * 50.0f);
            const coreVector2 vNewDir = coreVector2::Direction(vDir.Angle() + (TIME * 4.0f));

            // 
            pWay->SetPosition (coreVector3(vNewPos, 0.0f));
            pWay->SetDirection(coreVector3(vNewDir, 0.0f));

            // 
            if(!g_pForeground->IsVisiblePoint(vNewPos, 1.3f))
                this->DisableWay(i, false);
        }

        // 
        const coreFloat fOffset  = I_TO_F(i) * (1.0f/7.0f);
        const coreBool  bVisible = HAS_BIT(m_iWayVisible, i);

        // 
        pWay->SetAlpha    (bVisible ? 1.0f : 0.0f);
        pWay->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pArrow->SetPosition (pWay->GetPosition ());
        pArrow->SetDirection(pWay->GetDirection());
        pArrow->SetAlpha    (bVisible ? 1.0f : 0.4f);
        pArrow->SetTexOffset(pWay->GetTexOffset());
        
        
        // 
        m_avOldPos[i] = pWay->GetPosition().xy();
    }

    // 
    m_Way     .MoveNormal();
    m_WayArrow.MoveNormal();
    
    
    for(coreUintW i = 0u; i < GELU_WAYS; ++i)
    {
        const coreObject3D* pWay = (*m_Way.List())[i];
        if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayActive, i)) continue;

        if(coreVector2::Dot(g_pGame->FindPlayerSide(pWay->GetPosition().xy())->GetDirection().xy(), pWay->GetDirection().xy()) > 0.9f)
        {
            REMOVE_BIT(m_iWayVisible, i)
        }
        else
        {
            ADD_BIT(m_iWayVisible, i)

            const auto nBulletWayCollFunc = [&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vDiff = pBullet->GetPosition().xy() - pWay->GetPosition().xy();

                if(InBetween(vDiff, -pWay->GetCollisionRange().xy(), pWay->GetCollisionRange().xy()))
                    pBullet->Deactivate(true);
            };
            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nBulletWayCollFunc);
            g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nBulletWayCollFunc);   // TODO 1: noch immer falsch, geschosse gehen rein, weil mittelpunkt des geschosses verwendet wird
        }
    }

    // 
    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)   // copied from group "force rotation"
    {
        pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);

        if(!pPlayer->IsRolling())
        {
            const coreVector2 vPos = pPlayer->GetPosition().xy();

            for(coreUintW j = 0u; j < GELU_WAYS; ++j)
            {
                const coreObject3D* pWay = (*m_Way.List())[i];
                if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayActive, i) || pWay->GetAlpha()) continue;

                const coreVector2 vSize = pWay->GetCollisionRange().xy() * 1.1f;   // TODO 1: otherwise you can rotate when between blocks and take damage
                const coreVector2 vDiff = MapToAxis(pWay->GetPosition().xy() - vPos, pWay->GetDirection().xy());

                if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                {
                    pPlayer->SetDirection(pWay->GetDirection());
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                    break;
                }
            }
        }
    });

    // 
    for(coreUintW i = 0u; i < GELU_ORBS; ++i)
    {
        coreObject3D& oOrb = m_aOrbRaw[i];
        if(!oOrb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fScale;
        if(m_afOrbTime[i] > 0.0f)
        {
            // 
            m_afOrbTime[i].UpdateMin(3.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afOrbTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afOrbTime[i].UpdateMax(-3.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afOrbTime[i] - 1.0f);

            // 
            if(m_afOrbTime[i] <= -2.0f) this->DisableOrb(i, false);
        }

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        oOrb.SetSize     (coreVector3(2.9f,2.9f,2.9f) * fScale);
        oOrb.SetAlpha    (2.0f - fScale);
        oOrb.SetTexOffset(coreVector2(0.0f, FRACT(-2.0f * m_fAnimation + fOffset)));
    }

    // 
    m_Orb.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_LINES; ++i)
    {
        coreObject3D& oLine = m_aLineRaw[i];
        if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreUintW     iIndex1 = (m_iLineMode == 0u) ? ((i < 12u) ? (i + (i/3u) + 1u) : (i - 12u))      : ((i));
        const coreUintW     iIndex2 = (m_iLineMode == 0u) ? ((i < 12u) ? (i + (i/3u))      : (i - 12u + 4u)) : ((i + 1u) % GELU_ORBS);
        const coreObject3D& oOrb1   = m_aOrbRaw[iIndex1];
        const coreObject3D& oOrb2   = m_aOrbRaw[iIndex2];
        ASSERT((iIndex1 < GELU_ORBS) && (iIndex2 < GELU_ORBS))

        // 
        if(!this->IsOrbEnabled(iIndex1) || !this->IsOrbEnabled(iIndex2))
            this->DisableLine(i, true);

        coreFloat fScale;
        if(m_afLineTime[i] > 0.0f)
        {
            // 
            m_afLineTime[i].UpdateMin(3.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afLineTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afLineTime[i].UpdateMax(-3.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afLineTime[i] - 1.0f);

            // 
            if(m_afLineTime[i] <= -2.0f) this->DisableLine(i, false);
        }

        // 
        const coreVector2 vDiff   = (oOrb1.GetPosition().xy() - oOrb2.GetPosition().xy());
        const coreVector2 vPos    = (oOrb1.GetPosition().xy() + oOrb2.GetPosition().xy()) * 0.5f;
        const coreVector2 vDir    = vDiff.Normalized();
        const coreFloat   fLen    = vDiff.Length() * 0.5f;
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        oLine.SetPosition (coreVector3(vPos, 0.0f));
        oLine.SetSize     (coreVector3(fScale, fLen, fScale));
        oLine.SetDirection(coreVector3(vDir, 0.0f));
        oLine.SetAlpha    (2.0f - fScale);
        oLine.SetTexSize  (coreVector2(0.25f, fLen * (1.0f/12.8f)) * 0.9f);
        oLine.SetTexOffset(coreVector2(1.0f,1.0f) * FRACT(-1.0f * m_fAnimation + fOffset));
    }

    // 
    m_Line.MoveNormal();
}