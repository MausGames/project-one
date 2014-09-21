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
    coreObject2D m_aSideArea[2];   // 
    coreObject2D m_aSideLine[2];   // 

    coreLabel m_FPS;               // 
    float m_fFPSValue;             // 


public:
    cPostProcessing()noexcept;
    ~cPostProcessing();

    // apply post-processing
    void Apply();


private:
    DISABLE_COPY(cPostProcessing)
};


#endif // _P1_GUARD_POSTPROCESSING_H_