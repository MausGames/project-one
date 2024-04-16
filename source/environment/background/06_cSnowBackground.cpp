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
cSnowBackground::cSnowBackground()noexcept
: m_vSnowMove (coreVector2(0.0f,0.0f))
, m_fSnowWave (0.0f)
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("snow", "snow", 1u, 4.0f);
/*
    // allocate stone list
    pList1 = new coreBatchList(SNOW_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SNOW_STONE_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_desert.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_STONE_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_STONE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
          //  if((fHeight > -20.0f) && (fHeight < -15.0f) )//&& (((vPosition.x < 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 80 < 40)) || (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
            if((fHeight > -20.0f) && (fHeight < -16.0f) && ((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(7.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2, vPosition, POW2(6.0f)))
                {
                    // determine object type
                    const coreBool bType = Core::Rand->Bool();

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel(bType ? "environment_desert_04.md3" : "environment_desert_05.md3");

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 3.0f);
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                    //pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    if(bType) pList1->BindObject(pObject);
                         else pList2->BindObject(pObject);
                         break;
                }
            }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.3f);
        this->_StoreHeight(pList2, 0.3f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SNOW_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SNOW_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }
*/
    // 
    pList1 = new coreBatchList(SNOW_REED_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(SNOW_REED_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "environment_plant.png");
        //oBase.DefineTexture(0u, "environment_desert.png");
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
                //if((fHeight > -16.0f) && (fHeight < -5.0f) && (((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 80 < 40)) || (F_TO_SI(vPosition.y+160.0f) % 40 < 20))) //&& (ABS(vPosition.x) < 10.0f) )
                if((fHeight > -15.0f) && ((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 40 < 20))) //&& (ABS(vPosition.x) < 10.0f) )
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(9.0f)) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, POW2(9.0f)) )
                       // &&
                       //!cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(8.0f)) &&
                       //!cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(8.0f)))
                    {
                        // determine object type
                        const coreBool bType = Core::Rand->Bool(0.7f);//(F_TO_SI(vPosition.y+176.0f) % 80 < 30);//
                        
                        
                        // 
                        //const coreVector3 vDirection   = m_pOutdoor->RetrieveBackNormal(vPosition);
                        //const coreVector3 vTangent     = coreVector3::Cross(vDirection, -CAMERA_DIRECTION).Normalized();
                        //const coreVector3 vOrientation = coreVector3::Cross(vDirection, vTangent);
                    //const coreVector3 vNormal = m_pOutdoor->RetrieveBackNormal(vPosition);

                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                        //pObject->DefineTexture(0u, bType ? "environment_desert.png" : "environment_desert.png");
                        pObject->DefineModel(bType ? "environment_plant_06.md3" : "environment_plant_07.md3");
                        pObject->DefineModel("environment_plant_06.md3");
                        //pObject->DefineModel(bType ? "environment_desert_01.md3" : "environment_desert_01.md3");
                        //pObject->DefineModel(bType ? "environment_sea_06.md3" : "environment_sea_07.md3");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 1.5f * coreVector3(1.0f,1.0f,1.0f)    * ((bType) ? 0.7f : 1.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 1.5f * coreVector3(1.0f,1.0f,1.0f)    * ((bType) ? 0.7f : 0.7f));
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));

                        //pObject->SetDirection  (vOrientation);
                        //pObject->SetOrientation(vDirection);
                        //pObject->SetDirection(coreVector3::Cross(vNormal, coreVector3::Cross(vNormal, coreVector3(0.0f,0.0f,1.0f)).Normalized()));
                        pObject->SetColor3   (coreVector3(0.7f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));
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
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, SNOW_REED_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // 
    pList1 = new coreBatchList(SNOW_REED_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineTexture(0u, "default_white.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineModel("object_sting.md3");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_REED_NUM; ++i)
        {
            for(coreUintW j = 8u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_REED_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                //if(fHeight > -17.0f && (F_TO_SI(vPosition.y+160.0f) % 80 >= 40)) //&& (ABS(vPosition.x) < 10.0f) )//&& (((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 80 < 40)) || (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
                if(fHeight > -15.0f)// && ((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 40 >= 20))) //&& (ABS(vPosition.x) < 10.0f) )//&& (((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 80 < 40)) || (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(6.0f)) 
                    &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(8.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(8.0f)) //&&
                       //!cBackground::_CheckIntersection     (m_apGroundObjectList[1], vPosition, POW2(10.0f)) &&
                       //!cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(10.0f))
                       )
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.1f * 1.1f * 4.0f  * coreVector3(1.0f,1.0f,1.0f) * 0.7f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));

                            pObject->SetSize(coreVector3(1.5f,1.5f,10.0f));
                            pObject->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                            pObject->SetOrientation(coreVector3(pObject->GetPosition().x * -0.02f, 0.0f, 1.0f).Normalized());
                            pObject->SetColor3(coreVector3(0.0f,0.455f,0.694f) * Core::Rand->Float(0.85f, 1.0f));

                        //pObject->SetDirection  (vOrientation);
                        //pObject->SetOrientation(vDirection);
                        //pObject->SetDirection(coreVector3::Cross(vNormal, coreVector3::Cross(vNormal, coreVector3(0.0f,0.0f,1.0f)).Normalized()));
                        pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));
                       // pObject->SetColor3   (LERP(coreVector3(54.0f/255.0f, 204.0f/255.0f, 255.0f/255.0f), coreVector3(1.0f,1.0f,1.0f), Core::Rand->Float(1.0f)));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -2.0f);

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1, SNOW_REED_RESERVE);
        m_apGroundObjectList.insert(m_apGroundObjectList.begin(), pList1);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(SNOW_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_high.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < SNOW_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, SNOW_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor4   (coreVector4(coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f), 0.45f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite   (pList1, SNOW_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == SNOW_CLOUD_RESERVE)
    }

    // 
    m_Snow.DefineTexture(0u, "effect_snow.png");
    m_Snow.DefineProgram("effect_weather_snow_program");
    m_Snow.SetPosition  (coreVector2(0.0f,0.0f));
    m_Snow.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Snow.SetAlpha     (0.9f);
}


// ****************************************************************
// destructor
cSnowBackground::~cSnowBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cSnowBackground::__InitOwn()
{
    // 
    m_pWater = new cIceWater("environment_clouds_blue.png");

    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_snow.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cSnowBackground::__ExitOwn()
{
    // 
    SAFE_DELETE(m_pWater)

    // stop base sound-effect
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        if(pResource->EnableRef(this))
            pResource->Stop();
    });
}


// ****************************************************************
// 
void cSnowBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Snow.GetProgram().IsUsable()) return;
    if(!m_Snow.GetProgram()->Enable())  return;

    // 
    coreProgram* pLocal = m_Snow.GetProgram().GetResource();
    for(coreUintW i = 0u; i < SNOW_SNOW_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Snow.GetTexOffset() + coreVector2(0.56f,0.36f) * I_TO_F(POW2(i)) + coreVector2(0.13f * SIN(m_fSnowWave * (0.125f*PI) + I_TO_F(POW2(i))), 0.0f);
        const coreFloat   fNewScale     = 0.9f - 0.07f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Snow.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the snow background
void cSnowBackground::__MoveOwn()
{
    // 
    const coreVector2 vEnvMove   = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 5.4f;
    const coreVector2 vTexOffset = m_Snow.GetTexOffset() + (m_vSnowMove.InvertedX() + vEnvMove) * (0.9f * TIME);

    // 
    m_Snow.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Snow.SetTexSize  (vTexSize);
    m_Snow.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Snow.Move();

    // 
    m_fSnowWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}