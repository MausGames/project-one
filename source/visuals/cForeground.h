//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_FOREGROUND_H_
#define _P1_GUARD_FOREGROUND_H_


// ****************************************************************
// foreground definitions
//#define _FOREGROUND_FRAMEBUFFER_ (1)  // use multisampled frame buffer instead of screen-copy (currently faster ?!)


// ****************************************************************
// foreground aggregation class
class cForeground final : coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;       // foreground frame buffer (intern, multisampled)
    coreFrameBuffer m_iResolvedTexture;   // resolved texture

    coreMatrix4 m_mViewProj;              // foreground view-projection matrix


public:
    cForeground()noexcept;
    ~cForeground();

    // start and end foreground aggregation
    void Start();
    void End();

     // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iResolvedTexture;}   // different name

    // get object properties
    inline const coreMatrix4& GetViewProj()const {return m_mViewProj;}


private:
    DISABLE_COPY(cForeground)

    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_FOREGROUND_H_