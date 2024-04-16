///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WINDSCREEN_H_
#define _P1_GUARD_WINDSCREEN_H_

// TODO: rain (Metroid Prime)
// TODO: dirt (smaller spray drops, brownish)
// TODO: cracks (star, lightning)
// TODO: strong breaks (across screen), left and right are strongly shifted
// TODO: move weather-effects here, but make them controllable from the background ?
// TODO: try to keep optimized lists longer, instead of only for one frame


// ****************************************************************
// windscreen class
class cWindscreen final
{
private:
    coreLookupStr<coreBatchList*> m_apAddList;   // temporary additional objects

    cInk m_Ink;                                  // 

    bool m_bActive;                              // 


public:
    cWindscreen()noexcept;
    ~cWindscreen();

    DISABLE_COPY(cWindscreen)

    // render and move the windscreen
    void Render();
    void Move();

    // 
    void Update();

    // manage temporary objects
    void AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreFloat fLifeTime, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void ClearAdds(const coreBool bAnimated);

    // 
    inline cInk* GetInk() {return &m_Ink;}

    // 
    inline const coreBool& IsActive()const {return m_bActive;}
};


#endif // _P1_GUARD_WINDSCREEN_H_