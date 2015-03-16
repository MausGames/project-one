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
    ENEMY_STATUS_DEAD   = 0x01,   // completely removed from the game
    ENEMY_STATUS_SILENT = 0x02    //  no-shoot # 
};


// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
protected:
    coreSpline2* m_pPath;         // 
    coreVector2  m_vPathOffset;   // 

    coreFlow  m_fLifeTime;        // 
    coreUint8 m_iNumShots;        // 


public:
    cEnemy()noexcept;
    virtual ~cEnemy();

    FRIEND_CLASS(cGame)
    FRIEND_CLASS(cWater)
    DISABLE_COPY(cEnemy)
    ENABLE_ID

    // configure the enemy
    void Configure (const coreInt32& iHealth, const coreVector3& vColor);
    void SetupTrack(coreSpline2* pPath, const coreVector2& vPathOffset);

    // render and move the enemy
    void Render()override;
    void Move  ()override;

    // reduce current health
    void TakeDamage(const coreInt32& iDamage, cPlayer* pAttacker);

    // control life and death
    void Resurrect();
    void Resurrect(const coreVector2& vPosition, const coreVector2& vDirection);
    void Kill     (const coreBool&    bAnimated);


protected:
    // 
    coreBool _DefaultShooting      (const coreFloat& fFireRate);
    coreBool _DefaultMovementPath  (const coreFloat& fDistance);
    coreBool _DefaultMovementTarget(const coreVector2& vTarget, const coreFloat& fSpeedTurn, const coreFloat& fSpeedMove);


private:
    // render and move routines for derived classes (render functions executed by game and water)
    virtual void __RenderOwnBefore() {}
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


private:
    // 
    void __MoveOwn()override;
};


// ****************************************************************
// warrior enemy class
class cWarriorEnemy final : public cEnemy
{
public:
    cWarriorEnemy()noexcept;

    DISABLE_COPY(cWarriorEnemy)
    ASSIGN_ID(2, "Warrior")


private:
    // 
    void __MoveOwn()override;
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