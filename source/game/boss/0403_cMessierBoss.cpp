///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: zeit könnte auch rückwärts laufen, und wenn man ihn auf 100% bringt bevor die zeit auf 0 geht kommt geheime phase, mehr als 100% HP, label wobbelt um anzuzeigen, dass das ok ist, bonus punkte ?
// TODO 3: reversed bullets should disappear into boss with slight fade
// bullets using time shift can't be near each others along fly-dir, because of depth-order
// reverse bullets are created with wrong delay, der tick fürs erzeugen beim boss und fürs erzeugen an der wand überlagern sich nicht korrekt
// meteoriten-rotations angriff in mittel phase vor gravitation, meteoriten werden weggeschossen und dann geht gravitation los
// links und rechts teleporter, meteoriten kommen raus in unendlichkeit, sind manchmal so arranged (seitlich und von oben-schräg), dass man durch teleporter durch muss
// intro: meteoriten kommen zusammen, kurze drehung und wartezeit, dann bumm, und messier bricht raus schon mit schild


// ****************************************************************
// counter identifier


// ****************************************************************
// vector identifier
#define COVER_ROTATION (0u)


// ****************************************************************
// constructor
cMessierBoss::cMessierBoss()noexcept
: m_fRingTime   (0.0f)
, m_fTimeFactor (1.0f)
, m_fShootTime  (0.0f)
, m_fAnimation  (0.0f)
, m_fRotation   (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_messier_high.md3");
    this->DefineModelLow ("ship_boss_messier_low.md3");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 10.0f);
    this->SetOrientation(coreVector3(1.0f,1.0f,0.0f).Normalized());

    // configure the boss
    this->Configure(5000, COLOR_SHIP_MAGENTA);

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        // load object resources
        m_aRing[i].DefineModel  ("object_ring.md3");
        m_aRing[i].DefineTexture(0u, "effect_energy.png");
        m_aRing[i].DefineProgram("effect_energy_spheric_program");
        m_aRing[i].SetColor3    (COLOR_ENERGY_MAGENTA * 0.7f);
        m_aRing[i].SetTexSize   (coreVector2(0.5f,1.0f) * 2.5f);
        m_aRing[i].SetTexOffset (coreVector2(I_TO_F(i) / I_TO_F(MESSIER_RINGS), 0.0f));
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMessierBoss::__ResurrectOwn()
{

}


// ****************************************************************
// 
void cMessierBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableRings(bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cMessierBoss::__RenderOwnOver()
{
    if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        DEPTH_PUSH_SHIP

        // 
        const coreUintW iStart = MESSIER_RINGS - F_TO_UI(FRACT(m_fRingTime) * MESSIER_RINGS);

        // 
        for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
            m_aRing[(iStart + i) % MESSIER_RINGS].Render();
    }
}


// ****************************************************************
// 
void cMessierBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    if(this->ReachedDeath()) this->Kill(true);   

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    ASSERT(pSquad1->GetNumEnemies() == MESSIER_ENEMIES)

    // 
    cSpaceBackground* pBackground = d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground());
    pBackground->SetMeteorSpeed(m_fTimeFactor);

    // 
    m_fAnimation.UpdateMod(1.0f * m_fTimeFactor, 2.0f*PI);
    m_fRotation .UpdateMod(1.0f * m_fTimeFactor, 2.0f*PI);

    // 
    const coreVector2 vBossDir = coreVector2::Direction(m_fRotation);
    this->SetDirection(coreVector3(coreVector2(1.0f,-1.0f).Normalized() * vBossDir.x, vBossDir.y).Normalized());

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->_StartBoss();

                //this->__EnableRings();

                for(coreUintW i = 0u; i < MESSIER_ENEMIES; ++i)
                {
                    pSquad1->GetEnemy(i)->Resurrect();
                }
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        this->SetPosition(coreVector3(0.0f,0.6f,0.0f) * FOREGROUND_AREA3);

        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            m_fTimeFactor = LERPH3(1.0f, -1.0f, CLAMP((fTime - 0.3f) * 3.0f, 0.0f, 1.0f));
        });

        if(this->__PhaseTicker(5.0f))
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = coreVector2(1.0f,-1.0f).Normalized();

            this->__AddBullet(1.0f, vPos, vDir);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        this->SetPosition(coreVector3(0.0f,0.0f,0.0f) * FOREGROUND_AREA3);
#if 0
        PHASE_CONTROL_TICKER(0u, 0u, 0.3f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreFloat   fBase = I_TO_F(iTick) * (0.015f*PI);

            for(coreUintW j = 24u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 180.0f / 24.0f) + fBase);
                const coreFloat   fSpeed = ((j / 6u) % 2u) ? 0.6f : 0.35f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.1f);
            }
        });
#endif
        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(m_fLifeTime * (0.08f*PI));   // * -1.0f 

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos,  vDir)            ->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, -vDir)            ->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos,  vDir.Rotated90())->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, -vDir.Rotated90())->ChangeSize(1.4f);
        });
    }

    // ################################################################
    // ################################################################

    // 
    if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_fRingTime.Update(0.4f);

        for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
        {
            // 
            const coreFloat   fTime   = FRACT(MAX(m_fRingTime + (I_TO_F(i) / I_TO_F(MESSIER_RINGS)), 0.0f));
            const coreFloat   fOffset = (I_TO_F(i) / I_TO_F(MESSIER_RINGS)) * 4.0f*PI + 1.1f*PI;
            const coreVector2 vDir    = coreVector2::Direction(0.5f*PI * m_fRingTime + fOffset);
            const coreVector2 vOri    = coreVector2::Direction(1.0f*PI * m_fRingTime + fOffset);

            // 
            m_aRing[i].SetPosition   (this->GetPosition());
            m_aRing[i].SetSize       (coreVector3(1.0f,1.0f,1.0f) * LERP(10.0f, 30.0f, fTime));
            m_aRing[i].SetDirection  (coreVector3(vDir, 0.0f));
            m_aRing[i].SetOrientation(OriRoundDir(vOri, vDir));
            m_aRing[i].SetAlpha      (LERPH3(0.0f, 1.0f, MIN(1.0f - fTime, 4.0f * fTime, 1.0f)));
            m_aRing[i].Move();
        }

        // 
        const coreVector2 vCameraPos = g_pEnvironment->GetCameraPos().xy();
        const coreFloat   fRotaTime  = -TIME * CLAMP((m_fRingTime + 1.0f) * 0.5f, 0.0f, 1.0f);

        // 
        const auto nRotaFunc = [&](const coreVector2 vPosition)
        {
            return coreMatrix3::Rotation(fRotaTime * MAX(1.0f - vPosition.LengthSq() / POW2(80.0f), 0.01f)).m12();
        };

        // 
        coreBatchList* pList = pBackground->GetMeteorList();
        for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
        {
            coreObject3D* pMeteor = (*pList->List())[i];
            if(!pMeteor->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

            // 
            const coreVector2 vRelPos = pMeteor->GetPosition().xy() - vCameraPos;
            const coreMatrix2 vRota   = nRotaFunc(vRelPos);
            const coreVector2 vNewPos = vRelPos * vRota + vCameraPos;
            const coreVector2 vNewDir = pMeteor->GetDirection  ().xy() * vRota;
            const coreVector2 vNewOri = pMeteor->GetOrientation().xy() * vRota;

            // 
            pMeteor->SetPosition   (coreVector3(vNewPos, pMeteor->GetPosition   ().z));
            pMeteor->SetDirection  (coreVector3(vNewDir, pMeteor->GetDirection  ().z).Normalized());
            pMeteor->SetOrientation(coreVector3(vNewOri, pMeteor->GetOrientation().z).Normalized());
        }

        // 
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            // 
            const coreMatrix2 vRota   = nRotaFunc(pPlayer->GetPosition().xy());
            const coreVector2 vNewPos = pPlayer->GetPosition ().xy() * vRota;
            const coreVector2 vNewDir = pPlayer->GetDirection().xy() * vRota;

            // 
            pPlayer->SetPosition (coreVector3(vNewPos, 0.0f));
            pPlayer->SetDirection(coreVector3(vNewDir, 0.0f));
        });

        // 
        m_avVector[COVER_ROTATION].x += TIME * 0.5f * STEPH3(0.0f, 10.0f, m_fRingTime);
        pBackground->SetCoverDir(coreVector2::Direction(m_avVector[COVER_ROTATION].x));
    }




    // 
    g_pGame->GetBulletManagerEnemy()->ForEachBullet([this](cBullet* OUTPUT pBullet)
    {
        if(!pBullet->HasStatus(BULLET_STATUS_IMMORTAL)) return;

        // 
        pBullet->SetSpeed(m_fTimeFactor * I_TO_F(pBullet->GetDamage()) * 0.1f);

        // 
        const coreVector2 vPos  = pBullet->GetPosition().xy();
        const coreVector2 vDiff = this->GetPosition().xy() - vPos;

        // 
        if(coreVector2::Dot(vDiff, pBullet->GetFlyDir()) > 0.0f)
        {
            pBullet->Deactivate(false);
        }

        // 
        if((pBullet->GetFlyTime() >= 0.5f) &&
           ((vPos.x < -FOREGROUND_AREA.x * 1.2f) ||
            (vPos.x >  FOREGROUND_AREA.x * 1.2f) ||
            (vPos.y < -FOREGROUND_AREA.y * 1.2f) ||
            (vPos.y >  FOREGROUND_AREA.y * 1.2f)))
        {
            pBullet->Deactivate(false);
        }
    });




    // (stage functions)   
    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
    {
        STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

        const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * 2.0f*PI / 8.0f - m_fAnimation);

        pEnemy->SetPosition (this->GetPosition() + coreVector3(vDir * 18.0f, 0.0f));
        pEnemy->SetDirection(coreVector3(coreVector2::Direction((m_fAnimation + I_TO_F(i))), 0.0f));

        //const coreVector2 vDir2 = coreVector2::Direction(I_TO_F(i) * 2.0f*PI / 4.0f + m_fAnimation * ((i < 4u) ? 0.25f : 1.25f));

        //pEnemy->SetPosition (this->GetPosition() + coreVector3(vDir2 * ((i < 4u) ? 38.0f : 24.0f), 0.0f));
        //pEnemy->SetDirection(coreVector3(coreVector2::Direction((m_fAnimation + I_TO_F(i))), 0.0f));
        //
        //pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    });
}


// ****************************************************************
// 
void cMessierBoss::__EnableRings()
{
    WARN_IF(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_fRingTime = -1.0f;

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aRing[i]);
    }

    // 
    g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_MAGENTA);
}


// ****************************************************************
// 
void cMessierBoss::__DisableRings(const coreBool bAnimated)
{
    if(!m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_aRing[i]);
    }
}


// ****************************************************************
// 
void cMessierBoss::__AddBullet(const coreFloat fSpeed, const coreVector2 vPosition, const coreVector2 vDirection)
{
    const coreVector2 vPos = (m_fTimeFactor > 0.0f) ? vPosition : (vPosition + vDirection * g_pForeground->RayIntersection(vPosition, vDirection, 1.2f));
    const coreVector2 vDir = vDirection;

    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(F_TO_SI(fSpeed * 10.0f), fSpeed, this, vPos, vDir)->ChangeSize(1.2f)->AddStatus(BULLET_STATUS_IMMORTAL);
}


// ****************************************************************
// 
coreBool cMessierBoss::__PhaseTicker(const coreFloat fRate)
{
    m_fShootTime.Update(fRate * m_fTimeFactor);

    if(!InBetween(m_fShootTime, 0.0f, 1.0f))
    {
        m_fShootTime = FmodRange(m_fShootTime, 0.0f, 1.0f);
        return true;
    }

    return false;
}