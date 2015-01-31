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
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
public:
    cEnemy()noexcept;
    virtual ~cEnemy();

    DISABLE_COPY(cEnemy)
    ENABLE_ID

    // render and move the enemy
    void Render()override;
    void Move  ()override;


private:
    // render and move routines for derived classes
    virtual void __RenderOwn() {}   // TODO # like bullet ?
    virtual void __MoveOwn()   {}
};


// ****************************************************************
// scout enemy class
class cScoutEnemy final : public cEnemy
{
public:
    cScoutEnemy()noexcept;

    DISABLE_COPY(cScoutEnemy)
    ASSIGN_ID(1, "ScoutEnemy")
};


// ****************************************************************
// warrior enemy class
class cWarriorEnemy final : public cEnemy
{
public:
    cWarriorEnemy()noexcept;

    DISABLE_COPY(cWarriorEnemy)
    ASSIGN_ID(2, "WarriorEnemy")
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
    ASSIGN_ID(3, "StarEnemy")


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
    ASSIGN_ID(4, "ArrowEnemy")


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
    ASSIGN_ID(5, "MinerEnemy")
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
    ASSIGN_ID(6, "FreezerEnemy")


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
    ASSIGN_ID(7, "CinderEnemy")


private:
    // move the cinder enemy
    void __MoveOwn()override;
};


#endif // _P1_GUARD_ENEMY_H_