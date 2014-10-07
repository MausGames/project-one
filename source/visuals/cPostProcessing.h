//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_POSTPROCESSING_H_
#define _P1_GUARD_POSTPROCESSING_H_


// ****************************************************************
// post-processing class
class cPostProcessing final : coreObject2D
{
private:
    coreObject2D m_aSideArea[2];   // objects outside of the game area
    coreObject2D m_aSideLine[2];   // additional highlight objects

    coreByte m_iAlternate;         // alternating value to render side-objects at lower frequency


public:
    cPostProcessing()noexcept;
    ~cPostProcessing();

    // apply post-processing
    void Apply();

    // recompile post-processing shader-program
    void Recompile();

    // get current side
    inline const coreByte& GetAlternate()const {return m_iAlternate;}


private:
    DISABLE_COPY(cPostProcessing)
};


#endif // _P1_GUARD_POSTPROCESSING_H_