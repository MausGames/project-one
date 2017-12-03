//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSnowBackground::cSnowBackground()noexcept
: m_vSnowDirection (coreVector2(0.0f,1.0f))
, m_fSnowWave      (0.0f)
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("snow", "snow", 1u, 4.0f);

    // 
    m_pWater = new cIceWater("environment_clouds_blue.png");

    // allocate stone list
    pList1 = new coreBatchList(SNOW_STONE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_stone_01.md3");
        oBase.DefineTexture(0u, "environment_stone_diff.png");
        oBase.DefineTexture(1u, "environment_stone_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_STONE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_STONE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -20.0f) && (fHeight < -17.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 25.0f))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(2.0f, 2.6f));
                    pObject->SetDirection  (coreVector3::Rand());
                    pObject->SetOrientation(coreVector3::Rand());
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.2f);

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1, SNOW_STONE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate reed lists
    pList1 = new coreBatchList(SNOW_REED_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_reed_01.md3");
        oBase.DefineTexture(0u, "environment_snow_diff.png");
        oBase.DefineTexture(1u, "environment_snow_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < SNOW_REED_NUM; ++i)
        {
            for(coreUintW j = 4u; j--; )   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, SNOW_REED_NUM);
                const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

                // test for valid values
                if((fHeight > -20.0f) && (fHeight < -15.0f) && (F_TO_SI(vPosition.y+176.0f) % 80 < 60))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f))
                    {
                        // create object
                        coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, 0.0f));
                        pObject->SetSize     (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 2.1f);
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetColor3   (LERP(coreVector3(54.0f/255.0f, 204.0f/255.0f, 255.0f/255.0f), coreVector3(1.0f,1.0f,1.0f), Core::Rand->Float(1.0f)));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // 
        this->_StoreHeight(pList1, -0.8f);

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
// render the snow background
void cSnowBackground::__RenderOwn()
{
    // enable the shader-program
    if(!m_Snow.GetProgram().IsUsable()) return;
    if(!m_Snow.GetProgram()->Enable())  return;

    // 
    for(coreUintW i = 0u; i < SNOW_SNOW_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Snow.GetTexOffset() + coreVector2(0.56f,0.36f) * I_TO_F(i*i) + coreVector2(0.13f * SIN(m_fSnowWave * (0.125f*PI) + I_TO_F(i*i)), 0.0f);
        const coreFloat   fNewScale     = 0.9f - 0.07f * I_TO_F(i);

        m_Snow.GetProgram()->SendUniform(PRINT("u_av3OverlayTransform[%zu]", i), coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    // 
    glDisable(GL_DEPTH_TEST);
    m_Snow.Render();
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the snow background
void cSnowBackground::__MoveOwn()
{
    // 
    const coreVector2 vMove      = m_vSnowDirection * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 5.4f;
    const coreVector2 vTexOffset = m_Snow.GetTexOffset() + (coreVector2(0.0f,0.0f) + vMove) * (0.9f * Core::System->GetTime());

    // 
    m_Snow.SetDirection((m_vSnowDirection.InvertedX() * coreMatrix3::Rotation(g_pEnvironment->GetDirection()).m12()).Normalized());
    m_Snow.SetTexSize  (vTexSize);
    m_Snow.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Snow.Move();

    // 
    m_fSnowWave.UpdateMod(SQRT(MAX(ABS(g_pEnvironment->GetSpeed()), 1.0f)), 16.0f);

    // TODO: improve snow texture to little flakes (broken quads) 
}