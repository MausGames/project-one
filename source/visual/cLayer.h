///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_LAYER_H_
#define _P1_GUARD_LAYER_H_


// ****************************************************************
// fullscreen layer class
class cLayer final : public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;       // layer frame buffer (multisampled)
    coreFrameBuffer m_ResolvedTexture;   // resolved texture

    coreFullscreen m_Fullscreen;         // 


public:
    cLayer()noexcept;

    DISABLE_COPY(cLayer)

    // 
    void Start();
    void End();

    // clear frame buffer
    inline void Clear() {m_ResolvedTexture.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_ResolvedTexture;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;
};


#endif // _P1_GUARD_LAYER_H_