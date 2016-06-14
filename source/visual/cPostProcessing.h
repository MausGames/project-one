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

// TODO: don't update shader uniforms on single fullscreen objects (postprocessing, transition, glow, etc.)
// TODO: remove watermark


// ****************************************************************
// post-processing definitions
#define POST_TEXTURE_UNIT_ENVIRONMENT (1u)   // texture unit for the environment frame buffer
#define POST_TEXTURE_UNIT_FOREGROUND  (0u)   // same for foreground
#define POST_TEXTURE_UNIT_GLOW        (2u)   // same for glow
#define POST_TEXTURE_UNIT_DISTORTION  (3u)   // same for distortion


// ****************************************************************
// post-processing class
class cPostProcessing final : public coreObject2D, public coreResourceRelation
{
private:
    coreProgramPtr m_pProgramSimple;      // 
    coreProgramPtr m_pProgramDistorted;   // 
    coreProgramPtr m_pProgramDebug;       // 

    coreObject2D m_aSideArea[2];          // objects outside of the game area
    coreLabel    m_Watermark;             // temporary Twitter watermark


public:
    cPostProcessing()noexcept;

    DISABLE_COPY(cPostProcessing)

    // apply post-processing
    void Apply();

    // recompile post-processing shader-programs
    void Recompile();

    // set side-object opacity
    void SetSideOpacity(const coreFloat fValue);

    // 
    inline void SetSaturation(const coreFloat fSaturation) {ASSERT((fSaturation >= 0.0f) && (fSaturation <= 1.0f)) this->SetColor3(coreVector3(0.06f + 0.94f * (1.0f - fSaturation), this->GetColor3().yz()));}
    inline void SetValue     (const coreFloat fValue)      {ASSERT((fValue      >= 0.0f) && (fValue      <= 1.0f)) this->SetColor3(coreVector3(this->GetColor3().x, fValue, this->GetColor3().z));}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset bInit)final;
};


#endif // _P1_GUARD_POSTPROCESSING_H_