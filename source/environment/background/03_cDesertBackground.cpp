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
cDesertBackground::cDesertBackground()noexcept
: cBackground  (false)
, m_vSandMove  (coreVector2(0.0f,-1.0f))
, m_fSandWave  (0.0f)

, m_avTrailHit  {}
, m_fTrailBlend (0.0f)
, m_bTrail      (false)
, m_vGroundPos  (coreVector2(0.0f,0.0f))
, m_iToken      (0u)
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "crack", OUTDOOR_ALGORITHM_DESERT, 4.0f, false);

    // 
    pList1 = new coreBatchList(DESERT_PILLAR_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(DESERT_PILLAR_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        coreBool bBlack = false;

        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_pillar_01_diff.png");
        oBase.DefineTexture(1u, "environment_pillar_01_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DESERT_PILLAR_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, DESERT_PILLAR_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);
            const coreVector3 vNormal   = (m_pOutdoor->RetrieveBackNormal(vPosition) * coreVector3(1.0f,0.2f,1.0f)).Normalized();

            // test for valid values
            if((fHeight > -22.0f) && (fHeight < -18.0f) && (vNormal.z < 0.9f))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(12.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2, vPosition, POW2(12.0f)))
                {
                    // determine object type
                    const coreBool bType = Core::Rand->Bool(0.5f);

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel(bType ? "environment_pillar_01.md3" : "environment_pillar_02.md3");

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 2.9f * 1.3f);
                    pObject->SetDirection  (vNormal);
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (bBlack ? 0.25f : 1.0f));

                    // add object to the list
                    if(bType) pList1->BindObject(pObject);
                         else pList2->BindObject(pObject);

                     // 
                     bBlack = !bBlack;
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -0.5f);
        this->_StoreHeight(pList2, -0.5f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, DESERT_PILLAR_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, DESERT_PILLAR_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(DESERT_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(DESERT_STONE_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        coreBool bBlack = false;

        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_pillar_02_diff.png");
        oBase.DefineTexture(1u, "environment_pillar_02_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DESERT_STONE_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, DESERT_STONE_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -23.0f) && (fHeight < -18.0f) && (F_TO_SI(vPosition.y+176.0f) % 80 < 60))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(5.0f))  &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(5.0f))  &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(10.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(10.0f)))
                    {
                        // determine object type
                        const coreBool bType = Core::Rand->Bool();

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_pillar_03.md3" : "environment_pillar_04.md3");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * (Core::Rand->Bool() ? 2.0f : 1.5f) * 1.3f);
                        pObject->SetDirection(coreVector3::Rand());
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (bBlack ? 0.25f : 1.0f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);

                        // 
                        bBlack = !bBlack;
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, DESERT_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, DESERT_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(DESERT_PLANT_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_desert_02.md3");
        oBase.DefineTexture(0u, "environment_desert.png");
        oBase.DefineTexture(1u, "default_normal.webp");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DESERT_PLANT_NUM; ++i)
        {
            for(coreUintW j = 2u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, DESERT_PLANT_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -18.0f) && ((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(7.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(9.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(9.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(9.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[3], vPosition, POW2(9.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 3.0f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.3f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, DESERT_PLANT_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    pList1 = new coreBatchList(DESERT_CACTUS_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_desert_01.md3");
        oBase.DefineTexture(0u, "environment_desert.png");
        oBase.DefineTexture(1u, "default_normal.webp");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < DESERT_CACTUS_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, DESERT_CACTUS_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -17.0f))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(9.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[4], vPosition, POW2(12.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 4.0f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -4.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, DESERT_CACTUS_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    m_Sand.DefineTexture(0u, "effect_sand.png");
    m_Sand.DefineProgram("effect_weather_sand_program");
    m_Sand.SetPosition  (coreVector2(0.0f,0.0f));
    m_Sand.SetColor3    (coreVector3(200.0f/255.0f, 186.0f/255.0f, 156.0f/255.0f) * 0.95f);
    m_Sand.SetAlpha     (0.9f);

    // 
    m_Veil.DefineProgram("menu_color_program");
    m_Veil.SetPosition  (coreVector2(0.0f,0.0f));
    m_Veil.SetColor3    (m_Sand.GetColor3() * 0.6f);
    m_Veil.SetAlpha     (0.0f);
    m_Veil.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    this->SetGroundDensity(5u, 0.0f);
}


// ****************************************************************
// destructor
cDesertBackground::~cDesertBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cDesertBackground::__InitOwn()
{
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_desert.wav");
    m_iToken = m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cDesertBackground::__ExitOwn()
{
    // stop base sound-effect
    if(!Core::Manager::Resource->DetachFunction(m_iToken))
    {
        m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
        {
            if(pResource->EnableRef(this))
                pResource->Stop();
        });
    }
}


// ****************************************************************
// 
void cDesertBackground::__RenderOwnAfter()
{
    // 
    if(m_Veil.IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        glDisable(GL_DEPTH_TEST);
        {
            // 
            m_Veil.Render();
        }
        glEnable(GL_DEPTH_TEST);
    }

    // enable the shader-program
    if(!m_Sand.GetProgram().IsUsable()) return;
    if(!m_Sand.GetProgram()->Enable())  return;

    coreRand oRand(1u);

    // 
    coreProgram* pLocal = m_Sand.GetProgram().GetResource();
    for(coreUintW i = 0u; i < DESERT_SAND_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Sand.GetTexOffset() + coreVector2::Rand(0.0f,1.0f, 0.0f,1.0f, &oRand) + coreVector2(0.26f * SIN(m_fSandWave * (0.125f*PI) + I_TO_F(POW2(i))), 0.0f);
        const coreFloat   fNewScale     = 1.6f - 0.12f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Sand.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the desert background
void cDesertBackground::__MoveOwn()
{
    // 
    const coreVector2 vEnvMove   = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 2.3f;
    const coreVector2 vTexOffset = m_Sand.GetTexOffset() + (m_vSandMove.InvertedX() + vEnvMove) * (0.4f * TIME);

    // 
    m_Sand.SetSize     (coreVector2(1.0f,1.0f) * SQRT2 * ENVIRONMENT_SCALE_FACTOR);
    m_Sand.SetDirection(g_pEnvironment->GetDirection());
    m_Sand.SetTexSize  (vTexSize);
    m_Sand.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Sand.Move();

    // 
    m_fSandWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // 
    m_Veil.SetSize     (coreVector2(1.0f,1.0f) * SQRT2 * ENVIRONMENT_SCALE_FACTOR);
    m_Veil.SetDirection(g_pEnvironment->GetDirection());
    m_Veil.SetEnabled  (m_Veil.GetAlpha() ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Veil.Move();

    // 
    if(m_pBaseSound.IsUsable() && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
        m_pBaseSound->SetPitch (1.0f + 0.1f * (m_vSandMove.Length() - 1.0f));
    }
    
    
    
    
    if(m_bTrail) m_fTrailBlend.UpdateMin( 0.1f, 1.0f);
            else m_fTrailBlend.UpdateMax(-0.1f, 0.0f);
    
    if(m_fTrailBlend)
    {
        // 
        const coreVector2 vEnvDirection = g_pEnvironment->GetDirection();
        const coreFloat   fEnvSpeed     = g_pEnvironment->GetSpeed();
        const coreFloat   fEnvFlyOffset = g_pEnvironment->GetFlyOffset();
    
        if(TIME && (fEnvSpeed > 0.0f))
        {
            // 
            if(fEnvFlyOffset * OUTDOOR_DETAIL < m_vGroundPos.y) m_vGroundPos.y -= I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL;
        
            const coreFloat fOffest = 14.0f * OUTDOOR_DETAIL;
            
            // 
            const coreVector2 vPrevGroundPos = m_vGroundPos;
            m_vGroundPos = coreVector2(SIN((fEnvFlyOffset + fOffest / OUTDOOR_DETAIL) * (0.075f*PI)) * -12.5f, fEnvFlyOffset * OUTDOOR_DETAIL);
            
            if(!vPrevGroundPos.IsNull())
            {
                const coreVector2 vBodyPos = MapToAxis(coreVector2(m_vGroundPos.x, fOffest), vEnvDirection);
                const coreVector2 vBodyDir = MapToAxis(m_vGroundPos - vPrevGroundPos, vEnvDirection).Normalized(vEnvDirection);
            
                const coreVector2 vTrailPos1 = vBodyPos - vBodyDir * 8.0f + vBodyDir.Rotated90() * 6.8f;
                const coreVector2 vTrailPos2 = vBodyPos - vBodyDir * 8.0f - vBodyDir.Rotated90() * 6.8f;
                this->__CreateTrail(0u, coreVector3(vTrailPos1, g_pEnvironment->RetrieveSafeHeight(vTrailPos1) - 0.0f));
                this->__CreateTrail(1u, coreVector3(vTrailPos2, g_pEnvironment->RetrieveSafeHeight(vTrailPos2) - 0.0f));
            }
        }
    }
}


// ****************************************************************
// 
void cDesertBackground::__CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect)
{
    ASSERT(iIndex < ARRAY_SIZE(m_avTrailHit))

    // always track intersection without rotation (to account for boss rotating with background)
    const coreVector2 vEnvDirection    = g_pEnvironment->GetDirection();
    const coreVector3 vRelIntersection = MapToAxisInv(vIntersect, vEnvDirection);

    // 
    constexpr coreFloat fMin = 1.7f;
    coreVector3 vOldHit = m_avTrailHit[iIndex];

    // 
    if(vOldHit.IsNull()) vOldHit = vRelIntersection;
    else
    {
        while(true)
        {
            // 
            const coreVector3 vDiff = vRelIntersection - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

            // 
            const coreVector3 vNewHit      = LERP(vOldHit, vRelIntersection, fMin*RCP(fLen));
            //const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            //const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
           // if(((ABS(vOldOnScreen.x) < 0.65f) && (ABS(vOldOnScreen.y) < 0.65f)) ||    
           //    ((ABS(vNewOnScreen.x) < 0.65f) && (ABS(vNewOnScreen.y) < 0.65f)))     
            {
                // 
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(3.6f, fMin*0.7f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // add rotation back to final position
                const coreVector3 vAbsPos = MapToAxis(vDecalPos, vEnvDirection);

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "default_white.webp");
                pObject->DefineProgram("effect_decal_single_program");   // TODO 1: own decal_color shader (similar to menu_color, without textur)

                // set object properties
                pObject->SetSize     (coreVector3(vDecalSize, 1.0f));
                pObject->SetDirection(coreVector3(vDecalDir,  0.0f));
                pObject->SetColor4   (coreVector4(0.0f, 0.0f, 0.0f, 0.5f * m_fTrailBlend));

                // add object to background
                this->AddDecal(pObject, vAbsPos, 256u, "effect_decal_single_inst_program", LIST_KEY);
            }

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avTrailHit[iIndex] = vOldHit;
}