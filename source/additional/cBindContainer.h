//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BINDCONTAINER_H_
#define _P1_GUARD_BINDCONTAINER_H_


// ****************************************************************
// bind container interface
class cBindContainer
{
private:
    coreSet<coreObject3D*>  m_apObject;   // bound objects
    coreSet<coreBatchList*> m_apList;     // bound lists with objects


public:
    cBindContainer()noexcept {}
    inline ~cBindContainer();

    DISABLE_COPY(cBindContainer)

    // manage objects
    inline void BindObject  (coreObject3D* pObject) {ASSERT(!m_apObject.count(pObject)) m_apObject.insert(pObject);}
    inline void UnbindObject(coreObject3D* pObject) {ASSERT( m_apObject.count(pObject)) m_apObject.erase (pObject);}
    inline void ClearObjects()                      {m_apObject.clear();}

    // manage lists with objects
    inline void BindList  (coreBatchList* pList) {ASSERT(!m_apList.count(pList)) m_apList.insert(pList);}
    inline void UnbindList(coreBatchList* pList) {ASSERT( m_apList.count(pList)) m_apList.erase (pList);}
    inline void ClearLists()                     {m_apList.clear();}

    // 
    inline const coreSet<coreObject3D*>&  GetObjectSet()const {return m_apObject;}
    inline const coreSet<coreBatchList*>& GetListSet  ()const {return m_apList;}
};


// ****************************************************************
// destructor
inline cBindContainer::~cBindContainer()
{
    ASSERT(m_apObject.empty() && m_apList.empty())

    // remove all remaining objects and lists
    this->ClearObjects();
    this->ClearLists();
}


#endif // _P1_GUARD_BINDCONTAINER_H_