//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cCrossfieldBoss::cCrossfieldBoss()noexcept
: m_Boomerang      (CROSSFIELD_BOOMERANGS)
, m_BoomerangTrail (CROSSFIELD_BOOMERANGS * CROSSFIELD_TRAILS)
, m_iPackedDir     (0u)
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModel   ("ship_boss_crossfield_high.md3");
    this->DefineModelLow("ship_boss_crossfield_low.md3");

    // 
    this->SetSize(coreVector3(3.0f,3.0f,3.0f));

    // configure the boss
    this->Configure(10000, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());



    m_Duplicate.DefineModel  ("ship_boss_crossfield_high.md3");
    m_Duplicate.DefineTexture(0u, "effect_energy.png");
    m_Duplicate.DefineProgram("effect_energy_spheric_program");
    m_Duplicate.SetPosition  (coreVector3(0.0f,100.0f,0.0f));
    m_Duplicate.SetSize      (this->GetSize());
    m_Duplicate.SetColor3    (COLOR_RED_F * 0.8f);
    m_Duplicate.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    g_pOutlineFull->BindObject(&m_Duplicate);
    g_pGlow->BindObject(&m_Duplicate);


    for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
    {
        m_aDuplicateTrail[i].DefineModel  ("ship_boss_crossfield_low.md3");
        m_aDuplicateTrail[i].DefineTexture(0u, "effect_energy.png");
        m_aDuplicateTrail[i].DefineProgram("effect_energy_spheric_program");
        m_aDuplicateTrail[i].SetPosition  (coreVector3(0.0f,100.0f,0.0f));
        m_aDuplicateTrail[i].SetSize      (this->GetSize());
        m_aDuplicateTrail[i].SetColor3    (COLOR_RED_F * (0.0f + 0.2f * I_TO_F(i+1u)));
        m_aDuplicateTrail[i].SetAlpha     (0.15f * I_TO_F(i+1u));

        g_pGlow->BindObject(&m_aDuplicateTrail[i]);
    }



    m_Boomerang     .DefineProgram("effect_energy_spheric_inst_program");
    m_BoomerangTrail.DefineProgram("effect_energy_spheric_inst_program");


    for(coreUintW i = 0u; i < CROSSFIELD_BOOMERANGS; ++i)
    {
        coreObject3D* pBoomerang = new coreObject3D();
        pBoomerang->DefineModel  ("object_boomerang.md3");
        pBoomerang->DefineTexture(0u, "effect_energy.png");
        pBoomerang->DefineProgram("effect_energy_spheric_program");
        pBoomerang->SetSize      (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
        pBoomerang->SetColor3    (COLOR_RED_F * 0.8f);
        pBoomerang->SetTexSize   (coreVector2(0.5f,0.5f));
        pBoomerang->SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

        m_Boomerang.BindObject(pBoomerang);
    }



    for(coreUintW i = 0u; i < CROSSFIELD_BOOMERANGS * CROSSFIELD_TRAILS; ++i)
    {
        const coreFloat fValue = I_TO_F((i % 3u) + 1u);

        coreObject3D* pTrail = new coreObject3D();
        pTrail->DefineModel  ("object_boomerang.md3");
        pTrail->DefineTexture(0u, "effect_energy.png");
        pTrail->DefineProgram("effect_energy_spheric_program");
        pTrail->SetSize      (coreVector3(1.0f,1.0f,1.0f) * (1.0f + 0.1f*fValue));
        pTrail->SetColor3    (COLOR_RED_F * (0.0f + 0.2f * fValue));
        pTrail->SetAlpha     (0.15f * fValue);
        pTrail->SetTexSize   (coreVector2(0.5f,0.5f));
        pTrail->SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

        m_BoomerangTrail.BindObject(pTrail);
    }


    //(*m_apBoomerang.List())[0]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[0]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[1]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[2]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //
    //(*m_apBoomerang.List())[1]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[3]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[4]->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //(*m_apTrail.List())[5]->SetEnabled(CORE_OBJECT_ENABLE_ALL);

    //(*m_apBoomerang.List())[1]->SetPosition(coreVector3(FOREGROUND_AREA.x * 1.35f * 0.5f, 0.0f, 0.0f));

    // 
    g_pOutlineFull->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_BoomerangTrail);



}


// ****************************************************************
// destructor
cCrossfieldBoss::~cCrossfieldBoss()
{
    // 
    FOR_EACH(it, *m_Boomerang     .List()) SAFE_DELETE(*it)
    FOR_EACH(it, *m_BoomerangTrail.List()) SAFE_DELETE(*it)

    // 
    g_pOutlineFull->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_BoomerangTrail);


    g_pOutlineFull->UnbindObject(&m_Duplicate);
    g_pGlow->UnbindObject(&m_Duplicate);

    for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
        g_pGlow->UnbindObject(&m_aDuplicateTrail[i]);
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwnBefore()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
            m_aDuplicateTrail[i].Render();

        // 
        m_BoomerangTrail.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwnAfter()
{
    // 
    m_Duplicate.Render();

    // 
    m_Boomerang.Render();
}


// ****************************************************************
// 
void cCrossfieldBoss::__MoveOwn()
{
#define COUNTER_DUPLICATE_STATUS (0u)
#define COUNTER_CURRENT_SIDE     (1u)

    //cPlayer* pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());

    m_fAnimation.Update(0.2f);


    const coreObject3D* pBase = m_aiCounter[COUNTER_DUPLICATE_STATUS] ? &m_Duplicate : this;

    const coreVector2 vOldPosition = pBase->GetPosition ().xy();
    const coreFloat   fOldAngle    = pBase->GetDirection().xy().Angle();




    // 
    if(m_iPhase == 0u)
    {
        // 
        PHASE_MAIN_MOVE(coreVector2(-0.667f,2.0f), coreVector2(-0.667f,-1.5f), 0.5f, LERP_LINEAR)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                // 
                this->__RotateDirection(0.0f*PI, 6.0f*PI, fTime);
            }

            PHASE_SUB_EXIT()
            {
                // 
                ++m_iPhase;
            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 1u)
    {
        // 
        PHASE_MAIN_MOVE(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), 0.5f, LERP_BREAK)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                // 
                if((fTime > 0.85f) && (g_pGame->GetMission()->GetCurBoss() != this))
                {
                    g_pGame->GetMission()->SetCurBoss(this);
                    g_pGame->GetInterface()->ShowBoss(this);
                }

                // 
                this->__RotateDirection(0.0f, 5.0f*PI, fTime);
            }

            PHASE_SUB_EXIT()
            {
                // 
                ++m_iPhase;
            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 2u || m_iPhase == 5u)
    {
        ++m_iPhase;


    }

    // 
    else if(m_iPhase == 3u || m_iPhase == 6u)
    {
        // 
        PHASE_MAIN_SHOOT(0u, 0xFFFFu, 6.0f)

            PHASE_SUB_UPDATE(const coreUint16& iShot)
            {
                //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy()+coreVector2(3.0f,0.0f), coreVector2(0.0f, -SIGN(this->GetPosition().y)));
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy()+coreVector2(1.5f,0.0f), coreVector2(0.0f, -SIGN(this->GetPosition().y)))->MakeGreen();
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy()-coreVector2(1.5f,0.0f), coreVector2(0.0f, -SIGN(this->GetPosition().y)))->MakeGreen();
                //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy()-coreVector2(3.0f,0.0f), coreVector2(0.0f, -SIGN(this->GetPosition().y)));
                // g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), coreVector2(0.0f, -SIGN(this->GetPosition().y)));

                if(m_aiCounter[COUNTER_DUPLICATE_STATUS])
                {
                    //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy()+coreVector2(3.0f,0.0f), coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y)));
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy()+coreVector2(1.5f,0.0f), coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y)));
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy()-coreVector2(1.5f,0.0f), coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y)));
                    //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy()-coreVector2(3.0f,0.0f), coreVector2(0.0f, -SIGN(m_Duplicate.GetPosition().y)));
                }
            }

        PHASE_END

        // 
        PHASE_MAIN_MOVE(m_vOldPos, coreVector2(m_aiCounter[COUNTER_CURRENT_SIDE] ? 4.5f : -4.5f, 0.75f), 0.2f, LERP_SMOOTH)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                if(m_aiCounter[COUNTER_DUPLICATE_STATUS] == 1)
                {

                    m_Duplicate.SetAlpha(MIN(10.0f*fTime, 1.0f));

                    // 
                    if(fTime > 0.5f) m_aiCounter[COUNTER_DUPLICATE_STATUS] = 2;
                }

                // 
                if(m_aiCounter[COUNTER_DUPLICATE_STATUS])
                {
                    g_pEnvironment->SetTargetDirection(coreVector2::Direction((m_aiCounter[COUNTER_CURRENT_SIDE] ? 2.0f*PI : -2.5f*PI) * fTime));

                    if((m_iPhase == 6u) && (fTime > 0.333f) && (fTime < 0.667f))
                        this->SetPosition(this->GetPosition().InvertedY());
                }

                // 
                this->__RotateDirection(1.0f*PI, 11.0f*PI, m_aiCounter[COUNTER_CURRENT_SIDE] ? fTime : (1.0f - fTime));
            }

            PHASE_SUB_EXIT()
            {
                ++m_iPhase;
                PHASE_RESET_TIMER_SHOOT(0u)


            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 4u || m_iPhase == 7u)
    {
        // 
        PHASE_MAIN_SHOOT(0u, 60u, 10.0f)

            PHASE_SUB_UPDATE(const coreUint16& iShot)
            {
                coreMatrix3 mRota  = coreMatrix3::Rotation(0.05f*PI);
                coreMatrix3 mRota2 = mRota;
                mRota2._12 *= -1.0f;
                mRota2._21 *= -1.0f;

                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(),  this->GetDirection().xy().InvertedX()*mRota);
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), -this->GetDirection().xy().InvertedX()*mRota);
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(),  this->GetDirection().xy().InvertedX()*mRota2);
                g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), -this->GetDirection().xy().InvertedX()*mRota2);
                //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(),  this->GetDirection().xy().Rotated90());
                //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), -this->GetDirection().xy().Rotated90());

                if(m_aiCounter[COUNTER_DUPLICATE_STATUS])
                {
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(),  m_Duplicate.GetDirection().xy().InvertedX()*mRota) ->MakeGreen();
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), -m_Duplicate.GetDirection().xy().InvertedX()*mRota) ->MakeGreen();
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(),  m_Duplicate.GetDirection().xy().InvertedX()*mRota2)->MakeGreen();
                    g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), -m_Duplicate.GetDirection().xy().InvertedX()*mRota2)->MakeGreen();
                    //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(),  m_Duplicate.GetDirection().xy().Rotated90());
                    //g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, m_Duplicate.GetPosition().xy(), -m_Duplicate.GetDirection().xy().Rotated90());
                }
            }

            PHASE_SUB_EXIT()
            {
                ++m_iPhase;
                m_aiCounter[COUNTER_CURRENT_SIDE] = 1 - m_aiCounter[COUNTER_CURRENT_SIDE];

                PHASE_RESET_TIMER_MOVE
            }

        PHASE_END

        // 
        PHASE_MAIN_MOVE(m_vOldPos, m_vOldPos, 1.0f/6.0f, LERP_SMOOTH)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                // 
                //this->_RotateDirection(1.0f*PI, 5.0f*PI, 0.5f + 0.5f * COS(2.0f*PI*(m_aiCounter[COUNTER_CURRENT_SIDE] ? fTime : (1.0f - fTime))));
                this->__RotateDirection(1.0f*PI, 5.0f*PI, m_aiCounter[COUNTER_CURRENT_SIDE] ? fTime : (1.0f - fTime));
            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 8u)
    {
        // 
        PHASE_MAIN_MOVE(coreVector2(m_aiCounter[COUNTER_CURRENT_SIDE] ? -0.5f : 0.5f, 0.75f), coreVector2(m_aiCounter[COUNTER_CURRENT_SIDE] ? -0.5f : 0.5f, -1.5f), 0.5f, LERP_BREAK_REV)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                // 
                this->__RotateDirection(1.0f*PI, 5.0f*PI, m_aiCounter[COUNTER_CURRENT_SIDE] ? fTime : (1.0f - fTime));
            }

            PHASE_SUB_EXIT()
            {
                ++m_iPhase;
            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 9u)
    {
        // 
        PHASE_MAIN_MOVE(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), 0.5f, LERP_BREAK)

            PHASE_SUB_UPDATE(const coreFloat& fTime)
            {
                // 
                this->__RotateDirection(1.0f*PI, 5.0f*PI, m_aiCounter[COUNTER_CURRENT_SIDE] ? fTime : (1.0f - fTime));
            }

            PHASE_SUB_EXIT()
            {
                if(m_aiCounter[COUNTER_DUPLICATE_STATUS] == 0)
                {
                    m_iPhase = 2u;
                    m_aiCounter[COUNTER_CURRENT_SIDE] = 1 - m_aiCounter[COUNTER_CURRENT_SIDE];

                    m_Duplicate.SetEnabled(CORE_OBJECT_ENABLE_ALL);
                    m_Duplicate.SetAlpha(0.0f);
                    m_aiCounter[COUNTER_DUPLICATE_STATUS] = 1;
                }
                else
                {
                    m_iPhase = 10u;
                }
            }

        PHASE_END
    }

    // 
    else if(m_iPhase == 10u)
    {

    }


    coreBool bChange = false;
    this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), 2.0f, &bChange), 0.0f));


    if(m_aiCounter[COUNTER_DUPLICATE_STATUS] == 1)
    {
        m_Duplicate.SetPosition (this->GetPosition ().InvertedX());
        m_Duplicate.SetDirection(this->GetDirection().InvertedX());
    }
    else if(m_aiCounter[COUNTER_DUPLICATE_STATUS] == 2)
    {
        m_Duplicate.SetPosition (-this->GetPosition ());
        m_Duplicate.SetDirection(-this->GetDirection());
    }
    m_Duplicate.SetOrientation(-this->GetOrientation());
    m_Duplicate.SetTexOffset  (coreVector2(0.0f, m_fAnimation));
    m_Duplicate.Move();


    if(Core::System->GetTime())
    {
        auto nAngleDiffFunc = [](const coreFloat& x, const coreFloat& y)
        {
            const coreFloat A = (x - y);
            if(A < -PI) return (A + 2.0f*PI);
            if(A >  PI) return (A - 2.0f*PI);
            return A;
        };


        const coreFloat fRcpTime = RCP(Core::System->GetTime());

        const coreVector2 vCurPosition = pBase->GetPosition ().xy();
        const coreFloat   fCurAngle    = pBase->GetDirection().xy().Angle();

        const coreVector2 vOffsetPosition = (vCurPosition - vOldPosition)        * fRcpTime * 0.05f;
        const coreFloat   fOffsetAngle    = nAngleDiffFunc(fCurAngle, fOldAngle) * fRcpTime * 0.05f;

        for(coreUintW i = 0u; i < CROSSFIELD_TRAILS; ++i)
        {
            const coreVector2 vNewPosition  = vCurPosition - vOffsetPosition                  * I_TO_F(CROSSFIELD_TRAILS-i);
            const coreVector2 vNewDirection = coreVector2::Direction(fCurAngle - fOffsetAngle * I_TO_F(CROSSFIELD_TRAILS-i)).InvertX();

            m_aDuplicateTrail[i].SetPosition   (coreVector3(vNewPosition,  0.0f));
            m_aDuplicateTrail[i].SetDirection  (coreVector3(vNewDirection, 0.0f));
            m_aDuplicateTrail[i].SetOrientation(pBase->GetOrientation());
            m_aDuplicateTrail[i].SetTexOffset  (coreVector2(0.0f, m_fAnimation));
            m_aDuplicateTrail[i].Move();
        }
    }

    // 
    m_Boomerang     .MoveNormal();
    m_BoomerangTrail.MoveNormal();
}


// ****************************************************************
// 
coreVector2 cCrossfieldBoss::__RepeatPosition(const coreVector2& vPosition, const coreFloat& fThreshold, coreBool* OUTPUT pbChange)
{
    const coreVector2 vThreshVec = fThreshold * FOREGROUND_AREA;

    // 
    coreVector2 vRepeatPos = vPosition;
    while(vRepeatPos.x < -vThreshVec.x) vRepeatPos.x += 2.0f * vThreshVec.x;
    while(vRepeatPos.x >  vThreshVec.x) vRepeatPos.x -= 2.0f * vThreshVec.x;
    while(vRepeatPos.y < -vThreshVec.y) vRepeatPos.y += 2.0f * vThreshVec.y;
    while(vRepeatPos.y >  vThreshVec.y) vRepeatPos.y -= 2.0f * vThreshVec.y;

    // 
    ASSERT(pbChange)
    *pbChange = (vRepeatPos.x != vPosition.x) || (vRepeatPos.y != vPosition.y);

    return vRepeatPos;
}


// ****************************************************************
// 
void cCrossfieldBoss::__RotateDirection(const coreFloat& fAngleFrom, const coreFloat& fAngleTo, const coreFloat& fLerp)
{
    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(LERP(fAngleFrom, fAngleTo, fLerp));
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// 
void cCrossfieldBoss::__EncodeDirection(const coreUintW& iIndex, const coreVector2& vDirection)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)

    // 
    if(vDirection.x + vDirection.y > 0.0f)
         ADD_BIT   (m_iPackedDir, 0u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 0u + 2u*iIndex);

    // 
    if(coreMath::InRange(vDirection.y, 0.0f, CORE_MATH_PRECISION))
         ADD_BIT   (m_iPackedDir, 1u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 1u + 2u*iIndex);

    STATIC_ASSERT(CROSSFIELD_BOOMERANGS*2u < sizeof(m_iPackedDir)*8u)
}


// ****************************************************************
// 
coreVector2 cCrossfieldBoss::__DecodeDirection(const coreUintW& iIndex)
{
    ASSERT(iIndex < CROSSFIELD_BOOMERANGS)

    // 
    const coreFloat P = CONTAINS_BIT(m_iPackedDir, 0u + 2u*iIndex) ? 1.0f : -1.0f;
    const coreBool  X = CONTAINS_BIT(m_iPackedDir, 1u + 2u*iIndex) ? true : false;

    // 
    return coreVector2((X) ? P : 0.0f, (!X) ? P : 0.0f);
}





    //FOR_EACH(it, *m_apBoomerang.List())
    //{
    //    if(!(*it)->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
    //
    //    const coreUintW iIndex = it - m_apBoomerang.List()->begin();
    //
    //    (*it)->SetTexOffset(coreVector2(0.0f, m_fAnimation));
    //
    //    const coreVector2 vPos = (*it)->GetPosition().xy();
    //    const coreVector2 vDir = nDecodeDirFunc(iIndex);
    //
    //    coreBool bChange = false;
    //    (*it)->SetPosition(coreVector3(this->_RepeatPosition(vPos + vDir * FOREGROUND_AREA * Core::System->GetTime() * 1.5f, 1.35f, &bChange), 0.0f));
    //
    //    if(bChange)
    //    {
    //        const coreVector2 vAbsDir = vDir.Abs();
    //        (*it)->SetPosition(coreVector3((*it)->GetPosition().xy() * vAbsDir + pPlayer->GetPosition().xy() * vAbsDir.yx(), 0.0f));
    //    }
    //
    //    const coreVector2 vDir2 = coreVector2::Direction((1.0f/16.0f)*PI * ((*it)->GetPosition().xy().x + (*it)->GetPosition().xy().y));
    //    (*it)->SetDirection(coreVector3(vDir2, 0.0f));
    //}


    //FOR_EACH(it, *m_apTrail.List())
    //{
    //    if(!(*it)->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
    //
    //    const coreUintW iIndex = (it - m_apTrail.List()->begin());
    //
    //    const coreObject3D* pParent = (*m_apBoomerang.List())[iIndex / 3u];
    //
    //    (*it)->SetTexOffset(coreVector2(0.0f, m_fAnimation));
    //
    //    const coreVector2 vDir = nDecodeDirFunc(iIndex / 3u);
    //
    //    (*it)->SetPosition(coreVector3(pParent->GetPosition().xy() - vDir * FOREGROUND_AREA * 0.06f*I_TO_F(3u-(iIndex % 3u)), 0.0f));
    //
    //    const coreVector2 vDir2 = coreVector2::Direction((1.0f/16.0f)*PI * ((*it)->GetPosition().xy().x + (*it)->GetPosition().xy().y));
    //    (*it)->SetDirection(coreVector3(vDir2, 0.0f));
    //}
