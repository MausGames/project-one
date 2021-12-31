///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CRASH_H_
#define _P1_GUARD_CRASH_H_

// TODO 3: make rotation start with correct direction + orientation (relative rotation)
// TODO 3: improve fall timing and speed
// TODO 5: add wreckage at impact
// TODO 3: add proper fallback if out of crash objects


// ****************************************************************
// crash definitions
#define CRASH_OBJECTS (8u)   // 


// ****************************************************************
// crash manager class
class cCrashManager final
{
private:
    // 
    struct sStatus final
    {
        coreVector3 vAim;    // 
        coreFlow    fTime;   // 
        void*       pData;   // 
    };


private:
    cLodObject m_aObject[CRASH_OBJECTS];      // 
    sStatus    m_aStatus[CRASH_OBJECTS];      // 

    coreFlow  m_fSmokeTimer;                  // 
    coreUint8 m_iActiveObjects;               // 

    std::function<void(void*)> m_nCallback;   // 


public:
    cCrashManager()noexcept;
    ~cCrashManager();

    DISABLE_COPY(cCrashManager)

    // render and move the crash manager
    void Render();
    void Move();

    // 
    void AddCrash    (const cLodObject& oBase, const coreVector2 vTarget, void* pData);
    void ClearCrashes(const coreBool bAnimated);

    // 
    template <typename F> void SetImpactCallback(F&& nCallback) {m_nCallback = nCallback;}   // [](void* pData) -> void

    // 
    inline coreBool IsActive()const {return (m_iActiveObjects > 0u);}


private:
    // 
    static void __EnableObject (cLodObject* OUTPUT pObject);
    static void __DisableObject(cLodObject* OUTPUT pObject, const coreBool bAnimated);
};


#endif // _P1_GUARD_CRASH_H_