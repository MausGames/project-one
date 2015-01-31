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

// TODO: don't upate shader uniforms on single fullscreen objects (postprocessing, transition, glow, etc.)


// ****************************************************************
// post-processing class
class cPostProcessing final : public coreObject2D
{
private:
    coreObject2D m_aSideArea[2];   // objects outside of the game area
    coreObject2D m_aSideLine[2];   // additional highlight objects


public:
    cPostProcessing()noexcept;

    DISABLE_COPY(cPostProcessing)

    // apply post-processing
    void Apply();

    // recompile post-processing shader-program
    void Recompile();
};


#endif // _P1_GUARD_POSTPROCESSING_H_