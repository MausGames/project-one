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
#define FOREGROUND_AREA (coreVector2(41.5f,41.5f))   // default range of the foreground plane (-0.5 to +0.5, needs to be adapted after camera height change)


// ****************************************************************
// foreground aggregation class
class cForeground final : public coreResourceRelation
{
private:
    coreTexturePtr m_pTexture;    // foreground texture
    coreMatrix4    m_mViewProj;   // foreground view-projection matrix


public:
    cForeground()noexcept;
    ~cForeground();

    DISABLE_COPY(cForeground)

    // start and end foreground aggregation
    void Start();
    void End();

    // access texture
    inline const coreTexturePtr& GetTexture() {return m_pTexture;}

    // get object properties
    inline const coreMatrix4& GetViewProj()const {return m_mViewProj;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_FOREGROUND_H_