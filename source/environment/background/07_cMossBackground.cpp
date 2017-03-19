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
cMossBackground::cMossBackground()noexcept
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("moss", "grass", 5u, 4.5f);

    // 
    m_pWater = new cRainWater();

    // 
    pList1 = new coreBatchList(MOSS_RAIN_NUM);
    pList1->DefineProgram("effect_decal_single_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowModel());
        oBase.DefineTexture(0u, "effect_rain.png");
        oBase.DefineProgram("effect_decal_single_program");

        for(coreUintW i = 0u; i < MOSS_RAIN_NUM; ++i)
        {
            // create object
            coreObject3D* pObject = CUSTOM_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition(coreVector3(0.0f,0.0f,0.0f));
            pObject->SetSize    (coreVector3(1.0f,1.0f,1.0f) * SQRT2 * 80.0f);
            pObject->SetAlpha   (0.6f);

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        m_apAirObjectList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(MOSS_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowModel());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < MOSS_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, MOSS_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = CUSTOM_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor4   (coreVector4(0.8f + 0.2f * fHeight/60.0f, 1.0f, 1.0f, 0.85f));
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite   (pList1, MOSS_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == MOSS_CLOUD_RESERVE)
    }
}


// ****************************************************************
// move the moss background
void cMossBackground::__MoveOwn()
{
    // 
    coreBatchList*      pList = m_apAirObjectList[0];
    const coreObject3D* pBase = (*pList->List()) [0];

    // 
    const coreFloat   fStrength  = 1.0f;
    const coreVector2 vPosition  = g_pEnvironment->GetCameraPos().xy();
    const coreVector2 vDirection = coreVector2(-1.0f, g_pEnvironment->GetSpeed()).Normalized();
    const coreFloat   fLength    = 1.0f - 0.04f * g_pEnvironment->GetSpeed();
    const coreVector2 vMove      = vDirection * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f, fLength) * (6.0f * fStrength);
    const coreVector2 vTexOffset = pBase->GetTexOffset() + (coreVector2(0.0f,-1.2f) + vMove) * coreVector2(1.0f, fLength) * (Core::System->GetTime() * fStrength);

    // 
    for(coreUintW i = 0u; i < MOSS_RAIN_NUM; ++i)
    {
        coreObject3D* pRain = (*pList->List())[i];

        pRain->SetPosition (coreVector3(vPosition,  15.0f * I_TO_F(i)));
        pRain->SetDirection(coreVector3(vDirection, 0.0f));
        pRain->SetTexSize  ((vTexSize));
        pRain->SetTexOffset((vTexOffset + coreVector2(0.56f,0.36f) * I_TO_F(i*i)).Processed(FRACT));
    }
    pList->MoveNormal();
}