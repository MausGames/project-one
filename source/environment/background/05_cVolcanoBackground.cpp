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
cVolcanoBackground::cVolcanoBackground()noexcept
: m_Smoke      (256u)
, m_fSparkTime (Core::Rand->Float(10.0f))
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("rock", "lava", OUTDOOR_ALGORITHM_VOLCANO, 4.0f, false);
    m_pOutdoor->LoadProgram(true);

    // allocate smoke list
    pList1 = new coreBatchList(VOLCANO_SMOKE_RESERVE);
    pList1->DefineProgram("effect_decal_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineProgram("effect_decal_program");

        for(coreUintW i = 0u; i < VOLCANO_SMOKE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, VOLCANO_SMOKE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight < -21.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(35.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition,0.0f));
                    pObject->SetSize    (coreVector3(0.0f,0.0f,0.0f));   // invisible

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, VOLCANO_SMOKE_RESERVE);
        m_apGroundObjectList.push_back(pList1);
    }

    // 
    pList1 = new coreBatchList(MOSS_GRAVE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_pyramid.md3");
        oBase.DefineTexture(0u, "environment_pyramid2_diff.png");
        oBase.DefineTexture(1u, "environment_pyramid2_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < MOSS_GRAVE_NUM/2; ++i)
        {
            const coreFloat fSize = Core::Rand->Bool() ? 5.0f : 3.0f;
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, MOSS_GRAVE_NUM/2);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition + coreVector2(fSize,0.0f));
            
            const coreFloat   fHeight2   = m_pOutdoor->RetrieveBackHeight(vPosition + coreVector2(-fSize,0.0f));
            const coreFloat   fHeight3   = m_pOutdoor->RetrieveBackHeight(vPosition + coreVector2(0.0f,fSize));
            const coreFloat   fHeight4   = m_pOutdoor->RetrieveBackHeight(vPosition + coreVector2(0.0f,-fSize));

            // test for valid values
            if((fHeight > -15.0f) && (fHeight2 > -15.0f) && (fHeight3 > -15.0f) && (fHeight4 > -15.0f))// && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(22.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.1f) * fSize * 1.2f * 1.3f);
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                    pObject->SetDirection  (coreVector3(1.0f,1.0f,0.0f).Normalized());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -2.0f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, MOSS_GRAVE_RESERVE);
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
        oBase.DefineTexture(0u, "environment_plant.png");
        //oBase.DefineTexture(0u, "environment_sea.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_REED_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_REED_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -18.0f) && (fHeight < -15.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(7.0f))  &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(7.0f))  &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(15.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(15.0f)))
                    {
                        // determine object type
                        const coreBool bType = Core::Rand->Bool(0.7f);
                        
                        
                    //const coreVector3 vNormal = m_pOutdoor->RetrieveBackNormal(vPosition);

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        pObject->DefineModel(bType ? "environment_plant_08.md3" : "environment_plant_09.md3");
                        //pObject->DefineModel(bType ? "environment_sea_06.md3" : "environment_sea_07.md3");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 1.5f);
                        pObject->SetSize     (pObject->GetSize() * coreVector3(coreVector2(1.0f,1.0f) * (bType ? 1.0f : 1.3f), 1.0f));
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        //pObject->SetDirection(coreVector3::Cross(vNormal, coreVector3::Cross(vNormal, coreVector3(0.0f,0.0f,1.0f)).Normalized()));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,0.0f) * Core::Rand->Float(0.85f, 1.0f));
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
        this->_StoreHeight(pList1, -0.4f);
        this->_StoreHeight(pList2,  0.0f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SNOW_REED_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SNOW_REED_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate spark list
    pList1 = new coreBatchList(VOLCANO_SPARK_RESERVE*2u);
    pList1->DefineProgram("effect_decal_single_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_algae2.png");
        oBase.DefineProgram("effect_decal_single_program");

        for(coreUintW i = 0u; i < VOLCANO_SPARK_NUM/4; ++i)
        {
            for(coreUintW j = 20u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, VOLCANO_SPARK_NUM/4);
                const coreFloat   fHeight   = Core::Rand->Float(10.0f, 50.0f);

                // test for valid values
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2((i % 2u) ? 2.0f : 15.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition(coreVector3(vPosition, fHeight));
                    pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                    //pObject->SetColor4  (coreVector4(COLOR_ENERGY_ORANGE * (0.8f + 0.2f * fHeight/40.0f), 0.95f));
                    pObject->SetColor4  (coreVector4(LERP(COLOR_ENERGY_ORANGE, COLOR_ENERGY_YELLOW, 0.9f), 0.95f));
                    pObject->SetColor3  (coreVector3(1.0f,1.0f,1.0f) * 0.6f);
                    pObject->SetAlpha   (Core::Rand->Float(0.3f, 0.8f));

                    // add object to the list
                    pList1->BindObject(pObject);
                    break;
                }
            }
        }

        // 
        coreSet<coreObject3D*>* papContent = pList1->List();
        if(papContent->size() % 2u)
        {
            POOLED_DELETE(s_MemoryPool, papContent->back())
            papContent->pop_back();
        }

        // 
        m_iSparkNum = papContent->size();
        ASSERT(!(m_iSparkNum % 2u))

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1, VOLCANO_SPARK_RESERVE*2u);
        m_apAirObjectList.push_back(pList1);
    }
    
    
    
    
    
    
#if 1
    // allocate cloud list
    pList1 = new coreBatchList(GRASS_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < GRASS_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, GRASS_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor4   (coreVector4(COLOR_FIRE_ORANGE * (0.8f + 0.2f * fHeight/60.0f), 0.85f));
            pObject->SetColor4   (coreVector4(coreVector3(1.0f,1.0f,1.0f) * (0.3f + 0.2f * fHeight/60.0f), 0.85f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite   (pList1, GRASS_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == GRASS_CLOUD_RESERVE)
    }
#endif

    


    // 
    m_Smoke.DefineProgram("effect_particle_smoke_program"); 
    m_Smoke.DefineProgram("effect_particle_fire_program"); 
    m_Smoke.DefineTexture(0u, "effect_smoke.png"); 

    pList1 = m_apGroundObjectList[0]; 
    m_aSmokeEffect.reserve(pList1->List()->size()); 
    FOR_EACH(it, *pList1->List()) 
    {
        m_aSmokeEffect.emplace_back(&m_Smoke); 
        m_aSmokeEffect.back().SetOrigin(*it); 
    }
}


// ****************************************************************
// destructor
cVolcanoBackground::~cVolcanoBackground()
{
    

    m_Smoke.ClearAll();  

    m_aSmokeEffect.clear();
    
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cVolcanoBackground::__InitOwn()
{
    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_volcano.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cVolcanoBackground::__ExitOwn()
{
    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cVolcanoBackground::__RenderOwnBefore()
{
    glDisable(GL_BLEND);
    {
        // 
        m_Lava.Render();
    }
    glEnable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    {
        
        
            glBlendFunc        (FOREGROUND_BLEND_SUM);
            {
                // render fire particle system
                m_Smoke.Render();
            }
            glBlendFunc        (FOREGROUND_BLEND_DEFAULT);
                       
        // 
       // m_Smoke.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cVolcanoBackground::__MoveOwn()
{
    // 
    m_Lava.SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_Lava.Move();
    
    
    const coreFloat fShove = g_pEnvironment->GetFlyShove();
    if(fShove)
    {
        m_Smoke.ForEachParticleAll([&](coreParticle* OUTPUT pParticle, const coreUintW i)
        {
            pParticle->GetBeginState().vPosition.y += fShove;
            pParticle->GetEndState  ().vPosition.y += fShove;
        });
    }


    // 
    coreBatchList* pList = m_apGroundObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pSmoke = (*pList->List())[i];
        if(!pSmoke->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        m_aSmokeEffect[i].CreateParticle(1, 6.0f, [](coreParticle* OUTPUT pParticle)
        {
            constexpr coreFloat fScale = 10.0f;
            pParticle->SetPositionRel(coreVector3(0.0f,0.0f,0.0f), coreVector3::Rand(1.0f) + coreVector3::Rand(-fScale, fScale) + coreVector3(0.0f,20.0f,10.0f) * 2.0f);
            pParticle->SetScaleAbs   (4.5f,                              11.5f * 2.0f);
            pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        PI*0.5f);
            pParticle->SetColor4Abs  (coreVector4(COLOR_FIRE_ORANGE*0.926f, 0.8f), coreVector4(COLOR_FIRE_ORANGE*0.926f, 0.0f));
            pParticle->SetSpeed      (0.2f * Core::Rand->Float(0.9f, 1.1f));
        });
    }

    m_Smoke.Move();


    // 
    m_fSparkTime.Update(0.25f * MAX(ABS(g_pEnvironment->GetSpeed()), 2.0f));

    // 
  //  coreBatchList* 
        pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
    {
        coreObject3D* pSpark = (*pList->List())[i];
        if(!pSpark->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(POW2(i) % m_iSparkNum);
        const coreFloat   fTime   = m_fSparkTime * ((i % 2u) ? 1.0f : -1.0f) + fOffset;
        const coreFloat   fPos    = SIN(fTime * 0.1f + fOffset) * (I_TO_F(OUTDOOR_WIDTH) * OUTDOOR_DETAIL * 0.2f);
        const coreVector2 vDir    = coreVector2::Direction(fTime);

        // 
        pSpark->SetPosition (coreVector3(fPos, pSpark->GetPosition().yz()));
        pSpark->SetDirection(coreVector3(vDir, 0.0f));
    }
    pList->MoveNormal();


    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}