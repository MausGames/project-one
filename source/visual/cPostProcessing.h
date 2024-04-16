///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_POSTPROCESSING_H_
#define _P1_GUARD_POSTPROCESSING_H_

// TODO 3: don't update shader uniforms on single fullscreen objects (postprocessing, transition, glow, etc.)
// TODO 3: also clamp fullscreen distortion similar to water distortion ?
// TODO 3: border might flicker-disappear when rotating, briefly passing 0.0f, and shaking screen
// TODO 3: if overdraw can be handled, then N "shadow-border" can be used when rotating (like on Dharuk) (own shader with discard ?)


// ****************************************************************
// post-processing definitions
#define POST_TEXTURE_UNIT_ENVIRONMENT (1u)   // texture unit for the environment frame buffer
#define POST_TEXTURE_UNIT_FOREGROUND  (0u)   // same for foreground
#define POST_TEXTURE_UNIT_GLOW        (2u)   // same for glow
#define POST_TEXTURE_UNIT_DISTORTION  (3u)   // same for distortion

#define POST_INTERIORS          (PLAYERS)
#define POST_WALLS_BASE         (2u)
#define POST_WALLS_EXTRA        (2u)
#define POST_WALLS              (POST_WALLS_BASE + POST_WALLS_EXTRA)
#define POST_BORDERS            (3u)
#define POST_CHROMA_FACTOR      (0.01f)
#define POST_DEFAULT_BORDER_MIN (0.3f)
#define POST_DEFAULT_BORDER_MAX (0.8f)


// ****************************************************************
// post-processing class
class cPostProcessing final : public coreObject2D, public coreResourceRelation
{
private:
    coreProgramPtr m_pProgramSimple;            // 
    coreProgramPtr m_pProgramDistorted;         // 
    coreProgramPtr m_pProgramTransparent;       // 
    coreProgramPtr m_pProgramChroma;            // 
    coreProgramPtr m_pProgramDebug;             // 

    coreObject2D m_aInterior[POST_INTERIORS];   // 
    coreObject2D m_aWall    [POST_WALLS];       // wallpapers outside of the game area
    coreObject2D m_aBorder  [POST_BORDERS];     // 
    coreObject2D m_Separator;                   // separator for split-screen

    coreFullscreen m_Black;                     // 

    coreFlow m_fSplitScreenValue;               // 
    coreBool m_bSplitScreen;                    // 

    coreVector2 m_vDirectionConfig;             // 
    coreVector2 m_vDirectionGame;               // 

    coreVector3 m_avData  [POST_INTERIORS];     // 
    coreFloat   m_afOffset[POST_WALLS];         // (-x, +x, -y, +y) 
    coreBool    m_bOffsetActive;                // 

    coreFloat m_fChroma;                        // 

    coreFlow m_fAnimation;                      // 

    coreFullscreen m_Frame;                     // 
    coreFloat      m_fFrameValue;               // 
    coreFlow       m_fFrameAnimation;           // 

    coreFlow  m_fReset;                         // 
    coreFloat m_fResetAngle;                    // 
    coreFloat m_afResetOffset[POST_WALLS];      // 


public:
    cPostProcessing()noexcept;

    DISABLE_COPY(cPostProcessing)

    // render and move post-processing
    void Render()final;
    void Move  ()final;
    
    inline void RenderTilt()
    {
        m_Black.Render();

        if((m_fFrameValue < 2.0f) && (!coreMath::IsNear(Core::System->GetResolution().AspectRatio(), 1.0f) || m_bOffsetActive))
        {
            // render wallpapers
            for(coreUintW i = m_bOffsetActive ? 0u : POST_WALLS_BASE; i < POST_WALLS; ++i)
                m_aWall[i].Render();
        }
    }

    // recompile post-processing shader-programs
    void Recompile();

    // 
    void Reset();

    // 
    void UpdateLayout();

    // 
    inline coreBool IsOffset  ()const {return m_bOffsetActive;}
    inline coreBool IsMirrored()const {return (this->GetSize().x < 0.0f);}

    // (wrapper) 
    inline void SetSoundVolume(const coreFloat fVolume) {ASSERT((fVolume >= 0.0f) && (fVolume <= 1.0f)) Core::Audio->SetSoundVolume(fVolume);}
    
    inline void SetFrameAnimation(const coreFloat fFrameAnimation) {m_fFrameAnimation = fFrameAnimation;}

    // 
    void        SetWallOpacity  (const coreFloat   fOpacity);
    inline void SetWallOffset   (const coreUintW   iIndex, const coreFloat fOffset)     {ASSERT(iIndex < POST_WALLS) m_afOffset[iIndex] = fOffset; if(fOffset) m_bOffsetActive = true;}
    inline void SetSplitScreen  (const coreBool    bSplitScreen)                        {m_bSplitScreen   = bSplitScreen;}
    inline void SetDirectionGame(const coreVector2 vDirectionGame)                      {m_vDirectionGame = vDirectionGame; ASSERT(vDirectionGame.IsNormalized())}
    inline void SetFrameValue   (const coreFloat   fFrameValue) {m_fFrameValue = fFrameValue;}
    inline void SetSaturation   (const coreUintW   iIndex, const coreFloat fSaturation) {ASSERT((iIndex < POST_INTERIORS) && (fSaturation >= 0.0f) && (fSaturation <= 1.0f)) m_avData[iIndex].x = LERP(0.0f, 0.94f, fSaturation);}
    inline void SetValue        (const coreUintW   iIndex, const coreFloat fValue)      {ASSERT((iIndex < POST_INTERIORS) && (fValue      >= 0.0f) && (fValue      <= 1.0f)) m_avData[iIndex].y = LERP(0.0f, 1.0f,  fValue);}
    inline void SetBorder       (const coreUintW   iIndex, const coreFloat fBorder)     {ASSERT((iIndex < POST_INTERIORS) && (fBorder     >= 0.0f) && (fBorder     <= 1.0f)) m_avData[iIndex].z = LERP(0.4f, 2.0f,  fBorder);}
    inline void SetSaturationAll(const coreFloat   fSaturation)                         {for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetSaturation(i, fSaturation);}
    inline void SetValueAll     (const coreFloat   fValue)                              {/*for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetValue     (i, fValue);*/ m_Black.SetAlpha(1.0f - fValue); m_Black.SetEnabled(m_Black.GetAlpha() ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);}
    inline void SetBorderAll    (const coreFloat   fBorder)                             {for(coreUintW i = 0u; i < POST_INTERIORS; ++i) this->SetBorder    (i, fBorder);}
    inline void SetChroma       (const coreFloat   fChroma)                             {m_fChroma = fChroma;}

    // 
    inline const coreVector2& GetDirectionGame()const {return m_vDirectionGame;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __UpdateInterior();
    void __UpdateWall();
    void __UpdateSeparator();
    void __UpdateData();
};


#endif // _P1_GUARD_POSTPROCESSING_H_