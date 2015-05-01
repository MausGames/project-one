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
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture
    m_iResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_iResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
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

    // fill background frame buffer
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);   // color-clear improves performance
    {
        glDisable(GL_BLEND);
        {
            // send shadow matrix to shader-programs
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT);
            cShadow::EnableShadowRead(SHADOW_HANDLE_OBJECT_INST);

            // render all additional objects
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

        glDisable(GL_DEPTH_TEST);
        {
            // render all transparent ground objects
            FOR_EACH(it, m_apDecalObjectList)
                (*it)->Render();

            // render all air objects
            FOR_EACH(it, m_apAirObjectList)
                (*it)->Render();
        }
        glEnable(GL_DEPTH_TEST);

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
    nControlObjectsFunc(&m_apGroundObjectList, 80.0f);
    nControlObjectsFunc(&m_apDecalObjectList,  80.0f);
    nControlObjectsFunc(&m_apAirObjectList,    75.0f);

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
    const coreUintW iCurSize = pObjectList->List()->size();
    for(coreUintW i = 0u; i < iCurSize; ++i)
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
    for(coreUintW i = 5u; i-- && it != et; )
    {
        // check for quadratic distance
        if(((*(--it))->GetPosition().xy() - vNewPos).LengthSq() < fDistanceSq)
            return true;
    }
    return false;
}