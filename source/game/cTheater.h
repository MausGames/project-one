//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
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

        constexpr sMute(cEnemy* pEnemy, const coreVector2& vDirection)noexcept;
        void Destruct();
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

    // 
    inline coreBool IsActive()const {return (!m_aMute.empty() || g_pSpecialEffects->IsActive());}
};


// ****************************************************************
// 
constexpr cTheater::sMute::sMute(cEnemy* pEnemy, const coreVector2& vDirection)noexcept
: pEnemy     (pEnemy)
, vDirection (vDirection)
{
}


// ****************************************************************
// 
template <typename T> RETURN_RESTRICT T* cTheater::AddMute(const coreVector3& vPosition, const coreVector2& vDirection)
{
    // 
    T* pEnemy = new T();
    pEnemy->AddStatus  (ENEMY_STATUS_SINGLE);
    pEnemy->Resurrect  (vPosition.xy(), vDirection);
    pEnemy->SetPosition(vPosition);

    // 
    m_aMute.emplace_back(pEnemy, vDirection);

    return pEnemy;
}


#endif // _P1_GUARD_THEATER_H_