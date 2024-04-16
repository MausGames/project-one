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
: cBackground (false)
, m_vSnowMove (coreVector2(0.0f,0.0f))
, m_fSnowWave (0.0f)
{
    coreBatchList* pList1;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("snow", "snow", OUTDOOR_ALGORITHM_SNOW, 4.0f, false);

    // 
    pList1 = new coreBatchList(SNOW_PLANT_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_plant_03.md3");
        oBase.DefineTexture(0u, "environment_plant.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_PLANT_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_PLANT_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -15.0f) && ((vPosition.x > 0.0f) == (F_TO_SI(vPosition.y+160.0f) % 40 < 20)))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(9.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 1.27f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (coreVector3(0.7f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SNOW_PLANT_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    pList1 = new coreBatchList(SNOW_STING_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("object_sting.md3");
        oBase.DefineTexture(0u, "default_white.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_STING_1_NUM; ++i)
        {
            for(coreUintW j = 8u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_STING_1_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if(fHeight > -15.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, POW2(6.0f)) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, POW2(8.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3(1.5f,1.5f,10.0f));
                        pObject->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                        pObject->SetOrientation(coreVector3(pObject->GetPosition().x * -0.02f, 0.0f, 1.0f).Normalized());
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -2.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SNOW_STING_1_RESERVE);
        m_apGroundObjectList.push_front(pList1);   // # front

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // 
    pList1 = new coreBatchList(SNOW_STING_2_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("object_sting.md3");
        oBase.DefineTexture(0u, "default_white.png");
        oBase.DefineTexture(1u, "default_normal.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_STING_2_NUM; ++i)
        {
            for(coreUintW j = 1u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_STING_2_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if(fHeight < -24.0f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(6.0f)))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                        pObject->SetSize       (coreVector3(1.5f,1.5f,10.0f));
                        pObject->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                        pObject->SetOrientation(coreVector3(pObject->GetPosition().x * 0.01f, 0.0f, 1.0f).Normalized());
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -2.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, SNOW_STING_2_RESERVE);
        m_apGroundObjectList.push_front(pList1);   // # front

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(SNOW_CLOUD_1_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_high.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < SNOW_CLOUD_1_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, SNOW_CLOUD_1_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f));
            pObject->SetAlpha    (0.45f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,4.0f, 0.0f,4.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, SNOW_CLOUD_1_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCapacity() == SNOW_CLOUD_1_RESERVE)
    }

    // allocate cloud list
    pList1 = new coreBatchList(SNOW_CLOUD_2_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < SNOW_CLOUD_2_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.05f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, SNOW_CLOUD_2_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f));
            pObject->SetAlpha    (0.85f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,4.0f, 0.0f,4.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, SNOW_CLOUD_2_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCapacity() == SNOW_CLOUD_2_RESERVE)
    }

    // 
    m_Snow.DefineTexture(0u, "effect_snow.png");
    m_Snow.DefineProgram("effect_weather_snow_program");
    m_Snow.SetPosition  (coreVector2(0.0f,0.0f));
    m_Snow.SetAlpha     (0.9f);

    // 
    this->SetGroundDensity(0u, 0.0f);
    this->SetAirDensity   (1u, 0.0f);
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
    for(coreUintW i = 0u, ie = CORE_GL_SUPPORT(ES2_restriction) ? SNOW_SNOW_NUM_LOW : SNOW_SNOW_NUM; i < ie; ++i)
    {
        const coreVector2 vNewTexOffset = m_Snow.GetTexOffset() + coreVector2(0.56f,0.36f) * I_TO_F(POW2(i)) + coreVector2(0.13f * SIN(m_fSnowWave * (0.125f*PI) + I_TO_F(POW2(i))), 0.0f);
        const coreFloat   fNewScale     = 0.8f - 0.06f * I_TO_F(i);

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
    m_Snow.SetSize     (coreVector2(1.0f,1.0f) * SQRT2 * ENVIRONMENT_SCALE_FACTOR);
    m_Snow.SetDirection(g_pEnvironment->GetDirection().InvertedX());
    m_Snow.SetTexSize  (vTexSize);
    m_Snow.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Snow.Move();

    // 
    m_fSnowWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // adjust volume of the base sound-effect
    if(m_pBaseSound.IsUsable() && m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
    
    
    
    const coreFloat fCloudMove = 0.0018f * (1.0f + ABS(g_pEnvironment->GetSpeed())) * TIME;

    coreBatchList* pList = m_apAirObjectList[0];
    for(coreUintW i = 0u, ie = LOOP_NONZERO(pList->GetSize()); i < ie; ++i)
    {
        coreObject3D* pCloud = (*pList->List())[i];
        pCloud->SetTexOffset((pCloud->GetTexOffset() + MapToAxis(coreVector2(fCloudMove * ((pCloud->GetDirection().x < 0.0f) ? -1.0f : 1.0f), 0.0f), pCloud->GetDirection().xy())).Processed(FRACT));
    }
    pList->MoveNormal();

    pList = m_apAirObjectList[1];
    if(pList->GetNumEnabled())
    {
        for(coreUintW i = 0u, ie = LOOP_NONZERO(pList->GetSize()); i < ie; ++i)
        {
            coreObject3D* pCloud = (*pList->List())[i];
            pCloud->SetTexOffset((pCloud->GetTexOffset() + MapToAxis(coreVector2(fCloudMove * ((pCloud->GetDirection().x < 0.0f) ? -1.0f : 1.0f), 0.0f), pCloud->GetDirection().xy())).Processed(FRACT));
        }
        pList->MoveNormal();
    }
}