//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENEMY_H_
#define _P1_GUARD_ENEMY_H_


// ****************************************************************
// enemy definitions
enum eEnemyStatus : coreUint8
{
    ENEMY_STATUS_DEAD   = 0x01u,   // completely removed from the game
    ENEMY_STATUS_SILENT = 0x02u    // not able to attack
};


// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
protected:
    coreFlow  m_fLifeTime;   // 
    coreUint8 m_iNumShots;   // 


public:
    cEnemy()noexcept;
    virtual ~cEnemy();

    FRIEND_CLASS(cGame)
    DISABLE_COPY(cEnemy)
    ENABLE_ID

    // configure the enemy
    void Configure(const coreInt32& iHealth, const coreVector3& vColor);

    // render and move the enemy
    void Render()override;
    void Move  ()override;

    // reduce current health
    void TakeDamage(const coreInt32& iDamage, cPlayer* pAttacker);

    // control life and death
    void Resurrect(const coreSpline2& oPath);
    void Resurrect(const coreVector2& vPosition, const coreVector2& vDirection);
    void Kill     (const coreBool&    bAnimated);

    // 
    coreBool DefaultMovePath     (const coreSpline2& oPath, const coreVector2& vOffset, const coreFloat& fDistance);
    coreBool DefaultMoveTarget   (const coreVector2& vTarget, const coreFloat& fSpeedTurn, const coreFloat& fSpeedMove);
    void     DefaultMoveLerp     (const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat& fTime);
    void     DefaultRotate       (const coreFloat& fAngle);
    void     DefaultRotateLerp   (const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime);
    void     DefaultOrientate    (const coreFloat& fAngle);
    void     DefaultOrientateLerp(const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime);
    void     DefaultMultiate     (const coreFloat& fAngle);
    coreBool DefaultShoot        (const coreFloat& fFireRate);

    // get object properties
    inline const coreFloat& GetLifeTime()const {return m_fLifeTime;}


private:
    // own routines for derived classes (render functions executed by game)
    virtual void __ResurrectOwn   () {}
    virtual void __KillOwn        () {}
    virtual void __RenderOwnWeak  () {}
    virtual void __RenderOwnStrong() {}
    virtual void __RenderOwnAfter () {}
    virtual void __MoveOwn        () {}
};


// ****************************************************************
// scout enemy class
class cScoutEnemy final : public cEnemy
{
public:
    cScoutEnemy()noexcept;

    DISABLE_COPY(cScoutEnemy)
    ASSIGN_ID(1, "Scout")
};


// ****************************************************************
// warrior enemy class
class cWarriorEnemy final : public cEnemy
{
public:
    cWarriorEnemy()noexcept;

    DISABLE_COPY(cWarriorEnemy)
    ASSIGN_ID(2, "Warrior")
};


// ****************************************************************
// star enemy class
class cStarEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cStarEnemy()noexcept;

    DISABLE_COPY(cStarEnemy)
    ASSIGN_ID(3, "Star")


private:
    // move the star enemy
    void __MoveOwn()override;
};


// ****************************************************************
// arrow enemy class
class cArrowEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cArrowEnemy()noexcept;

    DISABLE_COPY(cArrowEnemy)
    ASSIGN_ID(4, "Arrow")


private:
    // move the arrow enemy
    void __MoveOwn()override;
};


// ****************************************************************
// miner enemy class
class cMinerEnemy final : public cEnemy
{
public:
    cMinerEnemy()noexcept;

    DISABLE_COPY(cMinerEnemy)
    ASSIGN_ID(5, "Miner")
};


// ****************************************************************
// freezer enemy class
class cFreezerEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cFreezerEnemy()noexcept;

    DISABLE_COPY(cFreezerEnemy)
    ASSIGN_ID(6, "Freezer")


private:
    // move the freezer enemy
    void __MoveOwn()override;
};


// ****************************************************************
// cinder enemy class
class cCinderEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cCinderEnemy()noexcept;

    DISABLE_COPY(cCinderEnemy)
    ASSIGN_ID(7, "Cinder")


private:
    // move the cinder enemy
    void __MoveOwn()override;
};


#endif // _P1_GUARD_ENEMY_H_