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
cMossBackground::cMossBackground()noexcept
: m_vRainMove        (coreVector2(-0.5f,-1.2f))
, m_fLightningDelay  (Core::Rand->Float(15.0f, 30.0f))
, m_fLightningFlash  (0.0f)
, m_LightningTicker  (coreTimer(1.0f, 1.0f, 1u))
, m_fThunderDelay    (0.0f)
, m_iThunderIndex    (Core::Rand->Uint(ARRAY_SIZE(m_apThunder) - 1u))
, m_bEnableLightning (true)
, m_bEnableHeadlight (false)
{
    coreBatchList* pList1;
    coreBatchList* pList2;
    coreBatchList* pList3;

    // 
    this->__InitOwn();

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("moss", "blood", OUTDOOR_ALGORITHM_MOSS, 4.5f, false);

    // 
    pList1 = new coreBatchList(MOSS_TREE_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(MOSS_TREE_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");

    pList3 = new coreBatchList(MOSS_TREE_3_RESERVE);
    pList3->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < MOSS_TREE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, MOSS_TREE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -20.0f) && (fHeight < -18.0f))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(10.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList2, vPosition, POW2(10.0f)) &&
                   !cBackground::_CheckIntersectionQuick(pList3, vPosition, POW2(15.0f)))
                {
                    // determine object type
                    const coreUint8 iType = Core::Rand->Bool() ? 0u : (Core::Rand->Bool(0.75f) ? 1u : 2u);

                    // 
                    const coreVector3 vNormal = (m_pOutdoor->RetrieveBackNormal(vPosition) * coreVector3(1.0f,1.0f,2.0f)).Normalized();

                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);
                    pObject->DefineModel  (    (iType == 0u) ? "environment_tree_01.md3"      : ((iType == 1u) ? "environment_tree_02.md3"      : "environment_tree_03.md3"));
                    pObject->DefineTexture(0u, (iType == 0u) ? "environment_tree_01_diff.png" : ((iType == 1u) ? "environment_tree_02_diff.png" : "environment_tree_03_diff.png"));
                    pObject->DefineTexture(1u, (iType == 0u) ? "environment_tree_01_norm.png" : ((iType == 1u) ? "environment_tree_02_norm.png" : "environment_tree_03_norm.png"));

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f) - vNormal * 5.0f);
                    pObject->SetSize       (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(3.5f, 4.2f) * 1.3f);
                    pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                         if(iType == 0u) cBackground::_BindSorted(pList1, pObject);
                    else if(iType == 1u) cBackground::_BindSorted(pList2, pObject);
                    else                 cBackground::_BindSorted(pList3, pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 0.0f);
        this->_StoreHeight(pList2, 0.0f);
        this->_StoreHeight(pList3, 1.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, MOSS_TREE_1_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2, MOSS_TREE_2_RESERVE);
        m_apGroundObjectList.push_back(pList2);

        cBackground::_FillInfinite(pList3, MOSS_TREE_3_RESERVE);
        m_apGroundObjectList.push_back(pList3);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
        m_pOutdoor->GetShadowMap()->BindList(pList3);
    }

    // 
    pList1 = new coreBatchList(MOSS_GRAVE_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  ("environment_grave.md3");
        oBase.DefineTexture(0u, "environment_grave_diff.png");
        oBase.DefineTexture(1u, "environment_grave_norm.png");
        oBase.DefineProgram("object_ground_program");

        for(coreUintW i = 0u; i < MOSS_GRAVE_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(-0.45f, 0.45f), i, MOSS_GRAVE_NUM);
            const coreFloat   fHeight   = m_pOutdoor->RetrieveBackHeight(vPosition);

            // test for valid values
            if((fHeight > -26.0f) && (fHeight < -21.0f) && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
            {
                if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, POW2(5.0f)))
                {
                    // create object
                    coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

                    // set object properties
                    pObject->SetPosition   (coreVector3(vPosition, 0.0f));
                    pObject->SetSize       (coreVector3(1.0f,1.0f,2.0f) * (Core::Rand->Bool() ? 3.0f : 5.0f) * 1.2f * 1.3f);
                    pObject->SetDirection  (coreVector3(AlongCrossNormal(coreVector2::Rand()), 0.0f));
                    pObject->SetOrientation(coreVector3(0.0f, 0.0f, Core::Rand->Bool() ? 1.0f : -1.0f));
                    pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                    // add object to the list
                    pList1->BindObject(pObject);
                }
            }
        }

        // 
        this->_StoreHeight(pList1, 5.0f);

        // post-process list and add to the ground
        cBackground::_FillInfinite(pList1, MOSS_GRAVE_RESERVE);
        m_apGroundObjectList.push_back(pList1);

        // 
        m_pOutdoor->GetShadowMap()->BindList(pList1);

        // 
        m_apWaterRefList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(MOSS_CLOUD_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        // load object resources
        coreObject3D oBase;
        oBase.DefineModel  (Core::Manager::Object->GetLowQuad());
        oBase.DefineTexture(0u, "environment_clouds_mid.png");
        oBase.DefineProgram("environment_clouds_program");

        for(coreUintW i = 0u; i < MOSS_CLOUD_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = __BACKGROUND_SCANLINE(Core::Rand->Float(0.1f, 0.25f) * ((i % 2u) ? 1.0f : -1.0f), i, MOSS_CLOUD_NUM);
            const coreFloat   fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // create object
            coreObject3D* pObject = POOLED_NEW(s_MemoryPool, coreObject3D, oBase);

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.4f,2.4f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (0.5f + 0.2f * fHeight/60.0f));
            pObject->SetAlpha    (0.85f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,4.0f, 0.0f,4.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add to the air
        cBackground::_FillInfinite   (pList1, MOSS_CLOUD_RESERVE);
        cBackground::_SortBackToFront(pList1);
        m_apAirObjectList.push_back(pList1);

        ASSERT(pList1->GetCurCapacity() == MOSS_CLOUD_RESERVE)
    }

    // 
    m_Rain.DefineTexture(0u, "effect_rain.png");
    m_Rain.DefineProgram("effect_weather_rain_moss_program");
    m_Rain.SetPosition  (coreVector2(0.0f,0.0f));
    m_Rain.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Rain.SetAlpha     (0.6f);

    // 
    m_Lightning.DefineProgram("menu_color_program");
    m_Lightning.SetPosition  (coreVector2(0.0f,0.0f));
    m_Lightning.SetSize      (coreVector2(1.0f,1.0f));
    m_Lightning.SetAlpha     (0.0f);
    m_Lightning.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_apThunder[0] = Core::Manager::Resource->Get<coreSound>("environment_thunder_01.wav");
    m_apThunder[1] = Core::Manager::Resource->Get<coreSound>("environment_thunder_02.wav");
    m_apThunder[2] = Core::Manager::Resource->Get<coreSound>("environment_thunder_03.wav");
}


// ****************************************************************
// destructor
cMossBackground::~cMossBackground()
{
    // 
    this->__ExitOwn();
}


// ****************************************************************
// 
void cMossBackground::__InitOwn()
{
    // 
    m_pWater = new cRainWater("environment_clouds_grey.png");

    // load base sound-effect
    m_pBaseSound = Core::Manager::Resource->Get<coreSound>("environment_moss.wav");
    m_pBaseSound.OnUsableOnce([this, pResource = m_pBaseSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// 
void cMossBackground::__ExitOwn()
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
void cMossBackground::__RenderOwnAfter()
{
    // enable the shader-program
    if(!m_Rain.GetProgram().IsUsable()) return;
    if(!m_Rain.GetProgram()->Enable())  return;

    // 
    coreProgram* pLocal = m_Rain.GetProgram().GetResource();
    for(coreUintW i = 0u; i < MOSS_RAIN_NUM; ++i)
    {
        const coreVector2 vNewTexOffset = m_Rain.GetTexOffset() + coreVector2(0.56f,0.36f) * I_TO_F(POW2(i));
        const coreFloat   fNewScale     = 1.0f - 0.15f * I_TO_F(i);

        pLocal->SendUniform(s_asOverlayTransform[i], coreVector3(vNewTexOffset.Processed(FRACT), fNewScale));
    }

    glDisable(GL_DEPTH_TEST);
    {
        // (# render first) 
        m_Rain.Render();

        // 
        m_Lightning.Render();

        // 
        if(m_bEnableHeadlight) m_Headlight.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the moss background
void cMossBackground::__MoveOwn()
{
    // 
    const coreVector2 vEnvMove   = coreVector2(0.0f,1.0f) * (-0.35f * MAX0(g_pEnvironment->GetSpeed()));
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 6.0f;
    const coreVector2 vTexOffset = m_Rain.GetTexOffset() + (coreVector2(0.0f, -m_vRainMove.Length()) + vEnvMove) * (1.0f * TIME);

    // 
    m_Rain.SetDirection(MapToAxisInv(-m_vRainMove.InvertedX().Normalized(), g_pEnvironment->GetDirection()));
    m_Rain.SetTexSize  (vTexSize);
    m_Rain.SetTexOffset(vTexOffset.Processed(FRACT));
    m_Rain.Move();

    if(m_bEnableLightning)
    {
        // 
        m_fLightningDelay.Update(-1.0f);
        if(m_fLightningDelay <= 0.0f)
        {
            // 
             m_fLightningDelay = Core::Rand->Float(15.0f, 30.0f);

             // 
             m_LightningTicker.SetSpeed   (Core::Rand->Float(8.0f, 11.0f));
             m_LightningTicker.SetMaxLoops(Core::Rand->Bool(0.67f) ? 3u : 2u);
             m_LightningTicker.Play       (CORE_TIMER_PLAY_RESET);

             // 
             m_fThunderDelay = -1.0f;
        }
    }

    // 
    const coreFloat fPrevDelay = m_fThunderDelay;

    // 
    m_LightningTicker.Update(1.0f);
    m_fThunderDelay  .Update(1.0f);

    // 
    m_fLightningFlash.UpdateMax(-1.0f, 0.0f);
    const coreFloat fValue = MAX(MIN1(m_fLightningFlash), m_LightningTicker.GetStatus() ? m_LightningTicker.GetValue(CORE_TIMER_GET_REVERSED) : 0.0f);

    // 
    m_Lightning.SetColor3 (g_CurConfig.Graphics.iFlash ? coreVector3(1.0f,1.0f,1.0f) : COLOR_MENU_BLACK);
    m_Lightning.SetAlpha  (BLENDH3(fValue) * 0.7f);
    m_Lightning.SetEnabled(fValue ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Lightning.Move();

    // 
    if((fPrevDelay < 0.0f) && (m_fThunderDelay >= 0.0f))
    {
        m_iThunderIndex = (m_iThunderIndex + Core::Rand->Uint(1u, ARRAY_SIZE(m_apThunder) - 1u)) % ARRAY_SIZE(m_apThunder);
        m_apThunder[m_iThunderIndex]->PlayRelative(this, 0.0f, 1.0f, false, SOUND_AMBIENT);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_apThunder); ++i)
    {
        if(m_apThunder[i]->EnableRef(this))
        {
            m_apThunder[i]->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
        }
    }

    // adjust volume of the base sound-effect
    if(m_pBaseSound->EnableRef(this))
    {
        m_pBaseSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }
}


// ****************************************************************
// 
void cMossBackground::__UpdateOwn()
{
    // 
    if(m_bEnableHeadlight) m_Headlight.UpdateDefault(0u);
}