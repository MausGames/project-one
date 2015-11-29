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
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8);
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
    m_FrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture
    m_ResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8);
    m_ResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
}


// ****************************************************************
// destructor
cBackground::~cBackground()
{
    // remove all persistent objects
    auto nRemoveObjectsFunc = [](std::vector<coreBatchList*>* OUTPUT papList)
    {
        FOR_EACH(it, *papList)
        {
            // delete single objects within the list
            FOR_EACH(et, *(*it)->List()) SAFE_DELETE(*et)
            SAFE_DELETE(*it)
        }

        // clear memory
        papList->clear();
    };
    nRemoveObjectsFunc(&m_apGroundObjectList);
    nRemoveObjectsFunc(&m_apDecalObjectList);
    nRemoveObjectsFunc(&m_apAirObjectList);

    // remove all additional objects
    this->ClearObjects();

    // delete outdoor-surface object
    if(m_pOutdoor) m_pOutdoor->GetShadowMap()->ClearLists();
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

    // fill background frame buffer
    m_FrameBuffer.StartDraw();
    m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);   // color-clear improves performance
    {
        glDisable(GL_BLEND);
        {
            // send shadow matrix to shader-programs
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT);
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT_INST);

            // render all ground objects
            FOR_EACH(it, m_apGroundObjectList)
                (*it)->Render();

            // render the outdoor-surface
            if(m_pOutdoor) m_pOutdoor->Render();

            glEnable (GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            {
                // render all transparent ground objects
                FOR_EACH(it, m_apDecalObjectList)
                    (*it)->Render();

                // render all additional objects (instancing first)
                FOR_EACH(it, m_apAddList)   (*it)->Render();
                FOR_EACH(it, m_apAddObject) (*it)->Render();
            }
            glDisable(GL_BLEND);
            glEnable (GL_DEPTH_TEST);

            // render the water-surface
            if(m_pWater) m_pWater->Render(&m_FrameBuffer);
        }
        glEnable(GL_BLEND);

        // call individual render routine
        this->__RenderOwn();

        glDisable(GL_DEPTH_TEST);
        {
            // render all air objects
            FOR_EACH(it, m_apAirObjectList)
                (*it)->Render();
        }
        glEnable(GL_DEPTH_TEST);
    }

    // resolve frame buffer to texture
    m_FrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_ResolvedTexture);
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

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

    // control and move all persistent objects
    auto nControlObjectsFunc = [](std::vector<coreBatchList*>* OUTPUT papList, const coreFloat& fRange)
    {
        FOR_EACH(it, *papList)
        {
            coreBool bUpdate = false;

            // enable only objects in the current view
            FOR_EACH(et, *(*it)->List())
            {
                coreObject3D* pObject = (*et);

                // determine visibility and compare with current status
                const coreBool bIsVisible = coreMath::InRange(pObject->GetPosition().y, g_pEnvironment->GetCameraPos().y, fRange);
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
    nControlObjectsFunc(&m_apGroundObjectList, BACKGROUND_OBJECT_RANGE);
    nControlObjectsFunc(&m_apDecalObjectList,  BACKGROUND_OBJECT_RANGE);
    nControlObjectsFunc(&m_apAirObjectList,    BACKGROUND_OBJECT_RANGE - 5.0f);

    // control all additional objects
    auto nControlAddFunc = [](std::vector<coreObject3D*>* OUTPUT papObject, const coreFloat& fRange)
    {
        FOR_EACH_DYN(it, *papObject)
        {
            // keep object while in current view
            if(coreMath::InRange((*it)->GetPosition().y, g_pEnvironment->GetCameraPos().y, fRange)) DYN_KEEP(it)
            else
            {
                // remove object when not visible anymore
                SAFE_DELETE(*it)
                DYN_REMOVE(it, *papObject)
            }
        }
    };
    nControlAddFunc(&m_apAddObject, BACKGROUND_OBJECT_RANGE);
    FOR_EACH(it, m_apAddList) nControlAddFunc((*it)->List(), BACKGROUND_OBJECT_RANGE);

    // move all additional objects
    FOR_EACH(it, m_apAddObject) (*it)->Move();
    FOR_EACH(it, m_apAddList)   (*it)->MoveNormal();

    // call individual move routine
    this->__MoveOwn();
}


// ****************************************************************
// add additional object
void cBackground::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos)
{
    ASSERT(pObject)

    // set position and add object
    pObject->SetPosition(vRelativePos + coreVector3(g_pEnvironment->GetCameraPos().xy(), 0.0f));
    m_apAddObject.push_back(pObject);
}

void cBackground::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint8& iListIndex)
{
    ASSERT(pObject)

    // check for available pre-defined list (use fallback on error)
    WARN_IF(!m_apAddList.count(iListIndex)) this->AddObject(pObject, vRelativePos);
    else
    {
        // set position and add object to optimized list
        pObject->SetPosition(vRelativePos + coreVector3(g_pEnvironment->GetCameraPos().xy(), 0.0f));
        m_apAddList.at(iListIndex)->BindObject(pObject);
    }
}


// ****************************************************************
// pre-define an optimized list
void cBackground::AddList(const coreUint8& iListIndex, const coreUint32& iCapacity, const coreChar* pcProgramInstancedName)
{
    // check for available pre-defined list
    coreBatchList* pList;
    if(!m_apAddList.count(iListIndex))
    {
        // create and save new list
        pList = new coreBatchList(iCapacity);
        m_apAddList[iListIndex] = pList;
    }
    else
    {
        // reallocate existing list
        pList = m_apAddList[iListIndex];
        pList->Reallocate(iCapacity);
    }

    // load shader-program
    pList->DefineProgram(pcProgramInstancedName);
}


// ****************************************************************
// move additional objects (for infinite background)
void cBackground::ShoveObjects(const coreFloat& fOffset)
{
    // update objects and lists
    auto nUpdatePosFunc = [&fOffset](std::vector<coreObject3D*>* OUTPUT papObject)
    {
        FOR_EACH(it, *papObject)
        {
            // add offset to position
            const coreVector3& vPos = (*it)->GetPosition();
            (*it)->SetPosition(coreVector3(vPos.x, vPos.y + fOffset, vPos.z));
        }
    };
    nUpdatePosFunc(&m_apAddObject);
    FOR_EACH(it, m_apAddList) nUpdatePosFunc((*it)->List());
}


// ****************************************************************
// remove all additional objects
void cBackground::ClearObjects()
{
    // delete objects and lists
    FOR_EACH(it, m_apAddObject) SAFE_DELETE(*it)
    FOR_EACH(it, m_apAddList)
    {
        FOR_EACH(et, *(*it)->List()) SAFE_DELETE(*et)
        SAFE_DELETE(*it)
    }

    // clear memory
    m_apAddObject.clear();
    m_apAddList  .clear();
}


// ****************************************************************
// create infinite looking object list
void cBackground::_FillInfinite(coreBatchList* OUTPUT pObjectList)
{
    coreSet<coreObject3D*>* pContent = pObjectList->List();
    WARN_IF(pContent->size() < 2u) return;

    // 
    if((*pContent)[0]->GetPosition().y > (*pContent)[1]->GetPosition().y)
        std::reverse(pContent->begin(), pContent->end());

    // loop through all objects
    for(coreUintW i = 0u, ie = pContent->size(); i < ie; ++i)
    {
        coreObject3D* pOldObject = (*pContent)[i];

        // check for position at the start area
        if(pOldObject->GetPosition().y < I_TO_F(OUTDOOR_VIEW/2) * OUTDOOR_DETAIL)
        {
            // copy object and move it to the end area
            coreObject3D* pNewObject = new coreObject3D(*pOldObject);
            pNewObject->SetPosition(pNewObject->GetPosition() + coreVector3(0.0f, I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL, 0.0f));

            // bind the new object
            pObjectList->BindObject(pNewObject);
        }
        else break;
    }

    // reduce memory consumption
    pObjectList->ShrinkToFit();
}


// ****************************************************************
// check for intersection with other objects
coreBool cBackground::_CheckIntersection(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat& fDistanceSq)
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

coreBool cBackground::_CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat& fDistanceSq)
{
    auto it = pObjectList->List()->end();
    auto et = pObjectList->List()->begin();

    // compare only with last few objects
    for(coreUintW i = 6u; i-- && it != et; )
    {
        // check for quadratic distance
        if(((*(--it))->GetPosition().xy() - vNewPos).LengthSq() < fDistanceSq)
            return true;
    }
    return false;
}