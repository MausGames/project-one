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
cCalorMission::cCalorMission()noexcept
: m_Load        (CALOR_LOADS)
, m_pLoadOwner  (NULL)
, m_afLoadPower {}
, m_fAnimation  (0.0f)
{
    // 
    m_apBoss[0] = &m_Fenrir;
    m_apBoss[1] = &m_Shelob;
    m_apBoss[2] = &m_Zeroth;

    // 
    m_Load.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < CALOR_LOADS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pLoad = &m_aLoadRaw[i];
            pLoad->DefineModel  ("object_sphere.md3");
            pLoad->DefineTexture(0u, "effect_energy.png");
            pLoad->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLoad->SetTexSize(coreVector2(3.0f,3.0f));
            pLoad->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Load.BindObject(pLoad);
        }
    }

    // 
    m_LoadCopy.DefineTexture(0u, "effect_energy.png");
    m_LoadCopy.DefineProgram("effect_energy_invert_program");
    m_LoadCopy.SetColor3    (COLOR_ENERGY_RED);
    m_LoadCopy.SetTexSize   (coreVector2(3.0f,3.0f));
    m_LoadCopy.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    g_pGlow->BindList(&m_Load);
}


// ****************************************************************
// destructor
cCalorMission::~cCalorMission()
{
    // 
    m_Snow.Disable(0.0f);

    // 
    g_pGlow->UnbindList(&m_Load);

    // 
    this->DisableLoad(false);
}


// ****************************************************************
// 
void cCalorMission::EnableLoad(const cShip* pOwner)
{
    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    WARN_IF(oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pLoadOwner     = pOwner;
    m_afLoadPower[0] = 0.0f;
    m_afLoadPower[1] = 0.0f;
    m_afLoadPower[2] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nInitFunc(&oLoad + i);
    nInitFunc(&m_LoadCopy);

    // 
    m_LoadCopy.DefineModel(pOwner->GetModelHigh());
    g_pGlow->BindObject(&m_LoadCopy);
}


// ****************************************************************
// 
void cCalorMission::DisableLoad(const coreBool bAnimated)
{
    coreObject3D& oLoad = m_aLoadRaw[0];

    // 
    if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_pLoadOwner = NULL;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [&](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        for(coreUintW i = 0u; i < CALOR_LOADS; ++i) nExitFunc(&oLoad + i);
        nExitFunc(&m_LoadCopy);

        // 
        m_LoadCopy.DefineModel(NULL);
        g_pGlow->UnbindObject(&m_LoadCopy);
    }
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Snow.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_LoadCopy.Render();
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    m_Load.Render();
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_Snow.Move();

    // 
    for(coreUintW i = 0u; i < CALOR_LOADS; ++i)
    {
        coreObject3D& oLoad = m_aLoadRaw[i];
        if(!oLoad.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vDir   = coreVector2::Direction((I_TO_F(i) / I_TO_F(CALOR_LOADS)) * (2.0f*PI)).InvertedX();
        const coreFloat   fScale = CLAMP(m_afLoadPower[0] - I_TO_F(i), 0.0f, 1.0f);

        // 
        const cShip* pOwner = m_pLoadOwner;
        if(pOwner)
        {
            // 
            oLoad.SetPosition(coreVector3(pOwner->GetPosition().xy() + vDir * (pOwner->GetCollisionRadius() + 3.5f), 0.0f));
            oLoad.SetAlpha   (1.0f);
        }
        else
        {
            // 
            oLoad.SetPosition(coreVector3(oLoad.GetPosition().xy() + vDir * (50.0f * TIME), 0.0f));
            oLoad.SetAlpha   (MAX(oLoad.GetAlpha() - 3.0f * TIME, 0.0f));

            // 
            if(!i && !oLoad.GetAlpha()) this->DisableLoad(false);
        }

        // 
        oLoad.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.7f * fScale);
        oLoad.SetColor3   (COLOR_ENERGY_RED * ((fScale >= 1.0f) ? 1.0f : 0.5f));
        oLoad.SetTexOffset(coreVector2(0.0f, FRACT(-2.0f * m_fAnimation)));
    }

    // 
    m_Load.MoveNormal();

    // 
    if(m_pLoadOwner)
    {
        // 
        m_LoadCopy.SetPosition   (m_pLoadOwner->GetPosition   ());
        m_LoadCopy.SetSize       (m_pLoadOwner->GetSize       ());
        m_LoadCopy.SetDirection  (m_pLoadOwner->GetDirection  ());
        m_LoadCopy.SetOrientation(m_pLoadOwner->GetOrientation());
        m_LoadCopy.SetTexOffset  (coreVector2(FRACT(1.4f * m_fAnimation), 0.0f));   // TODO: fix model tex coords

        // 
        if(F_TO_UI(m_afLoadPower[0]) != F_TO_UI(m_afLoadPower[1]))
        {
            const coreUintW iIndex = F_TO_UI(m_afLoadPower[1]);
            ASSERT(iIndex < CALOR_LOADS)

            g_pSpecialEffects->CreateSplashColor(m_aLoadRaw[iIndex].GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
        }
    }
    m_LoadCopy.SetAlpha(m_afLoadPower[2] * m_aLoadRaw[0].GetAlpha());
    m_LoadCopy.Move();

    // 
    m_afLoadPower[0].UpdateMax(-0.6f, FLOOR(m_afLoadPower[0]));
    m_afLoadPower[1] = m_afLoadPower[0];
    m_afLoadPower[2].UpdateMax(-1.0f, 0.0f);
}