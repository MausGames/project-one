///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_EXHAUST_H_
#define _P1_GUARD_EXHAUST_H_


// ****************************************************************
// 
#define EXHAUST_BASES           (3u)    // 
#define EXHAUST_RENDER_CAPACITY (64u)   // 
#define EXHAUST_DATA_CAPACITY   (64u)   // 
#define EXHAUST_POOL_PAGESIZE   (32u)   // 

enum eExhaustType : coreUint8
{
    EXHAUST_TYPE_SCOUT = 0u,
    EXHAUST_TYPE_WARRIOR,
    EXHAUST_TYPE_STAR,
    EXHAUST_TYPE_ARROW,
    EXHAUST_TYPE_MINER,
    EXHAUST_TYPE_FREEZER,
    EXHAUST_TYPE_CINDER,
    EXHAUST_TYPE_METEOR,
    EXHAUST_TYPE_UFO
};


// ****************************************************************
// 
class cExhaustManager final
{
private:
    // 
    struct sExhaustData final
    {
        cEnemy*       pEnemy;     // 
        coreObject3D* pObject;    // 
        eExhaustType  eType;      // 
        coreUint8     iSubType;   // 
        coreUint8     iBase;      // 
    };


private:
    coreBatchList  m_aRenderList[EXHAUST_BASES];   // 
    coreObject3D   m_aPrototype [EXHAUST_BASES];   // 
    coreMemoryPool m_ObjectPool;                   // 

    coreList<sExhaustData> m_aData;                // 

    coreFlow m_fAnimation;                         // 


public:
    cExhaustManager()noexcept;
    ~cExhaustManager();

    DISABLE_COPY(cExhaustManager)

    // 
    void Render();
    void Move();

    // 
    void BindEnemy  (cEnemy* pEnemy, const eExhaustType eType);
    void UnbindEnemy(cEnemy* pEnemy, const eExhaustType eType);
};


#endif // _P1_GUARD_EXHAUST_H_