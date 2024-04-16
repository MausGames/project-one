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
cDesertBackground::cDesertBackground()noexcept
: m_vSandMove (coreVector2(0.0f,-1.0f))
, m_fSandWave (0.0f)
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "crack", 3u, 4.0f);

    coreBool bBlack = false;

    // 
    pList1 = new coreBatchList(DESERT_PILLAR_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(DESERT_PILLAR_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
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
            //const coreFloat fHeight = WATER_HEIGHT + (ABS(SIN(vPosition.y*0.075f*PI) * 0.25f - ((vPosition.x+0.5f) / I_TO_F(OUTDOOR_WIDTH) - 0.5f) * 4.0f) * 20.0f - 13.0f);


                    const coreVector3 vNormal = (m_pOutdoor->RetrieveBackNormal(vPosition) * coreVector3(1.0f,0.2f,1.0f)).Normalized();
                    
            // test for valid values
            if((fHeight > -22.0f) && (fHeight < -18.0f) && (vNormal.z < 0.9f)   )// && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
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
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f) - vNormal * coreVector3(1.0f,0.0f,1.0f) * 7.0f);
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(2.5f, 3.2f) * 1.3f);
                    pObject->SetDirection  (vNormal);
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (bBlack ? 0.25f : 1.0f));

                    // add object to the list
                    if(bType) pList1->BindObject(pObject);
                         else pList2->BindObject(pObject);
                         
                         bBlack = !bBlack;
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, DESERT_PILLAR_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, DESERT_PILLAR_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(DESERT_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(DESERT_STONE_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
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
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(5.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(5.0f)) &&
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
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * (Core::Rand->Bool(0.5f) ? 2.0f : 1.5f) * 1.3f);
                        pObject->SetDirection(coreVector3::Rand());
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * (bBlack ? 0.25f : 1.0f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                         
                         bBlack = !bBlack;
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, DESERT_STONE_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);

        cBackground::_FillInfinite(pList2, DESERT_STONE_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(GRASS_SHIP_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(GRASS_SHIP_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "ship_player.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < GRASS_SHIP_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_SHIP_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -18.0f))// && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(15.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2, vPosition, POW2(15.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(10.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(10.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(10.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[3], vPosition, POW2(10.0f)))
                {
                    // determine object type
                    const coreBool bType = Core::Rand->Bool();

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel(bType ? "ship_player_atk_high.md3" : "ship_player_def_high.md3");

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                    //pObject->SetDirection  (coreVector3::Rand());
                    //pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), Core::Rand->Float(-0.5f, 0.5f)).Normalized());
                    pObject->SetOrientation(coreVector3(0.0f,0.0f,-1.0f));
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f) * 0.5f);

                    // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.5f);
        this->_StoreHeight(pList2, 0.5f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, GRASS_SHIP_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, GRASS_SHIP_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }
    
    this->SetGroundDensity(4u, 0.0f);
    this->SetGroundDensity(5u, 0.0f);
    
#if 0
    // allocate stone list
    pList1 = new coreBatchList(SNOW_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_desert_04.md3");
        oBase.DefineTexture(0u, "environment_desert.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_STONE_NUM/8; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_STONE_NUM/8);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -17.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[3], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(6.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 3.0f);
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                    //pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.3f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SNOW_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    pList1 = new coreBatchList(SNOW_REED_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SNOW_REED_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        //oBase.DefineTexture(0u, "environment_plant.png");
        oBase.DefineTexture(0u, "environment_desert.png");
        //oBase.DefineTexture(0u, "environment_sea.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_REED_NUM/8; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_REED_NUM/8);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -17.0f) && (F_TO_SI(vPosition.y+176.0f) % 80 < 60))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(7.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(7.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[6], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[3], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[2], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(6.0f)))
                    {
                        // determine object type
                        const coreBool bType = Core::Rand->Bool();
                        
                        
                    //const coreVector3 vNormal = m_pOutdoor->RetrieveBackNormal(vPosition);

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_plant_06.md3" : "environment_plant_07.md3");
                        pObject->DefineModel(bType ? "environment_desert_01.md3" : "environment_desert_02.md3");
                        //pObject->DefineModel(bType ? "environment_sea_06.md3" : "environment_sea_07.md3");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 4.0f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        //pObject->SetDirection(coreVector3::Cross(vNormal, coreVector3::Cross(vNormal, coreVector3(0.0f,0.0f,1.0f)).Normalized()));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));
                       // pObject->SetColor3   (LERP(coreVector3(54.0f/255.0f, 204.0f/255.0f, 255.0f/255.0f), coreVector3(1.0f,1.0f,1.0f), Core::Rand->Float(1.0f)));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SNOW_REED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);

        cBackground::_FillInfinite(pList2, SNOW_REED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }
#endif
#if 0
    // 
    pList1 = new coreBatchList(GRASS_FLOWER_RESERVE);            
    pList1->DefineProgram("effect_decal_single_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "effect_soot.png");
        oBase.DefineProgram("effect_decal_single_program");

        for(coreUintW i = 0u; i < GRASS_FLOWER_NUM; ++i)
        {
            for(coreUintW j = 20u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, GRASS_FLOWER_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if(fHeight > -15.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(6.35f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3(coreVector2(1.0f,1.0f) * Core::Rand->Float(9.0f, 10.0f), 1.0f) * 1.3f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));
                        pObject->SetAlpha    (0.98f);

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -0.8f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, GRASS_FLOWER_RESERVE);
        m_apDecalObjectList.push_back(pList1);
    }
#endif
    // 
    m_Sand.DefineTexture(0u, "effect_sand.png");
    m_Sand.DefineProgram("effect_weather_sand_program");
    m_Sand.SetPosition  (coreVector2(0.0f,0.0f));
    m_Sand.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Sand.SetColor3    (coreVector3(200.0f/255.0f, 186.0f/255.0f, 156.0f/255.0f) * 1.02f);

    // load wind sound-effect
    m_pWindSound = Core::Manager::Resource->Get<coreSound>("environment_wind.wav");
    m_pWindSound.OnUsableOnce([this, pResource = m_pWindSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// destructor
cDesertBackground::~cDesertBackground()
{
    // stop wind sound-effect
    if(m_pWindSound->EnableRef(this))
        m_pWindSound->Stop();
}


// ****************************************************************
// 
void cDesertBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Sand.GetProgram().IsUsable()) return;
    if(!m_Sand.GetProgram()->Enable())  return;

    // 
    coreProgram* pLocal = m_Sand.GetProgram().GetResource();
    for(coreUintW i = 0u; i < DESERT_SAND_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Sand.GetTexOffset() + coreVector2(0.3f,0.3f) * I_TO_F(POW2(i)) + coreVector2(0.26f * SIN(m_fSandWave * (0.125f*PI) + I_TO_F(POW2(i))), 0.0f);
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
    m_Sand.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Sand.SetTexSize  (vTexSize);
    m_Sand.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Sand.Move();

    // 
    m_fSandWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // 
    if(m_pWindSound->EnableRef(this))
    {
        m_pWindSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
        m_pWindSound->SetPitch (1.0f + 0.1f * (m_vSandMove.Length() - 1.0f));
    }
}