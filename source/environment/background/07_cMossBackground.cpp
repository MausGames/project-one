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
cMossBackground::cMossBackground()noexcept
: m_vRainMove        (coreVector2(0.0f,-1.2f))
, m_fLightningDelay  (Core::Rand->Float(15.0f, 30.0f))
, m_LightningTicker  (coreTimer(1.0f, 1.0f, 1u))
, m_fThunderDelay    (0.0f)
, m_iThunderIndex    (Core::Rand->Int(ARRAY_SIZE(m_apThunder) - 1))
, m_bEnableLightning (true)
, m_bEnableHeadlight (false)
{
    coreBatchList* pList1;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("moss", "blood", 5u, 4.5f);

    // 
    m_pWater = new cRainWater("environment_clouds_grey.png");

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
            pObject->SetColor4   (coreVector4(coreVector3(1.0f,1.0f,1.0f) * (0.8f + 0.2f * fHeight/60.0f), 0.85f));
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

    // 
    m_Rain.DefineTexture(0u, "effect_rain.png");
    m_Rain.DefineProgram("effect_weather_rain_program");
    m_Rain.SetPosition  (coreVector2(0.0f,0.0f));
    m_Rain.SetSize      (coreVector2(1.0f,1.0f) * SQRT2);
    m_Rain.SetAlpha     (0.6f);

    // 
    m_Lightning.DefineTexture(0u, "default_white.png");
    m_Lightning.DefineProgram("default_2d_program");
    m_Lightning.SetPosition  (coreVector2(0.0f,0.0f));
    m_Lightning.SetSize      (coreVector2(1.0f,1.0f));
    m_Lightning.SetAlpha     (0.0f);
    m_Lightning.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_apThunder[0] = Core::Manager::Resource->Get<coreSound>("environment_thunder_01.wav");
    m_apThunder[1] = Core::Manager::Resource->Get<coreSound>("environment_thunder_02.wav");
    m_apThunder[2] = Core::Manager::Resource->Get<coreSound>("environment_thunder_03.wav");

    // 
    m_pRainSound = Core::Manager::Resource->Get<coreSound>("environment_rain.wav");
    m_pRainSound.OnUsableOnce([this, pResource = m_pRainSound]()
    {
        pResource->PlayRelative(this, 0.0f, 1.0f, true, SOUND_AMBIENT);
    });
}


// ****************************************************************
// destructor
cMossBackground::~cMossBackground()
{
    // 
    if(m_pRainSound->EnableRef(this))
        m_pRainSound->Stop();
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
        // 
        m_Rain     .Render();
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
    const coreVector2 vEnvMove   = coreVector2(0.0f,1.0f) * (-0.35f * g_pEnvironment->GetSpeed());
    const coreVector2 vTexSize   = coreVector2(1.0f,1.0f) * 6.0f;
    const coreVector2 vTexOffset = m_Rain.GetTexOffset() + (m_vRainMove.InvertedX() + vEnvMove) * (1.0f * TIME);

    // 
    m_Rain.SetDirection(g_pEnvironment->GetDirection().InvertedX());
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
    m_Lightning.SetAlpha  (m_LightningTicker.GetValue(CORE_TIMER_GET_REVERSED) * 0.7f);
    m_Lightning.SetEnabled(m_LightningTicker.GetStatus() ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Lightning.Move();

    // 
    if((fPrevDelay < 0.0f) && (m_fThunderDelay >= 0.0f))
    {
        m_iThunderIndex = (m_iThunderIndex + Core::Rand->Int(1, ARRAY_SIZE(m_apThunder) - 1)) % ARRAY_SIZE(m_apThunder);
        m_apThunder[m_iThunderIndex]->PlayRelative(this, 0.0f, 1.0f, false, SOUND_AMBIENT);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_apThunder); ++i)
    {
        if(m_apThunder[i]->EnableRef(this))
            m_apThunder[i]->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
    }

    // 
    if(m_pRainSound->EnableRef(this))
        m_pRainSound->SetVolume(g_pEnvironment->RetrieveTransitionBlend(this));
}


// ****************************************************************
// 
void cMossBackground::__UpdateOwn()
{
    // 
    if(m_bEnableHeadlight) m_Headlight.UpdateDefault();
}