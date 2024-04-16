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
: m_bActive (false)
{
}


// ****************************************************************
// destructor
cWindscreen::~cWindscreen()
{
    // remove all temporary objects
    this->ClearAdds(false);
}


// ****************************************************************
// render the windscreen
void cWindscreen::Render()
{
    if(m_bActive)
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render all temporary objects
            FOR_EACH(it, m_apAddList)
                (*it)->Render();

            // 
            m_Ink.Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// move the windscreen
void cWindscreen::Move()
{
    // 
    m_bActive = (!m_apAddList.empty() || m_Ink.IsActive());
    if(m_bActive)
    {
        // 
        const auto nControlAddFunc = [](coreList<coreObject3D*>* OUTPUT papObject)
        {
            FOR_EACH_DYN(it, *papObject)
            {
                // 
                const coreFloat fNewLifeTime = coreMath::BitsToFloat((*it)->GetStatus()) - TIME;
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

        // move all temporary objects
        FOR_EACH(it, m_apAddList)
            (*it)->MoveNormal();

        // 
        m_Ink.Move();
    }
}


// ****************************************************************
// add temporary object
void cWindscreen::AddObject(coreObject3D* pObject, const coreVector3 vRelativePos, const coreFloat fLifeTime, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey)
{
    ASSERT(pObject && (fLifeTime > 0.0f))

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
// remove all temporary objects
void cWindscreen::ClearAdds(const coreBool bAnimated)
{
    if(bAnimated)
    {
        // 
        FOR_EACH(it, m_apAddList)
        {
            FOR_EACH(et, *(*it)->List()) (*et)->SetStatus(coreMath::FloatToBits(MIN(coreMath::BitsToFloat((*et)->GetStatus()), 1.0f)));
        }
    }
    else
    {
        // delete objects and lists
        FOR_EACH(it, m_apAddList)
        {
            FOR_EACH(et, *(*it)->List()) MANAGED_DELETE(*et)
            SAFE_DELETE(*it)
        }

        // clear memory
        m_apAddList.clear();
    }
}