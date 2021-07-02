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
cCrashManager::cCrashManager()noexcept
: m_aStatus     {}
, m_fSmokeTimer (0.0f)
, m_nCallback   (NULL)
{
    // 
    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i)
    {
        m_aObject[i].DefineProgram("object_ship_program");
        m_aObject[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// destructor
cCrashManager::~cCrashManager()
{
    // 
    ASSERT(!m_nCallback)
}


// ****************************************************************
// render the crash manager
void cCrashManager::Render()
{
    // 
    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i) m_aObject[i].Render();
    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aObject[i]);
}


// ****************************************************************
// move the crash manager
void cCrashManager::Move()
{
    // 
    const cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
    WARN_IF(!pOutdoor) this->ClearCrashes(true);

    // 
    m_fSmokeTimer.Update(30.0f);

    // 
    const coreBool bSmokeTick = (m_fSmokeTimer >= 1.0f);
    if(bSmokeTick) m_fSmokeTimer = FRACT(m_fSmokeTimer);

    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i)
    {
        cLodObject& oObject = m_aObject[i];
        sStatus&    oStatus = m_aStatus[i];

        if(oObject.IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            // 
            oStatus.fTime.Update(1.0f);

            // 
            const coreVector3 vPos = oObject.GetPosition() + oStatus.vAim * (40.0f * (oStatus.fTime + 1.0f) * TIME);
            const coreVector2 vDir = coreVector2::Direction(2.0f*PI * oStatus.fTime);

            // 
            oObject.SetPosition   (vPos);
            oObject.SetDirection  (coreVector3(vDir, 0.0f));
            oObject.SetOrientation(OriRoundDir(vDir, vDir));
            oObject.Move();

            // 
            if(bSmokeTick) g_pSpecialEffects->CreateSplashSmoke(vPos, 5.0f, 1u, coreVector3(1.0f,1.0f,1.0f));

            if(vPos.z < pOutdoor->RetrieveHeight(vPos.xy()))
            {
                // 
                if(m_nCallback) m_nCallback(oStatus.pData);

                // load object resources
                coreObject3D* pDecal = MANAGED_NEW(coreObject3D);
                pDecal->DefineModel  (Core::Manager::Object->GetLowQuad());
                pDecal->DefineTexture(0u, "effect_soot.png");
                pDecal->DefineProgram("effect_decal_single_program");

                // set object properties
                pDecal->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 12.0f);
                pDecal->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                pDecal->SetColor3   (coreVector3(0.0f,0.0f,0.0f));

                // add object to background
                g_pEnvironment->GetBackground()->AddDecal(pDecal, vPos, CRASH_OBJECTS, "effect_decal_single_inst_program", LIST_KEY);

                // 
                cCrashManager::__DisableObject(&oObject, true);
            }
        }
    }
}


// ****************************************************************
// 
void cCrashManager::AddCrash(const cLodObject& oBase, const coreVector2 vTarget, void* pData)
{
    // 
    const cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
    if(!pOutdoor) return;

    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i)
    {
        cLodObject& oObject = m_aObject[i];
        sStatus&    oStatus = m_aStatus[i];

        if(!oObject.IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            // 
            oObject.DefineModelHigh(oBase.GetModelHigh());
            oObject.DefineModelLow (oBase.GetModelLow ());
            oObject.DefineTexture  (0u, oBase.GetTexture(0u));

            // 
            oObject.SetPosition   (oBase.GetPosition   ());
            oObject.SetSize       (oBase.GetSize       ());
            oObject.SetDirection  (oBase.GetDirection  ());
            oObject.SetOrientation(oBase.GetOrientation());
            oObject.SetColor3     (oBase.GetColor3     ());

            // 
            oStatus.vAim  = (coreVector3(vTarget, pOutdoor->RetrieveHeight(vTarget)) - oBase.GetPosition()).Normalized();
            oStatus.fTime = 0.0f;
            oStatus.pData = pData;

            // 
            cCrashManager::__EnableObject(&oObject);

            return;
        }
    }

    // 
    WARN_IF(true) {}
}


// ****************************************************************
// 
void cCrashManager::ClearCrashes(const coreBool bAnimated)
{
    for(coreUintW i = 0u; i < CRASH_OBJECTS; ++i)
    {
        cLodObject& oObject = m_aObject[i];

        if(oObject.IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            // 
            cCrashManager::__DisableObject(&oObject, bAnimated);
        }
    }

    // 
    std::memset(m_aStatus, 0, sizeof(m_aStatus));
}


// ****************************************************************
// 
void cCrashManager::__EnableObject(cLodObject* OUTPUT pObject)
{
    ASSERT(!pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL))

    // 
    pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    cShadow::GetGlobalContainer()->BindObject(pObject);
}

void cCrashManager::__DisableObject(cLodObject* OUTPUT pObject, const coreBool bAnimated)
{
    ASSERT(pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL))

    // 
    pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    cShadow::GetGlobalContainer()->UnbindObject(pObject);

    // 
    pObject->DefineModelHigh(NULL);
    pObject->DefineModelLow (NULL);
    pObject->DefineTexture  (0u, NULL);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(pObject->GetPosition());
}