///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_THEATER_H_
#define _P1_GUARD_THEATER_H_

// TODO: handle resetting the blink-property


// ****************************************************************
// 
#define THEATER_AREA_FACTOR (1.6f)   // 
#define THEATER_SPAWN_TIME  (8.0f)   // 
#define THEATER_SPAWN_RAND  (3.5f)   // 


// ****************************************************************
// theater class
class cTheater final
{
private:
    // 
    struct sMute final
    {
        cEnemy*     pEnemy;       // 
        coreVector2 vDirection;   // 
    };


private:
    std::vector<sMute> m_aMute;   // 

    coreFlow  m_fSpawnTimer;      // 
    coreUint8 m_iCurType;         // 
    coreBool  m_bCurSide;         // 


public:
    cTheater()noexcept;
    ~cTheater();

    DISABLE_COPY(cTheater)

    // render and move the theater
    void Render();
    void Move();

    // 
    template <typename T> RETURN_RESTRICT T* AddMute(const coreVector3& vPosition, const coreVector2& vDirection);
    void ClearMutes(const coreBool bAnimated);

    // 
    inline coreBool IsActive()const {return (!m_aMute.empty() || g_pSpecialEffects->IsActive());}


private:
    // 
    void __KillMute(sMute* OUTPUT pMute, const coreBool bAnimated);
};


// ****************************************************************
// 
template <typename T> RETURN_RESTRICT T* cTheater::AddMute(const coreVector3& vPosition, const coreVector2& vDirection)
{
    // 
    T* pEnemy = new T();
    pEnemy->AddStatus  (ENEMY_STATUS_SINGLE);
    pEnemy->Resurrect  (vPosition.xy(), vDirection);
    pEnemy->SetPosition(vPosition);   // # to override height

    // 
    sMute oMute;
    oMute.pEnemy     = pEnemy;
    oMute.vDirection = vDirection;

    // 
    m_aMute.push_back(oMute);

    return pEnemy;
}


#endif // _P1_GUARD_THEATER_H_