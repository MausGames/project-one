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
// 
class INTERFACE cBindContainer
{
protected:
    std::vector<coreObject3D*>  m_apObject;   // bound objects
    std::vector<coreBatchList*> m_apList;     // bound lists with objects


protected:
    cBindContainer()noexcept;
    ~cBindContainer();


public:
    DISABLE_COPY(cBindContainer)

    // manage objects
    inline void BindObject  (coreObject3D* pObject) {m_apObject.push_back(pObject);}
    inline void UnbindObject(coreObject3D* pObject) {FOR_EACH(it, m_apObject) {if((*it) == pObject) {m_apObject.erase(it); return;}} ASSERT(false)}
    inline void ClearObjects()                      {m_apObject.clear();}

    // manage lists with objects
    inline void BindList  (coreBatchList* pList) {m_apList.push_back(pList);}
    inline void UnbindList(coreBatchList* pList) {FOR_EACH(it, m_apList) {if((*it) == pList) {m_apList.erase(it); return;}} ASSERT(false)}
    inline void ClearLists()                     {m_apList.clear();}


protected:
    // 
    void _CheckDuplicates()const;
};


// ****************************************************************
// 
class INTERFACE cGlobalBindContainer
{
protected:
    static std::vector<coreObject3D*>  s_apGlobalObject;   // bound global objects
    static std::vector<coreBatchList*> s_apGlobalList;     // bound global lists with objects


protected:
    cGlobalBindContainer()noexcept {}
    ~cGlobalBindContainer()        {}


public:
    DISABLE_COPY(cGlobalBindContainer)

    // manage global objects
    static inline void BindGlobalObject  (coreObject3D* pObject) {s_apGlobalObject.push_back(pObject);}
    static inline void UnbindGlobalObject(coreObject3D* pObject) {FOR_EACH(it, s_apGlobalObject) {if((*it) == pObject) {s_apGlobalObject.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalObjects()                      {s_apGlobalObject.clear();}

    // manage global lists with objects
    static inline void BindGlobalList  (coreBatchList* pList) {s_apGlobalList.push_back(pList);}
    static inline void UnbindGlobalList(coreBatchList* pList) {FOR_EACH(it, s_apGlobalList) {if((*it) == pList) {s_apGlobalList.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalLists()                     {s_apGlobalList.clear();}


protected:
    // 
    static void _CheckGlobalDuplicates();

    // 
    static void _GlobalExit();
};


#endif // _P1_GUARD_BINDCONTAINER_H_