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
#define THEATER_AREA_FACTOR      (1.6f)   // 
#define THEATER_SPAWN_TIME       (6.0f)   // 
#define THEATER_SPAWN_TIME_RAND  (3.5f)   // 
#define THEATER_SPAWN_COUNT      (3u)     // 
#define THEATER_SPAWN_COUNT_RAND (1u)     // 


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
    coreUint8 m_iSpawnCount;      // 

    coreFloat m_fCurRow;          // 
    coreBool  m_bCurSide;         // 
    coreUint8 m_iCurType;         // 


public:
    cTheater()noexcept;
    ~cTheater();

    DISABLE_COPY(cTheater)

    // render and move the theater
    void Render();
    void Move();

    // 
    template <typename T> RETURN_RESTRICT T* AddMute(const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor);
    void ClearMutes(const coreBool bAnimated);

    // 
    inline coreBool IsActive()const {return (!m_aMute.empty() || g_pSpecialEffects->IsActive());}


private:
    // 
    void __KillMute(sMute* OUTPUT pMute, const coreBool bAnimated);
};


// ****************************************************************
// 
template <typename T> RETURN_RESTRICT T* cTheater::AddMute(const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor)
{
    // 
    T* pEnemy = new T();
    pEnemy->Configure  (100, vColor);
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