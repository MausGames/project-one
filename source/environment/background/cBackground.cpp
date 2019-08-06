///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreMemoryPool cBackground::s_MemoryPool = coreMemoryPool(sizeof(coreObject3D), 256u);


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
    // 
    m_aaiBaseHeight.clear();
    m_aaiBaseNormal.clear();

    // remove all persistent objects
    const auto nRemoveObjectsFunc = [](std::vector<coreBatchList*>* OUTPUT papList)
    {
        // delete objects and lists
        FOR_EACH(it, *papList)
        {
            FOR_EACH(et, *(*it)->List()) POOLED_DELETE(s_MemoryPool, *et)
            SAFE_DELETE(*it)
        }

        // clear memory
        papList->clear();
    };
    nRemoveObjectsFunc(&m_apGroundObjectList);
    nRemoveObjectsFunc(&m_apDecalObjectList);
    nRemoveObjectsFunc(&m_apAirObjectList);

    // remove all temporary objects
    this->ClearAdds();

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

    if(m_pOutdoor)
    {
        // update shadow and light map
        m_pOutdoor->GetShadowMap()->Update();
        m_pOutdoor->UpdateLightMap();
    }

    // 
    this->__UpdateOwn();

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
            FOR_EACH(it, m_apGroundObjectList) (*it)->Render();
            FOR_EACH(it, m_apGroundAddList)    (*it)->Render();

            // render the outdoor-surface
            if(m_pOutdoor) m_pOutdoor->Render();

            glEnable (GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            {
                // render all transparent ground objects
                FOR_EACH(it, m_apDecalObjectList) (*it)->Render();
                FOR_EACH(it, m_apDecalAddList)    (*it)->Render();
            }
            glDisable(GL_BLEND);
            glEnable (GL_DEPTH_TEST);

            // render the water-surface
            if(m_pWater) m_pWater->Render(&m_FrameBuffer);
        }
        glEnable(GL_BLEND);

        // call individual render routine (before air)
        this->__RenderOwnBefore();

        glDisable(GL_DEPTH_TEST);
        {
            // render all air objects
            FOR_EACH(it, m_apAirObjectList) (*it)->Render();
            FOR_EACH(it, m_apAirAddList)    (*it)->Render();
        }
        glEnable(GL_DEPTH_TEST);

        // call individual render routine (after air)
        this->__RenderOwnAfter();
    }

    // resolve frame buffer to texture
    m_FrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_ResolvedTexture);
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

    if(m_pOutdoor)
    {
        // invalidate shadow and light map
        if(g_CurConfig.Graphics.iShadow) m_pOutdoor->GetShadowMap()->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
        m_pOutdoor->GetLightMap()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    }
}


// ****************************************************************
// move the background
void cBackground::Move()
{
    // move the outdoor-surface
    if(m_pOutdoor)
    {
        m_pOutdoor->SetFlyOffset(g_pEnvironment->GetFlyOffset());
        m_pOutdoor->SetTransform(g_pEnvironment->GetFlyOffset(), g_pEnvironment->GetSideOffset(), g_pEnvironment->GetDirection());
        m_pOutdoor->Move();
    }

    // move the water-surface
    if(m_pWater)
    {
        m_pWater->SetFlyOffset(g_pEnvironment->GetFlyOffset());
        m_pWater->Move();
    }

    // control and move all persistent objects
    const auto nControlObjectsFunc = [this](std::vector<coreBatchList*>* OUTPUT papList, const coreFloat fRange, const coreMatrix2& mRota)
    {
        // cache current camera position (to improve performance)
        const coreVector2 vCameraPos = g_pEnvironment->GetCameraPos().xy();

        FOR_EACH(it, *papList)
        {
            WARN_IF((*it)->List()->empty()) continue;

            coreBool bUpdate = false;

            // 
            const coreFloat                fDensity      = (*it)->List()->front()->GetCollisionModifier().x;
            const std::vector<coreUint16>* paiBaseHeight = m_aaiBaseHeight.count(*it) ? &m_aaiBaseHeight.at(*it) : NULL;
            const std::vector<coreUint32>* paiBaseNormal = m_aaiBaseNormal.count(*it) ? &m_aaiBaseNormal.at(*it) : NULL;

            FOR_EACH(et, *(*it)->List())
            {
                coreObject3D* pObject = (*et);

                // determine visibility and compare with current status
                const coreVector2 vScreenPos = (pObject->GetPosition().xy() - vCameraPos) * mRota;
                const coreBool    bIsVisible = ((ABS(vScreenPos.x) + ABS(vScreenPos.y)) < fRange);
                if(bIsVisible != pObject->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    // 
                    const coreFloat fFract = FRACT(FMOD(pObject->GetPosition().y + I_TO_F(OUTDOOR_VIEW / 2u) * OUTDOOR_DETAIL, I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL));
                    pObject->SetEnabled(bIsVisible ? ((fFract < fDensity) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE) : CORE_OBJECT_ENABLE_NOTHING);

                    // recalculate properties when becoming visible
                    if(pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL))
                    {
                        const coreUintW iIndex = et - (*it)->List()->begin();

                        // retrieve height and add offset
                        if(paiBaseHeight)
                        {
                            const coreFloat fOffset = coreMath::Float16To32((*paiBaseHeight)[iIndex % paiBaseHeight->size()]);
                            const coreFloat fHeight = m_pOutdoor->RetrieveBackHeight(pObject->GetPosition().xy());
                            pObject->SetPosition(coreVector3(pObject->GetPosition().xy(), fHeight + fOffset));
                        }

                        // unpack normal and interpolate
                        if(paiBaseNormal)
                        {
                            const coreVector3 vNormal = coreVector3::UnpackSnorm011((*paiBaseNormal)[iIndex % paiBaseNormal->size()]);
                            const coreVector2 vLerp   = m_pOutdoor->CalcLerpVector(pObject->GetPosition().y);
                            pObject->SetDirection(LERP(coreVector3(0.0f,0.0f,1.0f), vNormal, vLerp.x).Normalized());
                        }
                    }

                    // 
                    bUpdate = true;
                }
            }

            // move only when necessary (also to reduce instancing updates)
            if(bUpdate) (*it)->MoveNormal();
        }
    };
    const coreMatrix2 mRota = coreMatrix3::Rotation(g_pEnvironment->GetDirection().InvertedX().Rotated45()).m12();
    nControlObjectsFunc(&m_apGroundObjectList, BACKGROUND_OBJECT_RANGE,         mRota);
    nControlObjectsFunc(&m_apDecalObjectList,  BACKGROUND_OBJECT_RANGE - 9.0f,  mRota);
    nControlObjectsFunc(&m_apAirObjectList,    BACKGROUND_OBJECT_RANGE - 11.0f, mRota);

    // control and move all temporary objects
    const auto nControlAddFunc = [](coreLookupStr<coreBatchList*>* OUTPUT papList, const coreFloat fRange)
    {
        // cache current camera position (to improve performance)
        const coreVector2 vCameraPos = g_pEnvironment->GetCameraPos().xy();

        FOR_EACH(it, *papList)
        {
            FOR_EACH_DYN(et, *(*it)->List())
            {
                coreObject3D* pObject = (*et);

                // 
                const coreBool bIsVisible = coreMath::IsNear(pObject->GetPosition().y, vCameraPos.y, fRange);
                const coreBool bIsAlive   = coreMath::IsNear(pObject->GetPosition().y, vCameraPos.y, fRange * BACKGROUND_ADD_EXTENSION);

                // 
                pObject->SetEnabled(bIsVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

                // keep object while in current view
                if(bIsAlive) DYN_KEEP(it)
                else
                {
                    // remove object when not visible anymore
                    MANAGED_DELETE(pObject)
                    DYN_REMOVE(et, *(*it)->List())
                }
            }

            // 
            (*it)->MoveNormal();
        }
    };
    nControlAddFunc(&m_apGroundAddList, BACKGROUND_OBJECT_RANGE);
    nControlAddFunc(&m_apDecalAddList,  BACKGROUND_OBJECT_RANGE - 9.0f);
    nControlAddFunc(&m_apAirAddList,    BACKGROUND_OBJECT_RANGE - 11.0f);

    // call individual move routine
    this->__MoveOwn();
}


// ****************************************************************
// add temporary object
#define __ADD_OBJECT(x)                                                                            \
{                                                                                                  \
    ASSERT(pObject && (ABS(vRelativePos.y) <= BACKGROUND_OBJECT_RANGE * BACKGROUND_ADD_EXTENSION)) \
                                                                                                   \
    /* check for available optimized list */                                                       \
    if(!(x).count(sListKey))                                                                       \
    {                                                                                              \
        /* create new list */                                                                      \
        coreBatchList* pList = new coreBatchList(iCapacity);                                       \
        pList->DefineProgram(sProgramInstancedName);                                               \
                                                                                                   \
        /* save new list */                                                                        \
        (x).emplace(sListKey, pList);                                                              \
    }                                                                                              \
                                                                                                   \
    /* set position and add object to optimized list */                                            \
    pObject->SetPosition(vRelativePos + coreVector3(g_pEnvironment->GetCameraPos().xy(), 0.0f));   \
    (x).at(sListKey)->BindObject(pObject);                                                         \
}

void cBackground::AddGround(coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey) {__ADD_OBJECT(m_apGroundAddList)}
void cBackground::AddDecal (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey) {__ADD_OBJECT(m_apDecalAddList)}
void cBackground::AddAir   (coreObject3D* pObject, const coreVector3& vRelativePos, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey) {__ADD_OBJECT(m_apAirAddList)}

#undef __ADD_OBJECT


// ****************************************************************
// move temporary objects (for infinite background)
void cBackground::ShoveAdds(const coreFloat fOffset)
{
    const auto nUpdatePosFunc = [&](coreLookupStr<coreBatchList*>* OUTPUT papList)
    {
        // update objects and lists
        FOR_EACH(it, *papList)
        {
            FOR_EACH(et, *(*it)->List())
            {
                // add offset to position
                const coreVector3& vPos = (*et)->GetPosition();
                (*et)->SetPosition(coreVector3(vPos.x, vPos.y + fOffset, vPos.z));
            }
        }
    };
    nUpdatePosFunc(&m_apGroundAddList);
    nUpdatePosFunc(&m_apDecalAddList);
    nUpdatePosFunc(&m_apAirAddList);
}


// ****************************************************************
// remove all temporary objects
void cBackground::ClearAdds()
{
    const auto nRemoveAddsFunc = [](coreLookupStr<coreBatchList*>* OUTPUT papList)
    {
        // delete objects and lists
        FOR_EACH(it, *papList)
        {
            FOR_EACH(et, *(*it)->List()) MANAGED_DELETE(*et)
            SAFE_DELETE(*it)
        }

        // clear memory
        papList->clear();
    };
    nRemoveAddsFunc(&m_apGroundAddList);
    nRemoveAddsFunc(&m_apDecalAddList);
    nRemoveAddsFunc(&m_apAirAddList);
}


// ****************************************************************
// 
#define __SET_DENSITY(x)                                                                    \
{                                                                                           \
    ASSERT((iIndex < (x).size()) && (fDensity >= 0.0f) && (fDensity <= 1.0f))               \
                                                                                            \
    /*  */                                                                                  \
    coreObject3D* pBase = (x)[iIndex]->List()->front();                                     \
    pBase->SetCollisionModifier(coreVector3(fDensity, pBase->GetCollisionModifier().yz())); \
} 

void cBackground::SetGroundDensity(const coreUintW iIndex, const coreFloat fDensity) {__SET_DENSITY(m_apGroundObjectList)}
void cBackground::SetDecalDensity (const coreUintW iIndex, const coreFloat fDensity) {__SET_DENSITY(m_apDecalObjectList)}
void cBackground::SetAirDensity   (const coreUintW iIndex, const coreFloat fDensity) {__SET_DENSITY(m_apAirObjectList)}

#undef __SET_DENSITY


// ****************************************************************
// 
void cBackground::_StoreHeight(const coreBatchList* pObjectList, const coreFloat fHeight)
{
    ASSERT(!m_aaiBaseHeight.count(pObjectList))

    // 
    m_aaiBaseHeight[pObjectList].push_back(coreMath::Float32To16(fHeight));
}

void cBackground::_StoreHeightList(const coreBatchList* pObjectList)
{
    ASSERT(!m_aaiBaseHeight.count(pObjectList))
    ASSERT(pObjectList->List()->back()->GetPosition().y < (I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL))

    // 
    std::vector<coreUint16>& oNew = m_aaiBaseHeight[pObjectList];
    oNew.reserve(pObjectList->List()->size());

    // 
    FOR_EACH(it, *pObjectList->List()) oNew.push_back(coreMath::Float32To16((*it)->GetPosition().z));
}

// ****************************************************************
// 
void cBackground::_StoreNormalList(const coreBatchList* pObjectList)
{
    ASSERT(!m_aaiBaseNormal.count(pObjectList))
    ASSERT(pObjectList->List()->back()->GetPosition().y < (I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL))

    // 
    std::vector<coreUint32>& oNew = m_aaiBaseNormal[pObjectList];
    oNew.reserve(pObjectList->List()->size());

    // 
    FOR_EACH(it, *pObjectList->List()) oNew.push_back((*it)->GetDirection().PackSnorm011());

    // TODO: may needs change later, what is normal, direction or orientation or both (store quaternion? (already available)) 
}


// ****************************************************************
// create infinite looking object list
void cBackground::_FillInfinite(coreBatchList* OUTPUT pObjectList, const coreUintW iReserve)
{
    coreSet<coreObject3D*>* pContent = pObjectList->List();
    WARN_IF(pContent->size() < 2u) return;

    // 
    if((*pContent)[0]->GetPosition().y > (*pContent)[1]->GetPosition().y)
        std::reverse(pContent->begin(), pContent->end());

    // 
    FOR_EACH(it, *pContent) (*it)->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // loop through all objects
    for(coreUintW i = 0u, ie = pContent->size(); i < ie; ++i)
    {
        const coreObject3D* pOldObject = (*pContent)[i];

        // check for position at the start area
        if(pOldObject->GetPosition().y < I_TO_F(OUTDOOR_VIEW / 2u) * OUTDOOR_DETAIL)
        {
            // copy object and move it to the end area
            coreObject3D* pNewObject = POOLED_NEW(s_MemoryPool, coreObject3D, *pOldObject);
            pNewObject->SetPosition(pNewObject->GetPosition() + coreVector3(0.0f, I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL, 0.0f));

            // bind the new object
            pObjectList->BindObject(pNewObject);
        }
        else break;
    }

    // reduce memory consumption
    ASSERT(pObjectList->List()->size() <= iReserve)
    ASSERT(pObjectList->List()->size() >= iReserve * 2u / 5u)
    pObjectList->ShrinkToFit();
}


// ****************************************************************
// 
void cBackground::_SortBackToFront(coreBatchList* OUTPUT pObjectList)
{
    coreSet<coreObject3D*>* pContent = pObjectList->List();
    WARN_IF(pContent->size() < 2u) return;

    // 
    std::sort(pContent->begin(), pContent->end(), [](const coreObject3D* A, const coreObject3D* B)
    {
        return (A->GetPosition().z < B->GetPosition().z);
    });
}


// ****************************************************************
// check for intersection with other objects
FUNC_PURE coreBool cBackground::_CheckIntersection(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq)
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

FUNC_PURE coreBool cBackground::_CheckIntersectionQuick(const coreBatchList* pObjectList, const coreVector2& vNewPos, const coreFloat fDistanceSq)
{
    auto it = pObjectList->List()->end();
    auto et = pObjectList->List()->begin();

    // compare only with last few objects
    for(coreUintW i = 6u; i-- && (it != et); )
    {
        // check for quadratic distance
        if(((*(--it))->GetPosition().xy() - vNewPos).LengthSq() < fDistanceSq)
            return true;
    }
    return false;
}


// ****************************************************************
// 
UNITY_BUILD
#include "01_cGrassBackground.cpp"
#include "02_cSeaBackground.cpp"
#include "03_cDesertBackground.cpp"
#include "04_cSpaceBackground.cpp"
#include "05_cVolcanoBackground.cpp"
#include "06_cSnowBackground.cpp"
#include "07_cMossBackground.cpp"
#include "08_cDarkBackground.cpp"
#include "51_cStomachBackground.cpp"
#include "52_cCaveBackground.cpp"
#include "99_cCloudBackground.cpp"