///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_POSTPROCESSING_H_
#define _P1_GUARD_POSTPROCESSING_H_

// TODO: don't update shader uniforms on single fullscreen objects (postprocessing, transition, glow, etc.)
// TODO: support box mode (4 walls), for scaling
// TODO: don't render wallpapers on 1:1
// TODO: also clamp fullscreen distortion similar to water distortion ?


// ****************************************************************
// post-processing definitions
#define POST_TEXTURE_UNIT_ENVIRONMENT (1u)   // texture unit for the environment frame buffer
#define POST_TEXTURE_UNIT_FOREGROUND  (0u)   // same for foreground
#define POST_TEXTURE_UNIT_GLOW        (2u)   // same for glow
#define POST_TEXTURE_UNIT_DISTORTION  (3u)   // same for distortion

#define POST_INTERIORS (PLAYERS)
#define POST_WALLS     (2u)


// ****************************************************************
// post-processing class
class cPostProcessing final : public coreObject2D, public coreResourceRelation
{
private:
    coreProgramPtr m_pProgramSimple;            // 
    coreProgramPtr m_pProgramDistorted;         // 
    coreProgramPtr m_pProgramDebug;             // 

    coreObject2D m_aInterior[POST_INTERIORS];   // 
    coreObject2D m_aWall    [POST_WALLS];       // wallpapers outside of the game area
    coreObject2D m_Separator;                   // separator for split-screen

    coreFlow m_fSplitScreenValue;               // 
    coreBool m_bSplitScreen;                    // 

    coreVector2 m_vDirectionConfig;             // 
    coreVector2 m_vDirectionGame;               // 


public:
    cPostProcessing()noexcept;

    DISABLE_COPY(cPostProcessing)

    // render and move post-processing
    void Render()final;
    void Move  ()final;

    // recompile post-processing shader-programs
    void Recompile();

    // 
    void UpdateLayout();

    // 
    void        SetWallOpacity  (const coreFloat    fOpacity);
    inline void SetSplitScreen  (const coreBool     bSplitScreen)                        {m_bSplitScreen   = bSplitScreen;}
    inline void SetDirectionGame(const coreVector2& vDirectionGame)                      {m_vDirectionGame = vDirectionGame; ASSERT(vDirectionGame.IsNormalized())}
    inline void SetSaturation   (const coreUintW    iIndex, const coreFloat fSaturation) {ASSERT((iIndex < POST_INTERIORS) && (fSaturation >= 0.0f) && (fSaturation <= 1.0f)) m_aInterior[iIndex].SetColor3(coreVector3(LERP(1.0f, 0.06f, fSaturation), m_aInterior[iIndex].GetColor3().yz()));}
    inline void SetValue        (const coreUintW    iIndex, const coreFloat fValue)      {ASSERT((iIndex < POST_INTERIORS) && (fValue      >= 0.0f) && (fValue      <= 1.0f)) m_aInterior[iIndex].SetColor3(coreVector3(m_aInterior[iIndex].GetColor3().x, fValue, m_aInterior[iIndex].GetColor3().z));}
    inline void SetBorder       (const coreUintW    iIndex, const coreFloat fBorder)     {ASSERT((iIndex < POST_INTERIORS) && (fBorder     >= 0.0f) && (fBorder     <= 1.0f)) m_aInterior[iIndex].SetColor3(coreVector3(m_aInterior[iIndex].GetColor3().xy(), LERP(0.4f, 2.6f, fBorder)));}
    inline void SetSaturationAll(const coreFloat    fSaturation)                         {for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetSaturation(i, fSaturation);}
    inline void SetValueAll     (const coreFloat    fValue)                              {for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetValue     (i, fValue);}
    inline void SetBorderAll    (const coreFloat    fBorder)                             {for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetBorder    (i, fBorder);}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __UpdateInterior();
    void __UpdateWall();
    void __UpdateSeparator();
};


#endif // _P1_GUARD_POSTPROCESSING_H_