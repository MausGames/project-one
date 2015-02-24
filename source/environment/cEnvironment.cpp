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
    // remove all persistent objects
    auto nRemoveObjectsFunc = [](std::vector<coreBatchList*>* papList)
    {
        FOR_EACH(it, *papList)
        {
            // delete single objects within the list
            FOR_EACH(et, *(*it)->List()) SAFE_DELETE(*et)
            SAFE_DELETE(*it)
        }
    };
    nRemoveObjectsFunc(&m_apGroundObjectList);
    nRemoveObjectsFunc(&m_apDecalObjectList);
    nRemoveObjectsFunc(&m_apAirObjectList);

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
        m_pWater->UpdateDepth(m_pOutdoor, m_apGroundObjectList);
    }

    // update shadow map
    if(m_pOutdoor) m_pOutdoor->GetShadowMap()->Update();

    // set environment clear color
    glClearColor(ENVIRONMENT_CLEAR_COLOR);

    // fill background frame buffer
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);   // color-clear improves performance (with multisampling and depth pre-pass)
    {
        // reset clear color
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glDisable(GL_BLEND);
        {
            // render depth pass with foreground objects
            cShadow::RenderForegroundDepth();

            // send shadow matrix to shader-programs
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT);
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT_INST);

            // render additional objects
            FOR_EACH(it, m_apAddObject)
                (*it)->Render();

            // render all ground objects
            FOR_EACH(it, m_apGroundObjectList)
                (*it)->Render();

            // render the outdoor-surface
            if(m_pOutdoor) m_pOutdoor->Render();

            // TODO # put transparent additional objects and decals here (to be in water) 

            // render the water-surface
            if(m_pWater) m_pWater->Render(&m_iFrameBuffer);
        }
        glEnable(GL_BLEND);

        glDepthMask(false);
        {
            // render all transparent ground objects
            FOR_EACH(it, m_apDecalObjectList)
                (*it)->Render();

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
    if(g_CurConfig.Graphics.iShadow && m_pOutdoor) m_pOutdoor->GetShadowMap()->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_DEPTH);
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

    // control and move all objects
    auto nControlObjectsFunc = [](std::vector<coreBatchList*>* papList, const float& fRange)
    {
        FOR_EACH(it, *papList)
        {
            bool bUpdate = false;

            // enable only objects in the current view
            FOR_EACH(et, *(*it)->List())
            {
                coreObject3D* pObject = (*et);

                // determine visibility and compare with current status
                const bool bIsVisible = coreMath::InRange(pObject->GetPosition().y, g_pEnvironment->GetCameraPos().y, fRange);
                if(bIsVisible != pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL))
                {
                    pObject->SetEnabled(bIsVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                    bUpdate = true;
                }
            }

            // move only when necessary (also to reduce instancing updates)
            if(bUpdate) (*it)->MoveNormal();
        }
    };
    nControlObjectsFunc(&m_apGroundObjectList, 80.0f);
    nControlObjectsFunc(&m_apDecalObjectList,  80.0f);
    nControlObjectsFunc(&m_apAirObjectList,    80.0f);

    // TODO # handle additional objects 

    // call individual move routine
    this->__MoveOwn();
}


// ****************************************************************
// add additional object
void cBackground::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos)
{
    // TODO # object gets shadow-shader 
}


// ****************************************************************
// remove all additional objects
void cBackground::ClearObjects()
{
    // TODO # 
}


// ****************************************************************
// create infinite looking object list
void cBackground::_FillInfinite(coreBatchList* OUTPUT pObjectList)
{
    // save current size and loop through all objects
    const coreUint iCurSize = coreUint(pObjectList->List()->size());
    for(coreUint i = 0; i < iCurSize; ++i)
    {
        coreObject3D* pOldObject = (*pObjectList->List())[i];

        // check for position at the start area
        if(pOldObject->GetPosition().y < I_TO_F(OUTDOOR_VIEW) * OUTDOOR_DETAIL * 0.5f)
        {
            // copy object and move it to the end area
            coreObject3D* pNewObject = new coreObject3D(*pOldObject);
            pNewObject->SetPosition(pNewObject->GetPosition() + coreVector3(0.0f, I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL, 0.0f));

            // bind the new object
            pObjectList->BindObject(pNewObject);
        }
    }
}


// ****************************************************************
// check for intersection with other objects
bool cBackground::_CheckIntersection(const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq)
{
    // loop through all objects
    FOR_EACH(it, *pObjectList->List())
    {
        // check for quadratic distance
        if(((*it)->GetPosition().xy() - vNewPos).LengthSq() < fDistanceSq)
            return true;
    }
    return false;
}

bool cBackground::_CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const float& fDistanceSq)
{
    auto it = pObjectList->List()->end();
    auto et = pObjectList->List()->begin();

    // compare only with last few objects
    for(int i = 5; i-- && it != et; )
    {
        // check for quadratic distance
        if(((*(--it))->GetPosition().xy() - vNewPos).LengthSq() < fDistanceSq)
            return true;
    }
    return false;
}


// ****************************************************************
// constructor
cEnvironment::cEnvironment()noexcept
: m_pBackground    (NULL)
, m_pOldBackground (NULL)
, m_Transition     (coreTimer(1.3f, 0.9f, 1))
, m_fFlyOffset     (0.0f)
, m_fSideOffset    (0.0f)
, m_vCameraPos     (CAMERA_POSITION)
, m_vLightDir      (LIGHT_DIRECTION)
, m_bActive        (false)
{
    // create environment frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineProgram("full_transition_program");
    m_MixObject.DefineTexture(2, "menu_background_black.png");
    m_MixObject.SetSize      (coreVector2(1.0f,1.0f));
    m_MixObject.Move();

    // reset transformation properties
    m_avDirection[1] = m_avDirection[0] = coreVector2(0.0f,1.0f);
    m_avSide     [1] = m_avSide     [0] = coreVector2(0.0f,0.0f);
    m_afSpeed    [1] = m_afSpeed    [0] = 2.0f;

    // load first background
    m_pBackground = new cNoBackground();
    this->ChangeBackground(MAX(Core::Config->GetInt("Game", "Background", 0), s_cast<int>(cGrassBackground::ID)));
}


// ****************************************************************
// destructor
cEnvironment::~cEnvironment()
{
    // save last background
    Core::Config->SetInt("Game", "Background", m_pBackground->GetID());

    // explicitly undefine to detach textures
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

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            {
                // mix both backgrounds together
                m_iFrameBuffer.StartDraw();
                m_MixObject.Render();
            }
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);

            // invalidate single backgrounds
            m_pOldBackground->GetResolvedTexture()->GetColorTarget(0).pTexture->Invalidate(0);
            m_pBackground   ->GetResolvedTexture()->GetColorTarget(0).pTexture->Invalidate(0);
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
    m_avSide     [0] =  m_avSide     [0] + (m_avSide     [1] - m_avSide     [0]) * fFactor * 8.0f;
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
        // update transition and move old background (do not update while new background is still loading)
        if(!Core::Manager::Resource->IsLoading() && m_bActive && m_Transition.Update(1.0f))
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
    case cNoBackground     ::ID: m_pBackground = new cNoBackground     (); break;
    case cGrassBackground  ::ID: m_pBackground = new cGrassBackground  (); break;
    case cSeaBackground    ::ID: m_pBackground = new cSeaBackground    (); break;
    case cDesertBackground ::ID: m_pBackground = new cDesertBackground (); break;
    case cSpaceBackground  ::ID: m_pBackground = new cSpaceBackground  (); break;
    case cVolcanoBackground::ID: m_pBackground = new cVolcanoBackground(); break;
    case cSnowBackground   ::ID: m_pBackground = new cSnowBackground   (); break;
    case cMossBackground   ::ID: m_pBackground = new cMossBackground   (); break;
    }

    if(m_pOldBackground)
    {
        // start transition
        m_Transition.Play(CORE_TIMER_PLAY_RESET);
        m_Transition.SetValue(-0.1f);

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
    if(bInit)
    {
        const int iID = P_TO_I(m_pBackground);

        // re-create background with saved ID
        m_pBackground = NULL;
        this->ChangeBackground(iID);

        // re-create environment frame buffer
        m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }
    else
    {
        const int iID = m_pBackground->GetID();

        // unbind textures and stop possible transition
        m_MixObject.DefineTexture(0, NULL);
        m_MixObject.DefineTexture(1, NULL);
        m_Transition.Stop();

        // delete both backgrounds
        SAFE_DELETE(m_pOldBackground)
        SAFE_DELETE(m_pBackground)

        // delete environment frame buffer
        m_iFrameBuffer.Delete();

        // save background ID
        m_pBackground = r_cast<cBackground*>(I_TO_P(iID));
    }
}


// ****************************************************************
// constructor
cGrassBackground::cGrassBackground()noexcept
{
    coreBatchList* pList1;
    coreBatchList* pList2;

    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("grass", "dust", 2, 4.0f);

    // create water-surface object
    m_pWater = new cWater();

    // allocate stone list
    pList1 = new coreBatchList(GRASS_STONES_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");
    {
        for(int j = 0; j < 2; ++j)   // types
        {
            const coreUint iStoneTries = j ? GRASS_STONES_2_NUM : GRASS_STONES_1_NUM;
            const float    fStoneSize  = j ? 2.0f               : 2.2f;

            // select position and height test function
            std::function<float(const float&, const float&)> nTestFunc;
            if(j) nTestFunc = [](const float& h, const float& y) {return (h > -23.0f && h < -18.0f && (F_TO_SI(y+160.0f) % 80 < 40));};
             else nTestFunc = [](const float& h, const float& y) {return (h > -20.0f && h < -18.0f);};

            for(coreUint i = 0; i < iStoneTries; ++i)
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(iStoneTries)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const float       fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(nTestFunc(fHeight, vPosition.y))
                {
                    if(!j || !cBackground::_CheckIntersection(pList1, vPosition, 16.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  ("environment_stone_01.md3");
                        pObject->DefineTexture(0, "environment_stone_diff.png");
                        pObject->DefineTexture(1, "environment_stone_norm.png");
                        pObject->DefineProgram("object_ground_program");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight+0.2f));
                        pObject->SetSize       (coreVector3::Rand(0.85f,1.3f, 0.85f,1.3f, 0.85f,1.3f) * Core::Rand->Float(1.0f, 1.3f) * fStoneSize);
                        pObject->SetDirection  (coreVector3::Rand());
                        pObject->SetOrientation(coreVector3::Rand());
                        pObject->SetColor3     (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.85f, 1.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                    }
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apGroundObjectList.push_back(pList1);

        // bind stone list to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
    }

    // allocate reed lists
    pList1 = new coreBatchList(GRASS_REEDS_1_RESERVE);
    pList1->DefineProgram("object_ground_inst_program");

    pList2 = new coreBatchList(GRASS_REEDS_2_RESERVE);
    pList2->DefineProgram("object_ground_inst_program");
    {
        for(coreUint i = 0; i < GRASS_REEDS_NUM; ++i)
        {
            for(coreUint j = 0; j < 3; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_REEDS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const float       fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -23.0f && fHeight < -18.0f && (F_TO_SI(vPosition.y+160.0f) % 80 < 40))
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersectionQuick(pList2,                  vPosition, 25.0f) &&
                       !cBackground::_CheckIntersection     (m_apGroundObjectList[0], vPosition, 25.0f))
                    {
                        // determine object type
                        const bool bType = (Core::Rand->Int(3) || fHeight >= -20.0f) ? true : false;

                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (bType ? "environment_reed_01.md3" : "environment_reed_02.md3");
                        pObject->DefineTexture(0, "environment_reed.png");
                        pObject->DefineTexture(1, "environment_grass_norm.png");
                        pObject->DefineProgram("object_ground_program");

                        // set object properties
                        pObject->SetPosition   (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize       (coreVector3::Rand(1.3f,1.6f, 1.3f,1.6f, 1.3f,1.6f) * 2.0f);
                        pObject->SetDirection  (coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
                        pObject->SetColor3     (coreVector3(1.0f, 1.0f * Core::Rand->Float(0.55f, 0.7f), 0.5f));

                        // add object to the list
                        if(bType) pList1->BindObject(pObject);
                             else pList2->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process lists and add them to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apGroundObjectList.push_back(pList1);

        cBackground::_FillInfinite(pList2);
        pList2->ShrinkToFit();
        m_apGroundObjectList.push_back(pList2);

        // bind reed lists to shadow map
        m_pOutdoor->GetShadowMap()->BindList(pList1);
        m_pOutdoor->GetShadowMap()->BindList(pList2);
    }

    // allocate flower list
    pList1 = new coreBatchList(GRASS_FLOWERS_RESERVE);
    pList1->DefineProgram("effect_decal_spheric_inst_program");
    {
        for(coreUint i = GRASS_FLOWERS_NUM; i--; )
        {
            for(coreUint j = 0; j < 15; ++j)   // tries
            {
                // calculate position and height
                const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.45f, 0.45f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_FLOWERS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
                const float       fHeight   = m_pOutdoor->RetrieveHeight(vPosition);

                // test for valid values
                if(fHeight > -14.5f)
                {
                    if(!cBackground::_CheckIntersectionQuick(pList1, vPosition, 225.0f))
                    {
                        // load object resources
                        coreObject3D* pObject = new coreObject3D();
                        pObject->DefineModel  (Core::Manager::Object->GetLowModel());
                        pObject->DefineTexture(0, "environment_flowers.png");
                        pObject->DefineProgram("effect_decal_spheric_program");

                        // set object properties
                        pObject->SetPosition (coreVector3(vPosition, fHeight-0.8f));
                        pObject->SetSize     (coreVector3(coreVector2(1.65f,1.65f) * Core::Rand->Float(9.0f, 10.0f), 1.0f));
                        pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
                        pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

                        // add object to the list
                        pList1->BindObject(pObject);
                        break;
                    }
                }
            }
        }

        // post-process list and add it to the ground
        cBackground::_FillInfinite(pList1);
        pList1->ShrinkToFit();
        m_apDecalObjectList.push_back(pList1);
    }

    // allocate cloud list
    pList1 = new coreBatchList(GRASS_CLOUDS_RESERVE);
    pList1->DefineProgram("environment_clouds_inst_program");
    {
        for(coreUint i = 0; i < GRASS_CLOUDS_NUM; ++i)
        {
            // calculate position and height
            const coreVector2 vPosition = coreVector2(Core::Rand->Float(-0.3f, 0.3f) * I_TO_F(OUTDOOR_WIDTH), (I_TO_F(i)/I_TO_F(GRASS_CLOUDS_NUM)) * I_TO_F(OUTDOOR_HEIGHT) - I_TO_F(OUTDOOR_VIEW/2)) * OUTDOOR_DETAIL;
            const float       fHeight   = Core::Rand->Float(20.0f, 60.0f);

            // load object resources
            coreObject3D* pObject = new coreObject3D();
            pObject->DefineModel  (Core::Manager::Object->GetLowModel());
            pObject->DefineTexture(0, "environment_clouds_mid.png");
            pObject->DefineProgram("environment_clouds_program");

            // set object properties
            pObject->SetPosition (coreVector3(vPosition, fHeight));
            pObject->SetSize     (coreVector3(coreVector2(2.1f,2.1f) * Core::Rand->Float(15.0f, 21.0f), 1.0f));
            pObject->SetDirection(coreVector3(coreVector2::Rand(), 0.0f));
            pObject->SetAlpha    (0.7f);
            pObject->SetTexOffset(coreVector2::Rand(0.0f,10.0f, 0.0f,10.0f));

            // add object to the list
            pList1->BindObject(pObject);
        }

        // post-process list and add it to the air
        cBackground::_FillInfinite(pList1);
        m_apAirObjectList.push_back(pList1);
    }

    // load nature sound-effect
    m_pNatureSound = Core::Manager::Resource->Get<coreSound>("environment_nature.wav");
    m_pNatureSound.GetHandle()->OnLoadOnce([&]()
    {
        m_pNatureSound->PlayRelative(this, 0.0f, 1.0f, 0.0f, true);
    });
}


// ****************************************************************
// destructor
cGrassBackground::~cGrassBackground()
{
    // stop nature sound-effect
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->Stop();

    // unbind all lists from shadow map
    m_pOutdoor->GetShadowMap()->ClearLists();
}


// ****************************************************************
// move the grass background
void cGrassBackground::__MoveOwn()
{
    // adjust volume of the nature sound-effect
    // TODO # sound-volume per config value 
    if(m_pNatureSound->EnableRef(this))
        m_pNatureSound->SetVolume(6.0f * g_pEnvironment->GetTransition().GetValue((g_pEnvironment->GetBackground() == this) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED));
}


// ****************************************************************
// constructor
cSeaBackground::cSeaBackground()noexcept
{
    // create outdoor-surface object 
    m_pOutdoor = new cOutdoor("dust", "dust", 2, 4.0f);
}