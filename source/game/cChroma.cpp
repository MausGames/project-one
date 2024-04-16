///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cChromaManager::cChromaManager()noexcept
: m_BulletManager (TYPE_CHROMA)
{
    // 
    m_BulletManager.PrefetchBullet<cChromaBullet>();
}


// ****************************************************************
// 
void cChromaManager::Render()
{
    // 
    m_BulletManager.Render();
}


// ****************************************************************
// 
void cChromaManager::Move()
{
    // 
    m_BulletManager.ForEachBulletTyped<cChromaBullet>([](cChromaBullet* OUTPUT pBullet)
    {
        const coreVector2 vPos  = pBullet->GetPosition().xy();
        const coreVector2 vMove = coreVector2(MIN(CHROMA_AREA.x - ABS(vPos.x), 0.0f) * SIGN(vPos.x),
                                              MIN(CHROMA_AREA.y - ABS(vPos.y), 0.0f) * SIGN(vPos.y));

        // 
        if(!vMove.IsNull()) pBullet->SetPosition(pBullet->GetPosition() + coreVector3(vMove * (3.0f * TIME), 0.0f));
    });

    // 
    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreBool  bRolling = pPlayer->IsRolling();
        const coreFloat fRange   = (bRolling ? POW2(40.0f) : POW2(15.0f));
        const coreFloat fSpeed   = (bRolling ? 200.0f      : 100.0f) * RCP(fRange) * TIME;

        // 
        m_BulletManager.ForEachBulletTyped<cChromaBullet>([&](cChromaBullet* OUTPUT pBullet)
        {
            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pBullet->GetPosition().xy();
            const coreFloat   fLen  = MAX(fRange - vDiff.LengthSq(), 0.0f);

            // 
            pBullet->SetPosition(pBullet->GetPosition() + coreVector3(vDiff.Normalized() * (fLen * fSpeed), 0.0f));
        });
    });

    // 
    m_BulletManager.Move();
}


// ****************************************************************
// 
void cChromaManager::AddChroma(const coreVector2& vPosition, const coreVector2& vDirection, const coreFloat fScale)
{
    // 
    m_BulletManager.AddBullet<cChromaBullet>(1, 0.0f, NULL, vPosition, vDirection)->ChangeScale(fScale);
}


// ****************************************************************
// 
void cChromaManager::ClearChromas(const coreBool bAnimated)
{
    // 
    m_BulletManager.ClearBullets(bAnimated);
}