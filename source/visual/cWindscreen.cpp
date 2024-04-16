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
cWindscreen::cWindscreen()noexcept
: m_fInkAnimation (0.0f)
, m_fInkDelay     (0.0f)
, m_vInkPosition  (coreVector4(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX))
, m_vInkAlpha     (coreVector2(0.0f,0.0f))
, m_bActive       (false)
{
    // 
    m_Ink.DefineTexture(0u, "menu_background_black.png");
    m_Ink.SetSize      (coreVector2(1.0f,1.0f));
    m_Ink.SetTexSize   (coreVector2(3.5f,3.5f));
}


// ****************************************************************
// destructor
cWindscreen::~cWindscreen()
{
    // remove all additional objects
    this->ClearObjects();
}


// ****************************************************************
// render the windscreen
void cWindscreen::Render()
{
    if(m_bActive)
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render all additional objects
            FOR_EACH(it, m_apAddObject) (*it)->Render();
            FOR_EACH(it, m_apAddList)   (*it)->Render();

            // render the ink
            this->__RenderInk();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// move the windscreen
void cWindscreen::Move()
{
    // 
    m_bActive = (!m_apAddObject.empty() || !m_apAddList.empty() || !m_vInkAlpha.IsNull());
    if(m_bActive)
    {
        // 
        const auto nControlAddFunc = [](std::vector<coreObject3D*>* OUTPUT papObject)
        {
            FOR_EACH_DYN(it, *papObject)
            {
                // 
                const coreFloat fNewLifeTime = coreMath::BitsToFloat((*it)->GetStatus()) - Core::System->GetTime();
                if(fNewLifeTime > 0.0f)
                {
                    // 
                    (*it)->SetAlpha (CLAMP(fNewLifeTime, 0.0f, 1.0f));
                    (*it)->SetStatus(coreMath::FloatToBits(fNewLifeTime));
                    DYN_KEEP(it)
                }
                else
                {
                    // 
                    MANAGED_DELETE(*it)
                    DYN_REMOVE(it, *papObject)
                }
            }
        };

        // 
        nControlAddFunc(&m_apAddObject);

        FOR_EACH_DYN(it, m_apAddList)
        {
            // 
            if(!(*it)->List()->empty())
            {
                // 
                nControlAddFunc((*it)->List());
                DYN_KEEP(it)
            }
            else
            {
                // 
                SAFE_DELETE(*it)
                DYN_REMOVE(it, m_apAddList)
            }
        }

        // move all additional objects
        FOR_EACH(it, m_apAddObject) (*it)->Move();
        FOR_EACH(it, m_apAddList)   (*it)->MoveNormal();

        // move the ink
        this->__MoveInk();
    }
}


// ****************************************************************
// add additional object
void cWindscreen::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreFloat fLifeTime)
{
    ASSERT(pObject)

    // set properties and add object
    pObject->SetPosition(vRelativePos);
    pObject->SetStatus  (coreMath::FloatToBits(fLifeTime));
    m_apAddObject.push_back(pObject);
}

void cWindscreen::AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreFloat fLifeTime, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey)
{
    ASSERT(pObject)

    // check for available optimized list
    if(!m_apAddList.count(sListKey))
    {
        // create new list
        coreBatchList* pList = new coreBatchList(iCapacity);
        pList->DefineProgram(sProgramInstancedName);

        // save new list
        m_apAddList.emplace(sListKey, pList);
    }

    // set properties and add object to optimized list
    pObject->SetPosition(vRelativePos);
    pObject->SetStatus  (coreMath::FloatToBits(fLifeTime));
    m_apAddList.at(sListKey)->BindObject(pObject);
}


// ****************************************************************
// remove all additional objects
void cWindscreen::ClearObjects()
{
    // delete objects and lists
    FOR_EACH(it, m_apAddObject) MANAGED_DELETE(*it)
    FOR_EACH(it, m_apAddList)
    {
        FOR_EACH(et, *(*it)->List()) MANAGED_DELETE(*et)
        SAFE_DELETE(*it)
    }

    // clear memory
    m_apAddObject.clear();
    m_apAddList  .clear();
}


// ****************************************************************
// 
void cWindscreen::EnableInk(const coreBool bLine)
{
    // 
    m_Ink.DefineProgram(bLine ? "effect_ink_line_program" : "effect_ink_point_program");
    m_Ink.SetStatus    (bLine ? 1 : 0);

    // 
    WARN_IF(!m_vInkAlpha.IsNull()) m_vInkAlpha = coreVector2(0.0f,0.0f);

    // 
    m_fInkDelay = 0.0f;
}


// ****************************************************************
// 
void cWindscreen::DisableInk(const coreFloat fDelay)
{
    // 
    if(!fDelay) m_vInkAlpha = coreVector2(0.0f,0.0f);

    // 
    m_fInkDelay = fDelay;
}


// ****************************************************************
// 
void cWindscreen::SetInkPosition(const coreUintW iIndex, const coreVector2& vPosition)
{
    // 
    const coreVector2 vProjectedPos = g_pForeground->Project2D(coreVector3(vPosition, 0.0f));

    // 
    m_vInkPosition.arr(iIndex * 2u)      = vProjectedPos.x;
    m_vInkPosition.arr(iIndex * 2u + 1u) = vProjectedPos.y;
}


// ****************************************************************
// 
void cWindscreen::SetInkAlpha(const coreUintW iIndex, coreFloat fAlpha)
{
    // 
    m_vInkAlpha.arr(iIndex) = fAlpha;
}


// ****************************************************************
// render the ink
void cWindscreen::__RenderInk()
{
    if(m_vInkAlpha.IsNull()) return;

    // enable the shader-program
    if(!m_Ink.GetProgram().IsUsable()) return;
    if(!m_Ink.GetProgram()->Enable())  return;

    // update all ink uniforms
    coreProgram* pLocal = m_Ink.GetProgram().GetResource();
    pLocal->SendUniform("u_v4Position", m_vInkPosition);
    pLocal->SendUniform("u_v2Alpha",    m_vInkAlpha * 0.32f + 0.68f);

    // get current animation value
    const coreFloat fAnimation = m_fInkAnimation * 0.3f;

    if(m_Ink.GetStatus())
    {
        coreVector2 avOffset[WINDSCREEN_INK_SAMPLES_LINE];

        // update line ink samples
        for(coreUintW i = 0u; i < WINDSCREEN_INK_SAMPLES_LINE; ++i)
        {
            const coreFloat fPartOff  = I_TO_F(i) / I_TO_F(WINDSCREEN_INK_SAMPLES_LINE);
            const coreFloat fPartTime = FMOD(fAnimation + fPartOff, 1.0f);

            avOffset[i] = coreVector2(0.675f * (0.5f - fPartTime), 0.05f * SIN(fPartTime * (4.0f*PI) + fPartOff * (2.0f*PI))) * 3.0f;
        }

        // merge two samples into one uniform
        for(coreUintW i = 0u; i < WINDSCREEN_INK_SAMPLES_LINE/2u; ++i)
        {
            pLocal->SendUniform(PRINT("u_av4Offset[%zu]", i), r_cast<coreVector4*>(avOffset)[i]);
        }
    }
    else
    {
        // update point ink samples
        for(coreUintW i = 0u; i < WINDSCREEN_INK_SAMPLES_POINT; ++i)
        {
            const coreFloat fPartOff  = I_TO_F(i) / I_TO_F(WINDSCREEN_INK_SAMPLES_POINT);
            const coreFloat fPartTime = FMOD(fAnimation + fPartOff, 1.0f);

            pLocal->SendUniform(PRINT("u_av2Offset[%zu]", i), coreVector2::Direction(fPartTime * (4.0f*PI)) * 0.14f);
        }
    }

    // render the ink overlay
    m_Ink.Render();
}


// ****************************************************************
// move the ink
void cWindscreen::__MoveInk()
{
    if(m_vInkAlpha.IsNull()) return;

    // 
    m_fInkAnimation.UpdateMod(0.1f, 10.0f);

    // 
    if(m_fInkDelay) m_vInkAlpha = (m_vInkAlpha - m_fInkDelay * Core::System->GetTime()).Processed(MAX, 0.0f);

    // move the ink overlay
    m_Ink.SetTexOffset(coreVector2(m_fInkAnimation * -0.4, 0.0));
    m_Ink.Move();
}