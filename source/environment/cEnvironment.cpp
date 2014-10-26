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
, m_pWater   (NULL)
{
    // create background frame buffer
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture
    m_iResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
}


// ****************************************************************
// destructor
cBackground::~cBackground()
{
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

    // delete outdoor-surface object
    SAFE_DELETE(m_pOutdoor)

    // delete water-surface object
    SAFE_DELETE(m_pWater)
}


// ****************************************************************
// render the background
void cBackground::Render()
{
    if(m_pWater)
    {
        // update water reflection and depth map
        m_pWater->UpdateReflection();
        m_pWater->UpdateDepth(m_pOutdoor);
    }

    // update shadow map
    if(m_pOutdoor) m_pOutdoor->GetShadowMap()->Update();

    // fill background frame buffer
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);   // color-clear improves performance (with multisampling and depth pre-pass)
    {
        glDisable(GL_BLEND);
        {
            // render depth pre-pass with foreground objects
            cShadow::RenderForegroundDepth();

            // send shadow matrix to single shader-program
                 if(!m_apAddObject.empty())        cShadow::ApplyShadowMatrix(m_apAddObject.front()->GetProgram());
            else if(!m_apGroundObjectList.empty()) cShadow::ApplyShadowMatrix(m_apGroundObjectList.front()->List()->front()->GetProgram());

            // render additional objects
            FOR_EACH(it, m_apAddObject)
                (*it)->Render();

            // send shadow matrix to instanced shader-program
            if(!m_apGroundObjectList.empty()) cShadow::ApplyShadowMatrix(m_apGroundObjectList.front()->GetProgram());

            // render all ground objects
            FOR_EACH(it, m_apGroundObjectList)
                (*it)->Render();

            // render the outdoor-surface
            if(m_pOutdoor) m_pOutdoor->Render();

            // TODO # transparent additional objects (not alpha-depended, think about explosion-splashs)

            // render the water-surface
            if(m_pWater) m_pWater->Render(&m_iFrameBuffer);
        }
        glEnable(GL_BLEND);

        glDepthMask(false);
        {
            // render all air objects
            FOR_EACH(it, m_apAirObjectList)
                (*it)->Render();
        }
        glDepthMask(true);

        // call individual render routine
        this->__RenderOwn();
    }

    // resolve frame buffer to texture
    m_iFrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_iResolvedTexture);
    m_iFrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

    // invalidate shadow map
    if(g_CurConfig.iShadow && m_pOutdoor) m_pOutdoor->GetShadowMap()->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the background
void cBackground::Move()
{
    // move the outdoor-surface
    if(m_pOutdoor)
    {
        m_pOutdoor->SetFlyOffset(g_pEnvironment->GetFlyOffset());
        m_pOutdoor->Move();
    }

    // move the water-surface
    if(m_pWater)
    {
        m_pWater->SetFlyOffset(g_pEnvironment->GetFlyOffset());
        m_pWater->Move();
    }

    // control and move ground objects
    FOR_EACH(it, m_apGroundObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            (*et)->SetEnabled(coreMath::InRange((*et)->GetPosition().y, g_pEnvironment->GetCameraPos().y, 80.0f) ? 
                              CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        (*it)->MoveNormal();
    }

    // control and move air objects
    FOR_EACH(it, m_apAirObjectList)
    {
        FOR_EACH(et, *(*it)->List())
            (*et)->SetEnabled(coreMath::InRange((*et)->GetPosition().y, g_pEnvironment->GetCameraPos().y, 100.0f) ?
                              CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        (*it)->MoveNormal();
    }

    // TODO # handle additional objects

    // call individual move routine
    this->__MoveOwn();
}


// ****************************************************************
// add additional object
void cBackground::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos)
{
    // TODO #

    // object gets shadow-shader (bool for trivial objects ? -> no, progam-swapping otherwise)
}


// ****************************************************************
// remove all additional objects
void cBackground::ClearObjects()
{
    // TODO #
}


// ****************************************************************
// create infinite looking object list
void cBackground::_FillInfinite(coreBatchList* pObjectList)
{
    // save current size and loop through all objects
    const coreUint iCurSize = pObjectList->List()->size();
    for(coreUint i = 0; i < iCurSize; ++i)
    {
        coreObject3D* pOldObject = (*pObjectList->List())[i];

        // check for position at the start area
        if(pOldObject->GetPosition().y < I_TO_F(OUTDOOR_VIEW) * OUTDOOR_DETAIL * 0.5f)
        {
            // copy object and move it to the end
            coreObject3D* pNewObject = new coreObject3D(*pOldObject);
            pNewObject->SetPosition(pNewObject->GetPosition() + coreVector3(0.0f, I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL, 0.0f));

            // bind the new object
            pObjectList->BindObject(pNewObject);
        }
    }
}


// ****************************************************************
// constructor
cEnvironment::cEnvironment()noexcept
: m_pBackground    (NULL)
, m_pOldBackground (NULL)
, m_Transition     (coreTimer(1.0f, 1.0f, 1))
, m_fFlyOffset     (0.0f)
, m_fSideOffset    (0.0f)
, m_vCameraPos     (CAMERA_POSITION)
, m_vLightDir      (LIGHT_DIRECTION)
{
    // create environment frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineProgram("full_transition_program");
    m_MixObject.SetSize     (coreVector2(1.0f, 1.0f));
    m_MixObject.SetTexSize  (coreVector2(1.0f,-1.0f));
    m_MixObject.SetTexOffset(coreVector2(0.0f, 1.0f));
    m_MixObject.Move();

    // reset transformation properties
    m_avDirection[1] = m_avDirection[0] = coreVector2(0.0f,1.0f);
    m_avSide     [1] = m_avSide     [0] = coreVector2(0.0f,0.0f);
    m_afSpeed    [1] = m_afSpeed    [0] = 0.0f;

    // load first background
    m_pBackground = new cEmptyBackground();
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
    // set environment camera and light
    Core::Graphics->SetCamera(m_vCameraPos, CAMERA_DIRECTION, coreVector3(m_avDirection[0], 0.0f));
    Core::Graphics->SetLight (0, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(m_vLightDir, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

    // render current background
    m_pBackground->Render(); 

    if(m_Transition.GetStatus())
    {
        if(m_MixObject.GetProgram().IsUsable())
        {
            // render old background
            m_pOldBackground->Render();

            // set transition time
            m_MixObject.GetProgram()->Enable();
            m_MixObject.GetProgram()->SendUniform("u_v1Transition", m_Transition.GetValue(CORE_TIMER_GET_NORMAL));

            // mix both backgrounds together
            m_iFrameBuffer.StartDraw();
            m_MixObject.Render();

            // invalidate single backgrounds
            m_pOldBackground->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
            m_pBackground   ->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
        }
    }
}


// ****************************************************************
// move the environment
void cEnvironment::Move()
{
    // update all transformation properties
    const float fFactor = Core::System->GetTime() * 2.0f;
    m_avDirection[0] = (m_avDirection[0] + (m_avDirection[1] - m_avDirection[0]) * fFactor).Normalize();
    m_avSide     [0] =  m_avSide     [0] + (m_avSide     [1] - m_avSide     [0]) * fFactor;
    m_afSpeed    [0] =  m_afSpeed    [0] + (m_afSpeed    [1] - m_afSpeed    [0]) * fFactor;

    // calculate global fly offset
    m_fFlyOffset += Core::System->GetTime() * m_afSpeed[0];
    while(m_fFlyOffset <  0.0f)                   m_fFlyOffset += I_TO_F(OUTDOOR_HEIGHT);
    while(m_fFlyOffset >= I_TO_F(OUTDOOR_HEIGHT)) m_fFlyOffset -= I_TO_F(OUTDOOR_HEIGHT);

    // calculate global side offset (only perpendicular to flight direction, never on diagonal camera (smooth with max-min))
    const coreVector2 vAbsDir = coreVector2(ABS(m_avDirection[0].x), ABS(m_avDirection[0].y));
    m_fSideOffset             = coreVector2::Dot(m_avDirection[0].yx(), m_avSide[0]) * (vAbsDir.Max() - vAbsDir.Min());

    // calculate camera and light values
    m_vCameraPos = coreVector3(m_fSideOffset, m_fFlyOffset * OUTDOOR_DETAIL, CAMERA_POSITION.z);
    m_vLightDir  = LIGHT_DIRECTION * coreMatrix4::RotationZ(m_avDirection[0] * coreVector2(-1.0f,1.0f));

    // move current background
    m_pBackground->Move();

    if(m_Transition.GetStatus())
    {
        // update transition and move old background
        if(m_Transition.Update(1.0f))
        {
            // delete old background
            m_MixObject.DefineTexture(0, NULL);
            SAFE_DELETE(m_pOldBackground)
        }
        else m_pOldBackground->Move();
    }
}


// ****************************************************************
// change current background
void cEnvironment::ChangeBackground(const coreByte& iID)
{
    if(m_pBackground) if(m_pBackground->GetID() == iID) return;

    // delete possible old background
    m_MixObject.DefineTexture(0, NULL);
    SAFE_DELETE(m_pOldBackground)

    // make current to old
    m_pOldBackground = m_pBackground;

    // create new background
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

    // check for available water-surface
    cWater* pWater = m_pBackground->GetWater();
    if(pWater) return MAX(fHeight, WATER_HEIGHT);

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

    // create water-surface object
    m_pWater = new cWater();

    // allocate stone list
    coreBatchList* pList = new coreBatchList(GRASS_STONES_RESERVE);
    pList->DefineProgram("object_inst_shadow_program");

    for(int j = 0; j < 2; ++j)
    {
        const int   iStoneTries = j ? GRASS_STONES_2_NUM  : GRASS_STONES_1_NUM;
        const float fStoneSize  = j ? GRASS_STONES_2_SIZE : GRASS_STONES_1_SIZE;

        // select position and height test function
        std::function<float(const float&, const float&)> pTestFunc;
        if(j) pTestFunc = [](const float& h, const float& y) {return (h > -23.0f && h < -18.0f && (F_TO_SI(y+160.0f) % 80 < 40));};
         else pTestFunc = [](const float& h, const float& y) {return (h > -20.0f && h < -18.0f);};
    
        for(int i = 0; i < iStoneTries; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(iStoneTries)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const float       fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

            // test for valid values
            if(pTestFunc(fHeight, vPosition.y))
            {
                // load object resources
                coreObject3D* pObject = new coreObject3D();
                pObject->DefineModel  ("rock.md5mesh");
                pObject->DefineTexture(0, "environment_stone_diff.png");
                pObject->DefineTexture(1, "environment_stone_norm.png");
                pObject->DefineProgram("object_shadow_program");

                // set object properties
                pObject->SetPosition   (coreVector3(vPosition, fHeight));
                pObject->SetDirection  (coreVector3::Rand());
                pObject->SetOrientation(coreVector3::Rand());
                pObject->SetSize       (coreVector3::Rand(0.75f,1.8f, 0.75f,1.8f, 0.75f,1.8f) * fStoneSize);
                pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.8f, 1.0f));

                // add object to the list
                pList->BindObject(pObject);
            }
        }
    }

    // post-process list and add it to the ground
    cBackground::_FillInfinite(pList);
    pList->ShrinkToFit();
    m_apGroundObjectList.push_back(pList);
    
    // bind stone list to shadow map
    m_pOutdoor->GetShadowMap()->BindList(pList);

    // allocate cloud list
    pList = new coreBatchList(GRASS_CLOUDS_RESERVE);
    pList->DefineProgram("environment_clouds_inst_program");

    for(int i = 0; i < GRASS_CLOUDS_NUM; ++i)
    {
        // calculate position and height
        const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.3f, 0.3f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_CLOUDS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
        const float       fHeight   = Core::Rand->Float(20.0f, 60.0f);

        // load object resources
        coreObject3D* pObject = new coreObject3D();
        pObject->DefineModel  ("default_square.md5mesh");
        pObject->DefineTexture(0, "environment_clouds_mid.png");
        pObject->DefineProgram("environment_clouds_program");

        // set object properties
        pObject->SetPosition   (coreVector3(vPosition, fHeight));
        pObject->SetDirection  (coreVector3(0.0f,0.0f,-1.0f));
        pObject->SetOrientation(coreVector3(coreVector2::Rand(), 0.0f));
        pObject->SetSize       (coreVector3(coreVector2(2.5f,2.5f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
        pObject->SetAlpha      (0.8f);
        pObject->SetTexOffset  (coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

        // add object to the list
        pList->BindObject(pObject);
    }

    // post-process list and add it to the air
    cBackground::_FillInfinite(pList);
    m_apAirObjectList.push_back(pList);

    // load nature sound-effect
    m_pNatureSound = Core::Manager::Resource->Get<coreSound>("environment_nature.wav");
    m_pNatureSound.GetHandle()->OnLoadOnce([&]()
    {
        m_pNatureSound->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cGrass::~cGrass()
{
    // stop nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->Stop();

    // unbind stone list from shadow map
    m_pOutdoor->GetShadowMap()->UnbindList(m_apGroundObjectList[0]);
}


// ****************************************************************
// move the grass background
void cGrass::__MoveOwn()
{
    // adjust volume of the nature sound-effect
    // TODO # sound-volume per config value
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->SetVolume(6.0f * g_pEnvironment->GetTransition().GetValue((g_pEnvironment->GetBackground() == this) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED));
}