///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WINDSCREEN_H_
#define _P1_GUARD_WINDSCREEN_H_

// TODO 5: rain (Metroid Prime)
// TODO 5: dirt (smaller spray drops, brownish)
// TODO 5: cracks (star, lightning)
// TODO 5: strong breaks (across screen), left and right are strongly shifted
// TODO 5: move weather-effects here, but make them controllable from the background ?
// TODO 3: try to keep optimized lists longer, instead of only for one frame (maybe keep for mission segment)


// ****************************************************************
// windscreen class
class cWindscreen final
{
private:
    coreMapStr<coreBatchList*> m_apAddList;   // temporary additional objects

    cInk m_Ink;                               // 

    coreBool m_bActive;                       // 


public:
    cWindscreen()noexcept;
    ~cWindscreen();

    DISABLE_COPY(cWindscreen)

    // render and move the windscreen
    void Render();
    void Move();

    // manage temporary objects
    void AddObject(coreObject3D* pObject, const coreVector3 vRelativePos, const coreFloat fLifeTime, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void ClearAdds(const coreBool bAnimated);

    // 
    inline cInk* GetInk() {return &m_Ink;}

    // 
    inline const coreBool& IsActive()const {return m_bActive;}
};


#endif // _P1_GUARD_WINDSCREEN_H_