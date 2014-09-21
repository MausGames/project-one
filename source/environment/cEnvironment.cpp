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
cBackground::cBackground()noexcept
: m_pOutdoor (NULL)
{
    // create background frame buffer
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture
    m_iResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);
}


// ****************************************************************
// destructor
cBackground::~cBackground()
{
    ASSERT(!m_pOutdoor)

    // remove all ground objects
    FOR_EACH(it, m_apGroundObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            SAFE_DELETE(*et)
        SAFE_DELETE(*it)
    }

    // remove all air objects
    FOR_EACH(it, m_apAirObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            SAFE_DELETE(*et)
        SAFE_DELETE(*it)
    }

    // remove all additional objects
    this->ClearObjects();

    // clear memory
    m_apGroundObjectList.clear();
    m_apAirObjectList.clear();
}


// ****************************************************************
// render the background
void cBackground::Render()
{
    // 
    this->RenderOwn();

    // resolve frame buffer to texture
    m_iFrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_iResolvedTexture);
    m_iFrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the background
void cBackground::Move()
{
    // 
    this->MoveOwn();

    // 
    FOR_EACH(it, m_apGroundObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            (*et)->SetEnabled(coreMath::InRange((*et)->GetPosition().y, Core::Graphics->GetCamPosition().y, 100.0f) ? 
                              CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        (*it)->MoveNormal();
    }

    // 
    FOR_EACH(it, m_apAirObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            (*et)->SetEnabled(coreMath::InRange((*et)->GetPosition().y, Core::Graphics->GetCamPosition().y, 100.0f) ?
                              CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        (*it)->MoveNormal();
    }
}


// ****************************************************************
// add additional object
void cBackground::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos)
{

}


// ****************************************************************
// remove all additional objects
void cBackground::ClearObjects()
{

}


// ****************************************************************
// constructor
cEnvironment::cEnvironment()noexcept
: m_pBackground    (NULL)
, m_pOldBackground (NULL)
, m_Transition     (coreTimer(1.0f, 1.0f, 1))
, m_fFlyOffset     (0.0f)
{
    // create environment frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create mix object
    m_MixObject.DefineProgram("full_transition_program");
    m_MixObject.SetSize     (coreVector2(1.0f, 1.0f));
    m_MixObject.SetTexSize  (coreVector2(1.0f,-1.0f));
    m_MixObject.SetTexOffset(coreVector2(0.0f, 1.0f));
    m_MixObject.Move();

    // reset transformation properties
    m_avDirection[1] = m_avDirection[0] = coreVector2(0.0f,1.0f);
    m_avSide     [1] = m_avSide     [0] = coreVector2(0.0f,0.0f);
    m_afSpeed    [1] = m_afSpeed    [0] = 6.0f;

    // load first background
    this->ChangeBackground(Core::Config->GetInt("Game", "Background", cGrass::ID));
}


// ****************************************************************
// destructor
cEnvironment::~cEnvironment()
{
    // save last background
    Core::Config->SetInt("Game", "Background", m_pBackground->GetID());

    // explicitly undefine the mix object
    m_MixObject.Undefine();

    // delete background instances
    SAFE_DELETE(m_pBackground)
    SAFE_DELETE(m_pOldBackground)
}


// ****************************************************************
// render the environment
void cEnvironment::Render()
{
    // 
    m_pBackground->Render(); 

    if(m_Transition.GetStatus())
    {
        // 
        m_pOldBackground->Render();

        // set transition time
        m_MixObject.GetProgram()->Enable();
        m_MixObject.GetProgram()->SendUniform("u_fTransition", m_Transition.GetValue(CORE_TIMER_GET_NORMAL));

        // 
        m_iFrameBuffer.StartDraw();
        m_MixObject.Render();

        // 
        m_pOldBackground->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
        m_pBackground   ->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    }
}


// ****************************************************************
// move the environment
void cEnvironment::Move()
{
    // 
    m_pBackground->Move();

    if(m_Transition.GetStatus())
    {
        // 
        if(m_Transition.Update(1.0f))
        {
            // 
            m_MixObject.DefineTexture(0, NULL);
            SAFE_DELETE(m_pOldBackground)
        }
        else m_pOldBackground->Move();
    }

    // 
    const float fFactor = Core::System->GetTime() * 2.0f;
    m_avDirection[0] = (m_avDirection[0] + (m_avDirection[1] - m_avDirection[0]) * fFactor).Normalize();
    m_avSide     [0] =  m_avSide     [0] + (m_avSide     [1] - m_avSide     [0]) * fFactor;
    m_afSpeed    [0] =  m_afSpeed    [0] + (m_afSpeed    [1] - m_afSpeed    [0]) * fFactor;

    // calculate side movement (only perpendicular to flight direction, never on diagonal camera (smooth with max-min))
    const coreVector2 vAbsDir   = coreVector2(ABS(m_avDirection[0].x), ABS(m_avDirection[0].y));
    const float       fRealSide = coreVector2::Dot(m_avDirection[0].yx(), m_avSide[0]) * (vAbsDir.Max() - vAbsDir.Min());

    // update and calculate the global fly offset
    m_fFlyOffset += Core::System->GetTime() * m_afSpeed[0];
    while(m_fFlyOffset <  0.0f)                  m_fFlyOffset += float(OUTDOOR_HEIGHT);
    while(m_fFlyOffset >= float(OUTDOOR_HEIGHT)) m_fFlyOffset -= float(OUTDOOR_HEIGHT);

    // 
    Core::Graphics->SetCamera(coreVector3(fRealSide, m_fFlyOffset * OUTDOOR_DETAIL, Core::Graphics->GetCamPosition().z),
                              Core::Graphics->GetCamDirection(),
                              coreVector3(m_avDirection[0], 0.0f));
}


// ****************************************************************
// change current background
void cEnvironment::ChangeBackground(const coreByte& iID)
{
    if(m_pBackground) if(m_pBackground->GetID() == iID) return;

    // 
    m_MixObject.DefineTexture(0, NULL);
    SAFE_DELETE(m_pOldBackground)
    m_pOldBackground = m_pBackground;

    // 
    switch(iID)
    {
    default: ASSERT(false)
    case cGrass::ID: m_pBackground = new cGrass(); break;
    }

    if(m_pOldBackground)
    {
        // start transition
        m_Transition.Play(CORE_TIMER_PLAY_RESET);

        // set transition textures
        m_MixObject.DefineTexture(0, m_pOldBackground->GetResolvedTexture()->GetColorTarget(0).pTexture);
        m_MixObject.DefineTexture(1, m_pBackground   ->GetResolvedTexture()->GetColorTarget(0).pTexture);

        // set transition type
        m_MixObject.GetProgram()->Enable();
        m_MixObject.GetProgram()->SendUniform("##transition_type", 0); 
    }
}


// ****************************************************************
// retrieve safe height value
float cEnvironment::RetrieveSafeHeight(const coreVector2& vPosition)
{
    // check for available outdoor-surface
    cOutdoor* pOutdoor = m_pBackground->GetOutdoor();
    if(!pOutdoor) return 0.0f;

    // retrieve height value
    const float fHeight = pOutdoor->RetrieveHeight(vPosition);

    // check for possible limitations
    const int& iID = m_pBackground->GetID();
    if(iID == cGrass::ID) return MAX(fHeight, WATER_HEIGHT);

    return fHeight;
}


// ****************************************************************
// reset with the resource manager
void cEnvironment::__Reset(const coreResourceReset& bInit)
{
    // TODO #
}


// ****************************************************************
// constructor
cGrass::cGrass()noexcept
{
    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("grass", "dust", 1, 4.0f);

    // 
    coreBatchList* pList = new coreBatchList(256);
    pList->DefineProgram("object_inst_program");

    for(int j = 0; j < 2; ++j)
    {
        const int   iStoneTries = j ? 768  : 512;
        const float fStoneSize  = j ? 2.0f : 2.5f;

        // 
        std::function<float(const float&, const float&)> pTestFunc;
        if(j) pTestFunc = [](const float& h, const float& y) {return (h > -23.0f && h < -18.0f && (int(y) % 80 < 40));};
         else pTestFunc = [](const float& h, const float& y) {return (h > -20.0f && h < -18.0f);};
    
        for(int i = 0; i < iStoneTries; ++i)
        {
            // 
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * float(OUTDOOR_WIDTH), ABS(float(i)/float(iStoneTries)) * float(OUTDOOR_HEIGHT)) * OUTDOOR_DETAIL;
            const float       fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

            // 
            if(pTestFunc(fHeight, vPosition.y))
            {
                // 
                coreObject3D* pObject = new coreObject3D();
                pObject->DefineModel  ("rock.md5mesh");
                pObject->DefineTexture(0, "environment_stone_diff.png");
                pObject->DefineTexture(1, "environment_stone_norm.png");
                pObject->DefineProgram("object_program");

                // 
                pObject->SetPosition   (coreVector3(vPosition, fHeight));
                pObject->SetDirection  (coreVector3::Rand());
                pObject->SetOrientation(coreVector3::Rand());
                pObject->SetSize       (coreVector3::Rand(0.75f,1.8f, 0.75f,1.8f, 0.75f,1.8f) * fStoneSize);
                pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.8f, 1.0f));

                // 
                pList->BindObject(pObject);
            }
        }
    }

    // 
    pList->ShrinkToFit();
    m_apGroundObjectList.push_back(pList);

    // 
    constexpr_var int iCloudNum = 96;
    pList = new coreBatchList(iCloudNum);
    pList->DefineProgram("environment_clouds_inst_program");

    for(int i = 0; i < iCloudNum; ++i)
    {
        // 
        const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.3f, 0.3f) * float(OUTDOOR_WIDTH), ABS(float(i)/float(iCloudNum)) * float(OUTDOOR_HEIGHT)) * OUTDOOR_DETAIL;
        const float       fHeight   = Core::Rand->Float(20.0f, 60.0f);

        // 
        coreObject3D* pObject = new coreObject3D();
        pObject->DefineModel  ("default_square.md5mesh");
        pObject->DefineTexture(0, "environment_clouds_mid.png");
        pObject->DefineProgram("environment_clouds_program");

        // 
        pObject->SetPosition   (coreVector3(vPosition, fHeight));
        pObject->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
        pObject->SetOrientation(coreVector3(coreVector2::Rand(), 0.0f));
        pObject->SetSize       (coreVector3(coreVector2(2.5f,2.5f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
        pObject->SetAlpha      (0.8f);
        pObject->SetTexOffset  (coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

        // 
        pList->BindObject(pObject);
    }

    // 
    m_apAirObjectList.push_back(pList);
}


// ****************************************************************
// destructor
cGrass::~cGrass()
{
    // delete outdoor-surface object
    SAFE_DELETE(m_pOutdoor)
}


// ****************************************************************
// render the grass background
void cGrass::RenderOwn()
{
    // 
    m_Water.UpdateReflection();

    // 
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
    {
        glDisable(GL_BLEND);
        {
            // 
            FOR_EACH(it, m_apGroundObjectList)
                (*it)->Render();

            // 
            m_pOutdoor->Render();

            // 
            m_Water.Render(&m_iFrameBuffer);
        }
        glEnable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        {
            // 
            FOR_EACH(it, m_apAirObjectList)
                (*it)->Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// move the grass background
void cGrass::MoveOwn()
{
    // move the outdoor-surface
    m_pOutdoor->SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_pOutdoor->Move();

    // move the water-surface
    m_Water.SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_Water.Move();

    // TODO #
    //m_apSound[0]->SetVolume(g_bEnviroSound ? (5.5f * g_pEnvironment->GetTransit()->GetCurrent(g_pEnvironment->GetID() != 3) * Core::Config->Get()->Sound.fSoundVolume*1.2f) : 0.0f);
}