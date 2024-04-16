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
// TODO: ships and objects moving in and out of ink should draw some clouds
// TODO: ink needs different color in point and line rendering


// ****************************************************************
// windscreen definitions
#define WINDSCREEN_INK_SAMPLES_POINT (3u)    // 
#define WINDSCREEN_INK_SAMPLES_LINE  (10u)   // 


// ****************************************************************
// windscreen class
class cWindscreen final
{
private:
    std::vector<coreObject3D*>    m_apAddObject;   // temporary additional objects
    coreLookupStr<coreBatchList*> m_apAddList;     // optimized lists for temporary additional objects

    coreObject2D m_Ink;                            // 
    coreFlow     m_fInkAnimation;                  // 
    coreFloat    m_fInkDelay;                      // 
    coreVector4  m_vInkPosition;                   // 
    coreVector2  m_vInkAlpha;                      // 

    bool m_bActive;                                // 


public:
    cWindscreen()noexcept;
    ~cWindscreen();

    DISABLE_COPY(cWindscreen)

    // render and move the windscreen
    void Render();
    void Move();

    // manage additional objects
    void AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreFloat fLifeTime);
    void AddObject(coreObject3D* pObject, const coreVector3& vRelativePos, const coreFloat fLifeTime, const coreUint32 iCapacity, const coreHashString& sProgramInstancedName, const coreHashString& sListKey);
    void ClearObjects();

    // 
    void EnableInk     (const coreBool  bLine);
    void DisableInk    (const coreFloat fDelay);
    void SetInkPosition(const coreUintW iIndex, const coreVector2& vPosition);
    void SetInkAlpha   (const coreUintW iIndex, const coreFloat    fAlpha);

    // 
    inline const coreBool& IsActive()const {return m_bActive;}


private:
    // render and move the ink
    void __RenderInk();
    void __MoveInk();
};


#endif // _P1_GUARD_WINDSCREEN_H_