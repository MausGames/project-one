//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTLINE_H_
#define _P1_GUARD_OUTLINE_H_


// ****************************************************************
// outline-effect class
class cOutline final
{
private:
    std::vector<coreObject3D*>  m_apObject;   // outlined objects
    std::vector<coreBatchList*> m_apList;     // lists with outlined objects

    coreProgramPtr s_pProgramSingle;          // shader-program for single outlined objects
    coreProgramPtr s_pProgramInstanced;       // shader-program for instanced outlined objects


public:
    cOutline()noexcept;
    ~cOutline();

    // apply outline-effect
    void Apply();

    // manage outlined objects
    inline void BindObject  (coreObject3D* pObject) {m_apObject.push_back(pObject);}
    inline void UnbindObject(coreObject3D* pObject) {FOR_EACH(it, m_apObject) {if((*it) == pObject) {m_apObject.erase(it); return;}} ASSERT(false)}
    inline void ClearObjects()                      {m_apObject.clear();}

    // manage lists with outlined objects
    inline void BindList  (coreBatchList* pList) {m_apList.push_back(pList);}
    inline void UnbindList(coreBatchList* pList) {FOR_EACH(it, m_apList) {if((*it) == pList) {m_apList.erase(it); return;}} ASSERT(false)}
    inline void ClearLists()                     {m_apList.clear();}


private:
    DISABLE_COPY(cOutline)
};


#endif // _P1_GUARD_OUTLINE_H_